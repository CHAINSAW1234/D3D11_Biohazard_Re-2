#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"

BEGIN(Client)

class CDoor final : public CInteractProps
{
public:
	enum DOOR_TEXTURE
	{
		WOOD,
		IRON
	};

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
		L_DOUBLEDOOR_OPEN,
		R_DOUBLEDOOR_OPEN,
		DOUBLEDOOR_END
	};
	enum DUMMYDOOR_STATE
	{
		DUMMY_OPEN //더미,,, 오픈한다
	};
	enum TYPE_DOOR
	{
		DOOR_ONE,
		DOOR_DOUBLE,
		DOOR_DUMMY
	};
	enum Lock_Type
	{
		LOCK_EMBLEM,
		LOCK_CHAIN
	};

	enum class EMBLEM_TYPE
	{
		HEART_EMBLEM,
		SPADE_EMBLEM,
		CLOVER_EMBLEM,
		DIA_EMBLEM,
		END_EMBLEM
	};

	enum PART_DOOR
	{
		PART_BODY,
		PART_LOCK,
		PART_EMBLEM,
		PART_EMBLEM_MARK,
		PART_KEY,
		PART_END


		//PART_HEART_EMBLEM,
		//PART_SPADE_EMBLEM,
		//PART_CULB_EMBLEM,
		//PART_DIA_EMBLEM,
		//PART_HEART_KEY,
		//PART_SPADE_KEY,
		//PART_CULB_KEY,
		//PART_DIA_KEY,
		//PART_END
	};

private:
	CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoor(const CDoor& rhs);
	virtual ~CDoor() = default;

public:
	virtual HRESULT								Initialize_Prototype() override;
	virtual HRESULT								Initialize(void* pArg) override;
	virtual void								Start() override;
	virtual void								Tick(_float fTimeDelta) override;
	virtual void								Late_Tick(_float fTimeDelta) override;
	virtual HRESULT								Render() override;

private:
	virtual HRESULT								Add_Components();
	virtual HRESULT								Add_PartObjects() override;
	virtual void									Do_Interact_Props() override;
	virtual HRESULT								Initialize_PartObjects() override;

private:
	HRESULT										Add_Locations();

public:
	inline const list<LOCATION_MAP_VISIT>&		Get_LinkedLocations() { return m_Linked_Locations; }
	inline TYPE_DOOR							Get_DoorType() { return static_cast<TYPE_DOOR>(m_eType); }

private:
	void										DoubleDoor_Tick(_float fTimeDelta);
	void										DoubleDoor_Late_Tick(_float fTimeDelta);
	void										OneDoor_Tick(_float fTimeDelta);
	void										OneDoor_Late_Tick(_float fTimeDelta);

	_float										Radian_To_Player();
	_float										Radian_To_Zombie(class CTransform* pTransform);
	_float									Distance_Zombie(class CTransform* pTransform);
	void										OneDoor_Active();
	void										DoubleDoor_Active();

public:
	virtual _float4								Get_Object_Pos() override;
	virtual _bool								Attack_Prop(class CTransform* pTransfromCom = nullptr) override;
	

public:
	inline _int									Get_HP() { return m_iHP; }
	inline _bool								Is_Lock() { return m_bLock; }
	inline class CCustomCollider*				Get_CustomCollider_Ptr() { return m_pMyCustomCollider; }

public :
	_bool										Get_Door_Lock()		{ return m_bLock; }				// 잠겼는지 아닌지(문이 잠김이 true)
	_bool										Get_FirstInteract()	{ return m_bFirstInteract; } // 접촉만 함 (탐색 - 빨간색)
	_bool										Get_Interact()		{ return m_bInteract; }			// 열었는지 아닌지(파란색)
	void										Set_Lock(_bool bLock=false) { m_bLock = bLock; }
private:
	_bool										m_bLock =	{ false };
	_bool										m_bAutoOpen =	{ false };
	_ubyte									m_eDoorTexture = { WOOD };
	_bool										m_bAttack = { false };
	_int										m_iHP = { 5 };
	_float										m_fDelayTime = { 0.f };
	_float										m_fDelayLockTime = { 0.f };
	_float										m_fTime = { 0.f };
	_ubyte										m_eType = {DOOR_ONE};

	_bool										m_bInteract = { false }; // 문을 열었는지(탐색 완료)

	class CCustomCollider*						m_pMyCustomCollider = { nullptr };
	_ubyte  									m_eOneState					= { ONEDOOR_STATIC };
	_ubyte  									m_eOneState_Prev			= { ONEDOOR_STATIC };

	_ubyte   									m_eDoubleState				= { DOUBLEDOOR_STATIC };
	_ubyte   									m_eDoubleState_Prev		= { DOUBLEDOOR_STATIC };
	_ubyte										m_eDoubleDoorType;
	list<LOCATION_MAP_VISIT>					m_Linked_Locations;
	CTransform*									m_pZombieTransform = { nullptr };

private :
	_bool										m_bKeyUsing = { true };
	_bool										m_bCamera = { false };
	_uint										m_iEmblemType = {};
	_int										m_iPropType = { 0 };
	_ubyte										m_eEmblemAnim_Type = {0};
	_bool										m_isCameraGimmick = {};

public:
	static CDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END