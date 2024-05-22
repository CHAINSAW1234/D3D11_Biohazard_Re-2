
#pragma once

#include "Base.h"
#include "Event_Call_Back.h"

BEGIN(Engine)

class ENGINE_DLL CPhysics_Controller : public CBase
{
protected:
	CPhysics_Controller();
	CPhysics_Controller(const CPhysics_Controller& rhs);
	virtual ~CPhysics_Controller() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Simulate(_float fTimeDelta);

	void			Create_Controller(_float4 Pos);
	void			Create_Rigid_Dynamic(_float4 Pos);
	void			Create_Rigid_Static(_float4 Pos);
	void			Create_Plane(_float4 Pos);

	class CCharacter_Controller* GetCharacter_Controller(_int Index);
	class CRigid_Dynamic*		 GetRigid_Dynamic(_int Index);
	_float4						 GetTranslation_Rigid_Dynamic(_int Index);
public:
	_bool			IsGrounded(PxController* Controller);
	_float4			GetPosition();

	void			InitTerrain();

	_matrix						 GetWorldMatrix_Rigid_Dynamic(_int Index);
	void			Cook_Mesh(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum);
	void			Move_CCT(_float4 Dir, _float fTimeDelta,_int Index);

	//Ragdoll Temp
	void			Create_Ragdoll();
	void			Init_Radoll();
	_float4			GetHeadPos();
	_float4			GetBodyPos();
	_float4			GetPelvisPos();
	_float4			GetLeft_Arm_Pos();
	_float4			GetRight_Arm_Pos();
	_float4			GetLeft_Fore_Arm_Pos();
	_float4			GetRight_Fore_Arm_Pos();
	_float4			GetLeft_Leg_Pos();
	_float4			GetRight_Leg_Pos();
	_float4			GetRight_Shin_Pos();
	_float4			GetLeft_Shin_Pos();

	void			SetColliderTransform(_float4x4 Transform);
	void			SetColliderTransform_Head(_float4x4 Transform);
	void			SetColliderTransform_Left_Arm(_float4x4 Transform);
	void			SetColliderTransform_Right_Arm(_float4x4 Transform);
	void			SetColliderTransform_Left_ForeArm(_float4x4 Transform);
	void			SetColliderTransform_Right_ForeArm(_float4x4 Transform);
	void			SetColliderTransform_Pelvis(_float4x4 Transform);
	void			SetColliderTransform_Left_Leg(_float4x4 Transform);
	void			SetColliderTransform_Right_Leg(_float4x4 Transform);
	void			SetColliderTransform_Left_Shin(_float4x4 Transform);
	void			SetColliderTransform_Right_Shin(_float4x4 Transform);
	void			SetJointTransform_Neck(_float4x4 Transform);
	void			SetJointTransform_L_Shoulder(_float4x4 Transform);
	void			SetJointTransform_R_Shoulder(_float4x4 Transform);
	void			SetJointTransform_Spine(_float4x4 Transform);
	void			SetJointTransform_Acetabulum_L(_float4x4 Transform);
	void			SetJointTransform_Acetabulum_R(_float4x4 Transform);
	void			SetJointTransform_Knee_L(_float4x4 Transform);
	void			SetJointTransform_Knee_R(_float4x4 Transform);

	PxTransform		ColliderTransform = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		ColliderTransform_Head = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		ColliderTransform_Right_Arm = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		ColliderTransform_Left_Arm = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		ColliderTransform_Right_ForeArm = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		ColliderTransform_Left_ForeArm = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		ColliderTransform_Pelvis = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		ColliderTransform_Left_Leg = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		ColliderTransform_Right_Leg = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		ColliderTransform_Left_Shin = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		ColliderTransform_Right_Shin = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		JointTransform_Neck = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		JointTransform_Spine = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		JointTransform_Shoulder_L = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		JointTransform_Shoulder_R = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		JointTransform_Acetabulum_L = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		JointTransform_Acetabulum_R = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		JointTransform_Knee_L = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform		JointTransform_Knee_R = { PxTransform(PxVec3(0.f,0.f,0.f)) };

	PxShape* m_Shape_Head = { nullptr };
	PxShape* m_Shape_Body = { nullptr };
	PxShape* m_Shape_Right_Arm = { nullptr };
	PxShape* m_Shape_Left_Arm = { nullptr };
	PxShape* m_Shape_Right_ForeArm = { nullptr };
	PxShape* m_Shape_Left_ForeArm = { nullptr };
	PxShape* m_Shape_Pelvis = { nullptr };
	PxShape* m_Shape_Left_Leg = { nullptr };
	PxShape* m_Shape_Right_Leg = { nullptr };
	PxShape* m_Shape_Left_Shin = { nullptr };
	PxShape* m_Shape_Right_Shin = { nullptr };

