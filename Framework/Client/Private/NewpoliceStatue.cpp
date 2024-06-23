#include "stdafx.h"
#include "NewpoliceStatue.h"

#include"Body_NewpoliceStatue.h"
CNewpoliceStatue::CNewpoliceStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CNewpoliceStatue::CNewpoliceStatue(const CNewpoliceStatue& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CNewpoliceStatue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNewpoliceStatue::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CNewpoliceStatue::Tick(_float fTimeDelta)
{
	__super::Check_Player();
	if (!m_bVisible)
	{
		m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());

		return;
	}

	if (m_pGameInstance->Get_KeyState('U') == DOWN)
		m_eState = POLICEHALLSTATUE_0;
	if (m_pGameInstance->Get_KeyState('I') == DOWN)
		m_eState = POLICEHALLSTATUE_1;

	if (m_pGameInstance->Get_KeyState('O') == DOWN)
		m_eState = POLICEHALLSTATUE_3;
	if (m_pGameInstance->Get_KeyState('P') == DOWN)
		m_eState = POLICEHALLSTATUE_2;

	__super::Tick(fTimeDelta);
}

void CNewpoliceStatue::Late_Tick(_float fTimeDelta)
{
	if (!Visible())
		return;

	if (m_bRender == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CNewpoliceStatue::Render()
{
	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;

	return S_OK;
}

HRESULT CNewpoliceStatue::Add_Components()
{
	


	return S_OK;
}

HRESULT CNewpoliceStatue::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CNewpoliceStatue::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_NewpoliceStatue::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CNewpoliceStatue::PART_BODY] = pBodyObj;

	/*Part_Item*/
	m_PartObjects[CNewpoliceStatue::PART_PART] = nullptr;

	return S_OK;
}

HRESULT CNewpoliceStatue::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CNewpoliceStatue::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CNewpoliceStatue* CNewpoliceStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNewpoliceStatue* pInstance = new CNewpoliceStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNewpoliceStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;

}
CGameObject* CNewpoliceStatue::Clone(void* pArg)
{
	CNewpoliceStatue* pInstance = new CNewpoliceStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CNewpoliceStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNewpoliceStatue::Free()
{
	__super::Free();

}
