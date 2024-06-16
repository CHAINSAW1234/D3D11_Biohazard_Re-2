#include "stdafx.h"

#include "InventorySelect.h"

CInventorySelect::CInventorySelect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice , pContext }
{
}

CInventorySelect::CInventorySelect(const CInventorySelect& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CInventorySelect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInventorySelect::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	return S_OK;
}

void CInventorySelect::FirstTick_Seting()
{
	if(false == m_IsChild)
	{
		m_pCursorTranform = dynamic_cast<CTransform*>(m_vecChildUI[0]->Get_Component(g_strTransformTag));
		Safe_AddRef(m_pCursorTranform);
		m_vOriginDiff = GetPositionVector() - m_vecChildUI[0]->GetPositionVector();
	}
}

void CInventorySelect::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Tick(fTimeDelta);

	_vector vTargetPos = GetPositionVector() - m_vOriginDiff;

	if(false == m_IsChild)
		m_pCursorTranform->Move_toTargetUI(vTargetPos, 10.f, 5.f);
}

void CInventorySelect::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CInventorySelect::Render()
{
	__super::Render();

	return S_OK;
}

void CInventorySelect::ResetPosition(_float4 fResetPos)
{
	if (false == m_IsChild)
		m_pCursorTranform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&fResetPos) - m_vOriginDiff);

	m_pTransformCom ->Set_State(CTransform::STATE_POSITION, fResetPos);
}

CInventorySelect* CInventorySelect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInventorySelect* pInstance = new CInventorySelect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CInventorySelect"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInventorySelect::Clone(void* pArg)
{
	CInventorySelect* pInstance = new CInventorySelect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CInventorySelect"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInventorySelect::Free()
{
	Safe_Release(m_pCursorTranform);
	__super::Free();
}
