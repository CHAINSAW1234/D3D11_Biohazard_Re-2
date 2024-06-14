#include "stdafx.h"
#include "..\Public\Player.h"

#include "FSM.h"
#include "Player_State_Move.h"
#include "Player_State_Hold.h"

#include "Body_Player.h"
#include "Head_Player.h"
#include "Hair_Player.h"

#include"CustomCollider.h"

#include "Character_Controller.h"
#include "Camera_Free.h"

#define MODEL_SCALE 0.01f

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject{ rhs }
{

}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Initialize_PartModels()))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_Scaled(MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);

	m_pController = m_pGameInstance->Create_Controller(_float4(0.f, 0.3f, 5.f, 1.f), &m_iIndex_CCT, this, 1.f, 0.475f);

	//For Camera.
	m_pTransformCom_Camera = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom_Camera)
		return E_FAIL;
	m_pTransformCom_Camera->SetRotationPerSec(0.75f);

	//For Camera.
	Load_CameraPosition();
	if (FAILED(Ready_Camera()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	Priority_Tick_PartObjects(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
#pragma region 예은ColTest - 컬링 방식에 따라 달라질 겁니당
	if (m_iCurCol != m_iPreCol)
	{
		m_iPreCol = m_iCurCol;
		m_bChange = true;
	}
	else
		m_bChange = false;

	m_fTimeTEST += fTimeDelta;
	if (m_pGameInstance->Get_KeyState(VK_F7) == DOWN && m_fTimeTEST > 0.1f)
	{
		m_fTimeTEST = 0.f;
		m_iCurCol++;
	}
	if (m_pGameInstance->Get_KeyState(VK_F6) == DOWN && m_fTimeTEST > 0.1f)
	{
		m_fTimeTEST = 0.f;
		m_iCurCol--;
	}
#pragma endregion 예은ColTest

#pragma region 이동과 카메라
	if (m_pController)
	{
		auto CameraPos = m_pController->GetPosition_Float4();
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, CameraPos);
		CameraPos.y += CONTROLLER_GROUND_GAP;
		m_pTransformCom_Camera->Set_State(CTransform::STATE_POSITION, CameraPos);
	}

	_vector			vMovedDirection = { XMLoadFloat3(&m_vRootTranslation) };
	if (DOWN == m_pGameInstance->Get_KeyState('B'))
	{
		_vector			vCurrentPostion = { m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) };
		vMovedDirection = XMVectorSetW(vCurrentPostion * -1.f, 0.f);
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('H'))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('K'))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}

	if (m_pController)
	{
		if (PRESSING == m_pGameInstance->Get_KeyState('W'))
		{
			m_bMove_Forward = true;
			m_bMove = true;

			if (m_bLerp_Move == false)
			{
				m_bLerp_Move = true;
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}
		}
		else
		{
			if (m_bMove_Forward && m_bCollision_Lerp == false)
			{
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}

			m_bMove_Forward = false;
		}

		if (PRESSING == m_pGameInstance->Get_KeyState('S'))
		{
			/*if (m_bTurnAround == false)
			{
				m_bTurnAround = true;

				auto CamLook = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_LOOK);
				CamLook.x = -CamLook.x;
				CamLook.z = -CamLook.z;
				m_vTurnAround_Look_Vector = Float4_Normalize(CamLook);
				m_fTurnAround_Time = 0.f;
			}*/

			m_bMove_Backward = true;
			m_bMove = true;

			if (m_bLerp_Move == false)
			{
				m_bLerp_Move = true;
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}
		}
		else
		{
			if (m_bMove_Backward && m_bCollision_Lerp == false)
			{
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}

			m_bMove_Backward = false;
		}

		_float4			vResultMoveDirFloat4 = {};
		XMStoreFloat4(&vResultMoveDirFloat4, vMovedDirection);
		m_pController->Move(vResultMoveDirFloat4, fTimeDelta);
	}

	if (m_bMove_Backward == false && m_bMove_Forward == false)
	{
		m_bLerp_Move = false;
		m_bMove = false;
	}

