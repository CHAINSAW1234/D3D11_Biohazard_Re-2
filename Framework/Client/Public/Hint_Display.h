#pragma once

#include "UI.h"

BEGIN(Client)

class CHint_Display final : public CUI
{
protected:
	CHint_Display(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHint_Display(const CHint_Display& rhs);
	virtual ~CHint_Display() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Start() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_Dead(_bool bDead) override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHint_Display* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END