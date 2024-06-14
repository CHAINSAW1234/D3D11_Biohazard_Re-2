#pragma once
#include "Customize_UI.h"

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
	void	Play_Font(_float fTimeDelta);


private :
	_float m_fTutorialFont_Timer = {};
	_float m_fTutorial_Life_Tiemr = {};


public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

