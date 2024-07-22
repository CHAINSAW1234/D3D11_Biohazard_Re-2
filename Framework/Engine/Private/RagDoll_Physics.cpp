#include "Physics_Controller.h"
#include "RagDoll_Physics.h"
#include "RagDoll.h"
#include "Skeletal_Mesh.h"
#include "Skeleton.h"
#include "Bone.h"
#include "Transform.h"
#include "GameInstance.h"

#define ZOMBIE
#define CHEST_SIZE 7.f
#define SIZE_MAG 1.5f


#pragma region Zombie Bone - Joint

#ifdef ZOMBIE

#define BONE_COUNT 150

#define JOINT_COUNT 20

#define NECK_BONE  143		//neck_0
#define SPINE_01_BONE 46		//spine_0
#define SPINE_02_BONE 59		//spine_1
#define SPINE_03_BONE 60		//spine_2
#define HEAD_BONE 144		//neck_1
#define PELVIS_BONE 3			//hips
#define L_LEG_BONE 4			//l_leg_femur
#define L_CALF_BONE 9		//l_leg_tibia
#define L_ANCLE_BONE 14		//l_leg_ankle
#define L_FOOT_BONE 15		//l_leg_ball
#define R_LEG_BONE 20		//r_leg_femur
#define R_CALF_BONE 21		//r_leg_tibia
#define R_ANCLE_BONE 26		//r_leg_ankle
#define R_FOOT_BONE 27		//r_leg_ball
#define L_ARM_BONE 62		//l_arm_humerus
#define L_FOREARM_BONE 67	//l_arm_radius
#define L_WRIST_BONE 70		//l_arm_wrist
#define L_HAND_BONE 77		//l_hand_middle_0
#define R_ARM_BONE 103		//r_arm_humerus
#define R_FOREARM_BONE 108	//r_arm_radius
#define R_WRIST_BONE 111		//r_arm_wrist
#define R_HAND_BONE 118		//r_hand_middle_0

#endif

#pragma endregion

#define MODEL_SCALE 0.009f
#define SIZE_VALUE 2.f
#define JOINT_GAP 0.1f
#define RADIUS 2.5f


PxRigidDynamic* CRagdoll_Physics::create_capsule_bone(uint32_t parent_idx, uint32_t child_idx, CRagdoll& ragdoll, float r, XMMATRIX rotation, COLLIDER_TYPE eType)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();
	XMVECTOR parent_pos = XMLoadFloat3(&joints[parent_idx].bind_pos_ws(m_model));
	parent_pos = XMVectorSetW(parent_pos, 1.f);
	XMVECTOR child_pos = XMLoadFloat3(&joints[child_idx].bind_pos_ws(m_model));
	child_pos = XMVectorSetW(child_pos, 1.f);

	XMVECTOR diff = XMVectorSubtract(parent_pos, child_pos);
	float len = XMVectorGetX(XMVector3Length(diff));
	len -= len * JOINT_GAP;

	r *= (MODEL_SCALE);

	float len_minus_2r = len - 2.0f * r;
	float half_height = len_minus_2r / 2.0f;

#pragma region 크기 줄이는 코드
	half_height *= MODEL_SCALE;
#pragma endregion

	XMVECTOR body_pos = XMVectorScale(XMVectorAdd(parent_pos, child_pos), 0.5f);
	body_pos = XMVectorSetW(body_pos, 1.f);

	r *= SIZE_VALUE;
	PxShape* shape = m_Physics->createShape(PxCapsuleGeometry(r, half_height), *m_material);

	PxFilterData filterData_Ragdoll;
	filterData_Ragdoll.word0 = COLLISION_CATEGORY::RAGDOLL;
	filterData_Ragdoll.word1 = COLLISION_CATEGORY::CCT | COLLISION_CATEGORY::COLLIDER;
	filterData_Ragdoll.word2 = m_iId;
	filterData_Ragdoll.word3 = eType;
	shape->setSimulationFilterData(filterData_Ragdoll);

	XMVECTOR rot_quat = XMQuaternionRotationMatrix(rotation);
	PxTransform local(to_quat(rot_quat));
	shape->setLocalPose(local);

	XMMATRIX inv_bind_pose = joints[parent_idx].inverse_bind_pose;
	inv_bind_pose.r[3] = XMVectorZero();
	inv_bind_pose.r[3] = XMVectorSetW(inv_bind_pose.r[3], 1.0f);
	XMMATRIX bind_pose = XMMatrixInverse(nullptr, inv_bind_pose);
	XMMATRIX bind_pose_ws = XMMatrixMultiply(m_model_without_scale, bind_pose);

	PxTransform px_transform(to_vec3(body_pos), to_quat(XMQuaternionRotationMatrix(bind_pose_ws)));

	PxRigidDynamic* body = m_Physics->createRigidDynamic(px_transform);
	body->setMass(m_mass);

	body->attachShape(*shape);

	m_ragdoll->m_rigid_bodies[parent_idx] = body;
	//body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	float sleepThreshold = 1.f;
	body->setSleepThreshold(sleepThreshold);

	return body;
}

PxRigidDynamic* CRagdoll_Physics::create_capsule_bone(uint32_t parent_idx, CRagdoll& ragdoll, XMVECTOR offset, float l, float r, XMMATRIX rotation, COLLIDER_TYPE eType)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	XMVECTOR parent_pos = XMLoadFloat3(&joints[parent_idx].bind_pos_ws(m_model)) + offset;
	parent_pos = XMVectorSetW(parent_pos, 1.f);

#pragma region 크기 줄이는 코드
	l *= MODEL_SCALE;
	r *= (MODEL_SCALE);
#pragma endregion

	r *= SIZE_VALUE;
	PxShape* shape = m_Physics->createShape(PxCapsuleGeometry(r, l), *m_material);

	PxFilterData filterData_Ragdoll;
	filterData_Ragdoll.word0 = COLLISION_CATEGORY::RAGDOLL;
	filterData_Ragdoll.word1 = COLLISION_CATEGORY::CCT | COLLISION_CATEGORY::COLLIDER;
	filterData_Ragdoll.word2 = m_iId;
	filterData_Ragdoll.word3 = eType;
	shape->setSimulationFilterData(filterData_Ragdoll);

	PxTransform local(to_quat(XMQuaternionRotationMatrix(rotation)));
	shape->setLocalPose(local);

	XMMATRIX invBindPose = joints[parent_idx].inverse_bind_pose;
	XMVECTOR scale, rotationQuat, translation;
	XMMatrixDecompose(&scale, &rotationQuat, &translation, invBindPose);
	XMMATRIX invBindPoseWithoutTranslation = XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(rotationQuat);

	XMMATRIX inv_bind_pose = invBindPoseWithoutTranslation;
	inv_bind_pose.r[3] = XMVectorZero();
	inv_bind_pose.r[3] = XMVectorSetW(inv_bind_pose.r[3], 1.0f);
	XMMATRIX bind_pose = XMMatrixInverse(nullptr, inv_bind_pose);
	XMMATRIX bind_pose_ws = XMMatrixMultiply(m_model_without_scale, bind_pose);

	PxTransform px_transform(to_vec3(parent_pos), to_quat(XMQuaternionRotationMatrix(bind_pose_ws)));

	PxRigidDynamic* body = m_Physics->createRigidDynamic(px_transform);

	body->attachShape(*shape);
	body->setMass(m_mass);

	m_ragdoll->m_rigid_bodies[parent_idx] = body;
	//body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	float sleepThreshold = 0.5f;
	body->setSleepThreshold(sleepThreshold);
	return body;
}

PxRigidDynamic* CRagdoll_Physics::create_sphere_bone(uint32_t parent_idx, CRagdoll& ragdoll, float r, COLLIDER_TYPE eType)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();
	XMVECTOR parent_pos = XMLoadFloat3(&joints[parent_idx].bind_pos_ws(m_model));
	parent_pos = XMVectorSetW(parent_pos, 1.f);

#pragma region 크기 줄이는 코드
	r *= (MODEL_SCALE);
#pragma endregion

	PxShape* shape = m_Physics->createShape(PxSphereGeometry(r), *m_material);

	PxFilterData filterData_Ragdoll;
	filterData_Ragdoll.word0 = COLLISION_CATEGORY::RAGDOLL;
	filterData_Ragdoll.word1 = COLLISION_CATEGORY::CCT | COLLISION_CATEGORY::COLLIDER;
	filterData_Ragdoll.word2 = m_iId;
	filterData_Ragdoll.word3 = eType;
	shape->setSimulationFilterData(filterData_Ragdoll);

	PxRigidDynamic* body = m_Physics->createRigidDynamic(PxTransform(to_vec3(parent_pos)));

	body->attachShape(*shape);
	body->setMass(m_mass);

	m_ragdoll->m_rigid_bodies[parent_idx] = body;
	//body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	float sleepThreshold = 0.5f;
	body->setSleepThreshold(sleepThreshold);

	return body;
}

PxD6Joint* CRagdoll_Physics::create_d6_joint(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t Bone_Pos, uint32_t Joint_Pos)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	_matrix p_tf = XMLoadFloat4x4((*m_vecBone)[Bone_Pos]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
	_vector p = XMVectorSet(p_tf.r[3].m128_f32[0], p_tf.r[3].m128_f32[1], p_tf.r[3].m128_f32[2], 1.0f);

	_vector q = XMQuaternionRotationMatrix(XMMatrixInverse(nullptr, joints[Joint_Pos].inverse_bind_pose));

	PxD6Joint* joint = PxD6JointCreate(*m_Physics,
		parent,
		parent->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))),
		child,
		child->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))));

	joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	config_d6_joint(PxReal(3.14 / 4.f), PxReal(3.14f / 4.f), PxReal(-3.14f / 8.f), PxReal(3.14f / 8.f), joint);

	joint->setBreakForce(FLT_MAX, FLT_MAX);

	return joint;
}

PxD6Joint* CRagdoll_Physics::create_d6_joint_Foot(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t Bone_Pos, uint32_t Joint_Pos)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	_matrix p_tf = XMLoadFloat4x4((*m_vecBone)[Bone_Pos]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
	_vector p = XMVectorSet(p_tf.r[3].m128_f32[0], p_tf.r[3].m128_f32[1], p_tf.r[3].m128_f32[2], 1.0f);

	_vector q = XMQuaternionRotationMatrix(XMMatrixInverse(nullptr, joints[Joint_Pos].inverse_bind_pose));

	PxD6Joint* joint = PxD6JointCreate(*m_Physics,
		parent,
		parent->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))),
		child,
		child->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))));

	joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	config_d6_joint(PxReal(3.14 / 8.f), PxReal(3.14f / 8.f), PxReal(-3.14f / 16.f), PxReal(3.14f / 16.f), joint);

	joint->setBreakForce(FLT_MAX, FLT_MAX);

	return joint;
}

PxD6Joint* CRagdoll_Physics::create_d6_joint_Head(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t Bone_Pos, uint32_t Joint_Pos)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	_matrix p_tf = XMLoadFloat4x4((*m_vecBone)[Bone_Pos]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
	_vector p = XMVectorSet(p_tf.r[3].m128_f32[0], p_tf.r[3].m128_f32[1], p_tf.r[3].m128_f32[2], 1.0f);

	_vector q = XMQuaternionRotationMatrix(XMMatrixInverse(nullptr, joints[Joint_Pos].inverse_bind_pose));

	PxD6Joint* joint = PxD6JointCreate(*m_Physics,
		parent,
		parent->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))),
		child,
		child->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))));

	joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	config_d6_joint(PxReal(3.14 / 12.f), PxReal(3.14f / 12.f), PxReal(-3.14f / 20.f), PxReal(3.14f / 20.f), joint);

	joint->setBreakForce(FLT_MAX, FLT_MAX);

	return joint;
}

void CRagdoll_Physics::create_revolute_joint(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t joint_pos, XMMATRIX rotation)
{
	//m_pRotationMatrix = &m_pTransform->Get_RotationMatrix_Pure();
	m_RotationMatrix = m_pTransform->Get_WorldMatrix_Pure();

	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	_matrix p_tf = XMLoadFloat4x4((*m_vecBone)[joint_pos]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
	_vector p = XMVectorSet(p_tf.r[3].m128_f32[0], p_tf.r[3].m128_f32[1], p_tf.r[3].m128_f32[2], 1.0f);

	p = XMVectorSetW(p, 1.f);
	_vector q = XMQuaternionRotationMatrix(XMMatrixInverse(nullptr, joints[joint_pos].inverse_bind_pose)/* * XMMatrixRotationY(PxPi)*/ * XMLoadFloat4x4(&m_RotationMatrix));

	PxRevoluteJoint* joint = PxRevoluteJointCreate(*m_Physics,
		parent,
		parent->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))),
		child,
		child->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))));

	joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	joint->setLimit(PxJointAngularLimitPair(-PxPi, 0.f));
	joint->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
}

void CRagdoll_Physics::config_d6_joint(physx::PxReal swing0, physx::PxReal swing1, physx::PxReal twistLo, physx::PxReal twistHi, physx::PxD6Joint* joint)
{
	joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLIMITED);
	joint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLIMITED);
	joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLIMITED);

	joint->setSwingLimit(physx::PxJointLimitCone(swing0, swing1));
	joint->setTwistLimit(physx::PxJointAngularLimitPair(twistLo, twistHi));

#pragma region Joint Drive Setting
	//PxD6JointDrive drive;
	//drive.stiffness = 1000;  // 스프링의 강도
	//drive.damping = 100;     // 감쇠
	//drive.forceLimit = PX_MAX_F32; // 힘의 한계. PX_MAX_F32는 실질적으로 무한대를 의미
	//drive.flags = PxD6JointDriveFlag::eACCELERATION; // 가속도 기반으로 드라이브를 계산

	//// SLERP 드라이브에 스프링 설정을 적용
	//joint->setDrive(PxD6Drive::eSLERP, drive);

	//PxTolerancesScale scale;
	//PxReal limitValue = 10.0f;
	//PxReal stiffness = 0.0f; // 스프링 강도
	//PxReal damping = 0.0f; // 감쇠

	//// 선형 제한 설정
	//PxJointLinearLimit linearLimit(limitValue);
	//joint->setLinearLimit(linearLimit);
#pragma endregion

	joint->setBreakForce(FLT_MAX, FLT_MAX); // 최대 힘과 토크를 무한대로 설정
}

CRagdoll_Physics::CRagdoll_Physics(PxScene* Scene, PxPhysics* Physics, PxDefaultAllocator allocator, PxDefaultErrorCallback Callback,
	PxFoundation* Foundation, PxDefaultCpuDispatcher* dispatcher, PxMaterial* Material)
{
	m_Scene = Scene;
	m_Physics = Physics;
	m_allocator = allocator;
	m_error_callback = Callback;
	m_foundation = Foundation;
	m_dispatcher = dispatcher;
	m_material = Material;

	m_FilterData.word0 = COLLISION_CATEGORY::RAGDOLL;
	m_FilterData.word1 = COLLISION_CATEGORY::CCT | COLLISION_CATEGORY::COLLIDER;
}

CRagdoll_Physics::CRagdoll_Physics(const CRagdoll_Physics& rhs)
{
}

HRESULT CRagdoll_Physics::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CRagdoll_Physics::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

bool CRagdoll_Physics::load_mesh(const string& name)
{
	m_skeletal_mesh = SkeletalMesh::load(name);

	if (!m_skeletal_mesh)
	{
		return false;
	}

	return true;
}

