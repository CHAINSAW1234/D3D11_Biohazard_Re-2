#pragma once
#include "UI.h"


BEGIN(Client)

class CTab_Window final : public CUI
{
protected:
	CTab_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTab_Window(const CTab_Window& rhs);
	virtual ~CTab_Window() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_IsActivation = { false };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTab_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END