#pragma once

#include "UI.h"

BEGIN(Client)

class CHint_Highliter final : public CUI
{
protected:
	CHint_Highliter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHint_Highliter(const CHint_Highliter& rhs);
	virtual ~CHint_Highliter() = default;

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
	static CHint_Highliter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END