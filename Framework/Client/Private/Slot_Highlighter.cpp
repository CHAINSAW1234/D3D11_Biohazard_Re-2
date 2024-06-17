#include "stdafx.h"

#include "Slot_Highlighter.h"

CSlot_Highlighter::CSlot_Highlighter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice , pContext }
{
}

CSlot_Highlighter::CSlot_Highlighter(const CSlot_Highlighter& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CSlot_Highlighter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSlot_Highlighter::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	return S_OK;
}

void CSlot_Highlighter::FirstTick_Seting()
{
	if(false == m_IsChild)
	{
		m_pCursorTranform = dynamic_cast<CTransform*>(m_vecChildUI[0]->Get_Component(g_strTransformTag));
		Safe_AddRef(m_pCursorTranform);
		m_vOriginDiff = GetPositionVector() - m_vecChildUI[0]->GetPositionVector();
	}
}

void CSlot_Highlighter::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Tick(fTimeDelta);

	_vector vTargetPos = GetPositionVector() - m_vOriginDiff;

	if(false == m_IsChild)
		m_pCursorTranform->Move_toTargetUI(vTargetPos, 10.f, 5.f);
}

void CSlot_Highlighter::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CSlot_Highlighter::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CSlot_Highlighter::ResetPosition(_float4 fResetPos)
{
	if (false == m_IsChild)
		m_pCursorTranform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&fResetPos) - m_vOriginDiff);

	m_pTransformCom ->Set_State(CTransform::STATE_POSITION, fResetPos);
}

CSlot_Highlighter* CSlot_Highlighter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSlot_Highlighter* pInstance = new CSlot_Highlighter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSlot_Highlighter"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSlot_Highlighter::Clone(void* pArg)
{
	CSlot_Highlighter* pInstance = new CSlot_Highlighter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CSlot_Highlighter"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSlot_Highlighter::Free()
{
	Safe_Release(m_pCursorTranform);
	__super::Free();
}
