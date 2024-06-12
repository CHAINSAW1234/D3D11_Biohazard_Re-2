#pragma once
#include "UI.h"


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

private:
	class CButton_UI* m_pMapButton = { nullptr };
	class CButton_UI* m_pInvenButton = { nullptr };
	class CButton_UI* m_pHintButton = { nullptr };

	//class CInventory_Item_UI* m_pInventory1 = { nullptr }; //테스트용 임시객체
	//class CInventory_Item_UI* m_pInventory2 = { nullptr }; //테스트용 임시객체

private:
	WINDOW_TYPE m_eWindowType = { INVENTORY };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Create_Button();
	HRESULT Create_Inventory();

public:
	static CTab_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END