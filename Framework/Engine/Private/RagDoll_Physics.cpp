#include "Physics_Controller.h"
#include "RagDoll_Physics.h"
#include "RagDoll.h"
#include "Skeletal_Mesh.h"
#include "Skeleton.h"
#include "Bone.h"

#define JOINT_COUNT 20
#define NECK_BONE  167
#define SPINE_01_BONE 60
#define SPINE_02_BONE 61
#define SPINE_03_BONE 62
#define HEAD_BONE 169
#define PELVIS_BONE 4
#define L_LEG_BONE 27
#define L_CALF_BONE 28
#define L_ANCLE_BONE 33
#define L_FOOT_BONE 34
#define R_LEG_BONE 41
#define R_CALF_BONE 42
#define R_ANCLE_BONE 47
#define R_FOOT_BONE 48
#define L_ARM_BONE 84
#define L_FOREARM_BONE 85
#define L_WRIST_BONE 86
#define L_HAND_BONE 87
#define R_ARM_BONE 126
#define R_FOREARM_BONE 127
#define R_WRIST_BONE 128
#define R_HAND_BONE 129

#define MODEL_SCALE 0.01f

PxRigidDynamic* CRagdoll_Physics::create_capsule_bone(uint32_t parent_idx, uint32_t child_idx, CRagdoll& ragdoll, float r, XMMATRIX rotation)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();
	XMVECTOR parent_pos = XMLoadFloat3(&joints[parent_idx].bind_pos_ws(m_model));
	parent_pos = XMVectorSetW(parent_pos, 1.f);
	XMVECTOR child_pos = XMLoadFloat3(&joints[child_idx].bind_pos_ws(m_model));
	child_pos = XMVectorSetW(child_pos, 1.f);

	XMVECTOR diff = XMVectorSubtract(parent_pos, child_pos);
	float len = XMVectorGetX(XMVector3Length(diff));
	len -= len * 0.2f;

	r *= (MODEL_SCALE);

	float len_minus_2r = len - 2.0f * r;
	float half_height = len_minus_2r / 2.0f;

#pragma region 크기 줄이는 코드
	half_height *= MODEL_SCALE;
#pragma endregion

	XMVECTOR body_pos = XMVectorScale(XMVectorAdd(parent_pos, child_pos), 0.5f);
	body_pos = XMVectorSetW(body_pos, 1.f);

	PxShape* shape = m_Physics->createShape(PxCapsuleGeometry(r, half_height), *m_material);

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

PxRigidDynamic* CRagdoll_Physics::create_capsule_bone(uint32_t parent_idx, CRagdoll& ragdoll, XMVECTOR offset, float l, float r, XMMATRIX rotation)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	XMVECTOR parent_pos = XMLoadFloat3(&joints[parent_idx].bind_pos_ws(m_model)) + offset;
	parent_pos = XMVectorSetW(parent_pos, 1.f);

#pragma region 크기 줄이는 코드
	l *= MODEL_SCALE;
	r *= (MODEL_SCALE);
#pragma endregion

	PxShape* shape = m_Physics->createShape(PxCapsuleGeometry(r, l), *m_material);
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

PxRigidDynamic* CRagdoll_Physics::create_sphere_bone(uint32_t parent_idx, CRagdoll& ragdoll, float r)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();
	XMVECTOR parent_pos = XMLoadFloat3(&joints[parent_idx].bind_pos_ws(m_model));
	parent_pos = XMVectorSetW(parent_pos, 1.f);

#pragma region 크기 줄이는 코드
	r *= (MODEL_SCALE);
#pragma endregion

	PxShape* shape = m_Physics->createShape(PxSphereGeometry(r), *m_material);

	PxRigidDynamic* body = m_Physics->createRigidDynamic(PxTransform(to_vec3(parent_pos)));

	body->attachShape(*shape);
	body->setMass(m_mass);

	m_ragdoll->m_rigid_bodies[parent_idx] = body;
	//body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	float sleepThreshold = 0.5f;
	body->setSleepThreshold(sleepThreshold);

	return body;
}

