#pragma once
#include "Inventory.h"



BEGIN(Client)

class CInventory_Item_UI final : public CInventory
{
private:
	CInventory_Item_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventory_Item_UI(const CInventory_Item_UI& rhs);
	virtual ~CInventory_Item_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	void		Change_BoxType(_bool _item);

private :
	void		VoidBox();
	void		SelectBox(_float fTimeDelta);
	void		ClickOnBox(); 

private:
	CInventory_Item_UI*		m_pSelectBox = { nullptr };


private: /* Defualt Box만 사용*/
	_float2					m_fDistance_BetweenCursor = {}; /* Select Box와 Cursor 간의 거리*/
	_bool					m_isMouseAcess = { false }; 

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END