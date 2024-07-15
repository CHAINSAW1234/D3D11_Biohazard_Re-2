#pragma once
#include "Customize_UI.h"


BEGIN(Client)

class CHint_Directory final : public CCustomize_UI
{
protected:
	CHint_Directory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHint_Directory(const CHint_Directory& rhs);
	virtual ~CHint_Directory() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Start() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CHint_Directory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END