#pragma region Camera

	if (m_pCamera)
	{
		Calc_Camera_LookAt_Point(fTimeDelta);
	}

	RayCasting_Camera();

	if (PRESSING == m_pGameInstance->Get_KeyState(VK_RBUTTON))
	{
		if (m_bAim == false)
		{
			m_bAim = true;
			m_fLerpTime = 0.f;
			m_bLerp = true;
		}
	}
	else
	{
		if (m_bAim)
		{
			m_bLerp = true;
			m_fLerpTime = 0.f;
		}

		m_bAim = false;
	}
#pragma endregion

#pragma endregion


#pragma region 현진 추가
	m_pFSMCom->Update(fTimeDelta);

	Update_FSM();
#pragma endregion

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	Tick_PartObjects(fTimeDelta);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	Late_Tick_PartObjects(fTimeDelta);

#pragma region 예은 추가
	Col_Section();
#pragma endregion 

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif
}

HRESULT CPlayer::Render()
{

	return S_OK;
}

void CPlayer::Priority_Tick_PartObjects(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Priority_Tick(fTimeDelta);
}

void CPlayer::Tick_PartObjects(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);
}

void CPlayer::Late_Tick_PartObjects(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Late_Tick(fTimeDelta);
}
#pragma region 예은 추가
void CPlayer::Col_Section()
{
	/*list<CGameObject*>* pCollider = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Collider"));
	if(pCollider == nullptr)
		return;
	for (auto& iter: *pCollider)
	{
		if (m_pColliderCom->Intersect(static_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider")))))
		{
			CCustomCollider* pColCom = static_cast<CCustomCollider*>(iter);

			m_iCurCol = pColCom->Get_Col();
			m_iDir = pColCom->Get_Dir();

		}
	}*/
}

#pragma endregion

#pragma region 현진 추가
CModel* CPlayer::Get_Body_Model()
{
	return static_cast<CModel*>(m_PartObjects[0]->Get_Component(g_strModelTag));
}

void CPlayer::Change_State(STATE eState)
{
	m_pFSMCom->Change_State(eState);
}

void CPlayer::Update_FSM()
{
	if (m_pGameInstance->Get_KeyState(VK_RBUTTON) == PRESSING)
		Change_State(HOLD);
	else 
		Change_State(MOVE);
}

#pragma endregion

void CPlayer::Calc_YPosition_Camera()
{
	_float4 YDelta = Sub_Float4(m_pController->GetPosition_Float4(), m_vPrev_Position);
	YDelta.z = 0.f;
	YDelta.x = 0.f;
	m_vCameraPosition = Add_Float4_Coord(m_vCameraPosition, YDelta);
	m_vCamera_LookAt_Point = Add_Float4_Coord(m_vCamera_LookAt_Point, YDelta);
}

void CPlayer::Calc_Camera_Transform(_float fTimeDelta)
{
	_vector vLook = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_LOOK);
	_vector vUp = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_UP);
	_vector vRight = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT);
	_vector vLookAtPoint;
	_vector vPos = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_POSITION);

	auto Pos = m_pCamera->Get_Position_Float4();


	if (m_bMove_Forward)
	{
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.85f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 1.35f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 1.35f);
		}
	}

	if (m_bMove_Backward)
	{
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.35f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.35f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.85f);
		}
	}

	if (m_bMove == false)
	{
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.5f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.5f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos);
		}
	}


	if (m_bCollision_Lerp == false)
	{
		if (m_bLerp)
		{
			_vector vOrigin = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fLerpAmount_Right) + XMVectorScale(XMVector4Normalize(vUp), m_fLerpAmount_Up) + XMVectorScale(XMVector4Normalize(vLook), m_fLerpAmount_Look);
			_vector NewPos = XMVectorLerp(vOrigin, m_vCameraPosition, m_fLerpTime);
			m_pCamera->SetPosition(NewPos);
		}
		else
		{
			m_pCamera->SetPosition(m_vCameraPosition);
		}
	}
	else
	{
		if (m_bLerp)
		{
			_vector NewPos = XMVectorLerp(m_pCamera->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION), m_vCameraPosition, m_fLerpTime);
			m_pCamera->SetPosition(NewPos);
		}
		else
		{
			m_pCamera->SetPosition(m_vCameraPosition);
		}
	}

	vLookAtPoint = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Look) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Look) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Look);
	XMStoreFloat4(&m_vCamera_LookAt_Point, vLookAtPoint);
	m_pCamera->LookAt(m_vCamera_LookAt_Point);

	m_vLookPoint_To_Position_Dir = Sub_Float4(m_vCamera_LookAt_Point, m_vCameraPosition);

	_vector vCamPos = XMLoadFloat4(&m_vCameraPosition);
	_vector vLookAtPos = XMLoadFloat4(&m_vCamera_LookAt_Point);
	_vector vDelta = XMVector4Normalize(vLookAtPos - vCamPos);
	XMStoreFloat4(&m_vRayDir, vDelta);

	_vector vCharPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	vCharPos = XMVectorSetY(vCharPos, XMVectorGetY(vCharPos) + 1.f);
	_vector vCameraPos = XMLoadFloat4(&m_vCameraPosition);

	m_fRayDist = XMVectorGetX(XMVector3Length(vCharPos - vCameraPos));
}

