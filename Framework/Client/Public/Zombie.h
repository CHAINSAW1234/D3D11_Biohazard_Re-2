#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CPartObject;
END

BEGIN(Client)

#include "Zombie_Defines.h"
#include "Zombie_Enums.h"

class CZombie final : public CMonster
{
public:
	typedef struct tagZombieDesc : public MONSTER_DESC
	{
		LOCATION_MAP_VISIT			eLocation = { LOCATION_MAP_VISIT::LOCATION_MAP_VISIT_END };
		ZOMBIE_START_TYPE			eStart_Type = { ZOMBIE_START_TYPE::_IDLE };
		ZOMBIE_BODY_TYPE			eBodyModelType = { ZOMBIE_BODY_TYPE::_END };
		_bool						bJumpScare = { false };
		_uint						iJumpScareType = { 0 };
	}ZOMBIE_DESC;

	typedef struct tagZombieMaleDesc : public ZOMBIE_DESC
	{
		ZOMBIE_MALE_PANTS			ePantsType = { ZOMBIE_MALE_PANTS::_END };
		ZOMBIE_MALE_FACE			eFaceType = { ZOMBIE_MALE_FACE::_END };
		ZOMBIE_MALE_SHIRTS			eShirtsType = { ZOMBIE_MALE_SHIRTS::_END };
		ZOMBIE_MALE_HAT				eHatType = { ZOMBIE_MALE_HAT::_END };
	}ZOMBIE_MALE_DESC;

	typedef struct tagZombieFemaleDesc : public ZOMBIE_DESC
	{
		ZOMBIE_FEMALE_PANTS			ePantsType = { ZOMBIE_FEMALE_PANTS::_END };
		ZOMBIE_FEMALE_FACE			eFaceType = { ZOMBIE_FEMALE_FACE::_END };
		ZOMBIE_FEMALE_SHIRTS		eShirtsType = { ZOMBIE_FEMALE_SHIRTS::_END };
	}ZOMBIE_FEMALE_DESC;

	typedef struct tagZombieMaleBigDesc : public ZOMBIE_DESC
	{
		ZOMBIE_MALE_BIG_PANTS		ePantsType = { ZOMBIE_MALE_BIG_PANTS::_END };
		ZOMBIE_MALE_BIG_FACE		eFaceType = { ZOMBIE_MALE_BIG_FACE::_END };
		ZOMBIE_MALE_BIG_SHIRTS		eShirtsType = { ZOMBIE_MALE_BIG_SHIRTS::_END };
	}ZOMBIE_MALE_BIG_DESC;

public:
	typedef struct tagZombieStatus : public MONSTER_STATUS
	{
		
	}ZOMBIE_STATUS;

public:
	enum COLLIDERTYPE { COLLIDER_HEAD, COLLIDER_BODY, COLLIDER_END };
	enum class FACE_STATE { _UP, _DOWN, _END };
	enum class POSE_STATE { _CREEP, _UP, _END };

	enum class LOST_STATE_BIG { _HEAD, _UPPER_BODY, _LOW_BODY, _END };					//	대분류
	enum class LOST_STATE_MIDDLE { _L_LEG, _R_LEG, _L_ARM, _R_ARM, _END };				//	중분류
	enum class LOST_STATE_L_LEG { _END };												//	소분류
	enum class LOST_STATE_R_LEG { _END };												//	소분류
	enum class LOST_STATE_L_ARM { _END };												//	소분류
	enum class LOST_STATE_R_ARM { _END };												//	소분류

