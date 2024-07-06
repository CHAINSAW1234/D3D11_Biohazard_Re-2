#include "Physics_Controller.h"
#include "RagDoll_Physics.h"
#include "RagDoll.h"
#include "Skeletal_Mesh.h"
#include "Skeleton.h"
#include "Bone.h"
#include "Transform.h"

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

_bool CRagdoll_Physics::Init(const string& name)
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

	m_ragdoll->m_rigid_bodies.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_relative_joint_pos.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_body_rotations.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_body_pos_relative_to_joint.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_joint_rotations.resize(m_skeletal_mesh->skeleton()->num_bones());

	for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		m_ragdoll->m_rigid_bodies[i] = nullptr;

	// ---------------------------------------------------------------------------------------------------------------
	// Create rigid bodies for limbs
	// ---------------------------------------------------------------------------------------------------------------

	float     r = RADIUS * m_scale;
	_matrix rot = XMMatrixIdentity();
	rot = XMMatrixRotationZ(XM_PI * 0.5f);

#ifdef ZOMBIE
	m_Pelvis = create_capsule_bone(m_pelvis_idx, m_spine_02_idx, *m_ragdoll, CHEST_SIZE * m_scale, rot,COLLIDER_TYPE::PELVIS);
#endif

	m_Head = create_capsule_bone(m_head_idx, *m_ragdoll, XMVectorSet(0.0f, 3.0f * m_scale, 0.0f, 1.f), 4.0f * m_scale, 6.0f * m_scale, rot, COLLIDER_TYPE::HEAD);
	m_Leg_L = create_capsule_bone(m_thigh_l_idx, m_calf_l_idx, *m_ragdoll, r, rot,COLLIDER_TYPE::LEG_L);
	m_Leg_R = create_capsule_bone(m_thigh_r_idx, m_calf_r_idx, *m_ragdoll, r, rot, COLLIDER_TYPE::LEG_R);

#ifdef ZOMBIE
	m_Chest = create_capsule_bone(m_spine_02_idx, m_neck_01_idx, *m_ragdoll, CHEST_SIZE * m_scale, rot, COLLIDER_TYPE::CHEST);
#endif

	m_Calf_L = create_capsule_bone(m_calf_l_idx, m_foot_l_idx, *m_ragdoll, r, rot, COLLIDER_TYPE::CALF_L);
	m_Calf_R = create_capsule_bone(m_calf_r_idx, m_foot_r_idx, *m_ragdoll, r, rot, COLLIDER_TYPE::CALF_R);

	m_Arm_L = create_capsule_bone(m_upperarm_l_idx, m_lowerarm_l_idx, *m_ragdoll, r * SIZE_MAG,XMMatrixIdentity(), COLLIDER_TYPE::ARM_L);
	m_Arm_R = create_capsule_bone(m_upperarm_r_idx, m_lowerarm_r_idx, *m_ragdoll, r * SIZE_MAG, XMMatrixIdentity(), COLLIDER_TYPE::ARM_R);

	m_ForeArm_L = create_capsule_bone(m_lowerarm_l_idx,m_hand_l_idx, *m_ragdoll, r* SIZE_MAG,XMMatrixIdentity(),COLLIDER_TYPE::FOREARM_L);
	m_ForeArm_R = create_capsule_bone(m_lowerarm_r_idx,m_hand_r_idx, *m_ragdoll, r* SIZE_MAG,XMMatrixIdentity(),COLLIDER_TYPE::FOREARM_R);

	m_Hand_L = create_capsule_bone(m_hand_l_idx, m_middle_01_l_idx, *m_ragdoll, r* SIZE_MAG,XMMatrixIdentity(),COLLIDER_TYPE::HAND_L);
	m_Hand_R = create_capsule_bone(m_hand_r_idx, m_middle_01_r_idx, *m_ragdoll, r* SIZE_MAG,XMMatrixIdentity(),COLLIDER_TYPE::HAND_R);

	rot = XMMatrixRotationY(PxPi * 0.5f);

	m_Foot_L = create_capsule_bone(m_foot_l_idx, m_ball_l_idx, *m_ragdoll, r, rot,COLLIDER_TYPE::FOOT_L);
	m_Foot_R = create_capsule_bone(m_foot_r_idx, m_ball_r_idx, *m_ragdoll, r, rot,COLLIDER_TYPE::FOOT_R);

#ifdef ZOMBIE
	m_ragdoll->m_rigid_bodies[1] = m_Pelvis;
#endif

#pragma endregion
	for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
	{
		uint32_t        chosen_idx;
		PxRigidDynamic* body = m_ragdoll->find_recent_body((uint32_t)i, m_skeletal_mesh->skeleton(), chosen_idx);

		if (!body)
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

	// ---------------------------------------------------------------------------------------------------------------
	// Add rigid bodies to scene
	// ---------------------------------------------------------------------------------------------------------------

	// Chest and Head
	m_Scene->addActor(*m_Pelvis);
	m_Scene->addActor(*m_Head);

	// Left Arm
	m_Scene->addActor(*m_Arm_L);
	m_Scene->addActor(*m_ForeArm_L);
	m_Scene->addActor(*m_Hand_L);

	// Right Arm
	m_Scene->addActor(*m_Arm_R);
	m_Scene->addActor(*m_ForeArm_R);
	m_Scene->addActor(*m_Hand_R);

	// Left Leg
	m_Scene->addActor(*m_Leg_L);
	m_Scene->addActor(*m_Calf_L);
	m_Scene->addActor(*m_Foot_L);

	// Right Leg
	m_Scene->addActor(*m_Leg_R);
	m_Scene->addActor(*m_Calf_R);
	m_Scene->addActor(*m_Foot_R);

#ifdef ZOMBIE
	m_Scene->addActor(*m_Chest);
#endif
#pragma endregion
}