void CPlayer::Calc_Camera_LookAt_Point(_float fTimeDelta)
{
	POINT		ptDeltaPos = { m_pGameInstance->Get_MouseDeltaPos() };
	_float		fMouseSensor = { 0.1f };

	if (abs(ptDeltaPos.x) > 2.5f || abs(ptDeltaPos.y) > 2.5f)
	{
		m_bRotate_Delay = true;
		m_bRotate_Delay_Start = false;
		m_fRotate_Delay_Amount = 1.f;

		m_fPrev_Rotate_Amount_X = ptDeltaPos.x * 0.1f;
		m_fPrev_Rotate_Amount_Y = ptDeltaPos.y * 0.1f;
	}

	if (m_bRotate_Delay && (abs(ptDeltaPos.x) <= 2.5f && abs(ptDeltaPos.y) <= 2.5f))
	{
		m_bRotate_Delay_Start = true;
		m_fRotate_Delay_Amount = 1.f;
		m_bRotate_Delay = false;
	}

	if (m_bRotate_Delay_Start == true)
	{
		m_fRotate_Delay_Amount -= (fTimeDelta * 2.5f);

		if (m_fRotate_Delay_Amount <= 0.f)
		{
			m_bRotate_Delay_Start = false;
			m_fRotate_Delay_Amount = 0.f;
		}
	}

	if (m_bRotate_Delay_Start)
	{
		m_fRotate_Amount_X = (fTimeDelta * m_fRotate_Delay_Amount) * (_float)m_fPrev_Rotate_Amount_X * fMouseSensor;
		m_fRotate_Amount_Y = (fTimeDelta * m_fRotate_Delay_Amount) * (_float)m_fPrev_Rotate_Amount_Y * fMouseSensor;
	}
	else
	{
		m_fRotate_Amount_X = fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor;
		m_fRotate_Amount_Y = fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor;
	}

	if (m_bCollision_Lerp == false)
	{
		if (m_bAim == false)
			m_fLerpTime += fTimeDelta * 0.7f;
		else
			m_fLerpTime += fTimeDelta;
	}
	else
	{
		m_fLerpTime += fTimeDelta * 1.5f;
	}

	if (m_fLerpTime >= 1.f)
	{
		m_bLerp = false;
		m_fLerpTime = 1.f;

		if (m_bCollision_Lerp)
			m_bCollision_Lerp = false;
	}

	if (m_bTurnAround == false)
	{
		if (m_bAim == false)
		{
			//Parallel Rotate
			{
				m_pTransformCom_Camera->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRotate_Amount_X);
			}

			//Vertical Rotate
			{
				m_pTransformCom_Camera->Turn(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT), m_fRotate_Amount_Y);

				auto Camera_Look = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_LOOK);
				auto Camera_Parallel_Look = XMVectorSetY(Camera_Look, 0.f);

				/*if (m_bLerp)
				{
					auto CamPos = m_pCamera->Get_Position_Float4();
					auto RotatePos = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_POSITION);
					CamPos = Axis_Rotate_Vector(CamPos, _float4(0.f,1.f,0.f,0.f), RotatePos, m_fRotate_Amount_X);
					CamPos = Axis_Rotate_Vector(CamPos, m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT), RotatePos, m_fRotate_Amount_Y);
					m_pCamera->SetPosition(CamPos);
				}*/

				if (abs(GetAngleBetweenVectors_Radians(Camera_Look, Camera_Parallel_Look)) > 1.f)
				{
					m_pTransformCom_Camera->Turn(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT), -m_fRotate_Amount_Y);

					/*if (m_bLerp)
					{
						auto CamPos = m_pCamera->Get_Position_Float4();
						auto RotatePos = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_POSITION);
						CamPos = Axis_Rotate_Vector(CamPos, m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT), RotatePos, -m_fRotate_Amount_Y);
						m_pCamera->SetPosition(CamPos);
					}*/
				}
			}
		}
		else
		{
			//Parallel Rotate
			if ((_long)0 != ptDeltaPos.x)
			{
				m_pTransformCom_Camera->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRotate_Amount_X);
			}

			//Vertical Rotate
			if ((_long)0 != ptDeltaPos.y)
			{
				m_pTransformCom_Camera->Turn(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT), m_fRotate_Amount_Y);

				auto Camera_Look = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_LOOK);
				auto Camera_Parallel_Look = XMVectorSetY(Camera_Look, 0.f);
				if (abs(GetAngleBetweenVectors_Radians(Camera_Look, Camera_Parallel_Look)) > 1.f)
				{
					m_pTransformCom_Camera->Turn(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT), -m_fRotate_Amount_Y);
				}
			}
		}
	}
	else
	{
		m_fTurnAround_Time += fTimeDelta;

		if (m_fTurnAround_Time >= 1.f)
		{
			m_fTurnAround_Time = 1.f;
			m_bTurnAround = false;
		}

		auto CamPos = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_POSITION);
		auto Look = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_LOOK);
		XMStoreFloat4(&Look, XMVectorSlerp(XMVector3Normalize(XMLoadFloat4(&Look)), XMLoadFloat4(&m_vTurnAround_Look_Vector), m_fTurnAround_Time));
		CamPos = Add_Float4_Coord(CamPos, Look);

		m_pTransformCom_Camera->Look_At(CamPos);
	}

	Calc_Camera_Transform(fTimeDelta);

	if (DOWN == m_pGameInstance->Get_KeyState(VK_TAB))
		m_bMouseCursorClip = !m_bMouseCursorClip;

	if (true == m_bMouseCursorClip)
	{
		POINT ptPos = {};

		GetCursorPos(&ptPos);
		ScreenToClient(g_hWnd, &ptPos);

		RECT rc = {};
		GetClientRect(g_hWnd, &rc);

		ptPos.x = _long(_float(rc.right - rc.left) * 0.5f);
		ptPos.y = _long(_float(rc.bottom - rc.top) * 0.5f);

		m_pGameInstance->Set_MouseCurPos(ptPos);

		ClientToScreen(g_hWnd, &ptPos);
		SetCursorPos(ptPos.x, ptPos.y);
	}

	return;
}