_bool CRagdoll_Physics::Init(const string& name, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_ragdoll = new CRagdoll();

	m_selected_joints.resize(JOINT_COUNT);

	for (int i = 0; i < JOINT_COUNT; i++)
		m_selected_joints[i] = 0;

	if (!load_mesh(name))
		return false;

	m_joint_names.push_back(" -");

	for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		m_joint_names.push_back(m_skeletal_mesh->skeleton()->joints()[i].name);

	for (int i = 0; i < MAX_BONES; i++)
		m_pose_transforms.transforms[i] = XMMatrixIdentity();

	m_index_stack.reserve(MAX_BONES);
	m_joint_pos.reserve(MAX_BONES);

	m_joint_mat.resize(MAX_BONES);

	update_transforms();

	m_ragdoll_pose = new AnimRagdoll(m_skeletal_mesh->skeleton());

	SetBoneIndex();

	m_ragdoll->m_rigid_bodies.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_relative_joint_pos.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_body_rotations.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_body_pos_relative_to_joint.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_joint_rotations.resize(m_skeletal_mesh->skeleton()->num_bones());

#pragma region 부위 파괴
	m_ragdoll->m_rigid_bodies_BreakPart.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_relative_joint_pos_BreakPart.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_body_rotations_BreakPart.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_body_pos_relative_to_joint_BreakPart.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_joint_rotations_BreakPart.resize(m_skeletal_mesh->skeleton()->num_bones());
#pragma endregion 

#pragma region 옷
	m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_L.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_R.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_L.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_R.resize(m_skeletal_mesh->skeleton()->num_bones());

	m_ragdoll->m_relative_joint_pos_BreakPart_Cloth_Arm_L.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_body_rotations_BreakPart_Cloth_Arm_L.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_body_pos_relative_to_joint_BreakPart_Cloth_Arm_L.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_joint_rotations_BreakPart_Cloth_Arm_L.resize(m_skeletal_mesh->skeleton()->num_bones());

	m_ragdoll->m_relative_joint_pos_BreakPart_Cloth_Arm_R.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_body_rotations_BreakPart_Cloth_Arm_R.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_body_pos_relative_to_joint_BreakPart_Cloth_Arm_R.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_joint_rotations_BreakPart_Cloth_Arm_R.resize(m_skeletal_mesh->skeleton()->num_bones());

	m_ragdoll->m_relative_joint_pos_BreakPart_Cloth_Leg_L.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_body_rotations_BreakPart_Cloth_Leg_L.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_body_pos_relative_to_joint_BreakPart_Cloth_Leg_L.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_joint_rotations_BreakPart_Cloth_Leg_L.resize(m_skeletal_mesh->skeleton()->num_bones());

	m_ragdoll->m_relative_joint_pos_BreakPart_Cloth_Leg_R.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_body_rotations_BreakPart_Cloth_Leg_R.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_body_pos_relative_to_joint_BreakPart_Cloth_Leg_R.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_joint_rotations_BreakPart_Cloth_Leg_R.resize(m_skeletal_mesh->skeleton()->num_bones());
#pragma endregion

	m_vecBreak_Parent_Flag.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_vecBreak_Parent_Flag_Cloth_Arm_L.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_vecBreak_Parent_Flag_Cloth_Arm_R.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_vecBreak_Parent_Flag_Cloth_Leg_L.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_vecBreak_Parent_Flag_Cloth_Leg_R.resize(m_skeletal_mesh->skeleton()->num_bones());

	m_vecBreakPartFilter.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_vecBreakPartFilter_Cloth_Arm_L.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_vecBreakPartFilter_Cloth_Arm_R.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_vecBreakPartFilter_Cloth_Leg_L.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_vecBreakPartFilter_Cloth_Leg_R.resize(m_skeletal_mesh->skeleton()->num_bones());

	for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
	{
		m_ragdoll->m_rigid_bodies[i] = nullptr;
		m_ragdoll->m_rigid_bodies_BreakPart[i] = nullptr;
		m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_L[i] = nullptr;
		m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_R[i] = nullptr;
		m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_L[i] = nullptr;
		m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_R[i] = nullptr;
		m_vecBreakPartFilter[i] = false;
		m_vecBreakPartFilter_Cloth_Arm_L[i] = false;
		m_vecBreakPartFilter_Cloth_Arm_R[i] = false;
		m_vecBreakPartFilter_Cloth_Leg_L[i] = false;
		m_vecBreakPartFilter_Cloth_Leg_R[i] = false;
		m_vecBreak_Parent_Flag[i] = false;
		m_vecBreak_Parent_Flag_Cloth_Arm_L[i] = false;
		m_vecBreak_Parent_Flag_Cloth_Arm_R[i] = false;
		m_vecBreak_Parent_Flag_Cloth_Leg_L[i] = false;
		m_vecBreak_Parent_Flag_Cloth_Leg_R[i] = false;
	}

	m_pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(m_pGameInstance);

#pragma region Sound
	m_pTransformCom_Sound = CTransform::Create(pDevice, pContext);
	if (nullptr == m_pTransformCom_Sound)
		return E_FAIL;

	m_pGameInstance->Add_Object_Sound(m_pTransformCom_Sound, 10);

	m_vecRagdollSound_Filter.clear();
	m_vecRagdollSound_Filter.resize(13,false);
#pragma endregion

	return true;
}

void CRagdoll_Physics::update_transforms()
{
	XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	m_model_without_scale = XMMatrixMultiply(m_model_without_scale, translationMatrix);

	XMMATRIX scaleMatrix = XMMatrixScaling(m_scale, m_scale, m_scale);
	m_model = XMMatrixMultiply(m_model_without_scale, scaleMatrix);

	m_model_only_scale = XMMatrixIdentity();
	m_model_only_scale = XMMatrixScaling(m_scale, m_scale, m_scale);
}

void CRagdoll_Physics::Set_Kinematic(_bool boolean)
{
	m_simulate = true;

	m_ragdoll->set_kinematic(boolean);
}

void CRagdoll_Physics::SetSimulate(_bool boolean)
{
	Update(1 / 60.f);

	m_simulate = boolean;

	m_ragdoll->set_kinematic(boolean);

	Update(1 / 60.f);
}

