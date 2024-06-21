#include "stdafx.h"
#include "Body_Door.h"
#include "Player.h"
#include "Bone.h"
#include "PxCollider.h"

#include"Door.h"
CBody_Door::CBody_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CBody_Door::CBody_Door(const CBody_Door& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CBody_Door::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CBody_Door::Initialize(void* pArg)
{
	/*문자식 파트오브젝트 붙혀야하는데 뼈가 문고리에 없어서 직접 찍어야 하는데
	프로토타입 끝나고 뼈 붙혀보겠나이다*/

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(-1, false, 0, TEXT("Default"), 1.f);
	m_pModelCom->Set_TotalLinearInterpolation(0.2f);


	m_pModelCom->Active_RootMotion_Rotation(true);

#ifndef NON_COLLISION_PROP

	m_pPx_Collider = m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pTransformCom, &m_iPx_Collider_Id);

#endif

	switch (*m_pState)
	{
	case CDoor::DOOR_DOUBLE:
		m_vecRotationBone[ATC_ROOT] = m_pModelCom->Get_BonePtr("_00");
		m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_L] = m_pModelCom->Get_BonePtr("_01");
		m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R] = m_pModelCom->Get_BonePtr("_03");
		m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L] = m_pModelCom->Get_BonePtr("_04");
		m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R] = m_pModelCom->Get_BonePtr("_02");
		break;
	case CDoor::DOOR_ONE:
		m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_L] = m_pModelCom->Get_BonePtr("_01");
		m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_R] = m_pModelCom->Get_BonePtr("_00");
		break;
	}

	return S_OK;
}

HRESULT CBody_Door::Add_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, m_strModelComponentName,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(80.f);
	ColliderDesc.vCenter = _float3(-10.f, 1.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;
	if (*m_pState == CDoor::DOOR_DOUBLE)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc1{};

		ColliderDesc1.fRadius = _float(80.f);
		ColliderDesc1.vCenter = _float3(-180.f, 1.f, 0.f);
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Double"), (CComponent**)&m_pColDoubledoorCom, &ColliderDesc1)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBody_Door::Add_PartObjects()
{

	return S_OK;
}

HRESULT CBody_Door::Initialize_PartObjects()
{

	return S_OK;
}

void CBody_Door::DoubleDoor_Tick(_float fTimeDelta)
{
}

void CBody_Door::DoubleDoor_Late_Tick(_float fTimeDelta)
{
	switch (m_eDoubleState)
	{
	case  CDoor::LSIDE_DOUBLEDOOR_OPEN_L:
		//m_pModelCom->Set_TotalLinearInterpolation(0.2f); // 잘알아갑니다 꺼억
		m_pModelCom->Change_Animation(0, m_eDoubleState);
		break;
	case  CDoor::LSIDE_DOUBLEDOOR_OPEN_R:
		m_pModelCom->Change_Animation(0, m_eDoubleState);
		break;
	case  CDoor::RSIDE_DOUBLEDOOR_OPEN_L:
		m_pModelCom->Change_Animation(0, m_eDoubleState);
		break;
	case  CDoor::RSIDE_DOUBLEDOOR_OPEN_R:
		m_pModelCom->Change_Animation(0, m_eDoubleState);
		break;
	case  CDoor::DOUBLEDOOR_STATIC:
		m_pModelCom->Change_Animation(0, m_eDoubleState);
		break;
	
	}
	_float4 fTransform4 = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animations(m_pTransformCom, fTimeDelta, &fTransform3);

	Check_Col_Sphere_Player(); // 여긴 m_bCol 을 true로만 바꿔주기 때문에 반드시 false를 해주는 부분이 있어야함
	
	CCollider* pPlayerCol = static_cast<CCollider*>(m_pPlayer->Get_Component(TEXT("Com_Collider")));
	if (pPlayerCol->Intersect(m_pColDoubledoorCom))
		m_bDoubleCol = true;
	



	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[INTERACTPROPS_COL_SPHERE]);

	m_pGameInstance->Add_DebugComponents(m_pColDoubledoorCom);
	
#endif
}

void CBody_Door::DoubleDoor_Active()
{

	*m_pPlayerInteract = false;
	m_bActive = true;
	_vector vLook = XMVector4Normalize(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
	_vector vDir = XMVector4Normalize(m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	_float fScala = XMVectorGetX(XMVector4Dot(vLook, vDir));
	if (fScala > 1.f)
		fScala = 1.f;
	else if (fScala < -1.f)
		fScala = -1.f;

	if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
	{
		if (m_bCol && m_bDoubleCol)
			m_eDoubleState = LSIDE_DOUBLEDOOR_OPEN;
		else if (m_bCol)
			m_eDoubleState = LSIDE_DOUBLEDOOR_OPEN_L;
		else
			m_eDoubleState = RSIDE_DOUBLEDOOR_OPEN_R;

	}
	else
	{
		if (m_bCol && m_bDoubleCol)
			m_eDoubleState = RSIDE_DOUBLEDOOR_OPEN;
		else if (m_bCol)
			m_eDoubleState = LSIDE_DOUBLEDOOR_OPEN_R;
		else
			m_eDoubleState = RSIDE_DOUBLEDOOR_OPEN_L;
	}
	

}

void CBody_Door::OneDoor_Tick(_float fTimeDelta)
{

	if (m_bActive)
		m_fTime += fTimeDelta;

	if (m_fTime > 4.f)
	{
		m_fTime = 0.f;
		m_bActive = false;
		m_eOneState = ONEDOOR_STATIC;
	}

	if (m_bCol && !m_bActive)
	{
		//UI띄우고
		if (*m_pPlayerInteract)
			OneDoor_Active();
		m_bCol = false;
	}
	m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());

}

void CBody_Door::OneDoor_Late_Tick(_float fTimeDelta)
{
	Check_Col_Sphere_Player(); // 여긴 m_bCol 을 true로만 바꿔주기 때문에 반드시 false를 해주는 부분이 있어야함

	switch (m_eOneState)
	{
	case ONEDOOR_OPEN_L:
		//m_pModelCom->Set_TotalLinearInterpolation(0.2f); // 잘알아갑니다 꺼억
		m_pModelCom->Change_Animation(0, m_eOneState);
		break;
	case ONEDOOR_OPEN_R:
		m_pModelCom->Change_Animation(0, m_eOneState);
		break;
	case ONEDOOR_STATIC:
		m_pModelCom->Change_Animation(0, m_eOneState);
		break;
	}

	_float4 fTransform4 = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animations(m_pTransformCom, fTimeDelta, &fTransform3);




	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[INTERACTPROPS_COL_SPHERE]);
#endif
}

void CBody_Door::OneDoor_Active()
{
	*m_pPlayerInteract = false;
	m_bActive = true;
	_vector vLook = XMVector4Normalize(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
	_vector vDir = XMVector4Normalize(m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	_float fScala = XMVectorGetX(XMVector4Dot(vLook, vDir));
	if (fScala > 1.f)
		fScala = 1.f;
	else if (fScala < -1.f)
		fScala = -1.f;

	if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
		m_eOneState = ONEDOOR_OPEN_L;
	else
		m_eOneState = ONEDOOR_OPEN_R;




}


CBody_Door* CBody_Door::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Door* pInstance = new CBody_Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Door"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CBody_Door::Clone(void* pArg)
{
	CBody_Door* pInstance = new CBody_Door(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Door"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Door::Free()
{
	__super::Free();
	if(m_eType ==  DOOR_DOUBLE)
		Safe_Release(m_pColDoubledoorCom);
}
