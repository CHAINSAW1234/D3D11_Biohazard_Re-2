#include "stdafx.h"

#include "Hint.h"




CHint::CHint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice , pContext }
{
}

CHint::CHint(const CHint& rhs)
	: CUI{ rhs }
{
}

HRESULT CHint::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHint::Initialize(void* pArg)
{
	CUI::UI_DESC UIDesc = {};
	UIDesc.vPos = { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.008f };
	UIDesc.vSize = { g_iWinSizeX, g_iWinSizeY };

	if (FAILED(__super::Initialize(&UIDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Create_Display_Blinde()))
		return E_FAIL;

	if (FAILED(Create_Display()))
		return E_FAIL;

	if (FAILED(Create_Directory()))
		return E_FAIL;

	if (FAILED(Create_Directory_Highlighter()))
		return E_FAIL;


	m_bDead = true;


	return S_OK;
}

void CHint::Start()
{
	_float3 fMove = {};
	for (auto& iter : m_vecDirectory)
	{
		iter->Move(fMove);
		fMove += _float3{ 0.f, -23.5f, 0.f };
	}
}

void CHint::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

}

void CHint::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CHint::Render()
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

void CHint::Set_Dead(_bool bDead)
{
	m_bDead = bDead;

	m_pHighlighter->Set_Dead(bDead);

	for (_uint i = 0; i < 8; i++)
	{
		m_vecDirectory[i]->Set_Dead(bDead);
	}

	//m_pDisplay->Set_Dead(bDead);

	m_pDisplay_Blinde->Set_Dead(bDead);
}

void CHint::Acquire_Document(ITEM_NUMBER eAcquire_Document)
{

}

HRESULT CHint::Bind_ShaderResources()
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

HRESULT CHint::Create_Display_Blinde()
{
	ifstream inputFileStream;
	wstring selectedFilePath;
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Hint/Hint_DisplayBlind.dat");
	inputFileStream.open(selectedFilePath, ios::binary);

	CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Hint_Blind"),
		(CGameObject**)&m_pDisplay_Blinde, m_pDevice, m_pContext);

	if (nullptr == m_pDisplay_Blinde)
		return E_FAIL;

	Safe_AddRef(m_pDisplay_Blinde);
	m_pDisplay_Blinde->Set_Dead(true);

	return S_OK;
}

HRESULT CHint::Create_Display()
{


	return S_OK;
}

HRESULT CHint::Create_Directory()
{
	for (_uint i = 0; i < static_cast<_uint>(ITEM_READ_TYPE::END_NOTE); i++)
	{
		ifstream inputFileStream;
		wstring selectedFilePath;
		selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Hint/Hint_Directory.dat");
		inputFileStream.open(selectedFilePath, ios::binary);
		CHint_Directory* pDirectory = { nullptr };
		CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Hint_Directory"),
			(CGameObject**)&pDirectory, m_pDevice, m_pContext);

		if (nullptr == pDirectory)
			return E_FAIL;

		m_vecDirectory.push_back(pDirectory);
	}

	for (auto& iter : m_vecDirectory)
	{
		if (nullptr != iter)
		{
			Safe_AddRef(iter);
			iter->Set_Dead(true);
		}
	}

	return S_OK;
}

HRESULT CHint::Create_Directory_Highlighter()
{
	CGameObject* pGameOBJ = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Hint_Highliter"));
	m_pHighlighter = dynamic_cast<CHint_Highliter*>(pGameOBJ);

	if (nullptr == m_pHighlighter)
		return E_FAIL;

	return S_OK;
}

HRESULT CHint::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hint_BackGround"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CHint* CHint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHint* pInstance = new CHint(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHint"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHint::Clone(void* pArg)
{
	CHint* pInstance = new CHint(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHint"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHint::Free()
{
	__super::Free();
	Safe_Release(m_pDisplay_Blinde);
	//Safe_Release(m_pDisplay);
	for (auto& iter : m_vecDirectory)
	{
		Safe_Release(iter);
	}
	Safe_Release(m_pHighlighter);
}
