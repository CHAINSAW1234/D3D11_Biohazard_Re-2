#include "stdafx.h"
#include "..\Public\Player.h"

#include "FSM.h"
#include "Player_State_Move.h"
#include "Player_State_Hold.h"

#include "Body_Player.h"
#include "Head_Player.h"
#include "Hair_Player.h"
#include "Weapon.h"

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
	m_pController = m_pGameInstance->Create_Controller(_float4(0.f, 0.3f, 5.f, 1.f), &m_iIndex_CCT, this, 1.f, 0.45f, m_pTransformCom,
		m_pBodyModel->GetBoneVector(), "None");

	//For Camera.
	m_pTransformCom_Camera = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom_Camera)
		return E_FAIL;
	m_pTransformCom_Camera->SetRotationPerSec(0.75f);

	//For Camera.
	Load_CameraPosition();
	if (FAILED(Ready_Camera()))
		return E_FAIL;
	
	m_pGameInstance->SetPlayer(this);

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	Priority_Tick_PartObjects(fTimeDelta);

#pragma region 예은 스파이 나중에 FSM으로 옮길지도
	if (PRESSING == m_pGameInstance->Get_KeyState(VK_LBUTTON))
		m_bInteract = true;
	else
		m_bInteract = false;
#pragma endregion 

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

		if (PRESSING == m_pGameInstance->Get_KeyState('D'))
		{
			m_bMove_Right = true;
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
			if (m_bMove_Right && m_bCollision_Lerp == false)
			{
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}

			m_bMove_Right = false;
		}

		if (PRESSING == m_pGameInstance->Get_KeyState('A'))
		{
			m_bMove_Left = true;
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
			if (m_bMove_Left && m_bCollision_Lerp == false)
			{
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}

			m_bMove_Left = false;
		}

		_float4			vResultMoveDirFloat4 = {};
		XMStoreFloat4(&vResultMoveDirFloat4, vMovedDirection);
		m_pController->Move(vResultMoveDirFloat4, fTimeDelta);
	}

	if (m_bMove_Backward == false && m_bMove_Forward == false && m_bMove_Left == false && m_bMove_Right == false)
	{
		m_bLerp_Move = false;
		m_bMove = false;

		m_eMoveDir = MD_DEFAULT;
	}

	//For Camera.
	SetMoveDir();

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
		
		if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON))
		{
			RayCast_Shoot();
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

	CModel* pWeaponModel = { dynamic_cast<CModel*>(m_PartObjects[PART_WEAPON]->Get_Component(TEXT("Com_Model"))) };
	_float4x4* pRightWeaponCombinedMatrix = { const_cast<_float4x4*>(Get_Body_Model()->Get_CombinedMatrix("r_weapon")) };
	//pWeaponModel->Set_Surbodinate("root", true);
	//pWeaponModel->Set_Parent_CombinedMatrix_Ptr("root", pRightWeaponCombinedMatrix);
	CWeapon* pWeapon = dynamic_cast<CWeapon*>(m_PartObjects[PART_WEAPON]);
	pWeapon->Set_Socket(const_cast<_float4x4*>(Get_Body_Model()->Get_CombinedMatrix("r_weapon")));

	Update_Direction();
	Update_FSM();
	m_pFSMCom->Update(fTimeDelta);
#pragma endregion

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	Tick_PartObjects(fTimeDelta);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	Late_Tick_PartObjects(fTimeDelta);

	if (m_pController)
		m_pController->Update_Collider();

	Turn_Spine(fTimeDelta);		// 특정 조건에서 뼈를 돌림

#pragma region 예은 추가
	Col_Section();
#pragma endregion 

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif

#pragma region Camera Reset
	if (UP == m_pGameInstance->Get_KeyState('C'))
	{
		ResetCamera();
	}
#pragma endregion
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
	list<CGameObject*>* pCollider = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Collider"));
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
	}
}

#pragma endregion

#pragma region 현진 추가
CModel* CPlayer::Get_Body_Model()
{
	return static_cast<CModel*>(m_PartObjects[0]->Get_Component(g_strModelTag));
}

