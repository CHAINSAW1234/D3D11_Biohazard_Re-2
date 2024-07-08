#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CTutorial_UI final : public CCustomize_UI
{
private:
	CTutorial_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTutorial_UI(const CTutorial_UI& rhs);
	virtual ~CTutorial_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private :
	void	First_Behaivor(_float fTimeDelta);
	void	Play_Font(_float fTimeDelta);
	void	Player_First_Interact();
	void	Find_TextBox();
	void	Tutorial_Start();
	void	Operation_Tutorial(_float fTimeDelta);

private :
	CTextBox*			pTutorial_TextBox[2] = { nullptr }; /* 0이 아래쪽 1이 위쪽*/

	_float				m_fTutorialFont_Timer	= {};
	_float				m_fTutorial_Life_Tiemr	= {};

private :
	CTutorial_UI*		m_pTutorial_Supervise	= { nullptr };

	/* Player 쪽에서 전달하고 있는 변수 */
	UI_TUTORIAL_TYPE*	m_eTutorial_Type		= { nullptr };
	_bool*				m_pisTutorial_Notify	= { nullptr };

	_bool				m_isTutorial_Start		= { false };
	_float2				m_Origin_TextPosition	= {};

private :
	_bool				m_isTutorial[100] = {false};
	_bool				m_isStart = { false };
	_float				m_fPlayTimer = {};

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END