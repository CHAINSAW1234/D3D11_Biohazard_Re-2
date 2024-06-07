#include "stdafx.h"
#include "..\Public\Player.h"


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

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Initialize_PartModels()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_Scaled(MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);

	m_pController = m_pGameInstance->Create_Controller(_float4(0.f, 0.f, 0.f, 1.f), &m_iIndex_CCT, this);

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



	static _bool Temp = false;

	if (UP == m_pGameInstance->Get_KeyState(VK_BACK))
	{
		Temp = true;
	}

	if (Temp == false)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pController->GetPosition_Float4());


	_vector		vWorldPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vWorldPos);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	Tick_PartObjects(fTimeDelta);

	m_pGameInstance->SetWorldMatrix(m_pTransformCom->Get_WorldFloat4x4());
	m_pGameInstance->SetRotationMatrix(m_pTransformCom->Get_RotationMatrix_Pure());
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	Late_Tick_PartObjects(fTimeDelta);

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

	//For Camera.
	Calc_YPosition_Camera();

	if (PRESSING == m_pGameInstance->Get_KeyState('U'))
	{
		//For Camera.
		auto Pos_Prev = m_pController->GetPosition_Float4();

		_vector      vLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);
		vLook = { XMVectorScale(XMVector3Normalize(vLook),0.01f) };
		_vector      vMoveDir = { vLook };

		vMovedDirection += vMoveDir;

		_float4			vResultMoveDirFloat4 = {};
		XMStoreFloat4(&vResultMoveDirFloat4, vMovedDirection);
		m_pController->Move(vResultMoveDirFloat4, fTimeDelta);

		//For Camera.
		Pos_Prev = Sub_Float4(m_pController->GetPosition_Float4(), Pos_Prev);
		m_vCameraPosition = Add_Float4_Coord(m_vCameraPosition, Pos_Prev);
		m_vCamera_LookAt_Point = Add_Float4_Coord(m_vCamera_LookAt_Point, Pos_Prev);

		m_bMove_Forward = true;

		if (m_bLerp_Move == false)
		{
			m_bLerp_Move = true;
			m_bLerp = true;
			m_fLerpTime = 0.f;
		}
	}
	else
	{
		m_bMove_Forward = false;
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('J'))
	{
		//For Camera.
		auto Pos_Prev = m_pController->GetPosition_Float4();

		_vector      vLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);
		vLook = { XMVectorScale(XMVector3Normalize(vLook),0.01f) };
		_vector      vMoveDir = { -vLook };

		vMovedDirection += vMoveDir;

		_float4			vResultMoveDirFloat4 = {};
		XMStoreFloat4(&vResultMoveDirFloat4, vMovedDirection);
		m_pController->Move(vResultMoveDirFloat4, fTimeDelta);

		//For Camera.
		Pos_Prev = Sub_Float4(m_pController->GetPosition_Float4(), Pos_Prev);
		m_vCameraPosition = Add_Float4_Coord(m_vCameraPosition, Pos_Prev);
		m_vCamera_LookAt_Point = Add_Float4_Coord(m_vCamera_LookAt_Point, Pos_Prev);

		m_bMove_Backward = true;

		if (m_bLerp_Move == false)
		{
			m_bLerp_Move = true;
			m_bLerp = true;
			m_fLerpTime = 0.f;
		}
	}
	else
	{
		m_bMove_Backward = false;
	}

	if (m_bMove_Backward == false && m_bMove_Forward == false)
	{
		m_bLerp_Move = false;
	}
#pragma region Camera

	if (m_pCamera)
	{
		Calc_Camera_LookAt_Point(fTimeDelta);
	}

	RayCasting_Camera();

	//For Camera.
	m_vPrev_Position = m_pController->GetPosition_Float4();
#pragma endregion

#pragma region 예은 추가
	Col_Section();
#pragma endregion 

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
		m_bAim = false;
	}

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

