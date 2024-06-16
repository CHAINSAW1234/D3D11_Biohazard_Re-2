#pragma once

#include "Client_Defines.h"
#include "Customize_UI.h"


BEGIN(Client)

class CItem_UI final : public CCustomize_UI
{
protected:
	CItem_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_UI(const CItem_UI& rhs);
	virtual ~CItem_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	ITEM_NUMBER		m_eItemNumber = { ITEM_NUMBER::ITEM_NUMBER_END };

	_bool			m_bCountable = { false };

public:
	static CItem_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END