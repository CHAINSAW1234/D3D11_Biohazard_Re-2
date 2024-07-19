#include "stdafx.h"

#include "Hint.h"


constexpr _uint	 MAX_DOCUMENT = 8;


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

	if (FAILED(Load_DocumentINFO()))
		return E_FAIL;

	if (FAILED(Create_Display_Blinde()))
		return E_FAIL;

	if (FAILED(Create_Display()))
		return E_FAIL;

	if (FAILED(Create_Directory()))
		return E_FAIL;

	if (FAILED(Create_Directory_Highlighter()))
		return E_FAIL;

	if (FAILED(Create_Text_Button()))
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
		fMove += _float3{ 0.f, -63.f, 0.f };
	}

#pragma region 초기화
	Acquire_Document(ITEM_READ_TYPE::EXAMIN_ITEM);//아이템 검사
	Acquire_Document(ITEM_READ_TYPE::USE_KEY_ITEM);//열쇠 아이템 사용
	Acquire_Document(ITEM_READ_TYPE::ABOUT_MAP);//지도 활용
	Acquire_Document(ITEM_READ_TYPE::COMBIND_ITEM);//아이템 조합
	Acquire_Document(ITEM_READ_TYPE::HP_HEAL_ITEM);//체력 및 회복 아이템
#pragma endregion

#pragma region 테스트용
	Acquire_Document(ITEM_READ_TYPE::INCIDENT_LOG_NOTE); //사건일지
	Acquire_Document(ITEM_READ_TYPE::OPERATE_REPORT_NOTE); //작전보고서
	Acquire_Document(ITEM_READ_TYPE::OFFICER_NOTE); 
	Acquire_Document(ITEM_READ_TYPE::GUNPOWDER_NOTE);
#pragma endregion

}

void CHint::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	Button_Action();
	Directory_Seting();
	Hoverd_Highlight();
	Change_Display();

	m_pPoliceButton->Tick(fTimeDelta);
	m_pTutorialButton->Tick(fTimeDelta);
}

void CHint::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
	m_pPoliceButton->Late_Tick(fTimeDelta);
	m_pTutorialButton->Late_Tick(fTimeDelta);
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

void CHint::Directory_Seting()
{
	//아이템이 8개 미만이면 현재 어떤놈 먹었는지 기준으로 Directory를 정해준다
	if (MAX_DOCUMENT > m_mapAcqDoc[m_eCurrentDC].size())
	{
		for (_uint i = 0; i < m_mapAcqDoc[m_eCurrentDC].size(); i++)
		{
			ITEM_READ_TYPE eIRT = m_mapAcqDoc[m_eCurrentDC][i];
			wstring wstrName = m_mapDocumentInfo[eIRT].wstrName;
			m_vecDirectory[i]->Set_Directory(eIRT, wstrName);
		}
	}

	//아니면 최대 갯수 기준으로
	else
	{
		_int iStartnum = static_cast<int>(m_iCur_TopDoument);
		for (_uint i = 0; i < MAX_DOCUMENT; i++)
		{
			ITEM_READ_TYPE eIRT = m_mapAcqDoc[m_eCurrentDC][iStartnum + i];
			wstring wstrName = m_mapDocumentInfo[eIRT].wstrName;
			m_vecDirectory[i]->Set_Directory(eIRT, wstrName);
			iStartnum++;
		}
	}
}

