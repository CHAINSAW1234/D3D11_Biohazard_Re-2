#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CItemProp final : public CInteractProps
{
public:
	enum ITEM_STATE
	{
		ITEM_UNUSE,
		ITEM_USE,
		ITEM_END,
	};
	enum ITEM_PART
	{
		PART_BODY,
		PART_END,
	};
private:
	CItemProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemProp(const CItemProp& rhs);
	virtual ~CItemProp() = default;

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
	_ubyte  m_eState = { ITEM_UNUSE };


public:
	static CItemProp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END