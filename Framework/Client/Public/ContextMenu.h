#pragma once
#include "Customize_UI.h"


BEGIN(Client)

class CContextMenu final : public CGameObject
{
protected:
	CContextMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContextMenu(const CContextMenu& rhs);
	virtual ~CContextMenu() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void PopUp_Operation(_float fTimeDelta);
	void Idle_Operation(_float fTimeDelta);
	void Hide_Operation(_float fTimeDelta);

public:
	void MenuSeting(ITEM_TYPE eItemType, _bool bActive);

private:
	vector<class CButton_UI*>	m_vecMenuItem;

	UI_OPERRATION				m_eContext_State = { STATE_END };

	_float						m_fItemInterval = { 19.5f };

	_uint						m_iContextMenuCount = { 0 };

private:
	HRESULT Create_MenuItem();

public:
	static CContextMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END