void CRagdoll_Physics::create_ragdoll()
{
	if (!m_vecBone)
		return;

	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	// ---------------------------------------------------------------------------------------------------------------
	// Create rigid bodies for limbs
	// ---------------------------------------------------------------------------------------------------------------

	float     r = RADIUS * m_scale;
	_matrix rot = XMMatrixIdentity();
	rot = XMMatrixRotationZ(XM_PI * 0.5f);

#ifdef ZOMBIE
	if (m_Pelvis == nullptr)
	{
		m_Pelvis = create_capsule_bone(m_pelvis_idx, m_spine_02_idx, *m_ragdoll, CHEST_SIZE * m_scale, rot, COLLIDER_TYPE::PELVIS);
		m_Scene->addActor(*m_Pelvis);
	}
#endif

	if (m_Head == nullptr)
	{
		m_Head = create_capsule_bone(m_head_idx, *m_ragdoll, XMVectorSet(0.0f, 3.0f * m_scale, 0.0f, 1.f), 4.0f * m_scale, 6.0f * m_scale, rot, COLLIDER_TYPE::HEAD);
		m_Scene->addActor(*m_Head);
	}
	if (m_Leg_L == nullptr)
	{
		m_Leg_L = create_capsule_bone(m_thigh_l_idx, m_calf_l_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::LEG_L);
		m_Scene->addActor(*m_Leg_L);
	}
	if (m_Leg_R == nullptr)
	{
		m_Leg_R = create_capsule_bone(m_thigh_r_idx, m_calf_r_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::LEG_R);
		m_Scene->addActor(*m_Leg_R);
	}

#ifdef ZOMBIE
	if (m_Chest == nullptr)
	{
		m_Chest = create_capsule_bone(m_spine_02_idx, m_neck_01_idx, *m_ragdoll, CHEST_SIZE * m_scale, rot, COLLIDER_TYPE::CHEST);
		m_Scene->addActor(*m_Chest);
	}
#endif

	if (m_Calf_L == nullptr)
	{
		m_Calf_L = create_capsule_bone(m_calf_l_idx, m_foot_l_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::CALF_L);
		m_Scene->addActor(*m_Calf_L);
	}
	if (m_Calf_R == nullptr)
	{
		m_Calf_R = create_capsule_bone(m_calf_r_idx, m_foot_r_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::CALF_R);
		m_Scene->addActor(*m_Calf_R);
	}

	if (m_Arm_L == nullptr)
	{
		m_Arm_L = create_capsule_bone(m_upperarm_l_idx, m_lowerarm_l_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::ARM_L);
		m_Scene->addActor(*m_Arm_L);
	}

	if (m_Arm_R == nullptr)
	{
		m_Arm_R = create_capsule_bone(m_upperarm_r_idx, m_lowerarm_r_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::ARM_R);
		m_Scene->addActor(*m_Arm_R);
	}

	if (m_ForeArm_L == nullptr)
	{
		m_ForeArm_L = create_capsule_bone(m_lowerarm_l_idx, m_hand_l_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::FOREARM_L);
		m_Scene->addActor(*m_ForeArm_L);
	}

	if (m_ForeArm_R == nullptr)
	{
		m_ForeArm_R = create_capsule_bone(m_lowerarm_r_idx, m_hand_r_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::FOREARM_R);
		m_Scene->addActor(*m_ForeArm_R);
	}

	if (m_Hand_L == nullptr)
	{
		m_Hand_L = create_capsule_bone(m_hand_l_idx, m_middle_01_l_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::HAND_L);
		m_Scene->addActor(*m_Hand_L);
	}
	if (m_Hand_R == nullptr)
	{
		m_Hand_R = create_capsule_bone(m_hand_r_idx, m_middle_01_r_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::HAND_R);
		m_Scene->addActor(*m_Hand_R);
	}

	rot = XMMatrixRotationY(PxPi * 0.5f);

	if (m_Foot_L == nullptr)
	{
		m_Foot_L = create_capsule_bone(m_foot_l_idx, m_ball_l_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::FOOT_L);
		m_Scene->addActor(*m_Foot_L);
	}
	if (m_Foot_R == nullptr)
	{
		m_Foot_R = create_capsule_bone(m_foot_r_idx, m_ball_r_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::FOOT_R);
		m_Scene->addActor(*m_Foot_R);
	}

#ifdef ZOMBIE
	m_ragdoll->m_rigid_bodies[1] = m_Pelvis;
#endif

#pragma endregion

	if (m_Arm_L_High)
	{
		joints[m_Arm_L_Twist_0].parent_index = m_upperarm_high_l_idx;
		joints[m_Arm_L_Twist_1].parent_index = m_upperarm_high_l_idx;
		joints[m_Arm_L_Twist_2].parent_index = m_upperarm_high_l_idx;
		joints[m_Arm_L_Twist_3].parent_index = m_upperarm_high_l_idx;
		m_ragdoll->m_rigid_bodies[m_Arm_L_Twist_0] = m_Arm_L_High;
		m_ragdoll->m_rigid_bodies[m_Arm_L_Twist_1] = m_Arm_L_High;
		m_ragdoll->m_rigid_bodies[m_Arm_L_Twist_2] = m_Arm_L_High;
		m_ragdoll->m_rigid_bodies[m_Arm_L_Twist_3] = m_Arm_L_High;
		//m_vecBreakPartFilter[m_upperarm_high_l_idx] = false;
	}

	if (m_ForeArm_L_High)
	{
		joints[m_ForeArm_L_Twist_0].parent_index = m_lowerarm_l_idx;
		joints[m_ForeArm_L_Twist_1].parent_index = m_lowerarm_l_idx;
		joints[m_ForeArm_L_Twist_2].parent_index = m_lowerarm_l_idx;
		joints[m_ForeArm_L_Twist_3].parent_index = m_lowerarm_l_idx;
		m_ragdoll->m_rigid_bodies[m_ForeArm_L_Twist_0] = m_ForeArm_L_High;
		m_ragdoll->m_rigid_bodies[m_ForeArm_L_Twist_1] = m_ForeArm_L_High;
		m_ragdoll->m_rigid_bodies[m_ForeArm_L_Twist_2] = m_ForeArm_L_High;
		m_ragdoll->m_rigid_bodies[m_ForeArm_L_Twist_3] = m_ForeArm_L_High;
		m_vecBreakPartFilter[m_lowerarm_l_idx] = false;
	}

	if (m_Arm_R_High)
	{
		joints[m_Arm_R_Twist_0].parent_index = m_upperarm_high_r_idx;
		joints[m_Arm_R_Twist_1].parent_index = m_upperarm_high_r_idx;
		joints[m_Arm_R_Twist_2].parent_index = m_upperarm_high_r_idx;
		joints[m_Arm_R_Twist_3].parent_index = m_upperarm_high_r_idx;
		m_ragdoll->m_rigid_bodies[m_Arm_R_Twist_0] = m_Arm_R_High;
		m_ragdoll->m_rigid_bodies[m_Arm_R_Twist_1] = m_Arm_R_High;
		m_ragdoll->m_rigid_bodies[m_Arm_R_Twist_2] = m_Arm_R_High;
		m_ragdoll->m_rigid_bodies[m_Arm_R_Twist_3] = m_Arm_R_High;
		//m_vecBreakPartFilter[m_upperarm_high_r_idx] = false;
	}

	if (m_ForeArm_R_High)
	{
		joints[m_ForeArm_R_Twist_0].parent_index = m_lowerarm_r_idx;
		joints[m_ForeArm_R_Twist_1].parent_index = m_lowerarm_r_idx;
		joints[m_ForeArm_R_Twist_2].parent_index = m_lowerarm_r_idx;
		joints[m_ForeArm_R_Twist_3].parent_index = m_lowerarm_r_idx;
		m_ragdoll->m_rigid_bodies[m_ForeArm_R_Twist_0] = m_ForeArm_R_High;
		m_ragdoll->m_rigid_bodies[m_ForeArm_R_Twist_1] = m_ForeArm_R_High;
		m_ragdoll->m_rigid_bodies[m_ForeArm_R_Twist_2] = m_ForeArm_R_High;
		m_ragdoll->m_rigid_bodies[m_ForeArm_R_Twist_3] = m_ForeArm_R_High;
		m_vecBreakPartFilter[m_lowerarm_r_idx] = false;
	}

	if (m_Calf_L_High)
	{
		joints[m_Calf_L_Twist_0].parent_index = m_calf_l_idx;
		joints[m_Calf_L_Twist_1].parent_index = m_calf_l_idx;
		m_ragdoll->m_rigid_bodies[m_Calf_L_Twist_0] = m_Calf_L_High;
		m_ragdoll->m_rigid_bodies[m_Calf_L_Twist_1] = m_Calf_L_High;
		m_vecBreakPartFilter[m_calf_l_idx] = false;
	}

	if (m_Calf_R_High)
	{
		joints[m_Calf_R_Twist_0].parent_index = m_calf_r_idx;
		joints[m_Calf_R_Twist_1].parent_index = m_calf_r_idx;
		m_ragdoll->m_rigid_bodies[m_Calf_R_Twist_0] = m_Calf_R_High;
		m_ragdoll->m_rigid_bodies[m_Calf_R_Twist_1] = m_Calf_R_High;
		m_vecBreakPartFilter[m_calf_r_idx] = false;
	}

	if (m_Leg_L_High)
	{
		joints[m_Leg_L_Twist_0].parent_index = m_thigh_l_idx;
		joints[m_Leg_L_Twist_1].parent_index = m_thigh_l_idx;
		joints[m_Leg_L_Twist_2].parent_index = m_thigh_l_idx;
		m_ragdoll->m_rigid_bodies[m_Leg_L_Twist_0] = m_Leg_L_High;
		m_ragdoll->m_rigid_bodies[m_Leg_L_Twist_1] = m_Leg_L_High;
		m_ragdoll->m_rigid_bodies[m_Leg_L_Twist_2] = m_Leg_L_High;
		m_vecBreakPartFilter[m_thigh_l_idx] = false;
	}

	if (m_Leg_R_High)
	{
		joints[m_Leg_R_Twist_0].parent_index = m_thigh_r_idx;
		joints[m_Leg_R_Twist_1].parent_index = m_thigh_r_idx;
		joints[m_Leg_R_Twist_2].parent_index = m_thigh_r_idx;
		m_ragdoll->m_rigid_bodies[m_Leg_R_Twist_0] = m_Leg_R_High;
		m_ragdoll->m_rigid_bodies[m_Leg_R_Twist_1] = m_Leg_R_High;
		m_ragdoll->m_rigid_bodies[m_Leg_R_Twist_2] = m_Leg_R_High;
		m_vecBreakPartFilter[m_thigh_r_idx] = false;
	}

	auto iNumBone = m_skeletal_mesh->skeleton()->num_bones();

	for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
	{

		uint32_t        chosen_idx;
		PxRigidDynamic* body = m_ragdoll->find_recent_body((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

		if (!body || m_vecBreakPartFilter[chosen_idx] == true)
		{
			continue;
		}

		_matrix body_global_transform = to_mat4(body->getGlobalPose());
		_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
		_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
		_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
		joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

		_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
		m_ragdoll->m_relative_joint_pos[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
		m_ragdoll->m_original_body_rotations[i] = XMQuaternionRotationMatrix(body_global_transform);

		if (m_ragdoll->m_rigid_bodies[i])
		{
			// Rigid body position relative to the joint
			_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies[i]->getGlobalPose().p), 1.0f), m);

			m_ragdoll->m_body_pos_relative_to_joint[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_joint_rotations[i] = XMQuaternionRotationMatrix(bind_pose_ws);
		}
	}

	for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
	{
		if (m_vecBreak_Parent_Flag[i] == false)
			continue;

		uint32_t        chosen_idx;
		PxRigidDynamic* body = m_ragdoll->find_recent_body_BreakPart((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

		if (!body)
		{
			continue;
		}

		_matrix body_global_transform = to_mat4(body->getGlobalPose());
		_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
		_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
		_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
		joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

		_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
		m_ragdoll->m_relative_joint_pos_BreakPart[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
		m_ragdoll->m_original_body_rotations_BreakPart[i] = XMQuaternionRotationMatrix(body_global_transform);

		if (m_ragdoll->m_rigid_bodies[i])
		{
			// Rigid body position relative to the joint
			_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies[i]->getGlobalPose().p), 1.0f), m);

			m_ragdoll->m_body_pos_relative_to_joint_BreakPart[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_joint_rotations_BreakPart[i] = XMQuaternionRotationMatrix(bind_pose_ws);
		}
	}
#pragma endregion
}

void CRagdoll_Physics::create_partial_ragdoll(COLLIDER_TYPE eType)
{
	if (!m_vecBone)
		return;

	m_bPartialRagdoll = true;

	float     r = RADIUS * m_scale;
	_matrix rot = XMMatrixIdentity();
	rot = XMMatrixRotationZ(XM_PI * 0.5f);

	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	switch (eType)
	{
	case COLLIDER_TYPE::FOREARM_L:
	{

		//m_ForeArm_L = create_capsule_bone(m_lowerarm_l_idx, m_hand_l_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::FOREARM_L);

		if (!m_ForeArm_L_High)
		{
			m_ForeArm_L_High = create_capsule_bone(m_lowerarm_l_idx, m_hand_l_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::FOREARM_L);
		}
		else
		{
			break;
		}

		m_Hand_L = create_capsule_bone(m_hand_l_idx, m_middle_01_l_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::HAND_L);
		//m_Scene->addActor(*m_ForeArm_L);
		m_Scene->addActor(*m_ForeArm_L_High);
		m_Scene->addActor(*m_Hand_L);

		m_vecBreak_Parent_Flag[m_ForeArm_L_Twist_0] = true;
		joints[m_ForeArm_L_Twist_0].parent_index_BreakPart = m_lowerarm_l_idx;
		m_vecBreak_Parent_Flag[m_ForeArm_L_Twist_1] = true;
		joints[m_ForeArm_L_Twist_1].parent_index_BreakPart = m_lowerarm_l_idx;
		m_vecBreak_Parent_Flag[m_ForeArm_L_Twist_2] = true;
		joints[m_ForeArm_L_Twist_2].parent_index_BreakPart = m_lowerarm_l_idx;
		m_vecBreak_Parent_Flag[m_ForeArm_L_Twist_3] = true;
		joints[m_ForeArm_L_Twist_3].parent_index_BreakPart = m_lowerarm_l_idx;

		//m_vecBreak_Parent_Flag[m_ForeArm_L_Twist_0] = true;
		//joints[m_ForeArm_L_Twist_0].parent_index_BreakPart = joints[m_ForeArm_L_Twist_0].parent_index;
		//m_vecBreak_Parent_Flag[m_ForeArm_L_Twist_1] = true;
		//joints[m_ForeArm_L_Twist_1].parent_index_BreakPart = joints[m_ForeArm_L_Twist_1].parent_index;
		//m_vecBreak_Parent_Flag[m_ForeArm_L_Twist_2] = true;
		//joints[m_ForeArm_L_Twist_2].parent_index_BreakPart = joints[m_ForeArm_L_Twist_2].parent_index;
		//m_vecBreak_Parent_Flag[m_ForeArm_L_Twist_3] = true;
		//joints[m_ForeArm_L_Twist_3].parent_index_BreakPart = joints[m_ForeArm_L_Twist_3].parent_index;

		joints[m_ForeArm_L_Twist_0].parent_index = m_lowerarm_l_idx;
		joints[m_ForeArm_L_Twist_1].parent_index = m_lowerarm_l_idx;
		joints[m_ForeArm_L_Twist_2].parent_index = m_lowerarm_l_idx;
		joints[m_ForeArm_L_Twist_3].parent_index = m_lowerarm_l_idx;

		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			uint32_t        chosen_idx;
			PxRigidDynamic* body = m_ragdoll->find_recent_body((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

			if (!body || m_vecBreakPartFilter[chosen_idx] == true)
				continue;

			_matrix body_global_transform = to_mat4(body->getGlobalPose());
			_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
			_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
			joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

			_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
			m_ragdoll->m_relative_joint_pos[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_body_rotations[i] = XMQuaternionRotationMatrix(body_global_transform);

			if (m_ragdoll->m_rigid_bodies[i])
			{
				// Rigid body position relative to the joint
				_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
				p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies[i]->getGlobalPose().p), 1.0f), m);

				m_ragdoll->m_body_pos_relative_to_joint[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
				m_ragdoll->m_original_joint_rotations[i] = XMQuaternionRotationMatrix(bind_pose_ws);
			}
		}

		Update_Partial(1 / 60.f);

		//if (m_ForeArm_L && m_Hand_L)
		//	m_pWrist_L_Joint = create_d6_joint(m_ForeArm_L, m_Hand_L, m_hand_l_idx_Bone, m_hand_l_idx);

		if (m_ForeArm_L_High && m_Hand_L)
			m_Radius_L_Upper_Joint = create_d6_joint(m_ForeArm_L_High, m_Hand_L, m_hand_l_idx_Bone, m_hand_l_idx);

		Update_Partial_After(1 / 60.f);

		m_vecBreakPartFilter[m_lowerarm_high_l_idx] = true;
		m_vecBreakPartFilter[m_lowerarm_l_idx] = true;
		m_vecBreakPartFilter[m_hand_l_idx] = true;

		m_vecBreakPartFilter[m_ForeArm_L_Twist_0] = true;
		m_vecBreakPartFilter[m_ForeArm_L_Twist_1] = true;
		m_vecBreakPartFilter[m_ForeArm_L_Twist_2] = true;
		m_vecBreakPartFilter[m_ForeArm_L_Twist_3] = true;

		break;
	}
	case COLLIDER_TYPE::FOREARM_R:
	{

		//m_ForeArm_R = create_capsule_bone(m_lowerarm_r_idx, m_hand_r_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::FOREARM_R);
		if (!m_ForeArm_R_High)
		{
			m_ForeArm_R_High = create_capsule_bone(m_lowerarm_r_idx, m_hand_r_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::FOREARM_R);
		}
		else
		{
			break;
		}

		m_Hand_R = create_capsule_bone(m_hand_r_idx, m_middle_01_r_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::HAND_R);
		//m_Scene->addActor(*m_ForeArm_R);
		m_Scene->addActor(*m_ForeArm_R_High);
		m_Scene->addActor(*m_Hand_R);

		m_vecBreak_Parent_Flag[m_ForeArm_R_Twist_0] = true;
		joints[m_ForeArm_R_Twist_0].parent_index_BreakPart = m_lowerarm_r_idx;
		m_vecBreak_Parent_Flag[m_ForeArm_R_Twist_1] = true;
		joints[m_ForeArm_R_Twist_1].parent_index_BreakPart = m_lowerarm_r_idx;
		m_vecBreak_Parent_Flag[m_ForeArm_R_Twist_2] = true;
		joints[m_ForeArm_R_Twist_2].parent_index_BreakPart = m_lowerarm_r_idx;
		m_vecBreak_Parent_Flag[m_ForeArm_R_Twist_3] = true;
		joints[m_ForeArm_R_Twist_3].parent_index_BreakPart = m_lowerarm_r_idx;

		//m_vecBreak_Parent_Flag[m_ForeArm_R_Twist_0] = true;
		//joints[m_ForeArm_R_Twist_0].parent_index_BreakPart = joints[m_ForeArm_R_Twist_0].parent_index;
		//m_vecBreak_Parent_Flag[m_ForeArm_R_Twist_1] = true;
		//joints[m_ForeArm_R_Twist_1].parent_index_BreakPart = joints[m_ForeArm_R_Twist_1].parent_index;
		//m_vecBreak_Parent_Flag[m_ForeArm_R_Twist_2] = true;
		//joints[m_ForeArm_R_Twist_2].parent_index_BreakPart = joints[m_ForeArm_R_Twist_2].parent_index;
		//m_vecBreak_Parent_Flag[m_ForeArm_R_Twist_3] = true;
		//joints[m_ForeArm_R_Twist_3].parent_index_BreakPart = joints[m_ForeArm_R_Twist_3].parent_index; 

		joints[m_ForeArm_R_Twist_0].parent_index = m_lowerarm_r_idx;
		joints[m_ForeArm_R_Twist_1].parent_index = m_lowerarm_r_idx;
		joints[m_ForeArm_R_Twist_2].parent_index = m_lowerarm_r_idx;
		joints[m_ForeArm_R_Twist_3].parent_index = m_lowerarm_r_idx;

		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			uint32_t        chosen_idx;
			PxRigidDynamic* body = m_ragdoll->find_recent_body((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

			if (!body || m_vecBreakPartFilter[chosen_idx] == true)
				continue;

			_matrix body_global_transform = to_mat4(body->getGlobalPose());
			_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
			_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
			joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

			_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
			m_ragdoll->m_relative_joint_pos[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_body_rotations[i] = XMQuaternionRotationMatrix(body_global_transform);

			if (m_ragdoll->m_rigid_bodies[i])
			{
				// Rigid body position relative to the joint
				_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
				p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies[i]->getGlobalPose().p), 1.0f), m);

				m_ragdoll->m_body_pos_relative_to_joint[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
				m_ragdoll->m_original_joint_rotations[i] = XMQuaternionRotationMatrix(bind_pose_ws);
			}
		}

		Update_Partial(1 / 60.f);

		//if (m_ForeArm_R && m_Hand_R)
		//	m_pWrist_R_Joint = create_d6_joint(m_ForeArm_R, m_Hand_R, m_hand_r_idx_Bone, m_hand_r_idx);

		if (m_ForeArm_R_High && m_Hand_R)
			m_Radius_R_Upper_Joint = create_d6_joint(m_ForeArm_R_High, m_Hand_R, m_hand_r_idx_Bone, m_hand_r_idx);

		Update_Partial_After(1 / 60.f);

		m_vecBreakPartFilter[m_lowerarm_high_r_idx] = true;
		m_vecBreakPartFilter[m_hand_r_idx] = true;
		m_vecBreakPartFilter[m_lowerarm_r_idx] = true;

		m_vecBreakPartFilter[m_ForeArm_R_Twist_0] = true;
		m_vecBreakPartFilter[m_ForeArm_R_Twist_1] = true;
		m_vecBreakPartFilter[m_ForeArm_R_Twist_2] = true;
		m_vecBreakPartFilter[m_ForeArm_R_Twist_3] = true;

		break;
	}
	case COLLIDER_TYPE::ARM_L:
	{
		if (m_Arm_L_High == nullptr)
		{
			m_Arm_L_High = create_capsule_bone(m_upperarm_high_l_idx, m_lowerarm_l_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::ARM_L);
			m_Scene->addActor(*m_Arm_L_High);
		}
		else
		{
			break;
		}

		if (!m_ForeArm_L)
		{
			m_ForeArm_L = create_capsule_bone(m_lowerarm_l_idx, m_hand_l_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::FOREARM_L);
			m_Scene->addActor(*m_ForeArm_L);
		}
		if (!m_Hand_L)
		{
			m_Hand_L = create_capsule_bone(m_hand_l_idx, m_middle_01_l_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::HAND_L);
			m_Scene->addActor(*m_Hand_L);
		}

		m_vecBreak_Parent_Flag[m_Arm_L_Twist_0] = true;
		joints[m_Arm_L_Twist_0].parent_index_BreakPart = m_upperarm_l_idx;
		m_vecBreak_Parent_Flag[m_Arm_L_Twist_1] = true;
		joints[m_Arm_L_Twist_1].parent_index_BreakPart = m_upperarm_l_idx;
		m_vecBreak_Parent_Flag[m_Arm_L_Twist_2] = true;
		joints[m_Arm_L_Twist_2].parent_index_BreakPart = m_upperarm_l_idx;
		m_vecBreak_Parent_Flag[m_Arm_L_Twist_3] = true;
		joints[m_Arm_L_Twist_3].parent_index_BreakPart = m_upperarm_l_idx;

		joints[m_Arm_L_Twist_0].parent_index = m_upperarm_high_l_idx;
		joints[m_Arm_L_Twist_1].parent_index = m_upperarm_high_l_idx;
		joints[m_Arm_L_Twist_2].parent_index = m_upperarm_high_l_idx;
		joints[m_Arm_L_Twist_3].parent_index = m_upperarm_high_l_idx;


#pragma region For Cloth
		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			if (joints[i].parent_index != -1 && joints[i].parent_index != m_upperarm_high_l_idx && joints[i].parent_index != m_hand_l_idx
				&& joints[i].parent_index != m_lowerarm_l_idx)
			{
				m_vecBreak_Parent_Flag_Cloth_Arm_L[i] = true;
				joints[i].parent_index_BreakPart_Cloth_Arm_L = m_upperarm_high_l_idx;
				m_vecBreakPartFilter_Cloth_Arm_L[i] = true;
				m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_L[i] = m_Arm_L_High;
			}

		/*	if (joints[i].parent_index != -1 && m_ragdoll->find_recent_body(joints[i].parent_index, m_skeletal_mesh->skeleton()) == nullptr)
			{
				m_vecBreak_Parent_Flag_Cloth_Arm_L[i] = true;
				joints[i].parent_index_BreakPart_Cloth_Arm_L = m_upperarm_high_l_idx;
				m_vecBreakPartFilter_Cloth_Arm_L[i] = true;
				m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_L[i] = m_Arm_L_High;
			}*/
		}
#pragma endregion

#pragma region For Cloth.
		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			uint32_t        chosen_idx;
			PxRigidDynamic* body = m_ragdoll->find_recent_body_Cloth_Arm_L((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

			if (!body)
				continue;

			_matrix body_global_transform = to_mat4(body->getGlobalPose());
			_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
			_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
			joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

			_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
			m_ragdoll->m_relative_joint_pos_BreakPart_Cloth_Arm_L[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_body_rotations_BreakPart_Cloth_Arm_L[i] = XMQuaternionRotationMatrix(body_global_transform);

			if (m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_L[i])
			{
				// Rigid body position relative to the joint
				_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
				p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_L[i]->getGlobalPose().p), 1.0f), m);

				m_ragdoll->m_body_pos_relative_to_joint_BreakPart_Cloth_Arm_L[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
				m_ragdoll->m_original_joint_rotations_BreakPart_Cloth_Arm_L[i] = XMQuaternionRotationMatrix(bind_pose_ws);
			}
		}
#pragma endregion

		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			uint32_t        chosen_idx;
			PxRigidDynamic* body = m_ragdoll->find_recent_body((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

			if (!body || m_vecBreakPartFilter[chosen_idx] == true)
				continue;

			_matrix body_global_transform = to_mat4(body->getGlobalPose());
			_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
			_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
			joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

			_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
			m_ragdoll->m_relative_joint_pos[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_body_rotations[i] = XMQuaternionRotationMatrix(body_global_transform);

			if (m_ragdoll->m_rigid_bodies[i])
			{
				// Rigid body position relative to the joint
				_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
				p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies[i]->getGlobalPose().p), 1.0f), m);

				m_ragdoll->m_body_pos_relative_to_joint[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
				m_ragdoll->m_original_joint_rotations[i] = XMQuaternionRotationMatrix(bind_pose_ws);
			}
		}



		Update_Partial(1 / 60.f);

		//TEMP
		if (!m_pElbow_L_Joint)
			m_pElbow_L_Joint = create_d6_joint(m_Arm_L_High, m_ForeArm_L, m_lowerarm_l_idx_Bone, m_lowerarm_l_idx);

		if (m_ForeArm_L && m_Hand_L)
			m_pWrist_L_Joint = create_d6_joint(m_ForeArm_L, m_Hand_L, m_hand_l_idx_Bone, m_hand_l_idx);

		Update_Partial_After(1 / 60.f);

		m_vecBreakPartFilter[m_lowerarm_l_idx] = true;
		m_vecBreakPartFilter[m_hand_l_idx] = true;
		m_vecBreakPartFilter[m_upperarm_high_l_idx] = true;
		m_vecBreakPartFilter[m_Arm_L_Twist_0] = true;
		m_vecBreakPartFilter[m_Arm_L_Twist_1] = true;
		m_vecBreakPartFilter[m_Arm_L_Twist_2] = true;
		m_vecBreakPartFilter[m_Arm_L_Twist_3] = true;

		break;
	}
	case COLLIDER_TYPE::ARM_R:
	{
		if (m_Arm_R_High == nullptr)
		{
			m_Arm_R_High = create_capsule_bone(m_upperarm_high_r_idx, m_lowerarm_r_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::ARM_R);
			m_Scene->addActor(*m_Arm_R_High);

			m_ragdoll->m_rigid_bodies_BreakPart[m_upperarm_high_r_idx] = m_Arm_R_High;
		}
		else
		{
			break;
		}

		if (!m_ForeArm_R)
		{
			m_ForeArm_R = create_capsule_bone(m_lowerarm_r_idx, m_hand_r_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::FOREARM_R);
			m_Scene->addActor(*m_ForeArm_R);

			m_ragdoll->m_rigid_bodies_BreakPart[m_lowerarm_r_idx] = m_ForeArm_R;
		}
		if (!m_Hand_R)
		{
			m_Hand_R = create_capsule_bone(m_hand_r_idx, m_middle_01_r_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::HAND_R);
			m_Scene->addActor(*m_Hand_R);

			m_ragdoll->m_rigid_bodies_BreakPart[m_hand_r_idx] = m_Hand_R;
		}

		m_vecBreak_Parent_Flag[m_Arm_R_Twist_0] = true;
		joints[m_Arm_R_Twist_0].parent_index_BreakPart = m_upperarm_r_idx;
		m_vecBreak_Parent_Flag[m_Arm_R_Twist_1] = true;
		joints[m_Arm_R_Twist_1].parent_index_BreakPart = m_upperarm_r_idx;
		m_vecBreak_Parent_Flag[m_Arm_R_Twist_2] = true;
		joints[m_Arm_R_Twist_2].parent_index_BreakPart = m_upperarm_r_idx;
		m_vecBreak_Parent_Flag[m_Arm_R_Twist_3] = true;
		joints[m_Arm_R_Twist_3].parent_index_BreakPart = m_upperarm_r_idx;

		joints[m_Arm_R_Twist_0].parent_index = m_upperarm_high_r_idx;
		joints[m_Arm_R_Twist_1].parent_index = m_upperarm_high_r_idx;
		joints[m_Arm_R_Twist_2].parent_index = m_upperarm_high_r_idx;
		joints[m_Arm_R_Twist_3].parent_index = m_upperarm_high_r_idx;

#pragma region For Cloth
		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			if (joints[i].parent_index != -1 && joints[i].parent_index != m_upperarm_high_r_idx && joints[i].parent_index != m_hand_r_idx
				&& joints[i].parent_index != m_lowerarm_r_idx)
			{
				m_vecBreak_Parent_Flag_Cloth_Arm_R[i] = true;
				joints[i].parent_index_BreakPart_Cloth_Arm_R = m_upperarm_high_r_idx;
				m_vecBreakPartFilter_Cloth_Arm_R[i] = true;
				m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_R[i] = m_Arm_R_High;
			}

			/*if (joints[i].parent_index != -1 && m_ragdoll->find_recent_body(joints[i].parent_index, m_skeletal_mesh->skeleton()) == nullptr)
			{
				m_vecBreak_Parent_Flag_Cloth_Arm_R[i] = true;
				joints[i].parent_index_BreakPart_Cloth_Arm_R = m_upperarm_high_r_idx;
				m_vecBreakPartFilter_Cloth_Arm_R[i] = true;
				m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_R[i] = m_Arm_R_High;
			}*/
		}
#pragma endregion

#pragma region For Cloth.
		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			uint32_t        chosen_idx;
			PxRigidDynamic* body = m_ragdoll->find_recent_body_Cloth_Arm_R((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

			if (!body)
				continue;

			_matrix body_global_transform = to_mat4(body->getGlobalPose());
			_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
			_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
			joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

			_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
			m_ragdoll->m_relative_joint_pos_BreakPart_Cloth_Arm_R[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_body_rotations_BreakPart_Cloth_Arm_R[i] = XMQuaternionRotationMatrix(body_global_transform);

			if (m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_R[i])
			{
				// Rigid body position relative to the joint
				_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
				p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_R[i]->getGlobalPose().p), 1.0f), m);

				m_ragdoll->m_body_pos_relative_to_joint_BreakPart_Cloth_Arm_R[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
				m_ragdoll->m_original_joint_rotations_BreakPart_Cloth_Arm_R[i] = XMQuaternionRotationMatrix(bind_pose_ws);
			}
		}
#pragma endregion

		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			uint32_t        chosen_idx;
			PxRigidDynamic* body = m_ragdoll->find_recent_body((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

			if (!body || m_vecBreakPartFilter[chosen_idx] == true)
				continue;

			_matrix body_global_transform = to_mat4(body->getGlobalPose());
			_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
			_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
			joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

			_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
			m_ragdoll->m_relative_joint_pos[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_body_rotations[i] = XMQuaternionRotationMatrix(body_global_transform);

			if (m_ragdoll->m_rigid_bodies[i])
			{
				// Rigid body position relative to the joint
				_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
				p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies[i]->getGlobalPose().p), 1.0f), m);

				m_ragdoll->m_body_pos_relative_to_joint[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
				m_ragdoll->m_original_joint_rotations[i] = XMQuaternionRotationMatrix(bind_pose_ws);
			}
		}

		Update_Partial(1 / 60.f);

		//TEMP
		if (!m_pElbow_R_Joint)
			m_pElbow_R_Joint = create_d6_joint(m_Arm_R_High, m_ForeArm_R, m_lowerarm_r_idx_Bone, m_lowerarm_r_idx);

		if (m_ForeArm_R && m_Hand_R)
			m_pWrist_R_Joint = create_d6_joint(m_ForeArm_R, m_Hand_R, m_hand_r_idx_Bone, m_hand_r_idx);

		Update_Partial_After(1 / 60.f);

		m_vecBreakPartFilter[m_lowerarm_r_idx] = true;
		m_vecBreakPartFilter[m_hand_r_idx] = true;
		m_vecBreakPartFilter[m_upperarm_high_r_idx] = true;

		m_vecBreakPartFilter[m_Arm_R_Twist_0] = true;
		m_vecBreakPartFilter[m_Arm_R_Twist_1] = true;
		m_vecBreakPartFilter[m_Arm_R_Twist_2] = true;
		m_vecBreakPartFilter[m_Arm_R_Twist_3] = true;

		break;
	}
	case COLLIDER_TYPE::PELVIS:
	{
		//break; ////////////////////////////////////TEMP//////////////////////////////
		if (!m_Leg_L)
		{
			m_Leg_L = create_capsule_bone(m_thigh_l_idx, m_calf_l_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::LEG_L);
			m_Scene->addActor(*m_Leg_L);
		}
		if (!m_Leg_R)
		{
			m_Leg_R = create_capsule_bone(m_thigh_r_idx, m_calf_r_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::LEG_R);
			m_Scene->addActor(*m_Leg_R);
		}
		if (!m_Pelvis)
		{
			m_Pelvis = create_capsule_bone(m_pelvis_idx, m_spine_02_idx, *m_ragdoll, CHEST_SIZE * m_scale, rot, COLLIDER_TYPE::PELVIS);
			m_Scene->addActor(*m_Pelvis);
		}
		if (!m_Calf_L)
		{
			m_Calf_L = create_capsule_bone(m_calf_l_idx, m_foot_l_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::CALF_L);
			m_Scene->addActor(*m_Calf_L);
		}
		if (!m_Calf_R)
		{
			m_Calf_R = create_capsule_bone(m_calf_r_idx, m_foot_r_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::CALF_R);
			m_Scene->addActor(*m_Calf_R);
		}

		rot = XMMatrixRotationY(PxPi * 0.5f);

		if (!m_Foot_L)
		{
			m_Foot_L = create_capsule_bone(m_foot_l_idx, m_ball_l_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::FOOT_L);
			m_Scene->addActor(*m_Foot_L);
		}

		if (!m_Foot_R)
		{
			m_Foot_R = create_capsule_bone(m_foot_r_idx, m_ball_r_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::FOOT_R);
			m_Scene->addActor(*m_Foot_R);
		}

		m_ragdoll->m_rigid_bodies[1] = m_Pelvis;

		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			uint32_t        chosen_idx;
			PxRigidDynamic* body = m_ragdoll->find_recent_body((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

			if (!body || m_vecBreakPartFilter[chosen_idx] == true)
				continue;

			_matrix body_global_transform = to_mat4(body->getGlobalPose());
			_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
			_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
			joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

			_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
			m_ragdoll->m_relative_joint_pos[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_body_rotations[i] = XMQuaternionRotationMatrix(body_global_transform);

			if (m_ragdoll->m_rigid_bodies[i])
			{
				// Rigid body position relative to the joint
				_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
				p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies[i]->getGlobalPose().p), 1.0f), m);

				m_ragdoll->m_body_pos_relative_to_joint[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
				m_ragdoll->m_original_joint_rotations[i] = XMQuaternionRotationMatrix(bind_pose_ws);
			}
		}

		Update_Partial(1 / 60.f);

		// Pelvis to Thighs
		if (m_Pelvis && m_Leg_L)
			m_pHip_Joint_L = create_d6_joint(m_Pelvis, m_Leg_L, m_thigh_l_idx_Bone, m_thigh_l_idx);
		if (m_Pelvis && m_Leg_R)
			m_pHip_Joint_R = create_d6_joint(m_Pelvis, m_Leg_R, m_thigh_r_idx_Bone, m_thigh_r_idx);

		// Thighs to Calf
		if (m_Leg_L && m_Calf_L)
			m_pKnee_Joint_L = create_d6_joint(m_Leg_L, m_Calf_L, m_calf_l_idx_Bone, m_calf_l_idx);
		if (m_Leg_R && m_Calf_R)
			m_pKnee_Joint_R = create_d6_joint(m_Leg_R, m_Calf_R, m_calf_r_idx_Bone, m_calf_r_idx);

		// Calf to Foot
		if (m_Calf_L && m_Foot_L)
			m_pAnkle_Joint_L = create_d6_joint_Foot(m_Calf_L, m_Foot_L, m_ball_l_idx_Bone, m_foot_l_idx);
		if (m_Calf_R && m_Foot_R)
			m_pAnkle_Joint_R = create_d6_joint_Foot(m_Calf_R, m_Foot_R, m_ball_r_idx_Bone, m_foot_r_idx);

		Update_Partial_After(1 / 60.f);

		m_vecBreakPartFilter[m_thigh_l_idx] = true;
		m_vecBreakPartFilter[1] = true;
		m_vecBreakPartFilter[m_thigh_r_idx] = true;
		m_vecBreakPartFilter[m_pelvis_idx] = true;
		m_vecBreakPartFilter[m_calf_l_idx] = true;
		m_vecBreakPartFilter[m_calf_r_idx] = true;
		m_vecBreakPartFilter[m_foot_l_idx] = true;
		m_vecBreakPartFilter[m_foot_r_idx] = true;
		break;
	}
	case COLLIDER_TYPE::CALF_L:
	{
		//m_Calf_L = create_capsule_bone(m_calf_l_idx, m_foot_l_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::CALF_L);

		if (m_Calf_L_High == nullptr)
			m_Calf_L_High = create_capsule_bone(m_calf_l_idx, m_foot_l_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::CALF_L);
		else
			break;

		rot = XMMatrixRotationY(PxPi * 0.5f);

		m_Foot_L = create_capsule_bone(m_foot_l_idx, m_ball_l_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::FOOT_L);
		m_Scene->addActor(*m_Calf_L_High);
		//m_Scene->addActor(*m_Calf_L);
		m_Scene->addActor(*m_Foot_L);

		m_vecBreak_Parent_Flag[m_Calf_L_Twist_0] = true;
		joints[m_Calf_L_Twist_0].parent_index_BreakPart = joints[m_Calf_L_Twist_0].parent_index;
		m_vecBreak_Parent_Flag[m_Calf_L_Twist_1] = true;
		joints[m_Calf_L_Twist_1].parent_index_BreakPart = joints[m_Calf_L_Twist_1].parent_index;

		joints[m_Calf_L_Twist_0].parent_index = m_calf_l_idx;
		joints[m_Calf_L_Twist_1].parent_index = m_calf_l_idx;

		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			uint32_t        chosen_idx;
			PxRigidDynamic* body = m_ragdoll->find_recent_body((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

			if (!body || m_vecBreakPartFilter[chosen_idx] == true)
				continue;

			_matrix body_global_transform = to_mat4(body->getGlobalPose());
			_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
			_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
			joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

			_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
			m_ragdoll->m_relative_joint_pos[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_body_rotations[i] = XMQuaternionRotationMatrix(body_global_transform);

			if (m_ragdoll->m_rigid_bodies[i])
			{
				// Rigid body position relative to the joint
				_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
				p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies[i]->getGlobalPose().p), 1.0f), m);

				m_ragdoll->m_body_pos_relative_to_joint[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
				m_ragdoll->m_original_joint_rotations[i] = XMQuaternionRotationMatrix(bind_pose_ws);
			}
		}

		Update_Partial(1 / 60.f);

		if (m_Calf_L_High && m_Foot_L)
			m_pAnkle_Joint_L = create_d6_joint_Foot(m_Calf_L_High, m_Foot_L, m_ball_l_idx_Bone, m_foot_l_idx);

		Update_Partial_After(1 / 60.f);
		m_vecBreakPartFilter[m_calf_l_idx] = true;
		m_vecBreakPartFilter[m_foot_l_idx] = true;

		m_vecBreakPartFilter[m_Calf_L_Twist_0] = true;
		m_vecBreakPartFilter[m_Calf_L_Twist_1] = true;

		break;
	}
	case COLLIDER_TYPE::CALF_R:
	{
		//m_Calf_L = create_capsule_bone(m_calf_l_idx, m_foot_l_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::CALF_L);

		if (m_Calf_R_High == nullptr)
			m_Calf_R_High = create_capsule_bone(m_calf_r_idx, m_foot_r_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::CALF_R);
		else
			break;

		rot = XMMatrixRotationY(PxPi * 0.5f);

		m_Foot_R = create_capsule_bone(m_foot_r_idx, m_ball_r_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::FOOT_R);
		m_Scene->addActor(*m_Calf_R_High);
		//m_Scene->addActor(*m_Calf_L);
		m_Scene->addActor(*m_Foot_R);

		m_vecBreak_Parent_Flag[m_Calf_R_Twist_0] = true;
		joints[m_Calf_R_Twist_0].parent_index_BreakPart = joints[m_Calf_R_Twist_0].parent_index;
		m_vecBreak_Parent_Flag[m_Calf_R_Twist_1] = true;
		joints[m_Calf_R_Twist_1].parent_index_BreakPart = joints[m_Calf_R_Twist_1].parent_index;

		joints[m_Calf_R_Twist_0].parent_index = m_calf_r_idx;
		joints[m_Calf_R_Twist_1].parent_index = m_calf_r_idx;

		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			uint32_t        chosen_idx;
			PxRigidDynamic* body = m_ragdoll->find_recent_body((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

			if (!body || m_vecBreakPartFilter[chosen_idx] == true)
				continue;

			_matrix body_global_transform = to_mat4(body->getGlobalPose());
			_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
			_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
			joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

			_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
			m_ragdoll->m_relative_joint_pos[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_body_rotations[i] = XMQuaternionRotationMatrix(body_global_transform);

			if (m_ragdoll->m_rigid_bodies[i])
			{
				// Rigid body position relative to the joint
				_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
				p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies[i]->getGlobalPose().p), 1.0f), m);

				m_ragdoll->m_body_pos_relative_to_joint[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
				m_ragdoll->m_original_joint_rotations[i] = XMQuaternionRotationMatrix(bind_pose_ws);
			}
		}

		Update_Partial(1 / 60.f);

		if (m_Calf_R_High && m_Foot_R)
			m_pAnkle_Joint_R = create_d6_joint_Foot(m_Calf_R_High, m_Foot_R, m_ball_r_idx_Bone, m_foot_r_idx);

		Update_Partial_After(1 / 60.f);
		m_vecBreakPartFilter[m_calf_r_idx] = true;
		m_vecBreakPartFilter[m_foot_r_idx] = true;

		m_vecBreakPartFilter[m_Calf_R_Twist_0] = true;
		m_vecBreakPartFilter[m_Calf_R_Twist_1] = true;

		break;
	}
	case COLLIDER_TYPE::LEG_L:
	{
		if (m_Leg_L_High == nullptr)
		{
			m_Leg_L_High = create_capsule_bone(m_thigh_l_idx, m_calf_l_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::LEG_L);
			m_Scene->addActor(*m_Leg_L_High);
		}
		else
			break;

		if (!m_Calf_L)
		{
			m_Calf_L = create_capsule_bone(m_calf_l_idx, m_foot_l_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::CALF_L);
			m_Scene->addActor(*m_Calf_L);
		}

		rot = XMMatrixRotationY(PxPi * 0.5f);

		if (!m_Foot_L)
		{
			m_Foot_L = create_capsule_bone(m_foot_l_idx, m_ball_l_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::FOOT_L);
			m_Scene->addActor(*m_Foot_L);
		}

		m_vecBreak_Parent_Flag[m_Leg_L_Twist_0] = true;
		joints[m_Leg_L_Twist_0].parent_index_BreakPart = joints[m_Leg_L_Twist_0].parent_index;
		m_vecBreak_Parent_Flag[m_Leg_L_Twist_1] = true;
		joints[m_Leg_L_Twist_1].parent_index_BreakPart = joints[m_Leg_L_Twist_1].parent_index;
		m_vecBreak_Parent_Flag[m_Leg_L_Twist_2] = true;
		joints[m_Leg_L_Twist_2].parent_index_BreakPart = joints[m_Leg_L_Twist_2].parent_index;

		joints[m_Leg_L_Twist_0].parent_index = m_thigh_l_idx;
		joints[m_Leg_L_Twist_1].parent_index = m_thigh_l_idx;
		joints[m_Leg_L_Twist_2].parent_index = m_thigh_l_idx;

#pragma region Cloth
		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			if (joints[i].parent_index != -1 && joints[i].parent_index != m_foot_l_idx && joints[i].parent_index != m_thigh_l_idx && joints[i].parent_index != m_calf_l_idx)
			{
				m_vecBreak_Parent_Flag_Cloth_Leg_L[i] = true;
				joints[i].parent_index_BreakPart_Cloth_Leg_L = m_thigh_l_idx;
				m_vecBreakPartFilter_Cloth_Leg_L[i] = true;
				m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_L[i] = m_Leg_L_High;
			}

			/*if (joints[i].parent_index != -1 && m_ragdoll->find_recent_body(joints[i].parent_index, m_skeletal_mesh->skeleton()) == nullptr)
			{
				m_vecBreak_Parent_Flag_Cloth_Leg_L[i] = true;
				joints[i].parent_index_BreakPart_Cloth_Leg_L = m_thigh_l_idx;
				m_vecBreakPartFilter_Cloth_Leg_L[i] = true;
				m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_L[i] = m_Leg_L_High;
			}*/
		}
#pragma endregion

#pragma region For Cloth.
		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			uint32_t        chosen_idx;
			PxRigidDynamic* body = m_ragdoll->find_recent_body_Cloth_Leg_L((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

			if (!body)
				continue;

			_matrix body_global_transform = to_mat4(body->getGlobalPose());
			_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
			_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
			joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

			_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
			m_ragdoll->m_relative_joint_pos_BreakPart_Cloth_Leg_L[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_body_rotations_BreakPart_Cloth_Leg_L[i] = XMQuaternionRotationMatrix(body_global_transform);

			if (m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_L[i])
			{
				// Rigid body position relative to the joint
				_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
				p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_L[i]->getGlobalPose().p), 1.0f), m);

				m_ragdoll->m_body_pos_relative_to_joint_BreakPart_Cloth_Leg_L[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
				m_ragdoll->m_original_joint_rotations_BreakPart_Cloth_Leg_L[i] = XMQuaternionRotationMatrix(bind_pose_ws);
			}
		}
#pragma endregion

		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			uint32_t        chosen_idx;
			PxRigidDynamic* body = m_ragdoll->find_recent_body((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

			if (!body || m_vecBreakPartFilter[chosen_idx] == true)
				continue;

			_matrix body_global_transform = to_mat4(body->getGlobalPose());
			_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
			_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
			joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

			_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
			m_ragdoll->m_relative_joint_pos[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_body_rotations[i] = XMQuaternionRotationMatrix(body_global_transform);

			if (m_ragdoll->m_rigid_bodies[i])
			{
				// Rigid body position relative to the joint
				_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
				p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies[i]->getGlobalPose().p), 1.0f), m);

				m_ragdoll->m_body_pos_relative_to_joint[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
				m_ragdoll->m_original_joint_rotations[i] = XMQuaternionRotationMatrix(bind_pose_ws);
			}
		}

		Update_Partial(1 / 60.f);

		if (m_Leg_L_High && m_Calf_L)
			m_Knee_L_Upper_Joint = create_d6_joint(m_Leg_L_High, m_Calf_L, m_calf_l_idx_Bone, m_calf_l_idx);

		if (m_Calf_L && m_Foot_L)
			m_pAnkle_Joint_L = create_d6_joint_Foot(m_Calf_L, m_Foot_L, m_ball_l_idx_Bone, m_foot_l_idx);

		Update_Partial_After(1 / 60.f);


		m_vecBreakPartFilter[m_calf_l_idx] = true;
		m_vecBreakPartFilter[m_foot_l_idx] = true;
		m_vecBreakPartFilter[m_thigh_l_idx] = true;

		m_vecBreakPartFilter[m_Leg_L_Twist_0] = true;
		m_vecBreakPartFilter[m_Leg_L_Twist_1] = true;
		m_vecBreakPartFilter[m_Leg_L_Twist_2] = true;
		break;
	}
	case COLLIDER_TYPE::LEG_R:
	{
		if (m_Leg_R_High == nullptr)
		{
			m_Leg_R_High = create_capsule_bone(m_thigh_r_idx, m_calf_r_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::LEG_R);
			m_Scene->addActor(*m_Leg_R_High);
		}
		else
			break;

		if (!m_Calf_R)
		{
			m_Calf_R = create_capsule_bone(m_calf_r_idx, m_foot_r_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::CALF_R);
			m_Scene->addActor(*m_Calf_R);
		}

		rot = XMMatrixRotationY(PxPi * 0.5f);

		if (!m_Foot_R)
		{
			m_Foot_R = create_capsule_bone(m_foot_r_idx, m_ball_r_idx, *m_ragdoll, r * SIZE_MAG, rot, COLLIDER_TYPE::FOOT_R);
			m_Scene->addActor(*m_Foot_R);
		}

		m_vecBreak_Parent_Flag[m_Leg_R_Twist_0] = true;
		joints[m_Leg_R_Twist_0].parent_index_BreakPart = joints[m_Leg_R_Twist_0].parent_index;
		m_vecBreak_Parent_Flag[m_Leg_R_Twist_1] = true;
		joints[m_Leg_R_Twist_1].parent_index_BreakPart = joints[m_Leg_R_Twist_1].parent_index;
		m_vecBreak_Parent_Flag[m_Leg_R_Twist_2] = true;
		joints[m_Leg_R_Twist_2].parent_index_BreakPart = joints[m_Leg_R_Twist_2].parent_index;

		joints[m_Leg_R_Twist_0].parent_index = m_thigh_r_idx;
		joints[m_Leg_R_Twist_1].parent_index = m_thigh_r_idx;
		joints[m_Leg_R_Twist_2].parent_index = m_thigh_r_idx;

#pragma region Cloth
		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			if (joints[i].parent_index != -1 && joints[i].parent_index != m_foot_r_idx && joints[i].parent_index != m_thigh_r_idx && joints[i].parent_index != m_calf_r_idx)
			{
				m_vecBreak_Parent_Flag_Cloth_Leg_R[i] = true;
				joints[i].parent_index_BreakPart_Cloth_Leg_R = m_thigh_r_idx;
				m_vecBreakPartFilter_Cloth_Leg_R[i] = true;
				m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_R[i] = m_Leg_R_High;
			}

			/*if (joints[i].parent_index != -1 && m_ragdoll->find_recent_body(joints[i].parent_index, m_skeletal_mesh->skeleton()) == nullptr)
			{
				m_vecBreak_Parent_Flag_Cloth_Leg_R[i] = true;
				joints[i].parent_index_BreakPart_Cloth_Leg_R = m_thigh_r_idx;
				m_vecBreakPartFilter_Cloth_Leg_R[i] = true;
				m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_R[i] = m_Leg_R_High;
			}*/
		}
#pragma endregion

#pragma region For Cloth.
		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			uint32_t        chosen_idx;
			PxRigidDynamic* body = m_ragdoll->find_recent_body_Cloth_Leg_R((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

			if (!body)
				continue;

			_matrix body_global_transform = to_mat4(body->getGlobalPose());
			_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
			_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
			joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

			_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
			m_ragdoll->m_relative_joint_pos_BreakPart_Cloth_Leg_R[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_body_rotations_BreakPart_Cloth_Leg_R[i] = XMQuaternionRotationMatrix(body_global_transform);

			if (m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_R[i])
			{
				// Rigid body position relative to the joint
				_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
				p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_R[i]->getGlobalPose().p), 1.0f), m);

				m_ragdoll->m_body_pos_relative_to_joint_BreakPart_Cloth_Leg_R[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
				m_ragdoll->m_original_joint_rotations_BreakPart_Cloth_Leg_R[i] = XMQuaternionRotationMatrix(bind_pose_ws);
			}
		}
#pragma endregion

		for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		{
			uint32_t        chosen_idx;
			PxRigidDynamic* body = m_ragdoll->find_recent_body((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

			if (!body || m_vecBreakPartFilter[chosen_idx] == true)
				continue;

			_matrix body_global_transform = to_mat4(body->getGlobalPose());
			_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
			_matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
			joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

			_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
			m_ragdoll->m_relative_joint_pos[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_body_rotations[i] = XMQuaternionRotationMatrix(body_global_transform);

			if (m_ragdoll->m_rigid_bodies[i])
			{
				// Rigid body position relative to the joint
				_matrix m = XMMatrixInverse(nullptr, m_model * XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
				p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies[i]->getGlobalPose().p), 1.0f), m);

				m_ragdoll->m_body_pos_relative_to_joint[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
				m_ragdoll->m_original_joint_rotations[i] = XMQuaternionRotationMatrix(bind_pose_ws);
			}
		}

		Update_Partial(1 / 60.f);

		if (m_Leg_R_High && m_Calf_R)
			m_Knee_R_Upper_Joint = create_d6_joint(m_Leg_R_High, m_Calf_R, m_calf_r_idx_Bone, m_calf_r_idx);

		if (m_Calf_R && m_Foot_R)
			m_pAnkle_Joint_R = create_d6_joint_Foot(m_Calf_R, m_Foot_R, m_ball_r_idx_Bone, m_foot_r_idx);

		Update_Partial_After(1 / 60.f);


		m_vecBreakPartFilter[m_calf_r_idx] = true;
		m_vecBreakPartFilter[m_foot_r_idx] = true;
		m_vecBreakPartFilter[m_thigh_r_idx] = true;

		m_vecBreakPartFilter[m_Leg_R_Twist_0] = true;
		m_vecBreakPartFilter[m_Leg_R_Twist_1] = true;
		m_vecBreakPartFilter[m_Leg_R_Twist_2] = true;
		break;
	}
	}
}

