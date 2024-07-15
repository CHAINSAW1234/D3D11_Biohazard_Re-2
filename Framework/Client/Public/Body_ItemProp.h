#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CBody_ItemProp final : public CPart_InteractProps
{
public:
	typedef struct tagBodyItemProp_desc : public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
		_bool* pObtain = { nullptr };
		_int	   iItemIndex = { 0 };
	}BODY_ITEMPROPS_DESC;
private:
	CBody_ItemProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_ItemProp(const CBody_ItemProp& rhs);
	virtual ~CBody_ItemProp() = default;

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
	virtual void					Get_SpecialBone_Rotation() override;
	HRESULT						Initialize_Model();
public:
	virtual _float4				Get_Pos(_int iArg = 0) override;
	virtual _int						Get_PartObject_Props_ItemIndex() { return m_iItemIndex; }
private:
	_int								m_iItemIndex = { 0 };
	_bool								m_bRealDead = { false };
	_bool*							m_pObtain = { nullptr };


public:
	static CBody_ItemProp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END