_float3* CPlayer::Get_Body_RootDir()
{
	return static_cast<CBody_Player*>(m_PartObjects[0])->Get_RootTranslation();
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

void CPlayer::Update_Direction()
{
	DWORD dwDirection = 0;

	if (m_pGameInstance->Get_KeyState('W') == PRESSING &&
		m_pGameInstance->Get_KeyState('S') == PRESSING)
		;
	else if (m_pGameInstance->Get_KeyState('W') == PRESSING)
		dwDirection |= DIRECTION_FRONT;
	else if (m_pGameInstance->Get_KeyState('S') == PRESSING)
		dwDirection |= DIRECTION_BACK;

	if (m_pGameInstance->Get_KeyState('A') == PRESSING &&
		m_pGameInstance->Get_KeyState('D') == PRESSING)
		;
	else if (m_pGameInstance->Get_KeyState('A') == PRESSING)
		dwDirection |= DIRECTION_LEFT;
	else if (m_pGameInstance->Get_KeyState('D') == PRESSING)
		dwDirection |= DIRECTION_RIGHT;

	m_dwDirection = dwDirection;
}

_float CPlayer::Get_CamDegree()
{
	if (nullptr == m_pCamera || nullptr == m_pTransformCom)
		return 0;

	_float4 vCamLook = m_pCamera->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK);
	vCamLook.y = 0;
	vCamLook = XMVector3Normalize(vCamLook);

	_float4 vPlayerLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
	vPlayerLook.y = 0;
	vPlayerLook = XMVector3Normalize(vPlayerLook);

	return Cal_Degree_From_Directions_Between_Min180_To_180(vPlayerLook, vCamLook);
}