void CRagdoll_Physics::Update(_float fTimeDelta)
{
	m_pTransformCom_Sound->Set_WorldMatrix(m_pTransform->Get_WorldMatrix());

	if (m_bCulling)
		return;

	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		m_pose_transforms.transforms[i] = XMMatrixInverse(nullptr, joints[i].inverse_bind_pose);

	update_animations();
}

void CRagdoll_Physics::Update_Partial(_float fTimeDelta)
{
	if (m_bCulling)
		return;

	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		m_pose_transforms.transforms[i] = XMMatrixInverse(nullptr, joints[i].inverse_bind_pose);

	update_animations_partial();
}

void CRagdoll_Physics::Update_Partial_After(_float fTimeDelta)
{
	if (m_bCulling)
		return;

	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		m_pose_transforms.transforms[i] = XMMatrixInverse(nullptr, joints[i].inverse_bind_pose);

	update_animations_partial_after();
}

void CRagdoll_Physics::update_animations()
{
	if (m_bRagdoll == false && m_bPartialRagdoll == false)
		return;

	m_RotationMatrix = m_pTransform->Get_RotationMatrix_Pure();

	auto RotMat = m_RotationMatrix;
	RotMat._41 = m_pWorldMatrix->_41;
	RotMat._42 = m_pWorldMatrix->_42;
	RotMat._43 = m_pWorldMatrix->_43;

	auto joint = m_skeletal_mesh->skeleton()->joints();

	auto NumJoint = m_skeletal_mesh->skeleton()->num_bones();

	if (m_vecBone)
	{
		for (size_t i = 0; i < NumJoint; ++i)
		{
			m_Global_transforms.transforms[i] = XMLoadFloat4x4((*m_vecBone)[m_vecBoneIndex[i]]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
			m_Global_transforms_BreakPart.transforms[i] = XMLoadFloat4x4((*m_vecBone)[m_vecBoneIndex[i]]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);

			if(m_bPartialRagdoll == true)
			{
				m_Global_transforms_BreakPart_Cloth_Arm_L.transforms[i] = XMLoadFloat4x4((*m_vecBone)[m_vecBoneIndex[i]]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
				m_Global_transforms_BreakPart_Cloth_Arm_R.transforms[i] = XMLoadFloat4x4((*m_vecBone)[m_vecBoneIndex[i]]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
				m_Global_transforms_BreakPart_Cloth_Leg_L.transforms[i] = XMLoadFloat4x4((*m_vecBone)[m_vecBoneIndex[i]]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
				m_Global_transforms_BreakPart_Cloth_Leg_R.transforms[i] = XMLoadFloat4x4((*m_vecBone)[m_vecBoneIndex[i]]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
			}
		}
	}

	if (!m_simulate)
	{
		if (m_vecBone)
		{
			for (int i = 0; i < m_ragdoll->m_rigid_bodies.size(); i++)
			{
				if (m_ragdoll->m_rigid_bodies[i] && m_vecBreakPartFilter[i] == false)
				{
					XMMATRIX global_transform = m_Global_transforms.transforms[i];
					XMVECTOR body_pos_relative_to_joint = XMVectorSetW(m_ragdoll->m_body_pos_relative_to_joint[i], 1.f);

					XMMATRIX transform = m_model * global_transform;

					XMVECTOR pos_with_w = XMVectorSetW(body_pos_relative_to_joint, 1.0f);

					XMVECTOR temp = XMVector4Transform(pos_with_w, transform);

					_vector world_pos = XMVectorSet(XMVectorGetX(temp), XMVectorGetY(temp), XMVectorGetZ(temp), 1.f);

					XMMATRIX final_transform = m_model_without_scale * global_transform;

					XMVECTOR joint_rot = XMQuaternionRotationMatrix(final_transform);

					_vector diff_rot = XMQuaternionInverse(m_ragdoll->m_original_joint_rotations[i]) * joint_rot;

					_vector rotation = m_ragdoll->m_original_body_rotations[i] * diff_rot;

					rotation = XMQuaternionNormalize(rotation);

					PxTransform px_transform(to_vec3(world_pos), to_quat(XMQuaternionRotationMatrix(XMLoadFloat4x4((*m_vecBone)[m_vecBoneIndex[i]]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(&RotMat))));
					m_ragdoll->m_rigid_bodies[i]->setGlobalPose(px_transform);
				}
			}

			if (m_bPartialRagdoll == false)
			{
				if (m_bJoint_Set == false)
				{
					m_bJoint_Set = true;
					create_joint();
				}
			}
		}
	}
	else
	{
#pragma region Original Version
	/*	m_Global_transforms = *m_ragdoll_pose->apply(m_ragdoll, m_model_only_scale, m_model_without_scale);
		m_Global_transforms_BreakPart_Cloth_Arm_L = *m_ragdoll_pose->apply_BreakPart_Cloth_Arm_L(m_ragdoll, m_model_only_scale, m_model_without_scale, &m_vecBreakPartFilter_Cloth_Arm_L);
		m_Global_transforms_BreakPart_Cloth_Arm_R = *m_ragdoll_pose->apply_BreakPart_Cloth_Arm_R(m_ragdoll, m_model_only_scale, m_model_without_scale, &m_vecBreakPartFilter_Cloth_Arm_R);
		m_Global_transforms_BreakPart_Cloth_Leg_L = *m_ragdoll_pose->apply_BreakPart_Cloth_Leg_L(m_ragdoll, m_model_only_scale, m_model_without_scale, &m_vecBreakPartFilter_Cloth_Leg_L);
		m_Global_transforms_BreakPart_Cloth_Leg_R = *m_ragdoll_pose->apply_BreakPart_Cloth_Leg_R(m_ragdoll, m_model_only_scale, m_model_without_scale, &m_vecBreakPartFilter_Cloth_Leg_R);*/
#pragma endregion

#pragma region Using Thread
		function<void()> job1 = bind(&AnimRagdoll::apply_BreakPart_Cloth_Arm_L_NoReturn, m_ragdoll_pose, m_ragdoll, m_model_only_scale, m_model_without_scale, &m_vecBreakPartFilter_Cloth_Arm_L);
		function<void()> job2 = bind(&AnimRagdoll::apply_BreakPart_Cloth_Arm_R_NoReturn, m_ragdoll_pose, m_ragdoll, m_model_only_scale, m_model_without_scale, &m_vecBreakPartFilter_Cloth_Arm_R);
		function<void()> job3 = bind(&AnimRagdoll::apply_BreakPart_Cloth_Leg_L_NoReturn, m_ragdoll_pose, m_ragdoll, m_model_only_scale, m_model_without_scale, &m_vecBreakPartFilter_Cloth_Leg_L);
		function<void()> job4 = bind(&AnimRagdoll::apply_BreakPart_Cloth_Leg_R_NoReturn, m_ragdoll_pose, m_ragdoll, m_model_only_scale, m_model_without_scale, &m_vecBreakPartFilter_Cloth_Leg_R);
		function<void()> job5 = bind(&AnimRagdoll::apply_NoReturn, m_ragdoll_pose, m_ragdoll, m_model_only_scale, m_model_without_scale);
		CGameInstance::Get_Instance()->Insert_Job(job1);
		CGameInstance::Get_Instance()->Insert_Job(job2);
		CGameInstance::Get_Instance()->Insert_Job(job3);
		CGameInstance::Get_Instance()->Insert_Job(job4);
		CGameInstance::Get_Instance()->Insert_Job(job5);


		while (!CGameInstance::Get_Instance()->AllJobsFinished())
		{
			this_thread::yield();
		}

		m_Global_transforms = *m_ragdoll_pose->GetRagdollTransform();
		m_Global_transforms_BreakPart_Cloth_Arm_L = *m_ragdoll_pose->GetRagdollTransform_Arm_L();
		m_Global_transforms_BreakPart_Cloth_Arm_R = *m_ragdoll_pose->GetRagdollTransform_Arm_R();
		m_Global_transforms_BreakPart_Cloth_Leg_L = *m_ragdoll_pose->GetRagdollTransform_Leg_L();
		m_Global_transforms_BreakPart_Cloth_Leg_R = *m_ragdoll_pose->GetRagdollTransform_Leg_R();
#pragma endregion

		if (m_bPartialRagdoll == false)
		{
			m_Global_transforms_BreakPart = *m_ragdoll_pose->apply_BreakPart_Leg(m_ragdoll, m_model_only_scale, m_model_without_scale, &m_vecBreakPartFilter);
		}

		auto joint = m_skeletal_mesh->skeleton()->joints();

		if (m_bPartialRagdoll)
		{
			if (m_vecBone)
			{
				auto WorldMat = m_pTransform->Get_WorldMatrix_Pure();

				for (size_t i = 0; i < NumJoint; ++i)
				{
					if (!IsIdentityMatrix(m_Global_transforms.transforms[i]))
					{
						auto Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMat));
						auto Result = m_Global_transforms.transforms[i] * Inverse;
						m_BoneMatrices[m_vecBoneIndex[i]] = Result;
						m_BoneMatrices_Cloth_Arm_L[m_vecBoneIndex[i]] = Result;
						m_BoneMatrices_Cloth_Arm_R[m_vecBoneIndex[i]] = Result;
						m_BoneMatrices_Cloth_Leg_L[m_vecBoneIndex[i]] = Result;
						m_BoneMatrices_Cloth_Leg_R[m_vecBoneIndex[i]] = Result;
					}

					if (!IsIdentityMatrix(m_Global_transforms_BreakPart_Cloth_Arm_L.transforms[i]))
					{
						auto Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMat));
						auto Result = m_Global_transforms_BreakPart_Cloth_Arm_L.transforms[i] * Inverse;
						m_BoneMatrices_Cloth_Arm_L[m_vecBoneIndex[i]] = Result;
					}

					if (!IsIdentityMatrix(m_Global_transforms_BreakPart_Cloth_Arm_R.transforms[i]))
					{
						auto Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMat));
						auto Result = m_Global_transforms_BreakPart_Cloth_Arm_R.transforms[i] * Inverse;
						m_BoneMatrices_Cloth_Arm_R[m_vecBoneIndex[i]] = Result;
					}

					if (!IsIdentityMatrix(m_Global_transforms_BreakPart_Cloth_Leg_L.transforms[i]))
					{
						auto Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMat));
						auto Result = m_Global_transforms_BreakPart_Cloth_Leg_L.transforms[i] * Inverse;
						m_BoneMatrices_Cloth_Leg_L[m_vecBoneIndex[i]] = Result;
					}

					if (!IsIdentityMatrix(m_Global_transforms_BreakPart_Cloth_Leg_R.transforms[i]))
					{
						auto Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMat));
						auto Result = m_Global_transforms_BreakPart_Cloth_Leg_R.transforms[i] * Inverse;
						m_BoneMatrices_Cloth_Leg_R[m_vecBoneIndex[i]] = Result;
					}
				}
			}
		}
		else
		{
			if (m_vecBone)
			{
				m_RotationMatrix = m_pTransform->Get_RotationMatrix_Pure();

				auto WorldMat = m_RotationMatrix;

				for (size_t i = 0; i < NumJoint; ++i)
				{
					if (!IsIdentityMatrix(m_Global_transforms.transforms[i]))
					{
						auto Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMat));
						auto Result = m_Global_transforms.transforms[i] * Inverse;
						(*m_vecBone)[m_vecBoneIndex[i]]->Set_Combined_Matrix(Result);
						m_BoneMatrices[m_vecBoneIndex[i]] = Result;
						m_BoneMatrices_Cloth_Arm_L[m_vecBoneIndex[i]] = Result;
						m_BoneMatrices_Cloth_Arm_R[m_vecBoneIndex[i]] = Result;
						m_BoneMatrices_Cloth_Leg_L[m_vecBoneIndex[i]] = Result;
						m_BoneMatrices_Cloth_Leg_R[m_vecBoneIndex[i]] = Result;

						if (m_vecBreak_Parent_Flag[i] == true)
						{
							auto Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMat));
							auto Result = m_Global_transforms_BreakPart.transforms[i] * Inverse;
							(*m_vecBone)[m_vecBoneIndex[i]]->Set_Combined_Matrix(Result);
						}

						if (!IsIdentityMatrix(m_Global_transforms_BreakPart_Cloth_Arm_L.transforms[i]))
						{
							auto Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMat));
							auto Result = m_Global_transforms_BreakPart_Cloth_Arm_L.transforms[i] * Inverse;
							m_BoneMatrices_Cloth_Arm_L[m_vecBoneIndex[i]] = Result;
						}

						if (!IsIdentityMatrix(m_Global_transforms_BreakPart_Cloth_Arm_R.transforms[i]))
						{
							auto Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMat));
							auto Result = m_Global_transforms_BreakPart_Cloth_Arm_R.transforms[i] * Inverse;
							m_BoneMatrices_Cloth_Arm_R[m_vecBoneIndex[i]] = Result;
						}

						if (!IsIdentityMatrix(m_Global_transforms_BreakPart_Cloth_Leg_L.transforms[i]))
						{
							auto Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMat));
							auto Result = m_Global_transforms_BreakPart_Cloth_Leg_L.transforms[i] * Inverse;
							m_BoneMatrices_Cloth_Leg_L[m_vecBoneIndex[i]] = Result;
						}

						if (!IsIdentityMatrix(m_Global_transforms_BreakPart_Cloth_Leg_R.transforms[i]))
						{
							auto Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMat));
							auto Result = m_Global_transforms_BreakPart_Cloth_Leg_R.transforms[i] * Inverse;
							m_BoneMatrices_Cloth_Leg_R[m_vecBoneIndex[i]] = Result;
						}
					}
				}
			}
		}
	}
}

