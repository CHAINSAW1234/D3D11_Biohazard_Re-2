#include "stdafx.h"
#include "Collider.h"
#include "GameInstance.h"
#include "CustomCollider.h"

CCustomCollider::CCustomCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CGameObject{ pDevice, pContext }
{
}

CCustomCollider::CCustomCollider(const CCustomCollider& rhs) :
	CGameObject{ rhs }
{
}

HRESULT CCustomCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCustomCollider::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components((COLLIDER_DESC*)pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		COLLIDER_DESC* pColDesc = (COLLIDER_DESC*)pArg;
		m_pTransformCom->Set_WorldMatrix((pColDesc->worldMatrix));
		m_iColNum = pColDesc->iColNum;
		m_iDir =pColDesc->iDir;
		m_iRegionNum =pColDesc->iRegionNum;
		m_iFloor =pColDesc->iFloor;

	}

	return S_OK;
}

void CCustomCollider::Priority_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;
}

void CCustomCollider::Tick(_float fTimeDelta)
{

	if (m_bDead)
		return;
}

void CCustomCollider::Late_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;
#ifdef _DEBUG
#ifndef NO_COLLISTION
	m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif
#endif

}

HRESULT CCustomCollider::Render()
{
	return S_OK;
}

HRESULT CCustomCollider::Add_Components(COLLIDER_DESC* pCol)
{
	_vector vScaleVector;
	_vector vRotationQuat;
	_vector vTranslationVector;

	XMMatrixDecompose(&vScaleVector, &vRotationQuat, &vTranslationVector, pCol->worldMatrix);

	
	/* Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};
	ColliderDesc.bQuat = true;
	ColliderDesc.vCenter = vTranslationVector;
	ColliderDesc.vRotation = vRotationQuat;
	ColliderDesc.vSize = vScaleVector * _vector{ 1.7f,1.f,1.7f,1.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}


CCustomCollider* CCustomCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCustomCollider* pInstance = new CCustomCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCustomCollider"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCustomCollider::Clone(void* pArg)
{
	CCustomCollider* pInstance = new CCustomCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCustomCollider"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCustomCollider::Free()
{
	Safe_Release(m_pColliderCom);
	__super::Free();
}

