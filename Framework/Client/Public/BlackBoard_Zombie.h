#pragma once

#include "Client_Defines.h"
#include "BlackBoard.h"
#include "Body_Zombie.h"
#include "Zombie.h"

BEGIN(Engine)
class CTask_Node;
END

BEGIN(Client)

class CBlackBoard_Zombie : public CBlackBoard
{
public:
	typedef struct tagBlackBoardZombieDesc
	{
		class CZombie*				pAI = { nullptr };
	}BLACKBOARD_ZOMBIE_DESC;
private:
	CBlackBoard_Zombie();
	CBlackBoard_Zombie(const CBlackBoard_Zombie& rhs);
	virtual ~CBlackBoard_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

private:
	HRESULT							SetUp_Nearest_Window();

public:
	virtual void					Priority_Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;

private:
	void							Update_Timers(_float fTimeDelta);

	void							Update_Recognition_Timer(_float fTimeDelta);
	void							Update_StandUp_Timer(_float fTimeDelta);
	void							Update_LightlyHold_Timer(_float fTimeDelta);
	void							Update_Hold_Timer(_float fTImeDelta);

private:
	CGameObject*					Find_NearestObejct_In_Layer(const wstring& strLayerTag);

public:
	inline class CWindow*			Get_Nearest_Window() { return m_pNearest_Window; }
	void							Release_Nearest_Window();
	class CCustomCollider*			Get_Nearest_Window_CustomCollider();

private:
	void							Update_Status(_float fTimeDelta);

	void							Update_Status_Stamina(_float fTimeDelta);

private:
	void							Update_Look_Target(_float fTImeDelta);

public:	//	Interact
	_bool							Hit_Player();

public: // Setter
	void							SetPlayer(class CPlayer* pPlayer)
	{
		m_pPlayer = pPlayer;
	}
	class CZombie*					Get_AI()
	{
		return m_pAI;
	}

public: // Getter
	class CPlayer*					Get_Player()
	{
		return m_pPlayer;
	}

	inline CTask_Node*				Get_PreTaskNode() { return m_pPreTaskNode; }

	void							Set_Current_MotionType_Body(MOTION_TYPE eType);
	_uint							Get_Current_MotionType_Body();
	_uint							Get_Pre_MotionType_Body();

	CZombie::ZOMBIE_STATUS*			Get_ZombieStatus_Ptr();

	_int							Get_Current_AnimIndex(CMonster::PART_ID ePartID, PLAYING_INDEX eIndex);
	wstring							Get_Current_AnimLayerTag(CMonster::PART_ID ePartID, PLAYING_INDEX eIndex);
	CModel*							Get_PartModel(_uint iPartType);

	_matrix							Get_FirstKeyFrame_RootTransformationMatrix(CMonster::PART_ID eID, const wstring& strAnimLayerTag, _int iAnimIndex);

	inline _bool					Is_LookTarget() { return m_pAI->Is_LookTarget(); }

public:		/* Anim Controll */
	void							Reset_NonActive_Body(const list<_uint>& ActivePlayingIndices);

public:		/* Public Utility */
	_bool							Compute_Distance_To_Player_World(_float* pDistance);

	_bool							Compute_Direction_To_Player_World(_float3* pDirection);
	_bool							Compute_Direction_To_Player_Local(_float3* pDirection);

	_bool							Compute_Direction_From_Player_World(_float3* pDirection);
	_bool							Compute_Direction_From_Player_Local(_float3* pDirection);

	_bool							Compute_Direction_From_Hit_World(_float3* pDirection);
	_bool							Compute_Direction_From_Hit_Local(_float3* pDirection);

	_bool							Compute_Player_Angle_XZ_Plane_Local(_float* pAngle);

	_bool							Compute_Direction_To_Player_8Direction_Local(DIRECTION* pDiection);
	_bool							Compute_Direction_To_Player_4Direction_Local(DIRECTION* pDiection);

	_bool							Compute_Direction_To_Target_8Direction_Local(_fvector vTargetPosition, DIRECTION* pDirection);
	_bool							Compute_Direction_To_Target_4Direction_Local(_fvector vTargetPosition, DIRECTION* pDirection);

	_bool							Compute_Distance_To_Target(CGameObject* pTargetObject, _float* pDistance);
	_bool							Compute_Distance_To_Player(_float* pDistance);

	CTransform*						Get_Transform(CGameObject* pObject);
	CTransform*						Get_Transform_AI();

	/* For Bite Sequence Utility */
	_bool							Compute_HalfMatrix_Current_BiteAnim(const wstring& strBiteAnimLayerTag, _int iAnimIndex, _float4x4*	pResultMatrix);

private:	/* Private Utility */
	CModel*							Find_PartModel(_uint iPartID);

	CBody_Zombie*					Get_PartObject_Body();


public:		/* Anim Branch Check */
	ZOMBIE_BODY_ANIM_GROUP			Get_Current_AnimGroup(PLAYING_INDEX eIndex);
	ZOMBIE_BODY_ANIM_TYPE			Get_Current_AnimType(PLAYING_INDEX eIndex);


public:		/* Motion Blend Controll */
	vector<_float>					Get_BlendWeights(_uint iPartID);
	
protected:
	class CPathFinder*				m_pPathFinder = { nullptr };
	class CPlayer*					m_pPlayer = { nullptr };
	class CZombie*					m_pAI = { nullptr };

private:
	class CWindow*					m_pNearest_Window = { nullptr };

public:
	static CBlackBoard_Zombie* Create(void* pArg);

public:
	virtual void Free() override;
};

END