	enum class USE_STAMINA { _HOLD, _STAND_UP, _TURN_OVER, _END };

private:
	CZombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CZombie(const CZombie& rhs);
	virtual ~CZombie() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;
	virtual void						Priority_Tick(_float fTimeDelta) override;
	virtual void						Tick(_float fTimeDelta) override;
	virtual void						Late_Tick(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

public://For AIController;
	virtual void						Init_BehaviorTree_Zombie() override;
	
private:
	virtual HRESULT						Add_Components() override;
	virtual HRESULT						Bind_ShaderResources() override;
	virtual HRESULT						Add_PartObjects() override;
	virtual HRESULT						Initialize_Status() override;
	virtual HRESULT						Initialize_States();
	virtual HRESULT						Initialize_PartBreaker();
	virtual HRESULT						Initialize_Sounds();
	virtual HRESULT						Add_RagDoll_OtherParts();
	
private:
	virtual HRESULT						Add_SoundTags();

	HRESULT								Add_SoundTags_Idle_Voice();
	HRESULT								Add_SoundTags_Move_Voice();
	HRESULT								Add_SoundTags_Bite_Voice();
	HRESULT								Add_SoundTags_Bite_Reject_Voice();
	HRESULT								Add_SoundTags_Bite_Kill_Voice();
	HRESULT								Add_SoundTags_Bite_Creep_Voice();
	HRESULT								Add_SoundTags_Bite_Creep_Reject_Voice();
	HRESULT								Add_SoundTags_Bite_Creep_Kill_Voice();
	HRESULT								Add_SoundTags_KnockBack_Voice();
	HRESULT								Add_SoundTags_Stun_Voice();
	HRESULT								Add_SoundTags_StandUp_Voice();
	HRESULT								Add_SoundTags_Hit_EF();
	HRESULT								Add_SoundTags_Hold_Voice();
	HRESULT								Add_SoundTags_Foot_Foot();
	HRESULT								Add_SoundTags_Foot_Creep_Foot();
	HRESULT								Add_SoundTags_Drop_Body_Foot();
	HRESULT								Add_SoundTags_ETC();
	HRESULT								Add_SoundTags_Interact_Interact();
	HRESULT								Add_SoundTags_BreakHead_Break();
	HRESULT								Add_SoundTags_BreakOthers_Break();

public:
	void								Play_Random_Hit_Sound();
	void								Play_Random_Stun_Sound();
	void								Play_Random_KnockBack_Sound();
	void								Play_Random_StandUp_Sound();
	void								Play_Random_Idle_Sound();
	void								Play_Random_Move_Sound();
	void								Play_Random_Hold_Sound();
	void								Play_Random_Bite_Sound();
	void								Play_Random_Bite_Reject_Sound();
	void								Play_Random_Bite_Kill_Sound();
	void								Play_Random_Bite_Creep_Sound();
	void								Play_Random_Bite_Creep_Reject_Sound();
	void								Play_Random_Bite_Creep_Kill_Sound();
	void								Play_Random_Foot_Sound();
	void								Play_Random_Foot_Creep_Sound();
	void								Play_Random_Drop_Body_Sound();
	void								Play_Random_Broken_Head_Sound();
	void								Play_Random_Broken_Part_Sound();
	void								Play_Random_Knock_Window_Sound();
	void								Play_Random_Break_Window_Sound();
	void								Play_Random_Knock_Door_Sound();
	void								Play_Random_Open_Door_Sound();

public:
	inline _bool						Is_RagDoll() { return m_bRagdoll; }

public:
	void								Play_Animations_Body(_float fTimeDelta);

public:		/* For.IK Controll */
	void								Active_IK_Body(_bool isActive);

public:		/* For.Collision Part */
	inline COLLIDER_TYPE				Get_Current_IntersectCollider() { return m_eCurrentHitCollider; }
	inline HIT_TYPE						Get_Current_HitType() { return m_eCurrentHitType; }

public:		/* For.Face State */
	inline FACE_STATE					Get_FaceState() { return m_eFaceState; }
	inline void							Set_FaceState(FACE_STATE eFaceState) { m_eFaceState = eFaceState; }

public:		/* For.Pose State */
	inline POSE_STATE					Get_PoseState() { return m_ePoseState; }
	inline void							Set_PoseState(POSE_STATE ePoseState) { m_ePoseState = ePoseState; }

public:		/* For.Sleep Controll */
	inline _bool						Is_Sleep() { return m_isSleep; }
	inline void							Set_Sleep(_bool isSleep) { m_isSleep = isSleep; }

public:		/* For.Look Target Controll */
	inline _bool						Is_LookTarget() { return m_isLook_Target; }
	inline void							Set_LookTarget(_bool isLookTarget) { m_isLook_Target = isLookTarget; }

public:		/* For.Outdoor */
	inline _bool						Is_OutDoor() { return m_isOutDoor; }
	inline void							Set_OutDoor(_bool isOutDoor) { m_isOutDoor = isOutDoor; }

public:		/* For.Start_Trigger */
	inline _bool						Is_Start() { return m_isStart; }
	inline void							Set_Start(_bool isStart) { m_isStart = isStart; }

public:		/* For.Location Controll */
	_bool								Is_In_Location(LOCATION_MAP_VISIT eLocation);
	_bool								Is_In_Linked_Location(LOCATION_MAP_VISIT eLocation);
	_bool								Is_In_Linked_Location_CheckDummyDoor(LOCATION_MAP_VISIT eLocation, _bool* pIsDummyDoor);
	inline LOCATION_MAP_VISIT			Get_Location() { return m_eLocation; }
	inline void							Set_Location(LOCATION_MAP_VISIT eLocation) { m_eLocation = eLocation; }

public:		/* Access */
	inline MONSTER_STATE				Get_Current_MonsterState() { return m_eState; }
	inline _float3						Get_Current_HitDirection() { return m_vHitDirection; }

private:	/* Initialize_PartObjects_Models */
	virtual HRESULT 					Initialize_PartModels() override;

private:	/* For.Use Stamina */
	_bool								Is_Enough_Stamina(USE_STAMINA eAction);

public:		
	_bool								Use_Stamina(USE_STAMINA eAction);

public:		/* For.Start Type */
	inline ZOMBIE_START_TYPE			Get_StartType() { return m_eStartType; }

public:		/* For.Room Culling */
	void								Set_Render_RoomCulling(_bool isRender) { m_isRender = isRender; }



#pragma region 예은 추가 - 창문, 문 (난간은 생각을 해봐야합니다)
private:
	void								Col_EventCol();
private:
	_bool								m_bEvent = { false };
	_int								m_iPropNum = { 0 };
	_float								m_fEventCoolTime = { 0.f };
	JOMBIE_BEHAVIOR_COLLIDER_TYPE		m_eBeHavior_Col = {};
	vector<CGameObject*> m_InteractObjVec;
#pragma endregion

public:
	inline _int							Get_Current_Region() { return m_iCurrent_Region; }
	inline MAP_FLOOR_TYPE				Get_Current_Floor() { return m_eCurrent_Floor; }

private:
	void								Update_Region_Datas();

public:
	void								Change_Sound(const wstring& strSoundTag, _uint iSoundIndex);
	void								Set_Volume(_float fVolume, _uint iSoundIndex);
	void								Stop_Sound(_uint iSoundIndex);

public: // For Ragdoll
	void								SetRagdoll_StartPose();
public://For Decal
	virtual void						Perform_Skinning() override;
	virtual void						Ready_Decal() override;
	void								RayCast_Decal();

public:	 /* For.Part Breaker */
	inline _int							Get_New_BreakPartType() { return m_iNew_Break_PartType; }

private: // For AIController
	class CBlackBoard_Zombie*			m_pBlackBoard = { nullptr };

private:
	class CModel*						m_pBodyModel = { nullptr };
	class CModel*						m_pHeadModel = { nullptr };
	class CModel*						m_pShirtsModel = { nullptr };
	class CModel*						m_pPantsModel = { nullptr };
	class CModel*						m_pHatModel = { nullptr };

private:
	FACE_STATE							m_eFaceState = { FACE_STATE::_END };
	POSE_STATE							m_ePoseState = { POSE_STATE::_END };

	_bool								m_isSleep = { false };
	_bool								m_isLook_Target = { false };
	_bool								m_isStart = { false };
	_bool								m_isOutDoor = { false };

private:	/* For. Hit Interact */
	COLLIDER_TYPE						m_eCurrentHitCollider = { _END };
	HIT_TYPE							m_eCurrentHitType = { HIT_END };
	_float3								m_vHitDirection = {};

private:	/* For. Start Type */
	ZOMBIE_START_TYPE					m_eStartType = { ZOMBIE_START_TYPE::_END };

private:	/* For. Region Controller */
	_int								m_iCurrent_Region = { -1 };
	MAP_FLOOR_TYPE						m_eCurrent_Floor = { MAP_FLOOR_TYPE::FLOOR_END };

private:	/* For. Part Breaker */
	class CPart_Breaker_Zombie*			m_pPart_Breaker = { nullptr };
	_int								m_iNew_Break_PartType = { -1 };

#pragma region Effect
public:
	void								Ready_Effect();
	void								Release_Effect();
	void								Tick_Effect(_float fTimeDelta);
	void								Late_Tick_Effect(_float fTimeDelta);
	void								SetBlood();
	void								SetBiteBlood();
	void								Calc_Decal_Map();
	void								SetBlood_STG();
	void								ResetBiteEffect();
	_float4x4							GetDecalWorldMat();
	void								SetBlood_HeadBlow();
	void								PlayBloodSound(_uint iIndex);
	void								SetWindowDrop_Sound(_bool boolean)
	{
		m_bWindowDrop_Sound = true;
		m_WindowDropSound_Time = GetTickCount64();
	}
	void								StartWindowDropSound();
protected:
	vector<class CBlood*>				m_vecBlood;
	vector<class CBlood_Drop*>			m_vecBlood_Drop;

	ULONGLONG							m_BloodTime;
	ULONGLONG							m_BloodDelay;
	_uint								m_iBloodCount = { 0 };
	_uint								m_iBloodCount_HeadBlow = { 0 };
	_bool								m_bSetBlood = { false };
	_float4								m_vHitPosition;
	_float4								m_vHitNormal;
	_uint								m_iBloodType = { 0 };
	_uint								m_iBloodType_HeadBlow = { 0 };
	_float								m_fHitDistance = { 0.f };
	_uint								m_iMeshIndex_Hit = { 0 };

	_int								m_iHat_Type = { -1 };
	_int								m_iFace_ID = { -1 };
	_int								m_iShirts_ID = { -1 };
	_int								m_iPants_ID = { -1 };
	_int								m_iBody_ID = { -1 };

	_float4								m_vDecalPoint;
	_float4								m_vDecalNormal;
	class CDecal_SSD*					m_pDecal_SSD = { nullptr };

	//list<class CGameObject*>*			m_pDecal_Layer = { nullptr };
	vector<CDecal_SSD*>					m_vecDecal_SSD;
	_uint								m_iDecal_Index = { 0 };
	_bool								m_bBigAttack = { false };

	/*Impact*/
	class CImpact*						m_pImpact = { nullptr };
	vector<class CHit*>					m_vecHit = { nullptr };

	_float4								m_vMouthPos;
	_float4								m_vMouthLook;
	_float4x4							m_MouthBone_Combined;
	_bool								m_bBiteBlood = { false };

	ULONGLONG							m_HeadBlowEffectTime = { 0 };
	ULONGLONG							m_HeadBlowEffectDelay = { 0 };
	_bool								m_bHeadBlow = { false };
	_bool								m_bWindowDrop_Sound = { false };
	ULONGLONG							m_WindowDropSound_Time = { 0 };
	ULONGLONG							m_WindowDropSound_Delay = { 0 };
	_bool								m_bDecal_Hit = { false };
	ULONGLONG							m_RagdollWakeUpTime = { 0 };
	ULONGLONG							m_RagdollWakeUpDelay = { 0 };
	_bool								m_bRagdollWakeUp = { true };
	_bool								m_bDistance_Culling = { false };
#pragma endregion

private:
	_bool								m_isRender = { false };

public:
	void								Set_ManualMove(_bool isManualMove);
	_bool								IsPlayerNearBy();
	_bool								IsPlayerNearBy_JumpScare();
private:
	_bool								m_isManualMove = { false };
	_bool								m_bJumpScare = { false };
	_bool								m_bJumpScare_Ready = { false };
	_bool								m_bJumpScare_Kinematic = { false };
	_uint								m_iJumpScare_Type = { 0 };
private:
	LOCATION_MAP_VISIT					m_eLocation = { LOCATION_MAP_VISIT::LOCATION_MAP_VISIT_END };

	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>		m_SoundTags;

public:
	static CZombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END