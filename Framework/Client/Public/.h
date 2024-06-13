#pragma once
#include "Customize_UI.h"

class CHPBar_UI final : public CCustomize_UI
{
private :
	enum class CROSSHAIR_TYPE { CROSSHAIR_LEFT, CROSSHAIR_RIGHT, CROSSHAIR_UP, CROSSHAIR_DOWN, CROSSHAIR_END };

private:
	CHPBar_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHPBar_UI(const CHPBar_UI& rhs);
	virtual ~CHPBar_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

