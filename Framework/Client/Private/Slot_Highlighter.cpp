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

void CSlot_Highlighter::Start()
{
	if (false == m_IsChild)
	{
		static_cast<CSlot_Highlighter*>(m_vecChildUI[CURSOR_SH])->Set_SH_Role(CURSOR_SH);
		static_cast<CSlot_Highlighter*>(m_vecChildUI[GLITTER_SH])->Set_SH_Role(GLITTER_SH);

		m_pCursorTranform = static_cast<CTransform*>(m_vecChildUI[CURSOR_SH]->Get_Component(g_strTransformTag));
		Safe_AddRef(m_pCursorTranform);
		m_vOriginDiff = GetPositionVector() - m_vecChildUI[CURSOR_SH]->GetPositionVector();

		m_vecChildUI[GLITTER_SH]->Set_Dead(true);
	}
}

void CSlot_Highlighter::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Tick(fTimeDelta);

	_vector vTargetPos = GetPositionVector() - m_vOriginDiff;

	if (false == m_IsChild)
	{
		m_pCursorTranform->Move_toTargetUI(vTargetPos, 10.f, 5.f);
	}
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

HRESULT CSlot_Highlighter::Change_Tool()
{
	return S_OK;
}

void CSlot_Highlighter::Set_Dead(_bool bDead)
{
	m_bDead = bDead;

	for (auto& iter : m_vecChildUI)
	{
		if (true == m_isDragShadow)
		{
			switch (static_cast<CSlot_Highlighter*>(iter)->Get_SH_Role())
			{
			case CURSOR_SH: {
				iter->Set_Dead(bDead);
				break;
			}

			case GLITTER_SH: {
				iter->Set_Dead(bDead);
				break;
			}

			default:
				break;
			}
		}

		else
		{
			switch (static_cast<CSlot_Highlighter*>(iter)->Get_SH_Role())
			{
			case CURSOR_SH: {
				iter->Set_Dead(bDead);
				break;
			}

			case GLITTER_SH: {
				iter->Set_Dead(true);
				break;
			}


			default:
				break;
			}
		}

	}
}

void CSlot_Highlighter::ResetPosition(_float4 fResetPos)
{
	if (false == m_IsChild)
		m_pCursorTranform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&fResetPos) - m_vOriginDiff);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, fResetPos);
}

void CSlot_Highlighter::Set_DragShadow(_bool IsDragShadow)
{
	if (true == m_IsChild)
		return;

	m_isDragShadow = IsDragShadow;

	if (true == m_isDragShadow)
	{
		m_vecChildUI[GLITTER_SH]->Set_Dead(false);
		CTransform* pTransform = static_cast<CTransform*>(m_vecChildUI[GLITTER_SH]->Get_Component(g_strTransformTag));
		pTransform->Set_State(CTransform::STATE_POSITION, GetPositionVector());
		Set_Value_Color(&m_vColor[1]);
	}

	else
	{
		m_vecChildUI[GLITTER_SH]->Set_Dead(true);
		Set_Value_Color(&m_vColor[0]);
	}
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
