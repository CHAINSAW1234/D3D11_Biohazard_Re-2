#include "stdafx.h"

#include "Item_Discription.h"

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

		m_bDead = true;
	}

	return S_OK;
}

void CItem_Discription::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Tick(fTimeDelta);
}

void CItem_Discription::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);
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
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Tab_Window_BackGround"),
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
}