void CPlayer::Calc_YPosition_Camera()
{
	_float4 YDelta = Sub_Float4(m_pController->GetPosition_Float4(), m_vPrev_Position);
	YDelta.z = 0.f;
	YDelta.x = 0.f;
	m_vCameraPosition = Add_Float4_Coord(m_vCameraPosition, YDelta);
	m_vCamera_LookAt_Point = Add_Float4_Coord(m_vCamera_LookAt_Point, YDelta);
}

void CPlayer::Calc_Camera_LookAt_Point(_float fTimeDelta)
{
	POINT		ptDeltaPos = { m_pGameInstance->Get_MouseDeltaPos() };
	_float		fMouseSensor = { 0.1f };

	if (ptDeltaPos.x > 50.f || ptDeltaPos.y > 50.f)
	{
		m_bRotate_Delay = true;
	}
	else
	{
		m_bRotate_Delay = false;
	}

	if (m_bRotate_Delay && (ptDeltaPos.x < 1.f || ptDeltaPos.y < 1.f))
	{
		m_bRotate_Delay_Start = true;
		m_fRotate_Delay_Amount = 1.f;
	}

	if (m_bRotate_Delay_Start == true)
	{
		m_fRotate_Delay_Amount -= fTimeDelta;
	}

	if (m_bRotate_Delay_Start)
	{
		m_fRotate_Amount_X = fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor;
		m_fRotate_Amount_Y = fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor;
	}
	else
	{
		m_fRotate_Amount_X = fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor;
		m_fRotate_Amount_Y = fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor;
	}

	if (m_bAim == false)
		m_fLerpTime += fTimeDelta * 0.03f;
	else
		m_fLerpTime += fTimeDelta * 0.06f;

	if (m_fLerpTime >= 1.f)
	{
		m_bLerp = false;
		m_fLerpTime = 1.f;
	}

	if(m_bAim == false)
	{
		//Y Rotate
		{
			auto CharacterPos = m_pController->GetPosition_Float4();
			CharacterPos.y += 1.f;

			m_vCameraPosition = Axis_Rotate_Vector(m_vCameraPosition, _float4(0.f, 1.f, 0.f, 0.f), CharacterPos, fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor);
			auto Pos = Axis_Rotate_Vector(m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_POSITION), _float4(0.f, 1.f, 0.f, 0.f), CharacterPos, fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor);
			m_pTransformCom_Camera->Set_State(CTransform::STATE_POSITION, Pos);

			if (m_bLerp)
			{
				_vector NewPos = XMVectorLerp(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_POSITION), m_vCameraPosition, m_fLerpTime);
				m_pCamera->SetPosition(NewPos);
			}
			else
			{
				m_pCamera->SetPosition(m_vCameraPosition);
			}

			m_vCamera_LookAt_Point = Axis_Rotate_Vector(m_vCamera_LookAt_Point, _float4(0.f, 1.f, 0.f, 0.f), CharacterPos, fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor);
			m_vLookPoint_To_Position_Dir = Axis_Rotate_Vector(m_vLookPoint_To_Position_Dir, _float4(0.f, 1.f, 0.f, 0.f), CharacterPos, fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor);
			m_pTransformCom_Camera->Look_At(m_vCamera_LookAt_Point);
		}


		//X Rotate
		{
			auto CharacterPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
			CharacterPos.y += 1.f;

			m_vCameraPosition = Axis_Rotate_Vector(m_vCameraPosition, m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_RIGHT), CharacterPos, fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor);
			auto Pos = Axis_Rotate_Vector(m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_POSITION), m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_RIGHT), CharacterPos, fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor);
			m_pTransformCom_Camera->Set_State(CTransform::STATE_POSITION, Pos);

			if (m_fLerpTime != 1.f)
			{
				_vector NewPos = XMVectorLerp(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_POSITION), m_vCameraPosition, m_fLerpTime);
				m_pCamera->SetPosition(NewPos);
			}
			else
			{
				m_pCamera->SetPosition(m_vCameraPosition);
			}

			m_vCamera_LookAt_Point = Axis_Rotate_Vector(m_vCamera_LookAt_Point, m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_RIGHT), CharacterPos, fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor);
			m_vLookPoint_To_Position_Dir = Axis_Rotate_Vector(m_vLookPoint_To_Position_Dir, m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_RIGHT), CharacterPos, fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor);
			m_pTransformCom_Camera->Look_At(m_vCamera_LookAt_Point);
		}
	}
	else
	{
		//Y Rotate
		{
			auto CharacterPos = m_pController->GetPosition_Float4();
			CharacterPos.y += 1.f;

			m_vCameraPosition = Axis_Rotate_Vector(m_vCameraPosition, _float4(0.f, 1.f, 0.f, 0.f), CharacterPos, fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor);
			auto Pos = Axis_Rotate_Vector(m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_POSITION), _float4(0.f, 1.f, 0.f, 0.f), CharacterPos, fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor);
			m_pTransformCom_Camera->Set_State(CTransform::STATE_POSITION, Pos);

			if (m_bLerp)
			{
				_vector NewPos = XMVectorLerp(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_POSITION), m_vCameraPosition - m_vRayDir, m_fLerpTime);
				m_pCamera->SetPosition(NewPos);
			}
			else
			{
				m_pCamera->SetPosition(m_vCameraPosition);
			}

			m_vCamera_LookAt_Point = Axis_Rotate_Vector(m_vCamera_LookAt_Point, _float4(0.f, 1.f, 0.f, 0.f), CharacterPos, fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor);
			m_vLookPoint_To_Position_Dir = Axis_Rotate_Vector(m_vLookPoint_To_Position_Dir, _float4(0.f, 1.f, 0.f, 0.f), CharacterPos, fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor);
			m_pTransformCom_Camera->Look_At(m_vCamera_LookAt_Point);
		}


		//X Rotate
		{
			auto CharacterPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
			CharacterPos.y += 1.f;

			m_vCameraPosition = Axis_Rotate_Vector(m_vCameraPosition, m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_RIGHT), CharacterPos, fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor);
			auto Pos = Axis_Rotate_Vector(m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_POSITION), m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_RIGHT), CharacterPos, fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor);
			m_pTransformCom_Camera->Set_State(CTransform::STATE_POSITION, Pos);

			if (m_fLerpTime != 1.f)
			{
				_vector NewPos = XMVectorLerp(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_POSITION), m_vCameraPosition - m_vRayDir, m_fLerpTime);
				m_pCamera->SetPosition(NewPos);
			}
			else
			{
				m_pCamera->SetPosition(m_vCameraPosition);
			}

			m_vCamera_LookAt_Point = Axis_Rotate_Vector(m_vCamera_LookAt_Point, m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_RIGHT), CharacterPos, fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor);
			m_vLookPoint_To_Position_Dir = Axis_Rotate_Vector(m_vLookPoint_To_Position_Dir, m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_RIGHT), CharacterPos, fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor);
			m_pTransformCom_Camera->Look_At(m_vCamera_LookAt_Point);
		}
	}


	_vector vCamPos = XMLoadFloat4(&m_vCameraPosition);
	_vector vLookAtPos = XMLoadFloat4(&m_vCamera_LookAt_Point);
	_vector vDelta = XMVector4Normalize(vLookAtPos - vCamPos);
	XMStoreFloat4(&m_vRayDir, vDelta);

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
	_vector vCameraPosition;
	_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	vLookAtPoint = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist);
	XMStoreFloat4(&m_vCamera_LookAt_Point, vLookAtPoint);

	m_pCamera->SetPlayer(this);

	//m_pCamera->Create_Controller(m_vCameraPosition);
	m_pCamera->SetPosition(m_vCameraPosition);
	m_pCamera->LookAt(m_vCamera_LookAt_Point);

	m_pTransformCom_Camera = m_pCamera->Get_Transform();

	m_vLookPoint_To_Position_Dir = Sub_Float4(m_vCamera_LookAt_Point, m_vCameraPosition);

	_vector vCharPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	vCharPos = XMVectorSetY(vCharPos, XMVectorGetY(vCharPos) + 1.f);
	_vector vCameraPos = XMLoadFloat4(&m_vCameraPosition);

	m_fRayDist = XMVectorGetX(XMVector4Length(vCharPos - vCameraPos));

	return S_OK;
}

