#include "stdafx.h"

#include "Item_Discription.h"
#include "HotKey.h"

CHotKey::CHotKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice , pContext }
{
}

CHotKey::CHotKey(const CHotKey& rhs)
	: CUI{ rhs }
{
}

HRESULT CHotKey::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHotKey::Initialize(void* pArg)
{
	CUI::UI_DESC UIDesc = Read_HotKeyDat();

	if (FAILED(__super::Initialize(&UIDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_bDead = true;

	return S_OK;
}

void CHotKey::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;
}

void CHotKey::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);

}

HRESULT CHotKey::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHotKey::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isAlphaControl", &m_isAlphaControl, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CUI::UI_DESC CHotKey::Read_HotKeyDat()
{
	ifstream inputFileStream;
	wstring selectedFilePath;
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/HotKey.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	vector<CCustomize_UI::CUSTOM_UI_DESC> vecCustomInvenUIDesc;
	CCustomize_UI::ExtractData_FromDat(inputFileStream, &vecCustomInvenUIDesc, true);

	CUI::UI_DESC UIDesc = {};
	UIDesc.vPos = { vecCustomInvenUIDesc[0].worldMatrix._41, vecCustomInvenUIDesc[0].worldMatrix._42, vecCustomInvenUIDesc[0].worldMatrix._43, 1.f };

	UIDesc.vSize.x = sqrt(vecCustomInvenUIDesc[0].worldMatrix._11 * vecCustomInvenUIDesc[0].worldMatrix._11 +
		vecCustomInvenUIDesc[0].worldMatrix._12 * vecCustomInvenUIDesc[0].worldMatrix._12 + 
		vecCustomInvenUIDesc[0].worldMatrix._13 * vecCustomInvenUIDesc[0].worldMatrix._13);

	UIDesc.vSize.y =sqrt(vecCustomInvenUIDesc[0].worldMatrix._21 * vecCustomInvenUIDesc[0].worldMatrix._21 +
		vecCustomInvenUIDesc[0].worldMatrix._22 * vecCustomInvenUIDesc[0].worldMatrix._22 + 
		vecCustomInvenUIDesc[0].worldMatrix._23 * vecCustomInvenUIDesc[0].worldMatrix._23);

	for (_uint i = 1; i < 5; i++)
	{
		m_fPositions[i-1] = { vecCustomInvenUIDesc[i].worldMatrix._41, vecCustomInvenUIDesc[i].worldMatrix._42, vecCustomInvenUIDesc[i].worldMatrix._43};
	}

	return UIDesc;
}

HRESULT CHotKey::Init_InvenSlot()
{

	return S_OK;
}

HRESULT CHotKey::Create_InvenSlot(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI, _float3 fSetPos)
{

	return S_OK;
}

HRESULT CHotKey::Init_ItemUI()
{

	return S_OK;
}

HRESULT CHotKey::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAlphaSortTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WholeMouse"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}


CHotKey* CHotKey::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHotKey* pInstance = new CHotKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHotKey"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHotKey::Clone(void* pArg)
{
	CHotKey* pInstance = new CHotKey(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHotKey"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHotKey::Free()
{
	__super::Free();

}