void CPlayer::Turn_Spine(_float fTimeDelta)
{
	static _float		fAccLinearTime = { 0.f };
	//if (!m_isTurnSpine /*&& m_fSpineTurnLerfTimeDelta >= 1.f*/) {
	//	fAccLinearTime -= fTimeDelta;
	//	if (fAccLinearTime < 0.f)
	//		fAccLinearTime = 0.f;
	//}

	//else
	//{
	//	fAccLinearTime += fTimeDelta;
	//	if (fAccLinearTime > 0.5f)
	//	{
	//		fAccLinearTime = 0.5f;
	//	}
	//}

	fAccLinearTime = 0.5f;

	CModel*			pModel = { Get_Body_Model() };
	if (nullptr != pModel)
	{
		_matrix				HeadCombinedMatrix = { XMLoadFloat4x4(pModel->Get_CombinedMatrix("head")) };
		_matrix				CogCombinedMatrix = { XMLoadFloat4x4(pModel->Get_CombinedMatrix("COG")) };

		_vector				vScaleHead, vQuaternionHead, vTranslationHead;
		_vector				vScaleCog, vQuaternionCog, vTranslationCog;

		XMMatrixDecompose(&vScaleHead, &vQuaternionHead, &vTranslationHead, HeadCombinedMatrix);
		XMMatrixDecompose(&vScaleCog, &vQuaternionCog, &vTranslationCog, HeadCombinedMatrix);

		_vector				vCogQuaternionInv = { XMQuaternionInverse(vQuaternionCog) };

		_vector				vTotalQuaternionHead = { XMQuaternionMultiply(XMQuaternionNormalize(vCogQuaternionInv), XMQuaternionNormalize(vQuaternionHead)) };
		_matrix				HeadRotateMatrix = { XMMatrixRotationQuaternion(vTotalQuaternionHead) };

		_vector				vMyLook = { m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK) };
		_vector				vHeadWorldLook = { XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), /*HeadRotateMatrix * */m_pTransformCom->Get_WorldMatrix())};

		//	vHeadWorldLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);

		_matrix				CamWorldMatrix = { m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW) };
		_vector				vCamLook = { CamWorldMatrix.r[CTransform::STATE_LOOK]};/*
		_vector				vCamPosition = { CamWorldMatrix.r[CTransform::STATE_POSITION] };
		_vector				vCamFocusPosition = { vCamPosition + XMVector3Normalize(vCamLook) * 7.f };*/

		//_vector				vMyPosition = { m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) };
		//_vector				vDirectionToFocus = { vCamFocusPosition - vMyPosition };
		//		_vector				vCamLook = { m_pCamera->Get_Transform()->Get_State_Vector(CTransform::STATE_LOOK) };

		_float				fDot = { XMVectorGetX(XMVector3Dot(XMVector3Normalize(vHeadWorldLook), XMVector3Normalize(vCamLook))) };
		//	_float				fDot = { XMVectorGetX(XMVector3Dot(XMVector3Normalize(vHeadWorldLook), XMVector3Normalize(vCamLook))) };
		if (fabsf(fDot) > 0.9999f)
		{
			return;
		}

		_vector				vRotateAxis = { XMVector3Cross(XMVector3Normalize(vHeadWorldLook), XMVector3Normalize(vCamLook)) };
		_float				fAngle = { acosf(fDot) * fAccLinearTime / 0.5f };

		list<_uint>			ChildJointIndices; 
		pModel->Get_Child_ZointIndices("spine_0", "head", ChildJointIndices);

		_uint				iNumChildJoint = { static_cast<_uint>(ChildJointIndices.size()) };

		_float				fDevidedAngle = { fAngle / iNumChildJoint };

		vector<string>		BoneNames = { pModel->Get_BoneNames() };

		vRotateAxis = XMVector3TransformNormal(vRotateAxis, m_pTransformCom->Get_WorldMatrix_Inverse());

		_vector				vQuaternion = { XMQuaternionRotationAxis(vRotateAxis, fDevidedAngle) };
		_matrix				RotationMatrix = { XMMatrixRotationQuaternion(vQuaternion) };

		ChildJointIndices.pop_back();

		for(auto& iJointIndex : ChildJointIndices)
		{
			pModel->Add_Additional_Transformation_World(BoneNames[iJointIndex], RotationMatrix);
		}
	}



	//// 카메라 look 내적을 통해서 각도 구하기
	//_float4 vCamLook = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_LOOK);
	//_float4 vCamLandLook = vCamLook;
	//vCamLandLook.y = 0;
	//vCamLandLook = XMVector3Normalize(vCamLandLook);

	//_float fRadian = XMVector3AngleBetweenNormals(vCamLandLook, vCamLook).m128_f32[0];

	//// 외적으로 방향 찾기
	//_float vCross = XMVector3Cross(vCamLandLook, vCamLook).m128_f32[1];

	//// 각도를 -180 ~ 180도 사이로 세팅
	//_float fDegree = XMConvertToDegrees(fRadian * vCross / abs(vCross)); // 0 ~ 180 사이의 값 
	//// 각도가 0보다 크면 왼쪽이다
	//if (isnan(fDegree))
	//	m_fTargetSpineTurnAxis = 0.f;
	//else 
	//	m_fTargetSpineTurnAxis = fDegree;


	//m_fSpineTurnLerfTimeDelta += fTimeDelta;
	//if (m_fSpineTurnLerfTimeDelta > 1.f)
	//	m_fSpineTurnLerfTimeDelta = 1.f;

	//if (m_fSpineTurnLerfTimeDelta < 1.f) {
	//	if (m_isTurnSpine) {
	//		m_fCurSpineTurnAxis = Lerp(0, m_fTargetSpineTurnAxis, m_fSpineTurnLerfTimeDelta);
	//	}
	//	else {
	//		m_fCurSpineTurnAxis = Lerp(m_fTargetSpineTurnAxis, 0, m_fSpineTurnLerfTimeDelta);
	//	}
	//}
	//else {
	//	m_fCurSpineTurnAxis = m_isTurnSpine ? m_fTargetSpineTurnAxis : 0.f;
	//}

	////_matrix vSpineTransform = Get_Body_Model()->GetBoneTransform("spine_0");
	////_vector vTransform, vScale, vRotate;
	////XMMatrixDecompose(&vScale, &vRotate, &vTransform, vSpineTransform);

	////_float4 vSpineRight = vSpineTransform.r[0];
	////_float4 vSpinePos = vSpineTransform.r[3];

	////// 크 자 이 공 부
	////_matrix vNewSpineTransform = XMMatrixIdentity() * XMMatrixScalingFromVector(vScale) * XMMatrixRotationQuaternion(vRotate) *
	////	XMMatrixRotationAxis(m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT), XMConvertToRadians(m_fCurSpineTurnAxis)) * XMMatrixTranslationFromVector(vTransform);

	////_float3 pRootDir = { 0.f,0.f,0.f };
	////Get_Body_Model()->Set_CombinedMatrix(m_pTransformCom, Get_Body_RootDir(), "spine_0", vNewSpineTransform);
	//Get_Body_Model()->Add_Additional_Transformation_World("spine_0", XMMatrixRotationAxis(m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT), XMConvertToRadians(m_fCurSpineTurnAxis)));
	////Get_Body_Model()->Set_CombinedMatrix(m_pTransformCom, /*Get_Body_RootDir()*/ &pRootDir, "spine_0", vSpineTransform);
	

}

