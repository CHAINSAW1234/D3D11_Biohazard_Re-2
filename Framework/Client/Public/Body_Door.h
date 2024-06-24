#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"

BEGIN(Client)

class CBody_Door final : public CPart_InteractProps
{
public:
	typedef struct tag_BodyDoor : public PART_INTERACTPROPS_DESC
	{
		const _ubyte* pDoubleDoorType;
		const _ubyte* pOneDoorState;
		const _ubyte* pOneDoorState_Prev;
		const _ubyte* pDoubleDoorState;
		const _ubyte* pDoubleDoorState_Prev;

	}BODY_DOOR_DESC;
private:
	CBody_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Door(const CBody_Door& rhs);
	virtual ~CBody_Door() = default;
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


private:
	_bool				m_bLock =	{ false };
	_bool				m_bActive = { false };

	_bool				m_bDoubleCol = { false };

	const _ubyte*			m_pDoubleDoorType = {};
	const _ubyte*			m_pOneState = {};
	const _ubyte*			m_pOneState_Prev = {};
	const _ubyte*			m_pDoubleState = {};
	const _ubyte*			m_pDoubleState_Prev = {};

	_float			m_fTime = { 0.f };
public:
	static CBody_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END