void CRagdoll_Physics::update_animations_partial()
{
	m_RotationMatrix = m_pTransform->Get_RotationMatrix_Pure();

	auto RotMat = m_RotationMatrix;
	RotMat._41 = m_pWorldMatrix->_41;
	RotMat._42 = m_pWorldMatrix->_42;
	RotMat._43 = m_pWorldMatrix->_43;

	auto joint = m_skeletal_mesh->skeleton()->joints();

	auto NumJoint = m_skeletal_mesh->skeleton()->num_bones();

	if (m_vecBone)
	{
		for (size_t i = 0; i < NumJoint; ++i)
		{
			m_Global_transforms.transforms[i] = XMLoadFloat4x4((*m_vecBone)[m_vecBoneIndex[i]]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
		}
	}

	if (m_vecBone)
	{
		for (int i = 0; i < m_ragdoll->m_rigid_bodies.size(); i++)
		{
			if (m_ragdoll->m_rigid_bodies[i] && m_vecBreakPartFilter[i] == false)
			{
				XMMATRIX global_transform = m_Global_transforms.transforms[i];
				XMVECTOR body_pos_relative_to_joint = XMVectorSetW(m_ragdoll->m_body_pos_relative_to_joint[i], 1.f);

				XMMATRIX transform = m_model * global_transform;

				XMVECTOR pos_with_w = XMVectorSetW(body_pos_relative_to_joint, 1.0f);

				XMVECTOR temp = XMVector4Transform(pos_with_w, transform);

				_vector world_pos = XMVectorSet(XMVectorGetX(temp), XMVectorGetY(temp), XMVectorGetZ(temp), 1.f);

				XMMATRIX final_transform = m_model_without_scale * global_transform;

				XMVECTOR joint_rot = XMQuaternionRotationMatrix(final_transform);

				_vector diff_rot = XMQuaternionInverse(m_ragdoll->m_original_joint_rotations[i]) * joint_rot;

				_vector rotation = m_ragdoll->m_original_body_rotations[i] * diff_rot;

				rotation = XMQuaternionNormalize(rotation);

				PxTransform px_transform(to_vec3(world_pos), to_quat(XMQuaternionRotationMatrix(XMLoadFloat4x4((*m_vecBone)[m_vecBoneIndex[i]]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(&RotMat))));
				m_ragdoll->m_rigid_bodies[i]->setGlobalPose(px_transform);
			}
		}
	}
}

void CRagdoll_Physics::update_animations_partial_after()
{
	m_RotationMatrix = m_pTransform->Get_RotationMatrix_Pure();

	auto RotMat = m_RotationMatrix;
	RotMat._41 = m_pWorldMatrix->_41;
	RotMat._42 = m_pWorldMatrix->_42;
	RotMat._43 = m_pWorldMatrix->_43;

	auto joint = m_skeletal_mesh->skeleton()->joints();

	auto NumJoint = m_skeletal_mesh->skeleton()->num_bones();

	if (m_vecBone)
	{
		for (size_t i = 0; i < NumJoint; ++i)
		{
			m_Global_transforms.transforms[i] = XMLoadFloat4x4((*m_vecBone)[m_vecBoneIndex[i]]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
		}
	}

	m_Global_transforms = *m_ragdoll_pose->apply(m_ragdoll, m_model_only_scale, m_model_without_scale);

	if (m_vecBone)
	{
		m_RotationMatrix = m_pTransform->Get_RotationMatrix_Pure();

		auto WorldMat = m_RotationMatrix;

		for (size_t i = 0; i < NumJoint; ++i)
		{
			if (!IsIdentityMatrix(m_Global_transforms.transforms[i]))
			{
				auto Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMat));
				auto Result = m_Global_transforms.transforms[i] * Inverse;
				(*m_vecBone)[m_vecBoneIndex[i]]->Set_Combined_Matrix(Result);
			}
		}
	}
}

void CRagdoll_Physics::Init_Ragdoll()
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * MAX_COUNT_BONE);
	ZeroMemory(m_BoneMatrices_Cloth_Arm_L, sizeof(_float4x4) * MAX_COUNT_BONE);
	ZeroMemory(m_BoneMatrices_Cloth_Arm_R, sizeof(_float4x4) * MAX_COUNT_BONE);
	ZeroMemory(m_BoneMatrices_Cloth_Leg_L, sizeof(_float4x4) * MAX_COUNT_BONE);
	ZeroMemory(m_BoneMatrices_Cloth_Leg_R, sizeof(_float4x4) * MAX_COUNT_BONE);

	m_simulate = false;

	create_ragdoll();

	m_ragdoll->set_kinematic(true);

	m_bRagdoll = true;

	m_bPartialRagdoll = false;
}

