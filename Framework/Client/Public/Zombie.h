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

/* For.Status */
#define	STATUS_ZOMBIE_DEFAULT_RECOGNITION_DISTANCE		5.f
#define	STATUS_ZOMBIE_MAX_RECOGNITION_DISTANCE			7.f
#define STATUS_ZOMBIE_MAX_RECOGNITION_TIME				5.f

#define	STATUS_ZOMBIE_TRY_HOLD_TIME						3.f
#define	STATUS_ZOMBIE_TRY_HOLD_RANGE					3.f
#define	STATUS_ZOMBIE_VIEW_ANGLE						XMConvertToRadians(180.f)
#define	STATUS_ZOMBIE_HEALTH							100.f
#define	STATUS_ZOMBIE_ATTACK							1.f
#define	STATUS_ZOMBIE_BITE_RANGE						1.f

#define STATUS_ZOMBIE_LIGHTLY_HOLD_RANGE				1.f
#define STATUS_ZOMBIE_TRY_LIGHTLY_HOLD_TIME				0.5f

#define STATUS_ZOMBIE_TRY_STANDUP_TIME					2.f

class CZombie final : public CMonster
{
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
	
public://For Decal
	virtual void						Perform_Skinning() override;
	virtual void						Ready_Decal() override;

private: // For AIController
	class CBlackBoard_Zombie*			m_pBlackBoard = { nullptr };

private:
	class CModel*						m_pBodyModel = { nullptr };

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
#pragma endregion

public:
	inline void							Set_ManualMove(_bool isManualMove) { m_isManualMove = isManualMove; }

private:
	_bool								m_isManualMove = { false };

private:	/* For. Test */
	_int								m_iHeadHitCnt = { 0 };

public:
	static CZombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END