void CRagdoll_Physics::create_d6_joint(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t joint_pos)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	_matrix p_tf = XMLoadFloat4x4((*m_vecBone)[joint_pos]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
	_vector p = XMVectorSet(p_tf.r[3].m128_f32[0], p_tf.r[3].m128_f32[1], p_tf.r[3].m128_f32[2], 1.0f);

#pragma region 크기 줄이는 코드

#pragma endregion

	_vector q = XMQuaternionRotationMatrix(XMMatrixInverse(nullptr, joints[joint_pos].inverse_bind_pose));

	PxD6Joint* joint = PxD6JointCreate(*m_Physics,
		parent,
		parent->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))),
		child,
		child->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))));

	joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	config_d6_joint(3.14 / 4.f, 3.14f / 4.f, -3.14f / 8.f, 3.14f / 8.f, joint);

	joint->setBreakForce(FLT_MAX, FLT_MAX);
}

void CRagdoll_Physics::create_revolute_joint(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t joint_pos, XMMATRIX rotation)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	_matrix p_tf = XMLoadFloat4x4((*m_vecBone)[joint_pos]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
	_vector p = XMVectorSet(p_tf.r[3].m128_f32[0], p_tf.r[3].m128_f32[1], p_tf.r[3].m128_f32[2], 1.0f);

	p = XMVectorSetW(p, 1.f);
	_vector q = XMQuaternionRotationMatrix(XMMatrixInverse(nullptr, joints[joint_pos].inverse_bind_pose) * XMMatrixRotationY(PxPi) * XMLoadFloat4x4(m_pRotationMatrix));

	PxRevoluteJoint* joint = PxRevoluteJointCreate(*m_Physics,
		parent,
		parent->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))),
		child,
		child->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))));

	joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	joint->setLimit(PxJointAngularLimitPair(0.0f, PxPi));
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

	m_FilterData.word0 = COLLISION_CATEGORY::Category2;
	m_FilterData.word1 = COLLISION_CATEGORY::Category1 | COLLISION_CATEGORY::Category3;
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
	//m_skeletal_mesh = SkeletalMesh::load("../Bin/Resources/Models/LeonTest/LeonBody.fbx");
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

	for (int i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		m_joint_names.push_back(m_skeletal_mesh->skeleton()->joints()[i].name);

	for (int i = 0; i < MAX_BONES; i++)
		m_pose_transforms.transforms[i] = XMMatrixIdentity();

	m_index_stack.reserve(256);
	m_joint_pos.reserve(256);

	m_joint_mat.resize(MAX_BONES);

	update_transforms();

	m_ragdoll_pose = new AnimRagdoll(m_skeletal_mesh->skeleton());

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

void CRagdoll_Physics::create_ragdoll()
{
	if (!m_vecBone)
		return;

 	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	//For Find Bone Index
	//m_skeletal_mesh->skeleton()->find_joint_index("Head");

	uint32_t j_head_idx = 167;
	uint32_t j_neck_01_idx = 165;
	uint32_t j_spine_03_idx = 2;
	uint32_t j_spine_02_idx = 59;
	uint32_t j_spine_01_idx = 60;
	uint32_t j_pelvis_idx = 58;

	uint32_t j_thigh_l_idx = 25;
	uint32_t j_calf_l_idx = 26;
	uint32_t j_foot_l_idx = 31;
	uint32_t j_ball_l_idx = 32;

	uint32_t j_thigh_r_idx = 39;
	uint32_t j_calf_r_idx = 40;
	uint32_t j_foot_r_idx = 45;
	uint32_t j_ball_r_idx = 46;

	uint32_t j_upperarm_l_idx = 82;
	uint32_t j_lowerarm_l_idx = 83;
	uint32_t j_hand_l_idx = 84;
	uint32_t j_middle_01_l_idx = 85;

	uint32_t j_upperarm_r_idx = 124;
	uint32_t j_lowerarm_r_idx = 125;
	uint32_t j_hand_r_idx = 126;
	uint32_t j_middle_01_r_idx = 127;

	m_ragdoll->m_rigid_bodies.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_relative_joint_pos.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_body_rotations.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_body_pos_relative_to_joint.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_joint_rotations.resize(m_skeletal_mesh->skeleton()->num_bones());

	for (int i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		m_ragdoll->m_rigid_bodies[i] = nullptr;

	// ---------------------------------------------------------------------------------------------------------------
	// Create rigid bodies for limbs
	// ---------------------------------------------------------------------------------------------------------------

	float     r = 2.5f * m_scale;
	_matrix rot = XMMatrixIdentity();
	rot = XMMatrixRotationZ(XM_PI * 0.5f);

	m_Pelvis = create_capsule_bone(j_pelvis_idx, j_neck_01_idx, *m_ragdoll, 5.0f * m_scale, rot);
	m_ragdoll->m_rigid_bodies[2] = m_Pelvis;
	m_Head = create_capsule_bone(j_head_idx, *m_ragdoll, XMVectorSet(0.0f, 3.0f * m_scale, 0.0f, 1.f), 4.0f * m_scale, 6.0f * m_scale, rot);

	m_Leg_L = create_capsule_bone(j_thigh_l_idx, j_calf_l_idx, *m_ragdoll, r, rot);
	m_ragdoll->m_rigid_bodies[3] = m_Leg_L;
	m_Leg_R = create_capsule_bone(j_thigh_r_idx, j_calf_r_idx, *m_ragdoll, r, rot);
	m_ragdoll->m_rigid_bodies[9] = m_Leg_R;

	m_Calf_L = create_capsule_bone(j_calf_l_idx, j_foot_l_idx, *m_ragdoll, r, rot);
	m_Calf_R = create_capsule_bone(j_calf_r_idx, j_foot_r_idx, *m_ragdoll, r, rot);

	m_Arm_L = create_capsule_bone(j_upperarm_l_idx, j_lowerarm_l_idx, *m_ragdoll, r);
	m_Arm_R = create_capsule_bone(j_upperarm_r_idx, j_lowerarm_r_idx, *m_ragdoll, r);

	m_ForeArm_L = create_capsule_bone(j_lowerarm_l_idx, j_hand_l_idx, *m_ragdoll, r);
	m_ForeArm_R = create_capsule_bone(j_lowerarm_r_idx, j_hand_r_idx, *m_ragdoll, r);

	m_Hand_L = create_sphere_bone(j_middle_01_l_idx, *m_ragdoll, r);
	m_Hand_R = create_sphere_bone(j_middle_01_r_idx, *m_ragdoll, r);

	rot = XMMatrixIdentity();

	m_Foot_L = create_capsule_bone(j_foot_l_idx, j_ball_l_idx, *m_ragdoll, r, rot);
	m_Foot_R = create_capsule_bone(j_foot_r_idx, j_ball_r_idx, *m_ragdoll, r, rot);

#pragma endregion
	for (int i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
	{
		uint32_t        chosen_idx;
		PxRigidDynamic* body = m_ragdoll->find_recent_body(i, m_skeletal_mesh->skeleton(), chosen_idx);

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
#pragma endregion
	// ---------------------------------------------------------------------------------------------------------------
	// Create joints
	// ---------------------------------------------------------------------------------------------------------------

	//create_joint();
}

void CRagdoll_Physics::Update(_float fTimeDelta)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	for (int i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
		m_pose_transforms.transforms[i] = XMMatrixInverse(nullptr, joints[i].inverse_bind_pose);

	update_animations();
}

void CRagdoll_Physics::update_animations()
{
	if (m_bRagdoll == false)
		return;

	auto RotMat = m_pRotationMatrix;
	RotMat->_41 = m_pWorldMatrix->_41;
	RotMat->_42 = m_pWorldMatrix->_42;
	RotMat->_43 = m_pWorldMatrix->_43;

	auto joint = m_skeletal_mesh->skeleton()->joints();

	if (m_vecBone)
	{
		int i = 0;
		for (auto& it : *m_vecBone)
		{
			for (int i = 0; i < 173; ++i)
			{
				if (joint[i].name.empty() == false)
				{
					if (it->Get_Name() == joint[i].name)
					{
						m_Global_transforms.transforms[i] = XMLoadFloat4x4(it->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pWorldMatrix);
						++i;
					}
				}
			}
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

					for (auto& it : *m_vecBone)
					{
						if (it->Get_Name() == joint[i].name)
						{
							PxTransform px_transform(to_vec3(world_pos), to_quat(XMQuaternionRotationMatrix(XMLoadFloat4x4(it->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(RotMat))));
							m_ragdoll->m_rigid_bodies[i]->setGlobalPose(px_transform);
						}
					}
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
			int i = 0;

			auto WorldMat = m_pRotationMatrix;

			for (auto& it : *m_vecBone)
			{
				for (int i = 0; i < 173; ++i)
				{
					if (it->Get_Name() == joint[i].name)
					{
						if (!IsIdentityMatrix(m_Global_transforms.transforms[i]))
						{
							auto Inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(WorldMat));
							auto Result = m_Global_transforms.transforms[i] * Inverse;
							it->Set_Combined_Matrix(Result);
						}
					}
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
	uint32_t j_head_idx = HEAD_BONE;
	uint32_t j_neck_01_idx = NECK_BONE;
	uint32_t j_spine_03_idx = PELVIS_BONE;
	uint32_t j_spine_02_idx = SPINE_02_BONE;
	uint32_t j_spine_01_idx = SPINE_03_BONE;
	uint32_t j_pelvis_idx = SPINE_01_BONE;

	uint32_t j_thigh_l_idx = L_LEG_BONE;
	uint32_t j_calf_l_idx = L_CALF_BONE;
	uint32_t j_foot_l_idx = L_ANCLE_BONE;
	uint32_t j_ball_l_idx = L_FOOT_BONE;

	uint32_t j_thigh_r_idx = R_LEG_BONE;
	uint32_t j_calf_r_idx = R_CALF_BONE;
	uint32_t j_foot_r_idx = R_ANCLE_BONE;
	uint32_t j_ball_r_idx = R_FOOT_BONE;

	uint32_t j_upperarm_l_idx = L_ARM_BONE;
	uint32_t j_lowerarm_l_idx = L_FOREARM_BONE;
	uint32_t j_hand_l_idx = L_WRIST_BONE;
	uint32_t j_middle_01_l_idx = L_HAND_BONE;

	uint32_t j_upperarm_r_idx = R_ARM_BONE;
	uint32_t j_lowerarm_r_idx = R_FOREARM_BONE;
	uint32_t j_hand_r_idx = R_WRIST_BONE;
	uint32_t j_middle_01_r_idx = R_HAND_BONE;

	//Chest and Head
	create_d6_joint(m_Pelvis, m_Head, j_neck_01_idx);

	// Chest to Thighs
	create_d6_joint(m_Pelvis, m_Leg_L, j_thigh_l_idx);
	create_d6_joint(m_Pelvis, m_Leg_R, j_thigh_r_idx);

	// Thighs to Calf
	create_d6_joint(m_Leg_L, m_Calf_L, j_calf_l_idx);
	create_d6_joint(m_Leg_R, m_Calf_R, j_calf_r_idx);

	// Calf to Foot
	create_d6_joint(m_Calf_L, m_Foot_L, j_foot_l_idx);
	create_d6_joint(m_Calf_R, m_Foot_R, j_foot_r_idx);

	// Chest to Upperarm
	create_d6_joint(m_Pelvis, m_Arm_L, j_upperarm_l_idx);
	create_d6_joint(m_Pelvis, m_Arm_R, j_upperarm_r_idx);


	// Upperarm to Lowerman
	create_d6_joint(m_Arm_L, m_ForeArm_L, j_lowerarm_l_idx);
	create_d6_joint(m_Arm_R, m_ForeArm_R, j_lowerarm_r_idx);

	// Lowerarm to Hand
	create_d6_joint(m_ForeArm_L, m_Hand_L, j_hand_l_idx);
	create_d6_joint(m_ForeArm_R, m_Hand_R, j_hand_r_idx);
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
	}

	delete m_ragdoll;
	delete m_ragdoll_pose;
	delete m_skeletal_mesh->skeleton();
	delete m_skeletal_mesh;
}