void CRagdoll_Physics::create_joint()
{
#ifdef ZOMBIE
	m_pNeckJoint = create_d6_joint_Head(m_Chest, m_Head, m_neck_01_idx_Bone, m_neck_01_idx);

	if (!m_pUpSpine_Joint && m_vecBreakPartFilter[m_pelvis_idx] == false)
		m_pUpSpine_Joint = create_d6_joint_Head(m_Pelvis, m_Chest, m_spine_02_idx_Bone, m_spine_01_idx);

	// Pelvis to Thighs
	if (!m_pHip_Joint_L)
		m_pHip_Joint_L = create_d6_joint(m_Pelvis, m_Leg_L, m_thigh_l_idx_Bone, m_thigh_l_idx);
	if (!m_pHip_Joint_R)
		m_pHip_Joint_R = create_d6_joint(m_Pelvis, m_Leg_R, m_thigh_r_idx_Bone, m_thigh_r_idx);

	// Thighs to Calf
	if (!m_pKnee_Joint_L && m_Leg_L && m_Calf_L && m_Knee_L_Upper_Joint == nullptr)
		m_pKnee_Joint_L = create_d6_joint(m_Leg_L, m_Calf_L, m_calf_l_idx_Bone, m_calf_l_idx);
	if (!m_pKnee_Joint_R && m_Leg_R && m_Calf_R && m_Knee_R_Upper_Joint == nullptr)
		m_pKnee_Joint_R = create_d6_joint(m_Leg_R, m_Calf_R, m_calf_r_idx_Bone, m_calf_r_idx);

	// Calf to Foot
	if (!m_pAnkle_Joint_L && m_vecBreakPartFilter[m_foot_l_idx] == false)
		m_pAnkle_Joint_L = create_d6_joint_Foot(m_Calf_L, m_Foot_L, m_ball_l_idx_Bone, m_foot_l_idx);
	if (!m_pAnkle_Joint_R && m_vecBreakPartFilter[m_foot_r_idx] == false)
		m_pAnkle_Joint_R = create_d6_joint_Foot(m_Calf_R, m_Foot_R, m_ball_r_idx_Bone, m_foot_r_idx);

	// Chest to Upperarm
	if (!m_pClavicle_L_Joint)
		m_pClavicle_L_Joint = create_d6_joint(m_Chest, m_Arm_L, m_upperarm_l_idx_Bone, m_upperarm_l_idx);
	if (!m_pClavicle_R_Joint)
		m_pClavicle_R_Joint = create_d6_joint(m_Chest, m_Arm_R, m_upperarm_r_idx_Bone, m_upperarm_r_idx);

	//// Upperarm to Lowerman
	if (!m_pElbow_L_Joint && m_vecBreakPartFilter[m_lowerarm_l_idx] == false)
		m_pElbow_L_Joint = create_d6_joint(m_Arm_L, m_ForeArm_L, m_lowerarm_l_idx_Bone, m_lowerarm_l_idx);
	if (!m_pElbow_R_Joint && m_vecBreakPartFilter[m_lowerarm_r_idx] == false)
		m_pElbow_R_Joint = create_d6_joint(m_Arm_R, m_ForeArm_R, m_lowerarm_r_idx_Bone, m_lowerarm_r_idx);

	// Lowerarm to Hand
	if (!m_pWrist_L_Joint && m_vecBreakPartFilter[m_hand_l_idx] == false)
		m_pWrist_L_Joint = create_d6_joint(m_ForeArm_L, m_Hand_L, m_hand_l_idx_Bone, m_hand_l_idx);
	if (!m_pWrist_R_Joint && m_vecBreakPartFilter[m_hand_r_idx] == false)
		m_pWrist_R_Joint = create_d6_joint(m_ForeArm_R, m_Hand_R, m_hand_r_idx_Bone, m_hand_r_idx);
#endif
}