void CHint::Hoverd_Highlight()
{
	if (MAX_DOCUMENT >= m_mapAcqDoc[m_eCurrentDC].size())
	{
		for (_uint i = 0; i < m_mapAcqDoc[m_eCurrentDC].size(); i++)
		{
			if (true == m_vecDirectory[i]->IsMouseHover())
			{
				m_pHL_Trans->Set_State(CTransform::STATE_POSITION, m_vecDirectory[i]->GetPosition());
				ITEM_READ_TYPE eIRT = m_vecDirectory[i]->Get_DirectoryType();
				if (eIRT != m_pDisplay->Get_CurIRT())
				{
					DOCUMENT_INFO DocumentInfo = m_mapDocumentInfo[eIRT];
					m_pDisplay->Set_Display(eIRT, 0, DocumentInfo.fPosition, DocumentInfo.fSize, false);
					if (m_pHoveredDirectory != m_vecDirectory[i])
					{
						m_pHoveredDirectory = m_vecDirectory[i];
						m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_InvenSlot_Tick.mp3"), 0.5f);
					}
				}
			}
		}
	}

	else
	{
		for (_uint i = 0; i < MAX_DOCUMENT; i++)
		{
			if (true == m_vecDirectory[i]->IsMouseHover())
			{
				m_pHL_Trans->Set_State(CTransform::STATE_POSITION, m_vecDirectory[i]->GetPosition());
				ITEM_READ_TYPE eIRT = m_vecDirectory[i]->Get_DirectoryType();
				if (eIRT != m_pDisplay->Get_CurIRT())
				{
					DOCUMENT_INFO DocumentInfo = m_mapDocumentInfo[eIRT];
					m_pDisplay->Set_Display(eIRT, 0, DocumentInfo.fPosition, DocumentInfo.fSize, false);
					if (m_pHoveredDirectory != m_vecDirectory[i])
					{
						m_pHoveredDirectory = m_vecDirectory[i];
						m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_InvenSlot_Tick.mp3"), 0.5f);
					}
				}
			}
		}
	}
}

void CHint::Change_Display()
{
	if (DOWN == m_pGameInstance->Get_KeyState(VK_LEFT))
	{
		_uint iCurTexNum = m_pDisplay->Get_CurTexNum();
		if (0 != iCurTexNum)
		{
			m_pDisplay->Set_CurTexNum(--iCurTexNum);

			if (false == m_bLeftRight_Flip)
			{
				m_bLeftRight_Flip = !m_bLeftRight_Flip;
				m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Flip_page_Left.mp3"), 0.5f);
			}
			else
			{
				m_bLeftRight_Flip = !m_bLeftRight_Flip;
				m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Flip_page_Right.mp3"), 0.5f);
			}
		}
	}

	if (DOWN == m_pGameInstance->Get_KeyState(VK_RIGHT))
	{
		_uint iMaxTexCount = m_pDisplay->Get_CurTypeTexCount();
		_uint iCurTexNum = m_pDisplay->Get_CurTexNum();
		if (iMaxTexCount > iCurTexNum + 1)
		{
			m_pDisplay->Set_CurTexNum(++iCurTexNum);

			if (false == m_bLeftRight_Flip)
			{
				m_bLeftRight_Flip = !m_bLeftRight_Flip;
				//m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Flip_page_Left.mp3"), 0.5f);
				m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Flip_page_Left.mp3"), 0.5f);
			}
			else
			{
				m_bLeftRight_Flip = !m_bLeftRight_Flip;
				//m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Flip_page_Right.mp3"), 0.5f);
				m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Flip_page_Left.mp3"), 0.5f);
			}
		}
	}
}

