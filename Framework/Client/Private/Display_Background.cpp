#include "stdafx.h"

#include "Display_Background.h"
#include "Hint.h"

CHint_Display_Background::CHint_Display_Background(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice , pContext }
{
}

CHint_Display_Background::CHint_Display_Background(const CHint_Display_Background& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CHint_Display_Background::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHint_Display_Background::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;

		if (FAILED(Load_Texture()))
			return E_FAIL;
	}

	m_pOriginalTexture = m_pTextureCom;

	m_bDead = true;

	return S_OK;
}

void CHint_Display_Background::Start()
{

}

void CHint_Display_Background::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Tick(fTimeDelta);
}

void CHint_Display_Background::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CHint_Display_Background::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHint_Display_Background::Change_Tool()
{
	return S_OK;
}

void CHint_Display_Background::Set_BackGround(ITEM_READ_TYPE eIRT, _bool bDead)
{
	if (false == CHint_Display_Background::IsCanChangeBG_Tex(eIRT))
	{
		m_bDead = true;
	}
		
	else
	{
		m_bDead = bDead;
		m_fBlending = 0.5f;
		m_pTextureCom = m_mapDocumentTextures[eIRT];
	}
}

_bool CHint_Display_Background::IsCanChangeBG_Tex(ITEM_READ_TYPE eIRT)
{
	_bool isCanChageTex = true;

	switch (eIRT)
	{
	case Client::ITEM_READ_TYPE::OFFICER_NOTE:
	case Client::ITEM_READ_TYPE::EXAMIN_ITEM:
	case Client::ITEM_READ_TYPE::USE_KEY_ITEM:
	case Client::ITEM_READ_TYPE::ABOUT_MAP:
	case Client::ITEM_READ_TYPE::COMBIND_ITEM:
	case Client::ITEM_READ_TYPE::HP_HEAL_ITEM:
	case Client::ITEM_READ_TYPE::END_NOTE:
		isCanChageTex = false;
		break;
	default:
		break;
	}

	return isCanChageTex;
}

HRESULT CHint_Display_Background::Load_Texture()
{
	//"../Bin/DataFiles/Item_Discription/Document_TextureTag.json"

		 // JSON 파일 열기
	FILE* fp;
	errno_t err = fopen_s(&fp, "../Bin/DataFiles/Item_Discription/Hint_Display_Background.json", "r");
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
		for (rapidjson::Value::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
		{
			wstring wstrIRT = m_pGameInstance->ConvertToWString((*itr)["NAME"].GetString(), (*itr)["NAME"].GetStringLength());
			wstring wstrPrototypeTag = m_pGameInstance->ConvertToWString((*itr)["TEXTURE_TAG"].GetString(), (*itr)["TEXTURE_TAG"].GetStringLength());

			ITEM_READ_TYPE eIRT = CHint::Classify_IRT_By_Name(wstrIRT);

			//if(ITEM_READ_TYPE::OFFICER_NOTE == eIRT)//창균씨 바보

			CTexture* pTexture = dynamic_cast<CTexture*>(m_pGameInstance->Clone_Component(g_Level, wstrPrototypeTag));

			if (nullptr == pTexture)
				return E_FAIL;

			m_mapDocumentTextures[eIRT] = pTexture;
		}
	}

	fclose(fp);

	return S_OK;
}

CHint_Display_Background* CHint_Display_Background::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHint_Display_Background* pInstance = new CHint_Display_Background(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHint_Display_Background"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHint_Display_Background::Clone(void* pArg)
{
	CHint_Display_Background* pInstance = new CHint_Display_Background(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHint_Display_Background"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHint_Display_Background::Free()
{
	m_pTextureCom = m_pOriginalTexture;

	__super::Free();

	for (auto& Pair : m_mapDocumentTextures)
		Safe_Release(Pair.second);

	m_mapDocumentTextures.clear();
}
