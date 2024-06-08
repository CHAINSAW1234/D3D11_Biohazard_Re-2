#include "stdafx.h"
#include "..\Public\Player.h"


#include "Body_Player.h"
#include "Head_Player.h"
#include "Hair_Player.h"

#include"CustomCollider.h"

#include "Character_Controller.h"

#define MODEL_SCALE 0.01f

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject{ rhs }
{

}

HRESULT CPlayer::Initialize_Prototype()
{
	

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(0.f, 0.f, 0.f,1.f));
	m_pTransformCom->Set_Scaled(MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);

	m_pController = m_pGameInstance->Create_Controller(_float4(0.f, 0.f, 0.f, 1.f),&m_iIndex_CCT,this);
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
		m_pGameInstance->Set_RenderFieldShadow(true);
	}
#pragma endregion 예은ColTest

	
	
	static _bool Temp = false;

	if (UP == m_pGameInstance->Get_KeyState(VK_BACK))
	{
		Temp = true;
	}

	if(Temp == false)
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

	if (PRESSING == m_pGameInstance->Get_KeyState('U'))
	{
		_vector      vLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);
		vLook = { XMVectorScale(XMVector3Normalize(vLook),0.01f) };
		_vector      vMoveDir = { vLook };

		vMovedDirection += vMoveDir;
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('J'))
	{
		_vector      vLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);
		vLook = { XMVectorScale(XMVector3Normalize(vLook),0.01f) };
		_vector      vMoveDir = { -vLook };

		vMovedDirection += vMoveDir;
	}

	

#pragma region Terrain
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////TEST/////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	/*_vector		vMoveDir = { XMVectorSet(0.f, 0.f, 0.f, 0.f) };
	CVIBuffer_Terrain*		pTerrainBuffer = { dynamic_cast<CVIBuffer_Terrain*>(const_cast<CComponent*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"), 0))) };
	CTransform*				pTerrainTransform = { dynamic_cast<CTransform*>(const_cast<CComponent*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform"), 0))) };
	if (nullptr != pTerrainBuffer &&
		nullptr != pTerrainTransform)
	{
		_vector		vPosition = { m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) };
		_float4		vPickPosFloat4;
		pTerrainBuffer->Compute_Height(pTerrainTransform, vPosition, &vPickPosFloat4);

		_vector		vResultPos = { XMLoadFloat4(&vPickPosFloat4) };
		vMoveDir = vResultPos - vPosition;

		vMovedDirection += vMoveDir;
	}*/
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	//	m_pGameInstance->Move_CCT(vMoveDir, fTimeDelta, 0);
	_float4			vResultMoveDirFloat4 = {};
	XMStoreFloat4(&vResultMoveDirFloat4, vMovedDirection);
	m_pController->Move(vResultMoveDirFloat4, fTimeDelta);

#pragma endregion


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
	CPartObject*				pBodyObject = { nullptr };
	CBody_Player::BODY_DESC		BodyDesc{};

	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.pRootTranslation = &m_vRootTranslation;

	pBodyObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Body_Player"), &BodyDesc));
	if (nullptr == pBodyObject)
		return E_FAIL;

	m_PartObjects[CPlayer::PART_BODY] = pBodyObject;

	/* For.Part_Head */
	CPartObject*				pHeadObject = { nullptr };
	CHead_Player::HEAD_DESC		HeadDesc{};

	HeadDesc.pParentsTransform = m_pTransformCom;
	HeadDesc.pState = &m_eState;

	pHeadObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Head_Player"), &HeadDesc));
	if (nullptr == pHeadObject)
		return E_FAIL;

	m_PartObjects[CPlayer::PART_HEAD] = pHeadObject;

	/* For.Part_Hair */
	CPartObject*				pHairObject = { nullptr };
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
	CModel*			pBodyModel = { dynamic_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Model")))};
	CModel*			pHeadModel = { dynamic_cast<CModel*>(m_PartObjects[PART_HEAD]->Get_Component(TEXT("Com_Model"))) };
	CModel*			pHairModel = { dynamic_cast<CModel*>(m_PartObjects[PART_HAIR]->Get_Component(TEXT("Com_Model"))) };

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

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

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