void CPlayer::SetMoveDir()
{
	//F
	if (m_bMove_Forward == true && m_bMove_Backward == false && m_bMove_Right == false && m_bMove_Left == false)
	{
		m_eMoveDir = MD_F;
	}

	//B
	if (m_bMove_Forward == false && m_bMove_Backward == true && m_bMove_Right == false && m_bMove_Left == false)
	{
		m_eMoveDir = MD_B;
	}

	//R
	if (m_bMove_Forward == false && m_bMove_Backward == false && m_bMove_Right == true && m_bMove_Left == false)
	{
		m_eMoveDir = MD_R;
	}

	//L
	if (m_bMove_Forward == false && m_bMove_Backward == false && m_bMove_Right == false && m_bMove_Left == true)
	{
		m_eMoveDir = MD_L;
	}

	//FR
	if (m_bMove_Forward == true && m_bMove_Backward == false && m_bMove_Right == true && m_bMove_Left == false)
	{
		m_eMoveDir = MD_FR;
	}

	//FL
	if (m_bMove_Forward == true && m_bMove_Backward == false && m_bMove_Right == false && m_bMove_Left == true)
	{
		m_eMoveDir = MD_FL;
	}

	//BR
	if (m_bMove_Forward == false && m_bMove_Backward == true && m_bMove_Right == true && m_bMove_Left == false)
	{
		m_eMoveDir = MD_BR;
	}

	//BL
	if (m_bMove_Forward == false && m_bMove_Backward == true && m_bMove_Right == false && m_bMove_Left == true)
	{
		m_eMoveDir = MD_BL;
	}

	if (m_eMoveDir != m_ePrevMoveDir)
	{
		m_fLerpTime = 0.f;
		m_bLerp_Move = true;
		m_bLerp = true;
	}

	m_ePrevMoveDir = m_eMoveDir;
}

void CPlayer::ResetCamera()
{
	auto Char_RotMat = m_pTransformCom->Get_RotationMatrix_Pure();
	m_pTransformCom_Camera->Set_RotationMatrix_Pure(Char_RotMat);

	m_fRight_Dist_Look = m_fRight_Dist_Look_Default;
	m_fUp_Dist_Look = m_fRight_Dist_Look_Default;
	m_fLook_Dist_Look = m_fLook_Dist_Look_Default;

	m_fLerpAmount_Right = m_fRight_Dist_Look;
	m_fLerpAmount_Up = m_fUp_Dist_Look;
	m_fLerpAmount_Look = m_fLook_Dist_Look;
}

void CPlayer::RayCast_Shoot()
{
	_float4 vBlockPoint;
	if (m_pGameInstance->RayCast_Shoot(m_pCamera->GetPosition(), m_pCamera->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK), &vBlockPoint))
	{
		int a = 0;
	}
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

	switch (m_eMoveDir)
	{
	case MD_F:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.85f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 1.15f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 1.15f);
		}
		break;
	case MD_B:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.4f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.4f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.9f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.9f);
		}
		break;
	case MD_R:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.5f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 0.55f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 0.55f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.5f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 0.55f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 0.55f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos);
		}
		break;
	case MD_L:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.5f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 1.45f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 1.45f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.5f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 1.45f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 1.45f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos);
		}
		break;
	case MD_FR:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 0.55f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 0.55f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.85f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 1.15f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 0.55f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 0.55f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 1.15f);
		}
		break;
	case MD_FL:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 1.45f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 1.45f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.85f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 1.15f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 1.45f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 1.45f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 1.15f);
		}
		break;
	case MD_BR:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.4f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 0.55f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 0.55f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.4f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 0.55f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 0.55f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.85f);
		}
		break;
	case MD_BL:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.4f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 1.45f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 1.45f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.4f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 1.45f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 1.45f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.85f);
		}
		break;
	case MD_DEFAULT:
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
		break;
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

				if (abs(GetAngleBetweenVectors_Radians(Camera_Look, Camera_Parallel_Look)) > 1.f)
				{
					m_pTransformCom_Camera->Turn(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT), -m_fRotate_Amount_Y);
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
	if (m_pCamera == nullptr)
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

	m_fRight_Dist_Look_Default = m_fRight_Dist_Look;
	m_fUp_Dist_Look_Default = m_fUp_Dist_Look;
	m_fLook_Dist_Look_Default = m_fLook_Dist_Look;

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

	Get_Body_Model()->Change_Animation(0, CPlayer::ANIM_IDLE);
	Get_Body_Model()->Set_Loop(0, true);
	Get_Body_Model()->Set_BlendWeight(0, 1.f);
	Get_Body_Model()->Set_BlendWeight(1, 0.f);

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

	///* For.Part_Weapon */
	CPartObject* pWeaponObject = { nullptr };
	CPartObject::PARTOBJECT_DESC		WeaponDesc{};

	WeaponDesc.pParentsTransform = m_pTransformCom;

	pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_HandGun"), &WeaponDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;

	m_PartObjects[CPlayer::PART_WEAPON] = pWeaponObject;

	return S_OK;
}

HRESULT CPlayer::Initialize_PartModels()
{
	CModel* pBodyModel = { dynamic_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Model"))) };
	CModel* pHeadModel = { dynamic_cast<CModel*>(m_PartObjects[PART_HEAD]->Get_Component(TEXT("Com_Model"))) };
	CModel* pHairModel = { dynamic_cast<CModel*>(m_PartObjects[PART_HAIR]->Get_Component(TEXT("Com_Model"))) };


	m_pBodyModel = pBodyModel;

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
