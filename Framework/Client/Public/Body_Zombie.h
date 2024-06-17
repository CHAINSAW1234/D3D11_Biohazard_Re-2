#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

/* For. Bone Tag */
#define ROOT_BONE_TAG					"root"

/* For. Bone_Layer Tag */
#define BONE_LAYER_DEFAULT_TAG			TEXT("Default")
#define BONE_LAYER_LOWER_TAG			TEXT("Lower")
#define BONE_LAYER_UPPER_TAG			TEXT("Upper")

BEGIN(Client)

class CBody_Zombie final : public CPartObject
{
public:
	enum ANIM_LOOP {
		ANIM_IDLE_LOOP_A = 0,
		ANIM_IDLE_LOOP_B,
		ANIM_IDLE_LOOP_C,
		ANIM_IDLE_LOOP_D,
		ANIM_IDLE_LOOP_E,
		ANIM_IDLE_LOOP_F,

		ANIM_WALK_LOOP_A = 7,
		ANIM_WALK_LOOP_B = 11,
		ANIM_WALK_LOOP_C = 16,
		ANIM_WALK_LOOP_D = 21,
		ANIM_WALK_LOOP_E = 26,
		ANIM_WALK_LOOP_F = 31,

		ANIM_TURNING_LOOP_FL_A = 8,
		ANIM_TURNING_LOOP_FR_A,

		ANIM_TURNING_LOOP_FL_B = 12,
		ANIM_TURNING_LOOP_FR_B,

		ANIM_TURNING_LOOP_FL_C = 17,
		ANIM_TURNING_LOOP_FR_C,

		ANIM_TURNING_LOOP_FL_D = 22,
		ANIM_TURNING_LOOP_FR_D,

		ANIM_TURNING_LOOP_FL_E = 27,
		ANIM_TURNING_LOOP_FR_E,

		ANIM_TURNING_LOOP_FL_F = 32,
		ANIM_TURNING_LOOP_FR_F,		
	};

	enum ANIM_START {
		ANIM_WALK_START_A = 6,
		ANIM_WALK_START_B = 10,
		ANIM_WALK_START_C = 15,
		ANIM_WALK_START_D = 20,
		ANIM_WALK_START_E = 25,
		ANIM_WALK_START_F = 30,

		ANIM_WALK_LOOP_TO_B_WALK = 14,
		ANIM_WALK_LOOP_TO_C_WALK = 14,
		ANIM_WALK_LOOP_TO_D_WALK = 24,
		ANIM_WALK_LOOP_TO_E_WALK = 29,
		ANIM_WALK_LOOP_TO_F_WALK = 34,

		ANIM_WALK_TOTTER_F_A = 35,
		ANIM_WALK_TOTTER_F_B,
		ANIM_WALK_TOTTER_F_C,
		ANIM_WALK_TOTTER_F_D,
		ANIM_WALK_TOTTER_F_E,
		ANIM_WALK_TOTTER_F_F,

		ANIM_WALK_TURN_L180_A = 41,
		ANIM_WALK_TURN_R180_A,
		ANIM_WALK_TURN_L180_B,
		ANIM_WALK_TURN_R180_B,
		ANIM_WALK_TURN_L180_C,
		ANIM_WALK_TURN_R180_C,
		ANIM_WALK_TURN_L180_D,
		ANIM_WALK_TURN_R180_D,
		ANIM_WALK_TURN_L180_E,
		ANIM_WALK_TURN_R180_E,
		ANIM_WALK_TURN_L180_F,
		ANIM_WALK_TURN_R180_F, 


		ANIM_PIVOT_TURN_L90_A = 53,
		ANIM_PIVOT_TURN_R90_A,
		ANIM_PIVOT_TURN_L90_B,
		ANIM_PIVOT_TURN_R90_B,
		ANIM_PIVOT_TURN_L90_C,
		ANIM_PIVOT_TURN_R90_C,
		ANIM_PIVOT_TURN_L90_D,
		ANIM_PIVOT_TURN_R90_D,
		ANIM_PIVOT_TURN_L90_E,
		ANIM_PIVOT_TURN_R90_E,
		ANIM_PIVOT_TURN_L90_F,
		ANIM_PIVOT_TURN_R90_F,

