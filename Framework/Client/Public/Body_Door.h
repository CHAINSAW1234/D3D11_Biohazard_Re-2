#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"

#define DOOR_HIT_REACTION_TIME			0.08f

BEGIN(Client)

class CBody_Door final : public CPart_InteractProps
{
private:
	enum DOOR_BODY_PART
	{
		BODY_PART_L_LHANDLE,
		BODY_PART_L_RHANDLE,
		BODY_PART_R_LHANDLE,
		BODY_PART_R_HRANDLE,
		BODY_PART_END,
	};

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
	 HRESULT						Initialize_Model() ;
	virtual void					Get_SpecialBone_Rotation() override;

public:
	virtual _float4				Get_Pos(_int iArg = 0) override;

private:
	void								DoubleDoor_Tick(_float fTimeDelta);
	void								DoubleDoor_Late_Tick(_float fTimeDelta);
	void								OneDoor_Tick(_float fTimeDelta);
	void								OneDoor_Late_Tick(_float fTimeDelta);

private:
	void						Update_Hit_Reaction(_float fTimeDelta);

public:
	void						Hit(CTransform* pTransform);



private:
	string							m_strDoorPart[BODY_PART_END];
	_bool								m_bLock								= { false };
	_bool								m_bActivity							= { false };

	_bool								m_bDoubleCol						= { false };

	const _ubyte*				m_pDoubleDoorType				= {};
	const _ubyte*				m_pOneState							= {};
	const _ubyte*				m_pOneState_Prev					= {};
	const _ubyte*				m_pDoubleState						= {};
	const _ubyte*				m_pDoubleState_Prev			= {};

	_bool								m_isHit = { false };
	_bool								m_isInverseRotate_HitRecation = { false };
	_float								m_fAccHitTime = { 0.f };
	_bool								m_isHitFromFront = { false };

	_float							m_fTime								= { 0.f };
public:
	static CBody_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END