void CHint::Button_Action()
{
	if (DOWN == m_pGameInstance->Get_KeyState(VK_LBUTTON))
	{
		if (true == m_pPoliceButton->IsMouseHover() && POLICE != m_eCurrentDC)
		{
			m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Button_Click.mp3"), 0.5f);
			

			m_eCurrentDC = POLICE;
			m_pPoliceButton->Set_FontColor(XMVectorSet(1.0f, 1.f, 1.f, 1.f));
			m_pTutorialButton->Set_FontColor(XMVectorSet(0.8f, 0.8f, 0.8f, 1.f));

			for (auto& iter : m_vecDirectory)
				iter->Set_Dead(true);

			if (m_mapAcqDoc[m_eCurrentDC].size() >= MAX_DOCUMENT)
			{
				for (_uint i = 0; i < MAX_DOCUMENT; i++)
				{
					m_vecDirectory[i]->Set_Directory(m_mapAcqDoc[m_eCurrentDC][i], m_mapDocumentInfo[m_mapAcqDoc[m_eCurrentDC][i]].wstrName);
					m_vecDirectory[i]->Set_Dead(false);
				}
			}
			else
			{
				for (size_t i = 0; i < m_mapAcqDoc[m_eCurrentDC].size(); i++)
				{
					m_vecDirectory[i]->Set_Directory(m_mapAcqDoc[m_eCurrentDC][i], m_mapDocumentInfo[m_mapAcqDoc[m_eCurrentDC][i]].wstrName);
					m_vecDirectory[i]->Set_Dead(false);
				}
			}

			DOCUMENT_INFO DocuInfo = m_mapDocumentInfo[m_mapAcqDoc[m_eCurrentDC][0]];

			m_pDisplay->Set_Display(m_mapAcqDoc[m_eCurrentDC][0], 0, DocuInfo.fPosition, DocuInfo.fSize, false);
		}

		else if (true == m_pTutorialButton->IsMouseHover() && TUTORIAL != m_eCurrentDC)
		{
			m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"),TEXT("sound_ui_Button_Click.mp3"),0.5f);

			m_eCurrentDC = TUTORIAL;

			m_pTutorialButton->Set_FontColor(XMVectorSet(1.0f, 1.f, 1.f, 1.f));
			m_pPoliceButton->Set_FontColor(XMVectorSet(0.8f, 0.8f, 0.8f, 1.f));

			for (auto& iter : m_vecDirectory)
				iter->Set_Dead(true);

			if (m_mapAcqDoc[m_eCurrentDC].size() >= MAX_DOCUMENT)
			{
				for (_uint i = 0; i < MAX_DOCUMENT; i++)
				{
					m_vecDirectory[i]->Set_Directory(m_mapAcqDoc[m_eCurrentDC][i], m_mapDocumentInfo[m_mapAcqDoc[m_eCurrentDC][i]].wstrName);
					m_vecDirectory[i]->Set_Dead(false);
				}
			}
			else
			{
				for (size_t i = 0; i < m_mapAcqDoc[m_eCurrentDC].size(); i++)
				{
					m_vecDirectory[i]->Set_Directory(m_mapAcqDoc[m_eCurrentDC][i], m_mapDocumentInfo[m_mapAcqDoc[m_eCurrentDC][i]].wstrName);
					m_vecDirectory[i]->Set_Dead(false);
				}
			}

			DOCUMENT_INFO DocuInfo = m_mapDocumentInfo[m_mapAcqDoc[m_eCurrentDC][0]];

			m_pDisplay->Set_Display(m_mapAcqDoc[m_eCurrentDC][0], 0, DocuInfo.fPosition, DocuInfo.fSize, false);
		}
	}
}

void CHint::Set_Dead(_bool bDead)
{
	m_bDead = bDead;		

	if (0 != m_mapAcqDoc.size()) {
		m_pHighlighter->Set_Dead(bDead);
		m_pHL_Trans->Set_State(CTransform::STATE_POSITION, m_vecDirectory[0]->GetPosition());
		m_iCur_TopDoument = m_mapAcqDoc[m_eCurrentDC][0];
	}
		
	if (false == bDead)
	{
		if (m_mapAcqDoc[m_eCurrentDC].size() >= MAX_DOCUMENT)
		{
			for (_uint i = 0; i < MAX_DOCUMENT; i++)
			{
				m_vecDirectory[i]->Set_Dead(false);
				m_vecDirectory[i]->Set_Directory(m_mapAcqDoc[m_eCurrentDC][i], m_mapDocumentInfo[m_mapAcqDoc[m_eCurrentDC][i]].wstrName);
			}
		}
		else
		{
			for (size_t i = 0; i < m_mapAcqDoc[m_eCurrentDC].size(); i++)
			{
				m_vecDirectory[i]->Set_Dead(false);
				m_vecDirectory[i]->Set_Directory(m_mapAcqDoc[m_eCurrentDC][i], m_mapDocumentInfo[m_mapAcqDoc[m_eCurrentDC][i]].wstrName);
			}
		}
	} 

	else{
		m_eCurrentDC = POLICE;
		for (auto& iter : m_vecDirectory) 
		{
			iter->Set_Dead(true);
		}
	}

	m_pDisplay->Set_Dead(bDead);

	ITEM_READ_TYPE eIRT = m_vecDirectory[0]->Get_DirectoryType();
	DOCUMENT_INFO DocumentInfo = m_mapDocumentInfo[eIRT];
	m_pDisplay->Set_Display(eIRT, 0, DocumentInfo.fPosition, DocumentInfo.fSize, bDead);

	m_pDisplay_Blinde->Set_Dead(bDead);
}

