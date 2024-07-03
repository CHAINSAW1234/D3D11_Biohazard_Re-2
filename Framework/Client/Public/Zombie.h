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
		ZOMBIE_BODY_TYPE			eBodyModelType = { ZOMBIE_BODY_TYPE::_END };
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

	enum class USE_STAMINA { _BITE, _STAND_UP, _TURN_OVER, _END };

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

public:		/* Access */
	inline MONSTER_STATE				Get_Current_MonsterState() { return m_eState; }
	inline _float3						Get_Current_HitDirection() { return m_vHitDirection; }

private:	/* Initialize_PartObjects_Models */
	virtual HRESULT 					Initialize_PartModels() override;

private:	/* For.Use Stamina */
	_bool								Is_Enough_Stamina(USE_STAMINA eAction);

public:		
	_bool								Use_Stamina(USE_STAMINA eAction);


#pragma region 예은 추가 - 창문, 문 (난간은 생각을 해봐야합니다)
private:
	void								Col_EventCol();
private:
	_bool								m_bEvent = { false };
	_int								m_iPropNum = { 0 };
	_float							m_fEventCoolTime = { 0.f };
	JOMBIE_BEHAVIOR_COLLIDER_TYPE m_eBeHavior_Col = {};
	vector<CGameObject*> m_InteractObjVec;
#pragma endregion



public://For Decal
	virtual void						Perform_Skinning() override;
	virtual void						Ready_Decal() override;

private: // For AIController
	class CBlackBoard_Zombie*			m_pBlackBoard = { nullptr };

private:
	class CModel*						m_pBodyModel = { nullptr };
	class CModel*						m_pHeadModel = { nullptr };
	class CModel*						m_pHeadModel2 = { nullptr };
	class CModel*						m_pHeadModel3 = { nullptr };
	class CModel*						m_pShirtsModel = { nullptr };
	class CModel*						m_pShirts2Model = { nullptr };
	class CModel*						m_pPantsModel = { nullptr };
	class CModel*						m_pHatModel = { nullptr };

private:
	FACE_STATE							m_eFaceState = { FACE_STATE::_END };
	POSE_STATE							m_ePoseState = { POSE_STATE::_END };

	_bool								m_isSleep = { false };

private:	/* For. Hit Interact */
	COLLIDER_TYPE						m_eCurrentHitCollider = { _END };
	HIT_TYPE							m_eCurrentHitType = { HIT_END };
	_float3								m_vHitDirection = {};

#pragma region Effect
public:
	void								Ready_Effect();
	void								Release_Effect();
	void								Tick_Effect(_float fTimeDelta);
	void								Late_Tick_Effect(_float fTimeDelta);
	void								SetBlood();
	void								Calc_Decal_Map();
protected:
	vector<class CBlood*>				m_vecBlood;

	ULONGLONG							m_BloodTime;
	ULONGLONG							m_BloodDelay;
	_uint								m_iBloodCount = { 0 };
	_bool								m_bSetBlood = { false };
	_float4								m_vHitPosition;
	_float4								m_vHitNormal;
	_uint								m_iBloodType = { 0 };
	_float								m_fHitDistance = { 0.f };
	_uint								m_iMeshIndex_Hit = { 0 };

	_int								m_iHat_Type = { -1 };
	_int								m_iFace_ID = { -1 };
	_int								m_iShirts_ID = { -1 };
	_int								m_iPants_ID = { -1 };
	_int								m_iBody_ID = { -1 };
#pragma endregion

public:
	void								Set_ManualMove(_bool isManualMove);

private:
	_bool								m_isManualMove = { false };

public:
	static CZombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END