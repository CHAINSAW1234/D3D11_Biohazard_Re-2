#pragma once

#include "UI.h"
#include "Inventory_Manager.h"


BEGIN(Client)

class CTab_Window final : public CUI
{
public:
	enum WINDOW_TYPE{MINIMAP, INVENTORY, HINT, WINDOW_TYPE_END };

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

public :
	_bool*	Get_MinMapRender()	{ return &m_isMapRender; }
	_bool*	Get_MainRender()	{ return m_pInvenButton->Get_Dead_Ptr(); }
	
private:
	class CButton_UI* m_pMapButton = { nullptr };
	class CButton_UI* m_pInvenButton = { nullptr };
	class CButton_UI* m_pHintButton = { nullptr };

	CInventory_Manager* m_pInventory_Manager = { nullptr };

private:
	WINDOW_TYPE m_eWindowType = { INVENTORY };
	_bool		m_isMapRender = { false };

	_bool	m_isFristTick = { true };


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Create_Button();
	HRESULT Create_Inventory();
	HRESULT Creat_MiniMap();
	HRESULT Creat_Hint();


public:
	static CTab_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END