HRESULT CPlayer::Ready_Camera()
{
	m_pCamera = dynamic_cast<CCamera_Free*>(*(*m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, L"Layer_ZZZCamera")).begin());

	if (m_pCamera == nullptr)
		return E_FAIL;

	m_pCamera->SetPlayer(this);

	_vector vLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);
	_vector vUp = m_pTransformCom->Get_State_Vector(CTransform::STATE_UP);
	_vector vRight = m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT);
	_vector vLookAtPoint;
	_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	vLookAtPoint = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Look) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Look) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Look);
	XMStoreFloat4(&m_vCamera_LookAt_Point, vLookAtPoint);

	m_pCamera->SetPlayer(this);

	m_pCamera->SetPosition(m_vCameraPosition);
	m_pCamera->LookAt(m_vCamera_LookAt_Point);

	m_vLookPoint_To_Position_Dir = Sub_Float4(m_vCamera_LookAt_Point, m_vCameraPosition);

	_vector vCharPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	vCharPos = XMVectorSetY(vCharPos, XMVectorGetY(vCharPos) + 1.f);
	_vector vCameraPos = XMLoadFloat4(&m_vCameraPosition);

	m_fRayDist = XMVectorGetX(XMVector4Length(vCharPos - vCameraPos));

	m_vOrigin_LookAt_Point = m_vCamera_LookAt_Point;

	m_fLerpAmount_Look = m_fLook_Dist_Pos;
	m_fLerpAmount_Right = m_fRight_Dist_Pos;
	m_fLerpAmount_Up = m_fUp_Dist_Pos;

	return S_OK;
}