void CRagdoll_Physics::SetTransform(CTransform* pTransform)
{
	m_pTransform = pTransform;

	m_pWorldMatrix = m_pTransform->Get_WorldFloat4x4_Ptr();
}

void CRagdoll_Physics::SetSimulate_Partial(_bool boolean)
{
	//Update(1 / 60.f);

	m_simulate = boolean;

	m_ragdoll->set_kinematic(boolean);

	//Update(1 / 60.f);
}

void CRagdoll_Physics::ResetForce()
{
	if (m_Pelvis)
	{
		m_Pelvis->clearForce(PxForceMode::eFORCE);
		m_Pelvis->clearForce(PxForceMode::eIMPULSE);
		m_Pelvis->clearTorque(PxForceMode::eFORCE);
		m_Pelvis->clearTorque(PxForceMode::eIMPULSE);
	}

	if (m_Head)
	{
		m_Head->clearForce(PxForceMode::eFORCE);
		m_Head->clearForce(PxForceMode::eIMPULSE);
		m_Head->clearTorque(PxForceMode::eFORCE);
		m_Head->clearTorque(PxForceMode::eIMPULSE);
	}

	if (m_Arm_L)
	{
		m_Arm_L->clearForce(PxForceMode::eFORCE);
		m_Arm_L->clearForce(PxForceMode::eIMPULSE);
		m_Arm_L->clearTorque(PxForceMode::eFORCE);
		m_Arm_L->clearTorque(PxForceMode::eIMPULSE);
	}

	if (m_ForeArm_L)
	{
		m_ForeArm_L->clearForce(PxForceMode::eFORCE);
		m_ForeArm_L->clearForce(PxForceMode::eIMPULSE);
		m_ForeArm_L->clearTorque(PxForceMode::eFORCE);
		m_ForeArm_L->clearTorque(PxForceMode::eIMPULSE);
	}

	if (m_Hand_L)
	{
		m_Hand_L->clearForce(PxForceMode::eFORCE);
		m_Hand_L->clearForce(PxForceMode::eIMPULSE);
		m_Hand_L->clearTorque(PxForceMode::eFORCE);
		m_Hand_L->clearTorque(PxForceMode::eIMPULSE);
	}

	if (m_Arm_R)
	{
		m_Arm_R->clearForce(PxForceMode::eFORCE);
		m_Arm_R->clearForce(PxForceMode::eIMPULSE);
		m_Arm_R->clearTorque(PxForceMode::eFORCE);
		m_Arm_R->clearTorque(PxForceMode::eIMPULSE);
	}

	if (m_ForeArm_R)
	{
		m_ForeArm_R->clearForce(PxForceMode::eFORCE);
		m_ForeArm_R->clearForce(PxForceMode::eIMPULSE);
		m_ForeArm_R->clearTorque(PxForceMode::eFORCE);
		m_ForeArm_R->clearTorque(PxForceMode::eIMPULSE);
	}

	if (m_Hand_R)
	{
		m_Hand_R->clearForce(PxForceMode::eFORCE);
		m_Hand_R->clearForce(PxForceMode::eIMPULSE);
		m_Hand_R->clearTorque(PxForceMode::eFORCE);
		m_Hand_R->clearTorque(PxForceMode::eIMPULSE);
	}

	if (m_Leg_L)
	{
		m_Leg_L->clearForce(PxForceMode::eFORCE);
		m_Leg_L->clearForce(PxForceMode::eIMPULSE);
		m_Leg_L->clearTorque(PxForceMode::eFORCE);
		m_Leg_L->clearTorque(PxForceMode::eIMPULSE);
	}

	if (m_Calf_L)
	{
		m_Calf_L->clearForce(PxForceMode::eFORCE);
		m_Calf_L->clearForce(PxForceMode::eIMPULSE);
		m_Calf_L->clearTorque(PxForceMode::eFORCE);
		m_Calf_L->clearTorque(PxForceMode::eIMPULSE);
	}

	if (m_Foot_L)
	{
		m_Foot_L->clearForce(PxForceMode::eFORCE);
		m_Foot_L->clearForce(PxForceMode::eIMPULSE);
		m_Foot_L->clearTorque(PxForceMode::eFORCE);
		m_Foot_L->clearTorque(PxForceMode::eIMPULSE);
	}

	if (m_Leg_R)
	{
		m_Leg_R->clearForce(PxForceMode::eFORCE);
		m_Leg_R->clearForce(PxForceMode::eIMPULSE);
		m_Leg_R->clearTorque(PxForceMode::eFORCE);
		m_Leg_R->clearTorque(PxForceMode::eIMPULSE);
	}

	if (m_Calf_R)
	{
		m_Calf_R->clearForce(PxForceMode::eFORCE);
		m_Calf_R->clearForce(PxForceMode::eIMPULSE);
		m_Calf_R->clearTorque(PxForceMode::eFORCE);
		m_Calf_R->clearTorque(PxForceMode::eIMPULSE);
	}

	if (m_Foot_R)
	{
		m_Foot_R->clearForce(PxForceMode::eFORCE);
		m_Foot_R->clearForce(PxForceMode::eIMPULSE);
		m_Foot_R->clearTorque(PxForceMode::eFORCE);
		m_Foot_R->clearTorque(PxForceMode::eIMPULSE);
	}
}

void CRagdoll_Physics::Add_Force(_float4 vForce, COLLIDER_TYPE eType)
{
	switch (eType)
	{
	case COLLIDER_TYPE::HEAD:
	{
		auto fPower = rand() % 100;
		vForce = vForce * (fPower + 50.f);
		PxVec3 pxForce(vForce.x, vForce.y, vForce.z);
		m_Head->addForce(pxForce, PxForceMode::eIMPULSE);
		break;
	}
	case COLLIDER_TYPE::CHEST:
	{
		auto fPower = rand() % 100;
		vForce = vForce * (fPower + 50.f);
		PxVec3 pxForce(vForce.x, vForce.y, vForce.z);
		m_Chest->addForce(pxForce, PxForceMode::eIMPULSE);
		break;
	}
	case COLLIDER_TYPE::ARM_L:
	{
		auto fPower = rand() % 100;
		vForce = vForce * (fPower + 50.f);
		PxVec3 pxForce(vForce.x, vForce.y, vForce.z);
		m_Arm_L->addForce(pxForce, PxForceMode::eIMPULSE);
		break;
	}
	case COLLIDER_TYPE::ARM_R:
	{
		auto fPower = rand() % 100;
		vForce = vForce * (fPower + 50.f);
		PxVec3 pxForce(vForce.x, vForce.y, vForce.z);
		m_Arm_R->addForce(pxForce, PxForceMode::eIMPULSE);
		break;
	}
	case COLLIDER_TYPE::FOREARM_L:
	{
		auto fPower = rand() % 100;
		vForce = vForce * (fPower + 50.f);
		PxVec3 pxForce(vForce.x, vForce.y, vForce.z);
		m_ForeArm_L->addForce(pxForce, PxForceMode::eIMPULSE);
		break;
	}
	case COLLIDER_TYPE::FOREARM_R:
	{
		auto fPower = rand() % 100;
		vForce = vForce * (fPower + 50.f);
		PxVec3 pxForce(vForce.x, vForce.y, vForce.z);
		m_ForeArm_R->addForce(pxForce, PxForceMode::eIMPULSE);
		break;
	}
	case COLLIDER_TYPE::PELVIS:
	{
		auto fPower = rand() % 100;
		vForce = vForce * (fPower + 50.f);
		PxVec3 pxForce(vForce.x, vForce.y, vForce.z);
		m_Pelvis->addForce(pxForce, PxForceMode::eIMPULSE);
		break;
	}
	case COLLIDER_TYPE::LEG_L:
	{
		auto fPower = rand() % 100;
		vForce = vForce * (fPower + 50.f);
		PxVec3 pxForce(vForce.x, vForce.y, vForce.z);
		m_Leg_L->addForce(pxForce, PxForceMode::eIMPULSE);
		break;
	}
	case COLLIDER_TYPE::LEG_R:
	{
		auto fPower = rand() % 100;
		vForce = vForce * (fPower + 50.f);
		PxVec3 pxForce(vForce.x, vForce.y, vForce.z);
		m_Leg_R->addForce(pxForce, PxForceMode::eIMPULSE);
		break;
	}
	case COLLIDER_TYPE::CALF_L:
	{
		auto fPower = rand() % 100;
		vForce = vForce * (fPower + 50.f);
		PxVec3 pxForce(vForce.x, vForce.y, vForce.z);
		m_Calf_L->addForce(pxForce, PxForceMode::eIMPULSE);
		break;
	}
	case COLLIDER_TYPE::CALF_R:
	{
		auto fPower = rand() % 100;
		vForce = vForce * (fPower + 50.f);
		PxVec3 pxForce(vForce.x, vForce.y, vForce.z);
		m_Calf_R->addForce(pxForce, PxForceMode::eIMPULSE);
		break;
	}
	case COLLIDER_TYPE::FOOT_L:
	{
		auto fPower = rand() % 100;
		vForce = vForce * (fPower + 50.f);
		PxVec3 pxForce(vForce.x, vForce.y, vForce.z);
		m_Foot_L->addForce(pxForce, PxForceMode::eIMPULSE);
		break;
	}
	case COLLIDER_TYPE::FOOT_R:
	{
		auto fPower = rand() % 100;
		vForce = vForce * (fPower + 50.f);
		PxVec3 pxForce(vForce.x, vForce.y, vForce.z);
		m_Foot_R->addForce(pxForce, PxForceMode::eIMPULSE);
		break;
	}
	}
}

