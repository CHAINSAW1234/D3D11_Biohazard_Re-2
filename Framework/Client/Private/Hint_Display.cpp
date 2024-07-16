#include "stdafx.h"

#include "Hint_Display.h"
#include "Hint.h"

CHint_Display::CHint_Display(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice , pContext }
{
}

CHint_Display::CHint_Display(const CHint_Display& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CHint_Display::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHint_Display::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	if(FAILED(Load_Texture()))
		return E_FAIL;

	m_bDead = true;

	m_pOriginalTexture = m_pTextureCom;

	return S_OK;
}
 
void CHint_Display::Start()
{
	
}

void CHint_Display::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;
}

void CHint_Display::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CHint_Display::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CHint_Display::Set_Display(ITEM_READ_TYPE eItemReadType, _uint TextureNum, _float2 fPos, _float2 fSize)
{
	if (TextureNum < m_mapDocumentTextures[eItemReadType].size())
	{
		m_pTextureCom = m_mapDocumentTextures[eItemReadType][TextureNum];
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(fPos.x, fPos.y, 0.007f, 1.f));
		m_pTransformCom->Set_Scaled(fSize.x, fSize.y, 1.f);
		m_iCurTypeTexCount = m_mapDocumentTextures[eItemReadType].size();
		m_iCurTexNum = TextureNum;
	}
	else
	{
		m_bDead = true;
	}
}

HRESULT CHint_Display::Load_Texture()
{
	//"../Bin/DataFiles/Item_Discription/Document_TextureTag.json"

	 // JSON 파일 열기
	FILE* fp;
	errno_t err = fopen_s(&fp, "../Bin/DataFiles/Item_Discription/Document_TextureTag.json", "r");
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
			wstring wstrIRT = m_pGameInstance->ConvertToWString((*itr)["ITEM_READ_TYPE"].GetString(), (*itr)["ITEM_READ_TYPE"].GetStringLength());
			wstring wstrPrototypeTag = m_pGameInstance->ConvertToWString((*itr)["ProtoTypeTag"].GetString(), (*itr)["ProtoTypeTag"].GetStringLength());

			ITEM_READ_TYPE eIRT = CHint::Classify_IRT_By_Name(wstrIRT);
			
			CTexture* pTexture = dynamic_cast<CTexture*>(m_pGameInstance->Clone_Component(g_Level, wstrPrototypeTag));

			if (nullptr == pTexture)
				return E_FAIL;
			
			m_mapDocumentTextures[eIRT].push_back(pTexture);
		}
	}

	fclose(fp);

	return S_OK;
}

CHint_Display* CHint_Display::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHint_Display* pInstance = new CHint_Display(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHint_Display"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHint_Display::Clone(void* pArg)
{
	CHint_Display* pInstance = new CHint_Display(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHint_Display"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHint_Display::Free()
{
	m_pTextureCom = m_pOriginalTexture;
	__super::Free();

	for (_int i = 0; i < static_cast<_int>(ITEM_READ_TYPE::END_NOTE); i++)
	{
		vector<CTexture*>* vecCTextures = &(m_mapDocumentTextures[static_cast<ITEM_READ_TYPE>(i)]);
		for (auto& iter : *vecCTextures)
		{
			Safe_Release(iter);
		}
	}

	//m_mapDocumentTextures.clear();
}
