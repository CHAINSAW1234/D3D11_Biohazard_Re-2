#pragma once
#include "Customize_UI.h"


BEGIN(Client)

class CContextMenu final : public CCustomize_UI
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
	HRESULT Create_Background();

private:
	CGameObject* m_pBackground = { nullptr };

public:
	static CContextMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END