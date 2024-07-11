#include "stdafx.h"

#include "Item_Discription.h"
#include "TextBox.h"

CItem_Discription::CItem_Discription(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice , pContext }
{
}

CItem_Discription::CItem_Discription(const CItem_Discription& rhs)
	: CUI{ rhs }
{
}

HRESULT CItem_Discription::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem_Discription::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;

		if (FAILED(Add_Components()))
			return E_FAIL;

		if (FAILED(Load_Item_Discription()))
			return E_FAIL;

		if (FAILED(Create_TextBoxes()))
			return E_FAIL;

		m_bDead = true;
	}

	return S_OK;
}

void CItem_Discription::Start()
{
	m_pItemName->Set_isUIRender(false);
	m_pItemClassify->Set_isUIRender(false);
	m_pItemDiscription->Set_isUIRender(false);
}

void CItem_Discription::Tick(_float fTimeDelta)
{
	//if (true == m_bDead)
	//	return;

	if (ITEM_NUMBER_END != m_eItemNumber)
	{
		m_pItemName->Set_Text(m_mapItemDiscription[m_eItemNumber][NAME]);
		m_pItemClassify->Set_Text(m_mapItemDiscription[m_eItemNumber][CLASSIFY]);
		m_pItemDiscription->Set_Text(m_mapItemDiscription[m_eItemNumber][DISCRIPTION]);
	}

	else
	{
		m_pItemName->Set_Text(TEXT(""));
		m_pItemClassify->Set_Text(TEXT(""));
		m_pItemDiscription->Set_Text(TEXT(""));
	}
}

void CItem_Discription::Late_Tick(_float fTimeDelta)
{
	//if (true == m_bDead)
	//	return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FONT, this);

	m_pItemName->Late_Tick(fTimeDelta);
	m_pItemClassify->Late_Tick(fTimeDelta);
	m_pItemDiscription->Late_Tick(fTimeDelta);
}

HRESULT CItem_Discription::Render()
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


HRESULT CItem_Discription::Load_Item_Discription()
{
	//"../Bin/DataFiles/Item_Discription/Item_Discription.json"

	 // JSON 파일 열기
	FILE* fp;
	errno_t err = fopen_s(&fp, "../Bin/DataFiles/Item_Discription/Item_Discription.json", "r");
	if (err != 0)
		return E_FAIL;

	char readBuffer[65536]{};
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	// JSON 문서 파싱
	rapidjson::Document document;
	rapidjson::ParseResult result = document.Parse<rapidjson::kParseDefaultFlags | rapidjson::kParseValidateEncodingFlag>(readBuffer);

	if (true == result.IsError())
		return E_FAIL;

	document.ParseStream(is);

	// JSON 데이터 입력
	if (document.IsArray())
	{
		_uint iItem_Discription_Count = 0;

		for (rapidjson::Value::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
		{
			vector<wstring> vecDis;
			vecDis.push_back(m_pGameInstance->ConvertToWString((*itr)["NAME"].GetString(), (*itr)["NAME"].GetStringLength()));
			vecDis.push_back(m_pGameInstance->ConvertToWString((*itr)["CLASSIFY"].GetString(), (*itr)["CLASSIFY"].GetStringLength()));
			vecDis.push_back(m_pGameInstance->ConvertToWString((*itr)["DESCRIPTION"].GetString(), (*itr)["DESCRIPTION"].GetStringLength()));

			m_mapItemDiscription.emplace(static_cast<ITEM_NUMBER>(iItem_Discription_Count), vecDis);

			iItem_Discription_Count++;
		}
	}

	fclose(fp);

	return S_OK;
}

HRESULT CItem_Discription::Create_TextBoxes()
{
	//1600 800     900 450

	CTextBox::TextBox_DESC TextBoxDesc = {};
	//TextBoxDesc.vPos = { 240.f, -108.f };
	TextBoxDesc.vPos = { 1040.f, 558.f };
	TextBoxDesc.vSize = { 0.1f, 0.1f };
	TextBoxDesc.wstrText = { TEXT("") };
	TextBoxDesc.wstrFontType = { TEXT("Font_CGBold") };
	TextBoxDesc.vFontColor = { 1.f, 1.f, 1.f, 1.f };
	TextBoxDesc.iFontSize = { 15 };
	TextBoxDesc.isOuterLine = { false };
	TextBoxDesc.vOutLineColor = { 0.f, 0.f, 0.f, 1.f };
	TextBoxDesc.isUI_Render = { false };

	m_pItemName = static_cast<CTextBox*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TextBox"), &TextBoxDesc));
	if (nullptr == m_pItemName)
		return E_FAIL;
	m_pItemName->Set_isTransformBase(false);

	TextBoxDesc = {};
	//TextBoxDesc.vPos = { 1140.f, -138.f };
	TextBoxDesc.vPos = { 1040.f, 588.f };
	TextBoxDesc.vSize = { 0.1f, 0.1f };
	TextBoxDesc.wstrText = { TEXT("") };
	TextBoxDesc.wstrFontType = { TEXT("Font_CGBold") };
	TextBoxDesc.vFontColor = { 0.8f, 0.8f, 0.8f, 1.f };
	TextBoxDesc.iFontSize = { 13 };
	TextBoxDesc.isOuterLine = { false };
	TextBoxDesc.vOutLineColor = { 0.f, 0.f, 0.f, 1.f };
	TextBoxDesc.isUI_Render = { false };

	m_pItemClassify = static_cast<CTextBox*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TextBox"), &TextBoxDesc));
	if (nullptr == m_pItemClassify)
		return E_FAIL;
	m_pItemClassify->Set_isTransformBase(false);

	TextBoxDesc = {};
	//TextBoxDesc.vPos = { 240.f, -174.f };
	TextBoxDesc.vPos = { 1040.f, 624.f };
	TextBoxDesc.vSize = { 0.1f, 0.1f };
	TextBoxDesc.wstrText = { TEXT("") };
	TextBoxDesc.wstrFontType = { TEXT("Font_CGBold") };
	TextBoxDesc.vFontColor = { 1.f, 1.f, 1.f, 1.f };
	TextBoxDesc.iFontSize = { 14};
	TextBoxDesc.isOuterLine = { false };
	TextBoxDesc.vOutLineColor = { 0.f, 0.f, 0.f, 1.f };
	TextBoxDesc.isUI_Render = { false };

	m_pItemDiscription = static_cast<CTextBox*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TextBox"), &TextBoxDesc));
	if (nullptr == m_pItemDiscription)
		return E_FAIL;
	m_pItemClassify->Set_isTransformBase(false);

	return S_OK;
}

HRESULT CItem_Discription::Bind_ShaderResources()
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

HRESULT CItem_Discription::Add_Components()
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
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Item_Description_BG"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}


CItem_Discription* CItem_Discription::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Discription* pInstance = new CItem_Discription(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CItem_Discription"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CItem_Discription::Clone(void* pArg)
{
	CItem_Discription* pInstance = new CItem_Discription(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CItem_Discription"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Discription::Free()
{
	__super::Free();

	Safe_Release(m_pItemName);
	Safe_Release(m_pItemClassify);
	Safe_Release(m_pItemDiscription);
}
