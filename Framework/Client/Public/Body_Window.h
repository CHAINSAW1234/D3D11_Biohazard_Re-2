#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CWindow final : public CInteractProps
{
private:
	CWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWindow(const CWindow& rhs);
	virtual ~CWindow() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;
	virtual HRESULT				Bind_ShaderResources() override;

private:
	void Active();



private:
	_bool				m_bActive = { false };
	_float			m_fTime = { 0.f };


public:
	static CWindow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END