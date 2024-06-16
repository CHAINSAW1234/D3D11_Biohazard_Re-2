#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CBody_Zombie final : public CPartObject
{
public:
	enum ZOMBIE_BODY_ANIM_LOOP {
		ANIM_IDLE_LOOP_A = 0,
		ANIM_IDLE_LOOP_B,
		ANIM_IDLE_LOOP_C,
		ANIM_IDLE_LOOP_D,
		ANIM_IDLE_LOOP_E,
		ANIM_IDLE_LOOP_F,

		ANIM_WALK_LOOP_A = 7,

		ANIM_TURNING_LOOP_FL_A =8,
		ANIM_TURNING_LOOP_FR_A,

		ANIM_WALK_LOOP_A = 11,

		ANIM_TURNING_LOOP_FL_B = 12,
		ANIM_TURNING_LOOP_FR_B,

		ANIM_WALK_LOOP_TO_B_WALK = 14,


	};

	enum ZOMBIE_BODY_ANIM_START {
		ANIM_WALK_START_A = 6,
		ANIM_WALK_START_B = 10,

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
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;

	virtual HRESULT			Render() override;
	HRESULT					Render_LightDepth_Dir()override;
	HRESULT					Render_LightDepth_Point() override;
	HRESULT					Render_LightDepth_Spot()override;

private:
	HRESULT					Initialize_Model();

public:
	virtual void			SetRagdoll(_int iId) override;

private:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };

	_float3*				m_pRootTranslation = { nullptr };

	_float4x4				m_RotationMatrix;

	class CRagdoll_Physics* m_pRagdoll = { nullptr };
private:
	HRESULT					Add_Components();
	HRESULT					Bind_ShaderResources();

public:
	static CBody_Zombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END