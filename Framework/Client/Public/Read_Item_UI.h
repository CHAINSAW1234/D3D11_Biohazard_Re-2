#pragma once
#include "Interact_UI.h"

BEGIN(Client)

class CRead_Item_UI final : public CInteract_UI
{
public :
	enum class ITEM_READ_TYPE { INCIDENT_LOG_NOTE, OPERATE_REPORT_NOTE, TASK_NOTE, MEDICINAL_NOTE, OFFICER_NOTE, END_NOTE };
	/* 사건일지 */
private :
	enum class READ_UI_TYPE { INTRODUCE_READ, MAIN_READ, TEXTURE_READ, TEXT_LEFT_READ, TEXT_RIGHT_READ, ARROW_READ, END_READ };

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

private :
	void									Render_Destory(_bool _render);

	void									Introduce_Read(_float fTimeDelta);
	void									Texture_Read();
	void									Arrow_Read();
	void									Text_Read(_float fTimeDelta);

	CRead_Item_UI*							Find_ReadUI(READ_UI_TYPE _readType, _bool _child);
	void									Render_Condition();
	void									Reset();

private :
	/* intro가 참조하여 Read Type을 알려줄 것임 */
	READ_UI_TYPE							m_eRead_type		= { READ_UI_TYPE::END_READ };
	READ_ARROW_TYPE							m_eRead_Arrow_Type	= { READ_ARROW_TYPE::END_ARROW };
	ITEM_READ_TYPE							m_eBook_Type		= { ITEM_READ_TYPE::END_NOTE };

private :
	CRead_Item_UI*							m_pIntro_UI			= { nullptr };
	_float									m_fIntro_Timer		= { 0.0f };
	_bool									m_isRead_Start		= { false };


private :
	CRead_Item_UI*							m_pTexture_UI		= { nullptr }; /* Arrow가 가지고 있는 Texture */
	CRead_Item_UI*							m_pRead_Supervise	= { nullptr }; /* MAIN : 이 안에 부여할 TEXT와 TEXTURE를 가지고 있을 것이다. */
	_int									m_iBookCnt			= { 0 };
	_int									m_iBook_PrevCnt		= { 1 };

private : /* Text 관련*/
	map<ITEM_READ_TYPE, vector<wstring>>	m_BookText;
	_float2									m_fOriginPos_text = {};
	ITEM_READ_TYPE							eGara = { ITEM_READ_TYPE::INCIDENT_LOG_NOTE };

public:
	static CInteract_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END