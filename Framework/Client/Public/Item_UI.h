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


public:
	_bool				Get_isWorking() const { return m_isWorking; }
	void				Set_isWorking(_bool IsWorking) { m_isWorking = IsWorking; }

	ITEM_NUMBER			Get_ItemNumber() const { return m_eItemNumber; };
	void				Set_ItemNumber(ITEM_NUMBER eItmeNum) { m_eItemNumber = eItmeNum; }

	ITEM_TYPE			Get_InvenItemType() const { return m_eInvenItemType; };
	void				Set_InvenItemType(ITEM_TYPE eItmeType) { m_eInvenItemType = eItmeType; }

public:
	void				Reset_Item();
	void				Set_Item(ITEM_NUMBER eItmeNum);
	
private:
	_bool				m_isWorking = { false };

private:
	ITEM_NUMBER			m_eItemNumber = { ITEM_NUMBER::ITEM_NUMBER_END };
	ITEM_TYPE			m_eInvenItemType = { ITEM_TYPE::INVEN_ITEM_TYPE_END };

	_bool				m_bCountable = { false };
	_uint				m_iItemCount = { 0 };

public:
	static CItem_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END