void CRagdoll_Physics::SetBoneIndex()
{
	auto Skeleton = m_skeletal_mesh->skeleton();

	auto joint = Skeleton->joints();

	auto NumJoint = Skeleton->num_bones();

	m_vecBoneIndex.resize(NumJoint);

	for (size_t i = 0; i < NumJoint; ++i)
	{
		for (int j = 0; j < m_vecBone->size(); ++j)
		{
			if ((*m_vecBone)[j]->Get_Name() == joint[i].name)
			{
				m_vecBoneIndex[i] = j;
			}
		}
	}

	m_head_idx = Skeleton->Find_BoneIndex("neck_1");
	m_neck_01_idx = Skeleton->Find_BoneIndex("neck_0");
	m_pelvis_idx = Skeleton->Find_BoneIndex("spine_0");
	m_spine_01_idx = Skeleton->Find_BoneIndex("spine_1");
	m_spine_02_idx = Skeleton->Find_BoneIndex("spine_2");
	m_spine_03_idx = Skeleton->Find_BoneIndex("hips");

	m_thigh_l_idx = Skeleton->Find_BoneIndex("l_leg_femur");
	m_calf_l_idx = Skeleton->Find_BoneIndex("l_leg_tibia");
	m_foot_l_idx = Skeleton->Find_BoneIndex("l_leg_ankle");
	m_ball_l_idx = Skeleton->Find_BoneIndex("l_leg_ball");

	m_thigh_r_idx = Skeleton->Find_BoneIndex("r_leg_femur");
	m_calf_r_idx = Skeleton->Find_BoneIndex("r_leg_tibia");
	m_foot_r_idx = Skeleton->Find_BoneIndex("r_leg_ankle");
	m_ball_r_idx = Skeleton->Find_BoneIndex("r_leg_ball");

	m_upperarm_l_idx = Skeleton->Find_BoneIndex("l_arm_humerus");
	m_lowerarm_l_idx = Skeleton->Find_BoneIndex("l_arm_radius");
	m_hand_l_idx = Skeleton->Find_BoneIndex("l_arm_wrist");
	m_middle_01_l_idx = Skeleton->Find_BoneIndex("l_hand_middle_0");

	m_upperarm_r_idx = Skeleton->Find_BoneIndex("r_arm_humerus");
	m_lowerarm_r_idx = Skeleton->Find_BoneIndex("r_arm_radius");
	m_hand_r_idx = Skeleton->Find_BoneIndex("r_arm_wrist");
	m_middle_01_r_idx = Skeleton->Find_BoneIndex("r_hand_middle_0");

	//TEMP
	m_upperarm_high_l_idx = Skeleton->Find_BoneIndex("l_arm_high_joint_lower");
	m_lowerarm_high_l_idx = Skeleton->Find_BoneIndex("l_arm_middle_joint_lower");

	m_upperarm_high_r_idx = Skeleton->Find_BoneIndex("r_arm_high_joint_lower");
	m_lowerarm_high_r_idx = Skeleton->Find_BoneIndex("r_arm_middle_joint_lower");

	m_Arm_L_Twist_0 = Skeleton->Find_BoneIndex("l_arm_humerus_twist_0_H");
	m_Arm_L_Twist_1 = Skeleton->Find_BoneIndex("l_arm_humerus_twist_1_H");
	m_Arm_L_Twist_2 = Skeleton->Find_BoneIndex("l_arm_humerus_twist_2_H");
	m_Arm_L_Twist_3 = Skeleton->Find_BoneIndex("l_arm_humerus_twist_3_H");

	m_Arm_R_Twist_0 = Skeleton->Find_BoneIndex("r_arm_humerus_twist_0_H");
	m_Arm_R_Twist_1 = Skeleton->Find_BoneIndex("r_arm_humerus_twist_1_H");
	m_Arm_R_Twist_2 = Skeleton->Find_BoneIndex("r_arm_humerus_twist_2_H");
	m_Arm_R_Twist_3 = Skeleton->Find_BoneIndex("r_arm_humerus_twist_3_H");

	m_ForeArm_L_Twist_0 = Skeleton->Find_BoneIndex("l_arm_radius_twist_0_H");
	m_ForeArm_L_Twist_1 = Skeleton->Find_BoneIndex("l_arm_radius_twist_1_H");
	m_ForeArm_L_Twist_2 = Skeleton->Find_BoneIndex("l_arm_radius_twist_2_H");
	m_ForeArm_L_Twist_3 = Skeleton->Find_BoneIndex("l_arm_radius_twist_3_H");

	m_ForeArm_R_Twist_0 = Skeleton->Find_BoneIndex("r_arm_radius_twist_0_H");
	m_ForeArm_R_Twist_1 = Skeleton->Find_BoneIndex("r_arm_radius_twist_1_H");
	m_ForeArm_R_Twist_2 = Skeleton->Find_BoneIndex("r_arm_radius_twist_2_H");
	m_ForeArm_R_Twist_3 = Skeleton->Find_BoneIndex("r_arm_radius_twist_3_H");

	m_Leg_L_Twist_0 = Skeleton->Find_BoneIndex("l_leg_femur_twist_0_H");
	m_Leg_L_Twist_1 = Skeleton->Find_BoneIndex("l_leg_femur_twist_1_H");
	m_Leg_L_Twist_2 = Skeleton->Find_BoneIndex("l_leg_femur_twist_2_H");

	m_Leg_R_Twist_0 = Skeleton->Find_BoneIndex("r_leg_femur_twist_0_H");
	m_Leg_R_Twist_1 = Skeleton->Find_BoneIndex("r_leg_femur_twist_1_H");
	m_Leg_R_Twist_2 = Skeleton->Find_BoneIndex("r_leg_femur_twist_2_H");

	m_Calf_L_Twist_0 = Skeleton->Find_BoneIndex("l_leg_tibia_twist_0_H");
	m_Calf_L_Twist_1 = Skeleton->Find_BoneIndex("l_leg_tibia_twist_1_H");

	m_Calf_R_Twist_0 = Skeleton->Find_BoneIndex("r_leg_tibia_twist_0_H");
	m_Calf_R_Twist_1 = Skeleton->Find_BoneIndex("r_leg_tibia_twist_1_H");

	m_Cloth_L_Arm_01 = Skeleton->Find_BoneIndex("l_arm_clavicle");
	m_Cloth_L_Arm_02 = Skeleton->Find_BoneIndex("l_arm_humerus_twist_1_H");
	m_Cloth_L_Arm_03 = Skeleton->Find_BoneIndex("spine_1");
	m_Cloth_L_Arm_04 = Skeleton->Find_BoneIndex("spine_2");
	m_Cloth_R_Arm_05 = Skeleton->Find_BoneIndex("spine_0");

	m_Cloth_R_Arm_01 = Skeleton->Find_BoneIndex("r_arm_clavicle");
	m_Cloth_R_Arm_02 = Skeleton->Find_BoneIndex("r_arm_humerus_twist_1_H");
	m_Cloth_R_Arm_03 = Skeleton->Find_BoneIndex("spine_1");
	m_Cloth_R_Arm_04 = Skeleton->Find_BoneIndex("spine_2");
	m_Cloth_R_Arm_05 = Skeleton->Find_BoneIndex("spine_0");


	m_head_idx_Bone = Find_BoneIndex("neck_1");
	m_neck_01_idx_Bone = Find_BoneIndex("neck_0");
	m_pelvis_idx_Bone = Find_BoneIndex("spine_0");
	m_spine_01_idx_Bone = Find_BoneIndex("spine_1");
	m_spine_02_idx_Bone = Find_BoneIndex("spine_2");
	m_spine_03_idx_Bone = Find_BoneIndex("hips");

	m_thigh_l_idx_Bone = Find_BoneIndex("l_leg_femur");
	m_calf_l_idx_Bone = Find_BoneIndex("l_leg_tibia");
	m_foot_l_idx_Bone = Find_BoneIndex("l_leg_ankle");
	m_ball_l_idx_Bone = Find_BoneIndex("l_leg_ball");

	m_thigh_r_idx_Bone = Find_BoneIndex("r_leg_femur");
	m_calf_r_idx_Bone = Find_BoneIndex("r_leg_tibia");
	m_foot_r_idx_Bone = Find_BoneIndex("r_leg_ankle");
	m_ball_r_idx_Bone = Find_BoneIndex("r_leg_ball");

	m_upperarm_l_idx_Bone = Find_BoneIndex("l_arm_humerus");
	m_lowerarm_l_idx_Bone = Find_BoneIndex("l_arm_radius");
	m_hand_l_idx_Bone = Find_BoneIndex("l_arm_wrist");
	m_middle_01_l_idx_Bone = Find_BoneIndex("l_hand_middle_0");

	m_upperarm_r_idx_Bone = Find_BoneIndex("r_arm_humerus");
	m_lowerarm_r_idx_Bone = Find_BoneIndex("r_arm_radius");
	m_hand_r_idx_Bone = Find_BoneIndex("r_arm_wrist");
	m_middle_01_r_idx_Bone = Find_BoneIndex("r_hand_middle_0");

	//TEMP
	m_upperarm_high_l_idx_Bone = Find_BoneIndex("l_arm_high_joint_lower");
	m_lowerarm_high_l_idx_Bone = Find_BoneIndex("l_arm_middle_joint_lower");

	m_upperarm_high_r_idx_Bone = Find_BoneIndex("r_arm_high_joint_lower");
	m_lowerarm_high_r_idx_Bone = Find_BoneIndex("r_arm_middle_joint_lower");
}

_int CRagdoll_Physics::Find_BoneIndex(const string& strRootTag)
{
	_int		iIndex = { 0 };
	for (auto& Bone : *m_vecBone)
	{
		if (true == Bone->Compare_Name(strRootTag.c_str()))
			return iIndex;

		++iIndex;
	}

	return -1;
}

_float4 CRagdoll_Physics::GetRigidBodyPos(COLLIDER_TYPE eType)
{
	PxRigidDynamic* pBody = nullptr;
	switch (eType)
	{
	case COLLIDER_TYPE::HEAD:
		pBody = m_Head;
		break;
	case COLLIDER_TYPE::CHEST:
		pBody = m_Chest;
		break;
	case COLLIDER_TYPE::PELVIS:
		pBody = m_Pelvis;
		break;
	case COLLIDER_TYPE::ARM_L:

		break;
	case COLLIDER_TYPE::ARM_R:

		break;
	case COLLIDER_TYPE::FOREARM_L:

		break;
	case COLLIDER_TYPE::FOREARM_R:

		break;
	case COLLIDER_TYPE::HAND_L:

		break;
	case COLLIDER_TYPE::HAND_R:

		break;
	case COLLIDER_TYPE::CALF_L:

		break;
	case COLLIDER_TYPE::CALF_R:

		break;
	case COLLIDER_TYPE::LEG_L:

		break;
	case COLLIDER_TYPE::LEG_R:

		break;
	case COLLIDER_TYPE::FOOT_L:

		break;
	case COLLIDER_TYPE::FOOT_R:

		break;
	}

	if (pBody)
	{
		auto vPos = pBody->getGlobalPose().p;
		_float4 vPos_float = _float4(vPos.x, vPos.y, vPos.z, 1.f);

		return vPos_float;
	}
	else
	{
		return _float4(0.f, 0.f, 0.f, 1.f);
	}
}

_float4 CRagdoll_Physics::GetBodyPosition()
{
	_float4 vPos = _float4(0.f, 0.f, 0.f, 1.f);

	if (m_Chest)
	{
		auto ChestPos = m_Chest->getGlobalPose().p;
		vPos = _float4(ChestPos.x, ChestPos.y, ChestPos.z, 1.f);
	}

	return vPos;
}

CRagdoll_Physics* CRagdoll_Physics::Create()
{
	return nullptr;
}

void CRagdoll_Physics::Init_PartialRagdoll(COLLIDER_TYPE eType)
{
	switch (eType)
	{
	case COLLIDER_TYPE::FOREARM_L:
		m_iRagdollType |= RAGDOLL_TYPE::LEFT_FOREARM;
		break;
	case COLLIDER_TYPE::FOREARM_R:
		m_iRagdollType |= RAGDOLL_TYPE::RIGHT_FOREARM;
		break;
	case COLLIDER_TYPE::ARM_L:
		m_iRagdollType |= RAGDOLL_TYPE::LEFT_ARM;
		break;
	case COLLIDER_TYPE::ARM_R:
		m_iRagdollType |= RAGDOLL_TYPE::RIGHT_ARM;
		break;
	case COLLIDER_TYPE::PELVIS:
		break;////////////////////////////////TEMP///////////////////////////////////////
		m_iRagdollType |= RAGDOLL_TYPE::BODY_DIVIDE;
		break;
	case COLLIDER_TYPE::CALF_L:
		m_iRagdollType |= RAGDOLL_TYPE::LEFT_CALF;
		break;
	case COLLIDER_TYPE::CALF_R:
		m_iRagdollType |= RAGDOLL_TYPE::RIGHT_CALF;
		break;
	case COLLIDER_TYPE::LEG_L:
		m_iRagdollType |= RAGDOLL_TYPE::LEFT_LEG;
		break;
	case COLLIDER_TYPE::LEG_R:
		m_iRagdollType |= RAGDOLL_TYPE::RIGHT_LEG;
		break;
	}

	create_partial_ragdoll(eType);

	m_ragdoll->set_kinematic(true);

	m_bPartialRagdoll = true;
}

_float4x4* CRagdoll_Physics::GetCombinedMatrix_Ragdoll(_uint iIndex)
{
	return &m_BoneMatrices[iIndex];
}

_float4x4* CRagdoll_Physics::GetCombinedMatrix_Ragdoll_Cloth_Arm_R(_uint iIndex)
{
	return &m_BoneMatrices_Cloth_Arm_R[iIndex];
}

_float4x4* CRagdoll_Physics::GetCombinedMatrix_Ragdoll_Cloth_Leg_L(_uint iIndex)
{
	return &m_BoneMatrices_Cloth_Leg_L[iIndex];
}

_float4x4* CRagdoll_Physics::GetCombinedMatrix_Ragdoll_Cloth_Leg_R(_uint iIndex)
{
	return &m_BoneMatrices_Cloth_Leg_R[iIndex];
}

void CRagdoll_Physics::WakeUp()
{
	m_ragdoll->WakeUp();
}

void CRagdoll_Physics::PlaySound_Ragdoll(COLLIDER_TYPE eType)
{
	switch (eType)
	{
	case COLLIDER_TYPE::ARM_L:
		if (m_vecRagdollSound_Filter[0] == false)
		{
			if(m_Arm_L_High)
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(0, 2);

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 0);

				m_vecRagdollSound_Filter[0] = true;
			}
			else
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(3, 12);

				if (inum == 11)
					inum = 12;

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 0);

				m_vecRagdollSound_Filter[0] = true;
			}
		}
		break;
	case COLLIDER_TYPE::ARM_R:
		if (m_vecRagdollSound_Filter[1] == false)
		{
			if(m_Arm_R_High)
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(0,2);

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 1);

				m_vecRagdollSound_Filter[1] = true;
			}
			else
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(3, 12);

				if (inum == 11)
					inum = 12;

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 1);

				m_vecRagdollSound_Filter[1] = true;
			}
		}
		break;
	case COLLIDER_TYPE::FOREARM_L:
		if (m_vecRagdollSound_Filter[2] == false)
		{
			if(m_Arm_L_High || m_ForeArm_L_High)
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(0, 2);

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 2);

				m_vecRagdollSound_Filter[2] = true;
			}
			else
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(3, 12);

				if (inum == 11)
					inum = 12;

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 2);

				m_vecRagdollSound_Filter[2] = true;
			}
		}
		break;
	case COLLIDER_TYPE::FOREARM_R:
		if (m_vecRagdollSound_Filter[3] == false)
		{
			if(m_ForeArm_R_High || m_Arm_R_High)
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(0, 2);

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 3);

				m_vecRagdollSound_Filter[3] = true;
			}
			else
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(3, 12);

				if (inum == 11)
					inum = 12;

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 3);

				m_vecRagdollSound_Filter[3] = true;
			}
		}
		break;
	case COLLIDER_TYPE::HEAD:
		if (m_vecRagdollSound_Filter[4] == false)
		{
			const wchar_t* str = L"Break_Drop_";
			wchar_t result[32];
			_int inum = m_pGameInstance->GetRandom_Int(3, 12);

			if (inum == 11)
				inum = 12;

			std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

			m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 4);

			m_vecRagdollSound_Filter[4] = true;
		}
		break;
	case COLLIDER_TYPE::CHEST:
		if (m_vecRagdollSound_Filter[5] == false)
		{
			const wchar_t* str = L"Break_Drop_";
			wchar_t result[32];
			_int inum = m_pGameInstance->GetRandom_Int(3, 12);

			if (inum == 11)
				inum = 12;

			std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

			m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 5);

			m_vecRagdollSound_Filter[5] = true;
		}
		break;
	case COLLIDER_TYPE::LEG_L:
		if (m_vecRagdollSound_Filter[6] == false)
		{
			if(m_Leg_L_High)
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(0, 2);

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 6);

				m_vecRagdollSound_Filter[6] = true;
			}
			else
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(3, 12);

				if (inum == 11)
					inum = 12;

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 6);

				m_vecRagdollSound_Filter[6] = true;
			}
		}
		break;
	case COLLIDER_TYPE::LEG_R:
		if (m_vecRagdollSound_Filter[7] == false)
		{
			if(m_Leg_R_High)
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(0, 2);

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 7);

				m_vecRagdollSound_Filter[7] = true;
			}
			else
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(3,12);

				if (inum == 11)
					inum = 12;

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 7);

				m_vecRagdollSound_Filter[7] = true;
			}
		}
		break;
	case COLLIDER_TYPE::CALF_L:
		if (m_vecRagdollSound_Filter[8] == false)
		{
			if(m_Leg_L_High || m_Calf_L_High)
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(0,2);

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 8);

				m_vecRagdollSound_Filter[8] = true;
			}
			else
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(3, 12);

				if (inum == 11)
					inum = 12;

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 8);

				m_vecRagdollSound_Filter[8] = true;
			}
		}
		break;
	case COLLIDER_TYPE::CALF_R:
		if (m_vecRagdollSound_Filter[9] == false)
		{
			if(m_Calf_R_High)
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(0, 2);

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 9);

				m_vecRagdollSound_Filter[9] = true;
			}
			else
			{
				const wchar_t* str = L"Break_Drop_";
				wchar_t result[32];
				_int inum = m_pGameInstance->GetRandom_Int(3, 12);

				if (inum == 11)
					inum = 12;

				std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

				m_pGameInstance->Change_Sound_3D(m_pTransformCom_Sound, result, 9);

				m_vecRagdollSound_Filter[9] = true;
			}
		}
		break;
	}
}

_float4x4* CRagdoll_Physics::GetCombinedMatrix_Ragdoll_Cloth_Arm_L(_uint iIndex)
{
	return &m_BoneMatrices_Cloth_Arm_L[iIndex];
}

void CRagdoll_Physics::Free()
{
	if (m_Pelvis)
	{
		m_Pelvis->release();
		m_Head->release();
		m_Arm_L->release();
		m_Arm_R->release();
		m_Leg_R->release();
		m_Leg_L->release();
		m_ForeArm_L->release();
		m_ForeArm_R->release();
		if (m_Calf_R)
			m_Calf_R->release();
		if (m_Calf_L)
			m_Calf_L->release();
		m_Hand_L->release();
		m_Hand_R->release();
		m_Foot_R->release();
		m_Foot_L->release();

		if (m_Chest)
			m_Chest->release();
	}

	delete m_ragdoll;
	delete m_ragdoll_pose;
	delete m_skeletal_mesh->skeleton();
	delete m_skeletal_mesh;

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pTransformCom_Sound);
}
