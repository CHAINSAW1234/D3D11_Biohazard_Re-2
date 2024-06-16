#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CButton_UI final : public CCustomize_UI
{
protected:
	CButton_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CButton_UI(const CButton_UI& rhs);
	virtual ~CButton_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CButton_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END