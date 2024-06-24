#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CEventProp final : public CInteractProps
{
public:
	enum EVENT_PROP_TYPE
	{
		
	};
	enum EVENT_PROP_STATE
	{
		STATE_PLAY,
		STATE_STATIC,
		STATE_END,
	};
	enum EVENT_PART
	{
		PART_BODY,
		PART_END,
	};
private:
	CEventProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEventProp(const CEventProp& rhs);
	virtual ~CEventProp() = default;

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
	_ubyte			m_eState = { STATE_STATIC };


public:
	static CEventProp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END