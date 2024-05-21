#include "stdafx.h"
#include "..\Public\Player.h"


#include "Body_Player.h"
#include "Head_Player.h"
#include "Hair_Player.h"

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

	m_pTransformCom->Set_Scaled(0.045f, 0.045f, 0.045f);
	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	Priority_Tick_PartObjects(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pGameInstance->GetPosition_Physics());

	if(PRESSING == m_pGameInstance->Get_KeyState('H'))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('K'))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('U'))
	{
		auto Look = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
		Look.x = -Look.x;
		Look.z = -Look.z;
		m_pGameInstance->Move_CCT(Look, fTimeDelta,0);
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('J'))
	{
		m_pGameInstance->Move_CCT(m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK), fTimeDelta,0);
		m_eState |= STATE_RUN;
		if (m_eState & STATE_IDLE)
			m_eState ^= STATE_IDLE;
	}
	else
	{
		m_eState |= STATE_IDLE;
		if(m_eState & STATE_RUN)
			m_eState ^= STATE_RUN;
	}

	_vector		vWorldPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);


	/*if (GetKeyState(VK_LBUTTON) & 0x8000)
	{
		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		_float2		vMousePos = _float2(static_cast<_float>(ptMouse.x), static_cast<_float>(ptMouse.y));

		vWorldPos = m_pGameInstance->Compute_WorldPos(vMousePos, TEXT("Target_FieldDepth"));
	}*/
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vWorldPos);


	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	Tick_PartObjects(fTimeDelta);
	
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	Late_Tick_PartObjects(fTimeDelta);

	_float4			vMovedDirection = { Convert_Float3_To_Float4_Dir(m_vRootTranslation) };
	
	m_pGameInstance->Move_CCT(vMovedDirection, fTimeDelta, 0);

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

HRESULT CPlayer::Add_Components()
{
	/* Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	/* 로컬상의 정보를 셋팅한다. */
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


	_float4x4* pNeck1CombinedMatrix = { const_cast<_float4x4*>(pHeadModel->Get_CombinedMatrix("neck_1")) };
	pHairModel->Set_Surbodinate("neck_1", true);
	pHairModel->Set_Parent_CombinedMatrix_Ptr("neck_1", pNeck1CombinedMatrix);

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
