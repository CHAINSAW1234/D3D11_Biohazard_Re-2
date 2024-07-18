#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CLoading_UI final : public CCustomize_UI
{
private:
	enum class LOADING_UI_TYPE { BACKGROUND_TEXT, BACKGROUND_LOADING_UI, TYPING_BACKGROUND_LOADING_UI, TYPING_BODY_LOADING_UI, ROLLING_LOADING_UI, TYPING_HEAD_LOADING_UI, END_LOADING_UI };

private:
	CLoading_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoading_UI(const CLoading_UI& rhs);
	virtual ~CLoading_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Change_Tool() override;

public :
	void				Set_LoadingEnd(_bool _loading) { m_isLoading_Notification = _loading;  }

private:
	void				Operate_Typing(_float fTimeDelta);
	void				Typing_Head_Moving(_float fTimeDelta);
	void				Typing_Rolling_Moving(_float fTimeDelta);
	void				Render_Text(_float fTimeDelta);
	void				Render_Ending(float fTimeDelta);

private :
	void				Operate_Ending(_float fTimeDelta);
	void				Ending_Notification();
	void				Find_Loading_BackGround();

private :
	LOADING_UI_TYPE		m_eLoadingType	= { LOADING_UI_TYPE::END_LOADING_UI };

private : 
	_float4				m_fBody_Position			= {};
	_float4				m_vOrigin_Position			= {};

	_float				m_fLoading_Timer			= {};
	_float				m_fText_Timer				= { 1.5f };

	_int				m_iMaxText					= {};
	_int				m_iTextCnt					= {};

private :/* Loading Text */
	CLoading_UI*		m_pBackGroundUI				= { nullptr };
	_bool				m_isLoadingEnd				= { false };
	_bool				m_isLoading_Notification	= { false };

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END