void CHint::Acquire_Document(ITEM_READ_TYPE eAcquire_Document)
{
	DOCUMENT_CLASSIFY eDC = Document_Classify_ByNumber(eAcquire_Document);

	m_mapAcqDoc[eDC].push_back(eAcquire_Document);
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

HRESULT CHint::Load_DocumentINFO()
{
	//"../Bin/DataFiles/Item_Discription/Document_INFO.json"

	 // JSON 파일 열기
	FILE* fp;
	errno_t err = fopen_s(&fp, "../Bin/DataFiles/Item_Discription/Document_INFO.json", "r");
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
			wstring wstrDocumentClassify = m_pGameInstance->ConvertToWString((*itr)["CLASSIFY"].GetString(), (*itr)["CLASSIFY"].GetStringLength());
			wstring wstrDocumentName = m_pGameInstance->ConvertToWString((*itr)["NAME"].GetString(), (*itr)["NAME"].GetStringLength());
			_float fPosX = (*itr)["POSITION_X"].GetFloat();
			_float fPosY = (*itr)["POSITION_Y"].GetFloat();
			_float fSizeX = (*itr)["SIZE_X"].GetFloat();
			_float fSizeY = (*itr)["SIZE_Y"].GetFloat();

			DOCUMENT_INFO DocumentInfo = {};

			if (TEXT("POLICE") == wstrDocumentClassify)
				DocumentInfo.eDoc_Classify = POLICE;
			else if (TEXT("TUTORIAL") == wstrDocumentClassify)
				DocumentInfo.eDoc_Classify = TUTORIAL;
			else
				return E_FAIL;

			DocumentInfo.wstrName = wstrDocumentName;
			DocumentInfo.fPosition = { fPosX , fPosY };
			DocumentInfo.fSize = { fSizeX , fSizeY };

			m_mapDocumentInfo.emplace(Classify_IRT_By_Name(DocumentInfo.wstrName), DocumentInfo);
		}
	}

	fclose(fp);

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
	ifstream inputFileStream;
	wstring selectedFilePath;
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Hint/Hint_Display.dat");
	inputFileStream.open(selectedFilePath, ios::binary);

	CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Hint_Display"),
		(CGameObject**)&m_pDisplay, m_pDevice, m_pContext);

	if (nullptr == m_pDisplay)
		return E_FAIL;

	Safe_AddRef(m_pDisplay);
	m_pDisplay->Set_Dead(true);


	return S_OK;
}

HRESULT CHint::Create_Directory()
{
	for (_uint i = 0; i < MAX_DOCUMENT; i++)
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
	ifstream inputFileStream;
	wstring selectedFilePath;
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Hint/Hint_Highlighter.dat");
	inputFileStream.open(selectedFilePath, ios::binary);

	CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Hint_Highliter"),
		(CGameObject**)&m_pHighlighter, m_pDevice, m_pContext);

	if (nullptr == m_pHighlighter)
		return E_FAIL;

	m_pHL_Trans = m_pHighlighter->Get_Transform();

	Safe_AddRef(m_pHighlighter);
	Safe_AddRef(m_pHL_Trans);

	m_pHighlighter->Set_Dead(true);

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
	Safe_Release(m_pDisplay);
	for (auto& iter : m_vecDirectory)
	{
		Safe_Release(iter);
	}
	Safe_Release(m_pHighlighter);
	Safe_Release(m_pHL_Trans);
	Safe_Release(m_pPoliceButton);
	Safe_Release(m_pTutorialButton);
}

CHint::DOCUMENT_CLASSIFY CHint::Document_Classify_ByNumber(ITEM_READ_TYPE eIRT_Num)
{
	switch (eIRT_Num)
	{
	case Client::ITEM_READ_TYPE::INCIDENT_LOG_NOTE:
	case Client::ITEM_READ_TYPE::OPERATE_REPORT_NOTE:
	case Client::ITEM_READ_TYPE::TASK_NOTE:
	case Client::ITEM_READ_TYPE::MEDICINAL_NOTE:
	case Client::ITEM_READ_TYPE::OFFICER_NOTE:
	case Client::ITEM_READ_TYPE::GUNPOWDER_NOTE:
	case Client::ITEM_READ_TYPE::FIX_LOCKER_NOTE:
	case Client::ITEM_READ_TYPE::HAND_HELD_SAFE_NOTE:
	case Client::ITEM_READ_TYPE::RICKER_NOTE:
	case Client::ITEM_READ_TYPE::SAFE_PASSWARD_NOTE:
	case Client::ITEM_READ_TYPE::PAMPHLET:
		return POLICE;
		break;

	case Client::ITEM_READ_TYPE::EXAMIN_ITEM:
	case Client::ITEM_READ_TYPE::USE_KEY_ITEM:
	case Client::ITEM_READ_TYPE::ABOUT_MAP:
	case Client::ITEM_READ_TYPE::COMBIND_ITEM:
	case Client::ITEM_READ_TYPE::HP_HEAL_ITEM:
		return TUTORIAL;
		break;

	default:
		break;
	}

	return DC_END;
}

