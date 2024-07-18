#pragma once

#include "UI.h"

#include "Hint_Highliter.h"
#include "Hint_Directory.h"
#include "Hint_Display.h"
#include "Hint_Blind.h"

BEGIN(Client)

class CHint final : public CUI
{
private:
	enum DOCUMENT_CLASSIFY {POLICE, TUTORIAL, DC_END};

	typedef struct Tag_Document_INFO
	{
		DOCUMENT_CLASSIFY	eDoc_Classify = { DC_END };
		wstring				wstrName{ TEXT("") };
		_float2				fPosition = { 0.f, 0.f };
		_float2				fSize = { 0.f, 0.f };
	}DOCUMENT_INFO;

protected:
	CHint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHint(const CHint& rhs);
	virtual ~CHint() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Start() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	DOCUMENT_CLASSIFY Document_Classify_ByNumber(ITEM_READ_TYPE eIRT_Num);
	void Directory_Seting();
	void Hoverd_Highlight();
	void Change_Display();
	void Button_Action();

public:
	virtual void Set_Dead(_bool bDead) override;
	void Acquire_Document(ITEM_READ_TYPE eAcquire_Document);
	

private:
	CHint_Highliter*			m_pHighlighter = { nullptr };
	CTransform*					m_pHL_Trans = { nullptr };//하이라이트 트렌스폼
	CHint_Blind*				m_pDisplay_Blinde = { nullptr }; 
	vector<CHint_Directory*>	m_vecDirectory;
	CHint_Directory*			m_pHoveredDirectory = { nullptr };
	CHint_Display*				m_pDisplay = { nullptr };
	CTextBox*					m_pPoliceButton = { nullptr };
	CTextBox*					m_pTutorialButton = { nullptr };

private:
	ITEM_READ_TYPE										m_iCur_TopDoument = { ITEM_READ_TYPE::END_NOTE };	//현재 맨위에있는 이름
	DOCUMENT_CLASSIFY									m_eCurrentDC = { POLICE };							//현재 탭
	map <DOCUMENT_CLASSIFY, vector<ITEM_READ_TYPE>>		m_mapAcqDoc;										//습득한 문서
	map <ITEM_READ_TYPE, DOCUMENT_INFO>					m_mapDocumentInfo;									//문서 정보
	_bool												m_bLeftRight_Flip = { false };


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	HRESULT Load_DocumentINFO();
	HRESULT	Create_Display_Blinde();
	HRESULT	Create_Display();
	HRESULT Create_Directory();
	HRESULT Create_Directory_Highlighter();
	HRESULT Create_Text_Button();

public:
	static CHint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

public:
	static ITEM_READ_TYPE Classify_IRT_By_Name(wstring wstrName);
};

END