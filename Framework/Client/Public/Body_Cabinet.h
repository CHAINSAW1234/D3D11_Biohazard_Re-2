#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CBody_Cabinet final : public CPart_InteractProps
{
public:
	enum CABINET_STATE
	{
		CABINET_CLOSED,
		CABINET_OPEN,
		CABINET_OPENED,
		CABINET_END,
	};

private:
	CBody_Cabinet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Cabinet(const CBody_Cabinet& rhs);
	virtual ~CBody_Cabinet() = default;

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

private:
	void Active();



private:
	_bool				m_bLock =	{ false };
	_bool				m_bActive = { false };

	CABINET_STATE  m_eState = { CABINET_CLOSED };



public:
	static CBody_Cabinet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END