void CPlayer::Load_CameraPosition()
{
	string filePath = "../Camera_Position/Camera_Position";

	//File Import
	ifstream File(filePath, std::ios::binary | std::ios::in);

	File.read((char*)&m_vCameraPosition, sizeof(_float4));
	File.read((char*)&m_fRight_Dist_Look, sizeof(_float));
	File.read((char*)&m_fUp_Dist_Look, sizeof(_float));
	File.read((char*)&m_fLook_Dist_Look, sizeof(_float));

	m_fUp_Dist_Look -= CONTROLLER_GROUND_GAP;

	m_fLook_Dist_Pos = m_vCameraPosition.z;
	m_fRight_Dist_Pos = m_vCameraPosition.x;
	m_fUp_Dist_Pos = m_vCameraPosition.y - CONTROLLER_GROUND_GAP;

	File.close();
}

void CPlayer::RayCasting_Camera()
{
	_vector vLook = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_LOOK);
	_vector vUp = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_UP);
	_vector vRight = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT);
	_vector vLookAtPoint;
	_vector vPos = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_POSITION);
	_float4 CamPos = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fLerpAmount_Right) + XMVectorScale(XMVector4Normalize(vUp), m_fLerpAmount_Up) + XMVectorScale(XMVector4Normalize(vLook), m_fLerpAmount_Look);
	//_float4 CamPos = m_pCamera->Get_Position_Float4();

	_float4 BlockPoint;
	_float4 TempPos;
	//TempPos.x = CamPos.x + (m_vRayDir.x * m_fRayDist*1.5f);
	//TempPos.y = CamPos.y + (m_vRayDir.y * m_fRayDist*1.5f);
	//TempPos.z = CamPos.z + (m_vRayDir.z * m_fRayDist*1.5f);
	//TempPos.w = 1.f;

	TempPos = CamPos + (m_vRayDir * m_fRayDist);
	TempPos.w = 1.f;

	_float4 TempCamPos = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_POSITION);
	TempCamPos.y = m_pCamera->Get_Position_Float4().y;
	_float4 vTempDir = Float4_Normalize(m_pCamera->Get_Position_Float4() - TempCamPos);

	m_vRayDir.x *= -1.f;
	m_vRayDir.y *= -1.f;
	m_vRayDir.z *= -1.f;

	if (m_pGameInstance->SphereCast(TempPos, m_vRayDir, &BlockPoint, m_fRayDist))
	{
		m_fLerpTime = 0.f;
		m_bCollision_Lerp = true;
		m_bLerp = true;
		m_bMove_Forward = false;
		m_bMove_Backward = false;
		m_bLerp_Move = false;
		m_bMove = false;

		m_vRayDir.x *= -1.f;
		m_vRayDir.y *= -1.f;
		m_vRayDir.z *= -1.f;

		auto Position = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
		Position.y = m_vCameraPosition.y;
		_float4 DeltaDir = Float4_Normalize(Position - BlockPoint);
		BlockPoint.x += DeltaDir.x * 0.3f;
		BlockPoint.y += DeltaDir.y * 0.3f;
		BlockPoint.z += DeltaDir.z * 0.3f;

		auto vCharacterPos = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_POSITION) - BlockPoint;

		m_pCamera->SetPosition(BlockPoint);
	}
	else
	{
		m_vRayDir.x *= -1.f;
		m_vRayDir.y *= -1.f;
		m_vRayDir.z *= -1.f;
	}
}

