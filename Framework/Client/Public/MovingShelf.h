#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CMovingShelf final : public CInteractProps
{
public:
	enum SHELF_STATE
	{
		SHELF_DOWN,
		SHELF_UPLRIGHT,
		SHELF_END,
	};
	enum SHELF_PART
	{
		PART_BODY,
		PART_END
	};
private:
	CMovingShelf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMovingShelf(const CMovingShelf& rhs);
	virtual ~CMovingShelf() = default;

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
	void								Active();

public:
	virtual _float4				Get_Object_Pos() override;


private:
	_ubyte							m_eState = { CABINET_CLOSED };



public:
	static CMovingShelf* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END