	_bool	 m_bRagdoll = { false };
public:
	static CPhysics_Controller* Create();

protected:
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	PxDefaultAllocator m_DefaultAllocatorCallback;
	PxDefaultErrorCallback m_DefaultErrorCallback;
	PxDefaultCpuDispatcher* m_Dispatcher = nullptr;
	PxTolerancesScale		m_ToleranceScale;
	PxFoundation* m_Foundation = nullptr;
	PxPhysics* m_Physics = nullptr;
	PxScene* m_Scene = nullptr;
	PxMaterial* m_Material = nullptr;
	PxPvd* m_Pvd = nullptr;
	class CEventCallBack* m_EventCallBack = { nullptr };
	class CFilterCallBack* m_FilterCallBack = { nullptr };
	vector<PxRigidDynamic*> m_vecBody;
	PxControllerManager* m_Manager = { nullptr };
	PxController* m_Controller = { nullptr };

	PxVec3 m_Look;
	PxVec3 m_JumpVel;

	_float3* m_pVerticesPos = { nullptr };
	_uint* m_pIndices = { nullptr };

	PxTriangleMesh* m_TerrainMesh = { nullptr };
	PxRigidStatic* m_GroundPlane = { nullptr };
	PxRigidStatic* m_TerrainActor = { nullptr };


	PxPvdSceneClient* m_PvdClient = { nullptr };
	PxPvdTransport* m_Transport = { nullptr };

	PxCapsuleControllerDesc m_Controll_Desc;

	PxShape* m_Shape = { nullptr };

	vector<class CCharacter_Controller*> m_vecCharacter_Controller;
	vector<class CRigid_Dynamic*> m_vecRigid_Dynamic;


	_int						m_iCharacter_Controller_Count = { 0 };
	_int						m_iRigid_Dynamic_Count = { 0 };
	_int						m_iRigid_Static_Count = { 0 };

	_int						m_iMapMeshCount = { 0 };
	vector<PxRigidStatic*>			m_vecFullMapObject;



	//Ragdoll
	PxRigidDynamic*				m_pHead = { nullptr };
	PxRigidDynamic*				m_pBody = { nullptr };
	PxRigidDynamic*				m_pRightArm = { nullptr };
	PxRigidDynamic*				m_pRightForeArm = { nullptr };
	PxRigidDynamic*				m_pLeftArm = { nullptr };
	PxRigidDynamic*				m_pLeftForeArm = { nullptr };
	PxRigidDynamic*				m_pPelvis = { nullptr };
	PxRigidDynamic*				m_pRightLeg = { nullptr };
	PxRigidDynamic*				m_pRightShin = { nullptr };
	PxRigidDynamic*				m_pLeftLeg = { nullptr };
	PxRigidDynamic*				m_pLeftShin = { nullptr };

	PxSphericalJoint*			m_pNeck_Joint = { nullptr };
	PxSphericalJoint*			m_pPelvis_Joint = { nullptr };
	PxSphericalJoint*			m_pLeft_Shoulder_Joint = { nullptr };
	PxSphericalJoint*			m_pRight_Shoulder_Joint = { nullptr };
	PxSphericalJoint*			m_pLeft_Elbow_Joint = { nullptr };
	PxSphericalJoint*			m_pRight_Elbow_Joint = { nullptr };
	PxSphericalJoint*			m_pRight_Knee_Joint = { nullptr };
	PxSphericalJoint*			m_pLeft_Knee_Joint = { nullptr };
	PxSphericalJoint*			m_pRight_Hip_Joint = { nullptr };
	PxSphericalJoint*			m_pLeft_Hip_Joint = { nullptr };
	PxRigidDynamic* m_BodyCollider = { nullptr };
	PxRigidDynamic* m_HeadCollider = { nullptr };
	PxRigidDynamic* m_Left_Arm_Collider = { nullptr };
	PxRigidDynamic* m_Right_Arm_Collider = { nullptr };
	PxRigidDynamic* m_Left_ForeArm_Collider = { nullptr };
	PxRigidDynamic* m_Right_ForeArm_Collider = { nullptr };
	PxRigidDynamic* m_Pelvis_Collider = { nullptr };
	PxRigidDynamic* m_Left_Leg_Collider = { nullptr };
	PxRigidDynamic* m_Right_Leg_Collider = { nullptr };
	PxRigidDynamic* m_Left_Shin_Collider = { nullptr };
	PxRigidDynamic* m_Right_Shin_Collider = { nullptr };
private:
	_bool			m_bJump = { false };
public:
	virtual void Free() override;
};

END