HRESULT CPlayer::Add_Components()
{
	/* Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.vSize = _float3(0.8f, 1.2f, 0.8f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NavigationDesc{};

	NavigationDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
		return E_FAIL;

	if (FAILED(Add_FSM_States()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_FSM_States()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_FSM"), (CComponent**)&m_pFSMCom)))
		return E_FAIL;


	m_pFSMCom->Add_State(MOVE, CPlayer_State_Move::Create(this));

	m_pFSMCom->Add_State(HOLD, CPlayer_State_Hold::Create(this));
	m_pFSMCom->Change_State(MOVE);

	return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CPlayer::PART_END);

	/* For.Part_Body */
	CPartObject* pBodyObject = { nullptr };
	CBody_Player::BODY_DESC		BodyDesc{};

	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.pRootTranslation = &m_vRootTranslation;

	pBodyObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Body_Player"), &BodyDesc));
	if (nullptr == pBodyObject)
		return E_FAIL;

	m_PartObjects[CPlayer::PART_BODY] = pBodyObject;

	/* For.Part_Head */
	CPartObject* pHeadObject = { nullptr };
	CHead_Player::HEAD_DESC		HeadDesc{};

	HeadDesc.pParentsTransform = m_pTransformCom;
	HeadDesc.pState = &m_eState;

	pHeadObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Head_Player"), &HeadDesc));
	if (nullptr == pHeadObject)
		return E_FAIL;

	m_PartObjects[CPlayer::PART_HEAD] = pHeadObject;

	/* For.Part_Hair */
	CPartObject* pHairObject = { nullptr };
	CHair_Player::HAIR_DESC		HairDesc{};

	HairDesc.pParentsTransform = m_pTransformCom;
	HairDesc.pState = &m_eState;

	pHairObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Hair_Player"), &HairDesc));
	if (nullptr == pHairObject)
		return E_FAIL;

	m_PartObjects[CPlayer::PART_HAIR] = pHairObject;

	return S_OK;
}

HRESULT CPlayer::Initialize_PartModels()
{
	CModel* pBodyModel = { dynamic_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Model"))) };
	CModel* pHeadModel = { dynamic_cast<CModel*>(m_PartObjects[PART_HEAD]->Get_Component(TEXT("Com_Model"))) };
	CModel* pHairModel = { dynamic_cast<CModel*>(m_PartObjects[PART_HAIR]->Get_Component(TEXT("Com_Model"))) };

	if (nullptr == pBodyModel ||
		nullptr == pHeadModel ||
		nullptr == pHairModel)
		return E_FAIL;

	_float4x4* pNeck0CombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("neck_0")) };
	pHeadModel->Set_Surbodinate("neck_0", true);
	pHeadModel->Set_Parent_CombinedMatrix_Ptr("neck_0", pNeck0CombinedMatrix);

	_float4x4* pNeck1CombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("neck_1")) };
	pHeadModel->Set_Surbodinate("neck_1", true);
	pHeadModel->Set_Parent_CombinedMatrix_Ptr("neck_1", pNeck1CombinedMatrix);

	_float4x4* pHeadCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("head")) };
	pHeadModel->Set_Surbodinate("head", true);
	pHeadModel->Set_Parent_CombinedMatrix_Ptr("head", pHeadCombinedMatrix);

	_float4x4* pLeftArmCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("l_arm_clavicle")) };
	pHeadModel->Set_Surbodinate("l_arm_clavicle", true);
	pHeadModel->Set_Parent_CombinedMatrix_Ptr("l_arm_clavicle", pLeftArmCombinedMatrix);

	_float4x4* pRightArmCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("r_arm_clavicle")) };
	pHeadModel->Set_Surbodinate("r_arm_clavicle", true);
	pHeadModel->Set_Parent_CombinedMatrix_Ptr("r_arm_clavicle", pRightArmCombinedMatrix);

	_float4x4* pLeftTrapMuscleCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("l_trapA_muscle")) };
	pHeadModel->Set_Surbodinate("l_trapA_muscle", true);
	pHeadModel->Set_Parent_CombinedMatrix_Ptr("l_trapA_muscle", pLeftTrapMuscleCombinedMatrix);

	_float4x4* pRightTrapAMuscleCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("r_trapA_muscle")) };
	pHeadModel->Set_Surbodinate("r_trapA_muscle", true);
	pHeadModel->Set_Parent_CombinedMatrix_Ptr("r_trapA_muscle", pRightTrapAMuscleCombinedMatrix);


	//	pNeck1CombinedMatrix = { const_cast<_float4x4*>(pHeadModel->Get_CombinedMatrix("neck_1")) };
	pHairModel->Set_Surbodinate("neck_1", true);
	pHairModel->Set_Parent_CombinedMatrix_Ptr("neck_1", pNeck1CombinedMatrix);

	pHairModel->Set_Surbodinate("head", true);
	pHairModel->Set_Parent_CombinedMatrix_Ptr("head", pHeadCombinedMatrix);

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pFSMCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom_Camera);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);
	m_PartObjects.clear();

	m_PartObjects.clear();
}
