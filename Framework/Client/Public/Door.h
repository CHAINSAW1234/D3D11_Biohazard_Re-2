#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"

BEGIN(Client)

class CDoor final : public CInteractProps
{
public:
	enum ONEDOOR_STATE
	{
		ONEDOOR_OPEN_L,
		ONEDOOR_OPEN_R,
		ONEDOOR_STATIC,
		ONEDOOR_END
	};
	enum DOUBLEDOOR_STATE
	{
		LSIDE_DOUBLEDOOR_OPEN_L,
		LSIDE_DOUBLEDOOR_OPEN_R,
		RSIDE_DOUBLEDOOR_OPEN_L,
		RSIDE_DOUBLEDOOR_OPEN_R,
		DOUBLEDOOR_STATIC,
		LSIDE_DOUBLEDOOR_OPEN,
		RSIDE_DOUBLEDOOR_OPEN,
		DOUBLEDOOR_END
	};
	enum TYPE_DOOR
	{
		DOOR_ONE,
		DOOR_DOUBLE,
	};
	enum PART_DOOR
	{
		PART_BODY,
		PART_LOCK,
		PART_END

	};


private:
	CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoor(const CDoor& rhs);
	virtual ~CDoor() = default;

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

private:
	void DoubleDoor_Tick(_float fTimeDelta);
	void DoubleDoor_Late_Tick(_float fTimeDelta);
	void OneDoor_Tick(_float fTimeDelta);
	void OneDoor_Late_Tick(_float fTimeDelta);

	void OneDoor_Active();
	void DoubleDoor_Active();
public:
	virtual _float4 Get_Object_Pos() override;

private:
	_bool				m_bLock =	{ false };
	_bool				m_bActive = { false };

	_bool				m_bDoubleCol = { false };

	_float				m_fTime = { 0.f };
	_ubyte				m_eType = {DOOR_ONE};

	_ubyte  			m_eOneState = { ONEDOOR_STATIC };
	_ubyte  			m_eOneState_Prev = { ONEDOOR_STATIC };

	_ubyte   			m_eDoubleState = { DOUBLEDOOR_STATIC };
	_ubyte   			m_eDoubleState_Prev = { DOUBLEDOOR_STATIC };
	class CCollider* 	m_pColDoubledoorCom = { nullptr };
	_ubyte				m_eDoubleDoorType;
public:
	static CDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END