		ANIM_PIVOT_TURN_L180_A = 65,
		ANIM_PIVOT_TURN_R180_A,
		ANIM_PIVOT_TURN_L180_B,
		ANIM_PIVOT_TURN_R180_B,
		ANIM_PIVOT_TURN_L180_C,
		ANIM_PIVOT_TURN_R180_C,
		ANIM_PIVOT_TURN_L180_D,
		ANIM_PIVOT_TURN_R180_D,
		ANIM_PIVOT_TURN_L180_E,
		ANIM_PIVOT_TURN_R180_E,
		ANIM_PIVOT_TURN_L180_F,
		ANIM_PIVOT_TURN_R180_F,

		ANIM_STANDUP_FACEDOWN_F = 77,
		ANIM_STANDUP_FACEDOWN_B,
		ANIM_STANDUP_FACEDOWN_L,
		ANIM_STANDUP_FACEDOWN_R,

		ANIM_STANDUP_FACEUP_F = 81,
		ANIM_STANDUP_FACEUP_B,
		ANIM_STANDUP_FACEUP_L,
		ANIM_STANDUP_FACEUP_R,
	};

	enum PLAYING_INDEX{ INDEX_0, INDEX_1, INDEX_2, INDEX_3, INDEX_4, INDEX_5, 
	INDEX_END };

	enum MOTION_TYPE {
		MOTION_A, MOTION_B, MOTION_C, MOTION_D, MOTION_E, MOTION_F, MOTION_END 
	};

public:
	typedef struct tagBodyMonsterDesc : public CPartObject::PARTOBJECT_DESC
	{
		_float3*			pRootTranslation = { nullptr };
	}BODY_MONSTER_DESC;

private:
	CBody_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Zombie(const CBody_Zombie& rhs);
	virtual ~CBody_Zombie() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;
	virtual void						Priority_Tick(_float fTimeDelta) override;
	virtual void						Tick(_float fTimeDelta) override;
	virtual void						Late_Tick(_float fTimeDelta) override;

	virtual HRESULT						Render() override;
	HRESULT								Render_LightDepth_Dir()override;
	HRESULT								Render_LightDepth_Point() override;
	HRESULT								Render_LightDepth_Spot()override;

private:
	HRESULT								Initialize_Model();

public:
	virtual void						SetRagdoll(_int iId) override;

public:
	MOTION_TYPE							Get_Current_MotionType() { return m_ePreMotionType; }
	MOTION_TYPE							Get_Pre_MotionType() { return m_eCurrentMotionType; }

private:
	void								Update_Current_MotionType();

public:	/* For.Check Anim Type */
	_bool								Is_Start_Anim(_uint iAnimIndex);
	_bool								Is_Loop_Anim(_uint iAnimIndex);
	_bool								Is_Move_Anim(_uint iAnimIndex);
	_bool								Is_Turn_Anim(_uint iAnimIndex);

private:
	CModel*								m_pModelCom = { nullptr };
	CShader*							m_pShaderCom = { nullptr };
	CCollider*							m_pColliderCom = { nullptr };

	_float3*							m_pRootTranslation = { nullptr };

	_float4x4							m_RotationMatrix;

	class CRagdoll_Physics*				m_pRagdoll = { nullptr };

private:		/* For Anim_Controll */
	MOTION_TYPE							m_ePreMotionType = { MOTION_END };
	MOTION_TYPE							m_eCurrentMotionType = { MOTION_END };

	set<_uint>							m_StartAnimIndices;
	set<_uint>							m_LoopAnimIndices;

	set<_uint>							m_MoveAnimIndices;
	set<_uint>							m_TurnAnimIndices;

private:
	HRESULT								Add_Components();
	HRESULT								Bind_ShaderResources();

public:
	static CBody_Zombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END