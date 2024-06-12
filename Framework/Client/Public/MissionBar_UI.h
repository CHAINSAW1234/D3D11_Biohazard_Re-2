#pragma once
#include "Customize_UI.h"

class CMissionBar_UI final : public CCustomize_UI
{

private:
	CMissionBar_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMissionBar_UI(const CMissionBar_UI& rhs);
	virtual ~CMissionBar_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private :
	_bool					m_isMission = {};
	_float					m_fLightTimer = {};


public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