void CPlayer::Load_CameraPosition()
{
	string filePath = "../Camera_Position/Camera_Position";

	//File Import
	ifstream File(filePath, std::ios::binary | std::ios::in);

	File.read((char*)&m_vCameraPosition, sizeof(_float4));
	File.read((char*)&m_fRight_Dist, sizeof(_float));
	File.read((char*)&m_fUp_Dist, sizeof(_float));
	File.read((char*)&m_fLook_Dist, sizeof(_float));

	File.close();
}

void CPlayer::RayCasting_Camera()
{
	_float4 BlockPoint;
	_float4 TempPos;
	TempPos.x = m_vCameraPosition.x + (m_vRayDir.x * m_fRayDist);
	TempPos.y = m_vCameraPosition.y + (m_vRayDir.y * m_fRayDist);
	TempPos.z = m_vCameraPosition.z + (m_vRayDir.z * m_fRayDist);
	TempPos.w = 1.f;

	m_vRayDir.x *= -1.f;
	m_vRayDir.y *= -1.f;
	m_vRayDir.z *= -1.f;

	_float4 vRightDir = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_RIGHT);
	_float4 vLookDir = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_LOOK);
	vLookDir.y = 0.f;
	_float4 vBackDir = -vLookDir;
	_float4 vRightBack = vBackDir + vRightDir;
	vRightBack = Float4_Normalize(vRightBack);

	if (m_pGameInstance->SphereCast(TempPos, m_vRayDir, &BlockPoint, m_fRayDist))
	{
		m_fLerpTime = 0.f;
		m_bLerp = true;

		m_vRayDir.x *= -1.f;
		m_vRayDir.y *= -1.f;
		m_vRayDir.z *= -1.f;

		auto Position = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
		Position.y = m_vCameraPosition.y;
		_float4 DeltaDir = Float4_Normalize(Position - BlockPoint);
		BlockPoint.x += DeltaDir.x * 0.33f;
		BlockPoint.y += DeltaDir.y * 0.33f;
		BlockPoint.z += DeltaDir.z * 0.33f;

		m_pCamera->SetPosition(BlockPoint);
	}
	/*else if (m_pGameInstance->RayCast(m_vCameraPosition, vRightDir, &BlockPoint, 0.3f))
	{
		m_fLerpTime = 0.f;
		m_bLerp = true;

		BlockPoint.x -= vRightDir.x * 0.3f;
		BlockPoint.y -= vRightDir.y * 0.3f;
		BlockPoint.z -= vRightDir.z * 0.3f;
		m_pCamera->SetPosition(BlockPoint);

	}
	else if (m_pGameInstance->RayCast(m_vCameraPosition, vRightBack, &BlockPoint, 0.3f))
	{
		m_fLerpTime = 0.f;
		m_bLerp = true;

		BlockPoint.x -= vRightBack.x * 0.3f;
		BlockPoint.y -= vRightBack.y * 0.3f;
		BlockPoint.z -= vRightBack.z * 0.3f;
		m_pCamera->SetPosition(BlockPoint);
	}
	else if (m_pGameInstance->RayCast(m_vCameraPosition + vLookDir, vBackDir, &BlockPoint, 0.3f))
	{
		m_fLerpTime = 0.f;
		m_bLerp = true;

		BlockPoint.x -= vBackDir.x * 0.3f;
		BlockPoint.y -= vBackDir.y * 0.3f;
		BlockPoint.z -= vBackDir.z * 0.3f;
		m_pCamera->SetPosition(BlockPoint);
	}*/
}

HRESULT CPlayer::Add_Components()
{
	/* Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.vSize = _float3(0.8f, 1.2f, 0.8f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NavigationDesc{};

	NavigationDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
		return E_FAIL;

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

	Safe_Release(m_pColliderCom);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);
	m_PartObjects.clear();

	m_PartObjects.clear();
}