void CRagdoll_Physics::Update(_float fTimeDelta)
{
	if (m_bCulling)
		return;

	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		m_pose_transforms.transforms[i] = XMMatrixInverse(nullptr, joints[i].inverse_bind_pose);

	update_animations();
}

void CRagdoll_Physics::update_animations()
{
	if (m_bRagdoll == false)
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
		}
	}

	if (!m_simulate)
	{
		if (m_vecBone)
		{
			for (int i = 0; i < m_ragdoll->m_rigid_bodies.size(); i++)
			{
				if (m_ragdoll->m_rigid_bodies[i])
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

			if (m_bJoint_Set == false)
			{
				m_bJoint_Set = true;
				create_joint();
			}
		}
	}
	else
	{
		m_Global_transforms = *m_ragdoll_pose->apply(m_ragdoll, m_model_only_scale, m_model_without_scale);

		auto joint = m_skeletal_mesh->skeleton()->joints();

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
}

void CRagdoll_Physics::Init_Ragdoll()
{
	create_ragdoll();

	m_ragdoll->set_kinematic(true);

	m_bRagdoll = true;
}

void CRagdoll_Physics::create_joint()
{

#ifdef ZOMBIE
	m_pNeckJoint = create_d6_joint_Head(m_Chest, m_Head, m_neck_01_idx_Bone, m_neck_01_idx);
	m_pUpSpine_Joint = create_d6_joint_Head(m_Pelvis, m_Chest, m_spine_02_idx_Bone, m_spine_01_idx);

	// Pelvis to Thighs
	m_pHip_Joint_L = create_d6_joint(m_Pelvis, m_Leg_L, m_thigh_l_idx_Bone, m_thigh_l_idx);
	m_pHip_Joint_R = create_d6_joint(m_Pelvis, m_Leg_R, m_thigh_r_idx_Bone, m_thigh_r_idx);

	// Thighs to Calf
	m_pKnee_Joint_L = create_d6_joint(m_Leg_L, m_Calf_L, m_calf_l_idx_Bone, m_calf_l_idx);
	m_pKnee_Joint_R = create_d6_joint(m_Leg_R, m_Calf_R, m_calf_r_idx_Bone, m_calf_r_idx);

	// Calf to Foot
	m_pAnkle_Joint_L = create_d6_joint_Foot(m_Calf_L, m_Foot_L, m_ball_l_idx_Bone, m_foot_l_idx);
	m_pAnkle_Joint_R = create_d6_joint_Foot(m_Calf_R, m_Foot_R, m_ball_r_idx_Bone, m_foot_r_idx);

	// Chest to Upperarm
	m_pClavicle_L_Joint = create_d6_joint(m_Chest, m_Arm_L, m_upperarm_l_idx_Bone,m_upperarm_l_idx);
	m_pClavicle_R_Joint = create_d6_joint(m_Chest, m_Arm_R, m_upperarm_r_idx_Bone,m_upperarm_r_idx);

	// Upperarm to Lowerman
	m_pElbow_L_Joint = create_d6_joint(m_Arm_L, m_ForeArm_L, m_lowerarm_l_idx_Bone, m_lowerarm_l_idx);
	m_pElbow_R_Joint = create_d6_joint(m_Arm_R, m_ForeArm_R, m_lowerarm_r_idx_Bone, m_lowerarm_r_idx);

	// Lowerarm to Hand
	m_pWrist_L_Joint = create_d6_joint(m_ForeArm_L, m_Hand_L, m_hand_l_idx_Bone, m_hand_l_idx);
	m_pWrist_R_Joint = create_d6_joint(m_ForeArm_R, m_Hand_R, m_hand_r_idx_Bone, m_hand_r_idx);
#endif
}


void CRagdoll_Physics::SetTransform(CTransform* pTransform)
{
	m_pTransform = pTransform;

	m_pWorldMatrix = m_pTransform->Get_WorldFloat4x4_Ptr();
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





	m_head_idx_Bone = Find_BoneIndex("neck_1");
	m_neck_01_idx_Bone = Find_BoneIndex("neck_0");
	m_pelvis_idx_Bone = Find_BoneIndex("spine_0");
	m_spine_01_idx_Bone = Find_BoneIndex("spine_1");
	m_spine_02_idx_Bone = Find_BoneIndex("spine_2");
	m_spine_03_idx_Bone = Find_BoneIndex("hips");

	m_thigh_l_idx_Bone =Find_BoneIndex("l_leg_femur");
	m_calf_l_idx_Bone = Find_BoneIndex("l_leg_tibia");
	m_foot_l_idx_Bone = Find_BoneIndex("l_leg_ankle");
	m_ball_l_idx_Bone = Find_BoneIndex("l_leg_ball");

	m_thigh_r_idx_Bone =Find_BoneIndex("r_leg_femur");
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

CRagdoll_Physics* CRagdoll_Physics::Create()
{
	return nullptr;
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
		m_Calf_R->release();
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
}
