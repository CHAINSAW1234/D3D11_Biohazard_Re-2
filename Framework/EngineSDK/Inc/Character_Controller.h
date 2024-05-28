#pragma once

#include "Physics_Component.h"

BEGIN(Engine)

class ENGINE_DLL CCharacter_Controller : public CPhysics_Component
{
public:
	CCharacter_Controller(PxController* Controller,class CGameObject* pCharacter,PxScene* pScene,
		PxPhysics* pPhysics);
	CCharacter_Controller(const CCharacter_Controller& rhs);
	virtual ~CCharacter_Controller() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
public:
	virtual void Update() override;
public:
	static CCharacter_Controller* Create();

	virtual void Move(_float4 Dir, _float fTimeDelta) override;
	virtual void Move(PxVec3 Dir, _float fTimeDelta) override;
	virtual _float4 GetTranslation() override;
	virtual void	Release_Px() override;
	virtual _float4 GetPosition_Float4() override; 

	class CGameObject* m_pCharacter = { nullptr };

#pragma region 편의성 함수들
public:
	_bool			IsGrounded(PxController* Controller);

#pragma endregion

#pragma region Collider
public:
	void						SetColliderTransform_Body(_float4x4 Transform);
	void						SetColliderTransform_Head(_float4x4 Transform);
	void						SetColliderTransform_Left_Arm(_float4x4 Transform);
	void						SetColliderTransform_Right_Arm(_float4x4 Transform);
	void						SetColliderTransform_Left_ForeArm(_float4x4 Transform);
	void						SetColliderTransform_Right_ForeArm(_float4x4 Transform);
	void						SetColliderTransform_Pelvis(_float4x4 Transform);
	void						SetColliderTransform_Left_Leg(_float4x4 Transform);
	void						SetColliderTransform_Right_Leg(_float4x4 Transform);
	void						SetColliderTransform_Left_Shin(_float4x4 Transform);
	void						SetColliderTransform_Right_Shin(_float4x4 Transform);
	void						Create_Collider();
private:
	PxTransform					ColliderTransform_Body = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Head = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Right_Arm = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Left_Arm = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Right_ForeArm = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Left_ForeArm = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Pelvis = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Left_Leg = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Right_Leg = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Left_Shin = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Right_Shin = { PxTransform(PxVec3(0.f,0.f,0.f)) };

	PxShape*					m_Shape_Head = { nullptr };
	PxShape*					m_Shape_Body = { nullptr };
	PxShape*					m_Shape_Right_Arm = { nullptr };
	PxShape*					m_Shape_Left_Arm = { nullptr };
	PxShape*					m_Shape_Right_ForeArm = { nullptr };
	PxShape*					m_Shape_Left_ForeArm = { nullptr };
	PxShape*					m_Shape_Pelvis = { nullptr };
	PxShape*					m_Shape_Left_Leg = { nullptr };
	PxShape*					m_Shape_Right_Leg = { nullptr };
	PxShape*					m_Shape_Left_Shin = { nullptr };
	PxShape*					m_Shape_Right_Shin = { nullptr };

	PxRigidDynamic*				m_BodyCollider = { nullptr };
	PxRigidDynamic*				m_HeadCollider = { nullptr };
	PxRigidDynamic*				m_Left_Arm_Collider = { nullptr };
	PxRigidDynamic*				m_Right_Arm_Collider = { nullptr };
	PxRigidDynamic*				m_Left_ForeArm_Collider = { nullptr };
	PxRigidDynamic*				m_Right_ForeArm_Collider = { nullptr };
	PxRigidDynamic*				m_Pelvis_Collider = { nullptr };
	PxRigidDynamic*				m_Left_Leg_Collider = { nullptr };
	PxRigidDynamic*				m_Right_Leg_Collider = { nullptr };
	PxRigidDynamic*				m_Left_Shin_Collider = { nullptr };
	PxRigidDynamic*				m_Right_Shin_Collider = { nullptr };
#pragma endregion
public:
	virtual void Free() override;
};

END