HRESULT CHint::Create_Text_Button()
{

	CTextBox::TextBox_DESC TextBox_Desc= {};
	TextBox_Desc.wstrText = TEXT("경찰서");
	TextBox_Desc.wstrFontType = TEXT("Font_CGBold");
	TextBox_Desc.vFontColor = { 1.f, 1.f, 1.f, 1.f };
	TextBox_Desc.iFontSize = { 14 };
	TextBox_Desc.isUI_Render = { false };
	TextBox_Desc.vPos = { 740.f, 120.f };
	TextBox_Desc.vSize = { 100.f, 50.f };

	CGameObject* pPoliceButton = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TextBox"), &TextBox_Desc);
	m_pPoliceButton = dynamic_cast<CTextBox*>(pPoliceButton);

	if (nullptr == m_pPoliceButton)
		return E_FAIL;


	m_pPoliceButton->Set_isUIRender(true);

	TextBox_Desc.wstrText = TEXT("튜토리얼");
	TextBox_Desc.wstrFontType = TEXT("Font_CGBold");
	TextBox_Desc.vFontColor = { 1.f, 1.f, 1.f, 1.f };
	TextBox_Desc.iFontSize = { 14 };
	TextBox_Desc.isUI_Render = { false };
	TextBox_Desc.vPos = { 840.f, 120.f };
	TextBox_Desc.vSize = { 100.f, 50.f };


	CGameObject* pTutorialButton = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TextBox"), &TextBox_Desc);
	m_pTutorialButton = dynamic_cast<CTextBox*>(pTutorialButton);

	if (nullptr == m_pTutorialButton)
		return E_FAIL;

	m_pTutorialButton->Set_isUIRender(true);

	return S_OK;
}

ITEM_READ_TYPE CHint::Classify_IRT_By_Name(wstring wstrName)
{
	if (TEXT("사건 일지") == wstrName)
		return ITEM_READ_TYPE::INCIDENT_LOG_NOTE;

	else if (TEXT("작전 보고서") == wstrName)
		return ITEM_READ_TYPE::OPERATE_REPORT_NOTE;

	else if (TEXT("신입의 첫 번째 과제") == wstrName)
		return ITEM_READ_TYPE::TASK_NOTE;

	else if (TEXT("약초의 의학적 효능") == wstrName)
		return ITEM_READ_TYPE::MEDICINAL_NOTE;

	else if (TEXT("경관의 노트") == wstrName)
		return ITEM_READ_TYPE::OFFICER_NOTE;

	else if (TEXT("화약 사용") == wstrName)
		return ITEM_READ_TYPE::GUNPOWDER_NOTE;

	else if (TEXT("물품 보관함 단말기 관련 쪽지") == wstrName)
		return ITEM_READ_TYPE::FIX_LOCKER_NOTE;

	else if (TEXT("휴대용 금고 지침") == wstrName)
		return ITEM_READ_TYPE::HAND_HELD_SAFE_NOTE;

	else if (TEXT("만일의 생존자에게") == wstrName)
		return ITEM_READ_TYPE::RICKER_NOTE;

	else if (TEXT("내부용 쪽지") == wstrName)
		return ITEM_READ_TYPE::SAFE_PASSWARD_NOTE;

	else if (TEXT("가이드 책자") == wstrName)
		return ITEM_READ_TYPE::PAMPHLET;
	
	else if (TEXT("아이템 검사") == wstrName)
		return ITEM_READ_TYPE::EXAMIN_ITEM;

	else if (TEXT("열쇠 아이템 사용") == wstrName)
		return ITEM_READ_TYPE::USE_KEY_ITEM;

	else if (TEXT("지도 활용") == wstrName)
		return ITEM_READ_TYPE::ABOUT_MAP;

	else if (TEXT("아이템 조합") == wstrName)
		return ITEM_READ_TYPE::COMBIND_ITEM;

	else if (TEXT("체력 및 회복 아이템") == wstrName)
		return ITEM_READ_TYPE::HP_HEAL_ITEM;


	return ITEM_READ_TYPE::END_NOTE;
}