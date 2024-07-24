#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CRead_Item_UI final : public CCustomize_UI
{
public :
	/* ������� */
	enum class READ_UI_TYPE { INTRODUCE_READ, MAIN_READ, TEXTURE_READ, TEXT_LEFT_READ, TEXT_RIGHT_READ, ITEM_TYPE_READ, ARROW_READ, END_READ };
	
private :
	enum class READ_ARROW_TYPE { LEFT_ARROW, RIGHT_ARROW, END_ARROW };

private:
	CRead_Item_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRead_Item_UI(const CRead_Item_UI& rhs);
	virtual ~CRead_Item_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Change_Tool() override;

private:
	virtual void Start() override;


	/* ==== Setter Inline ====== */
public:
	void Set_ReadItem_Type(ITEM_READ_TYPE _readType);

	/* ==== Getter Inline ====== */
public:
	READ_UI_TYPE Get_UI_TYPE() { return m_eRead_type; }


	/* ==== Function ====== */
private :
	void									Operate_ReadUI(_float fTimeDelta);
	void									Introduce_Read(_float fTimeDelta); /* 1. ���� ���� �����ϴ� ��ü �Լ� */
	void									Texture_Read(); /* 2. ��ü�� Texture�� ������ �Լ� */
	void									Arrow_Read(); /* 3. ȭ��ǥ ��ü �Լ� */
	void									Text_Read(_float fTimeDelta); /* Text Texture �Լ� */
	
private :
	void									Check_UsingReadUI();
	CRead_Item_UI*							Find_ReadUI(READ_UI_TYPE _readType, _bool _child);
	void									Find_Cursor();
	void									Render_Condition();
	void									Reset(_bool isItem = false);
	void									Render_Destory(_bool _render); /* ���� ������ ���;� �ϴ� Read UI��� true�� Text Texture�� ������Ų��. */

private :
	/* intro�� �����Ͽ� Read Type�� �˷��� ���� */
	READ_UI_TYPE							m_eRead_type		= { READ_UI_TYPE::END_READ };
	READ_ARROW_TYPE							m_eRead_Arrow_Type	= { READ_ARROW_TYPE::END_ARROW };
	ITEM_READ_TYPE							m_eBook_Type		= { ITEM_READ_TYPE::END_NOTE };

	_bool									m_eReadUI_Using[static_cast<_uint>(ITEM_READ_TYPE::END_NOTE)] = { false };
	_bool*									m_pZoomOff			= { nullptr }; /* �� ���� */

private :
	CRead_Item_UI*							m_pIntro_UI			= { nullptr };
	_float									m_fIntro_Timer		= { 0.0f };
	
	_bool									m_isReadCall		= { false };
	_bool									m_isRead_Start		= { false };
	_bool									m_isChange			= { false };
	_bool									m_isPrevRender		= { false };

	_bool									m_isOneRender		= { false };

private :
	CRead_Item_UI*							m_pTexture_UI		= { nullptr }; /* Arrow�� ������ �ִ� Texture */
	CRead_Item_UI*							m_pRead_Supervise	= { nullptr }; /* MAIN : �� �ȿ� �ο��� TEXT�� TEXTURE�� ������ ���� ���̴�. */

	_int									m_iBookCnt			= { 0 };
	_int									m_iBook_PrevCnt		= { 1 };
	vector<CRead_Item_UI*>					m_ReadVec;

	class CCursor_UI*						m_pCursor			= { nullptr };
	_float3									m_vOriginScaled		= {};

private : /* Text ����*/
	map<ITEM_READ_TYPE, vector<wstring>>	m_BookText;
	_float2									m_fOriginPos_text = {};
	ITEM_READ_TYPE							eGara = { ITEM_READ_TYPE::INCIDENT_LOG_NOTE };

private :/* Item Read */
	_float4									m_vOrigionTexturePos = { };

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END