#include "Character_Controller.h"
#include "Event_Call_Back.h"
#include "GameObject.h"
#include "Bone.h"
#include "RagDoll.h"
#include "Skeletal_Mesh.h"
#include "Skeleton.h"

#define COLLIDER_COUNT 11
#define BONE_COUNT 150

#define MODEL_SCALE 0.01f
#define SIZE_VALUE 2.25f
#define JOINT_GAP 0.1f
#define RADIUS 3.5f

CCharacter_Controller::CCharacter_Controller(PxController* Controller, class CGameObject* pCharacter, PxScene* pScene,
	PxPhysics* pPhysics, class CTransform* pTransform, vector<class CBone*>* pBones, _int iId, const std::string& name)
{
	m_pController = Controller;
	m_pCharacter = pCharacter;
	++m_iRefCnt_Px;
	m_Scene = pScene;
	m_Physics = pPhysics;
	m_vecBone = pBones;
	m_pTransform = pTransform;
	m_material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);
	m_pWorldMatrix = pTransform->Get_WorldFloat4x4_Ptr();
	m_iId = iId;

	if (name != "None")
	{
		Build_Skeleton(name);

		SetBoneIndex();

		Create_Collider();
	}
}

CCharacter_Controller::CCharacter_Controller(const CCharacter_Controller& rhs)
{
}

HRESULT CCharacter_Controller::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CCharacter_Controller::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CCharacter_Controller::Update()
{

}

CCharacter_Controller* CCharacter_Controller::Create()
{
	return nullptr;
}

void CCharacter_Controller::Move(_float4 Dir, _float fTimeDelta)
{
	if (m_pController == nullptr)
		return;

	PxVec3 vDir = PxVec3(Dir.x, Dir.y, Dir.z);
	QueryFilterCallback filterCallback;
	PxControllerFilters controllerFilters;
	controllerFilters.mFilterCallback = &filterCallback;
	// 캐릭터 이동
	if (m_pController)
		PxControllerCollisionFlags flags = m_pController->move(vDir, 0.0f, fTimeDelta, controllerFilters);
}

void CCharacter_Controller::Move(PxVec3 Dir, _float fTimeDelta)
{
	if (m_pController == nullptr)
		return;

	QueryFilterCallback filterCallback;
	PxControllerFilters controllerFilters;
	controllerFilters.mFilterCallback = &filterCallback;
	// 캐릭터 이동
	if (m_pController)
		PxControllerCollisionFlags flags = m_pController->move(Dir, 0.0f, fTimeDelta, controllerFilters);
}

_float4 CCharacter_Controller::GetTranslation()
{
	if (m_pController == nullptr)
		return _float4(0.f,0.f,0.f,1.f);

	PxExtendedVec3 position = m_pController->getPosition();

	_float4 Pos;
	Pos.x = static_cast<_float>(position.x);
	Pos.y = static_cast<_float>(position.y) - CONTROLLER_GROUND_GAP;
	Pos.z = static_cast<_float>(position.z);
	Pos.w = 1.f;
	return Pos;
}

void CCharacter_Controller::Release_Px()
{
	if (m_pController)
	{
		m_pController->release();
		m_pController = nullptr;
		//m_pCharacter->Release_Controller();

		if (m_BodyCollider)
		{
			m_BodyCollider->release();

			if (m_HeadCollider)
				m_HeadCollider->release();

			if (m_Pelvis_Collider)
				m_Pelvis_Collider->release();

			if (m_Left_Arm_Collider)
				m_Left_Arm_Collider->release();

			if (m_Right_Arm_Collider)
				m_Right_Arm_Collider->release();

			if (m_Left_Leg_Collider)
				m_Left_Leg_Collider->release();

			if (m_Right_Leg_Collider)
				m_Right_Leg_Collider->release();

			if (m_Left_ForeArm_Collider)
				m_Left_ForeArm_Collider->release();

			if (m_Right_ForeArm_Collider)
				m_Right_ForeArm_Collider->release();

			if (m_Left_Shin_Collider)
				m_Left_Shin_Collider->release();

			if (m_Right_Shin_Collider)
				m_Right_Shin_Collider->release();

			if (m_Left_Hand_Collider)
				m_Left_Hand_Collider->release();

			if (m_Right_Hand_Collider)
				m_Right_Hand_Collider->release();

			if (m_Left_Foot_Collider)
				m_Left_Foot_Collider->release();

			if (m_Right_Foot_Collider)
				m_Right_Foot_Collider->release();

			m_Right_ForeArm_Collider = nullptr;
			m_BodyCollider = nullptr;
			m_HeadCollider = nullptr;
			m_Left_Arm_Collider = nullptr;
			m_Right_Arm_Collider = nullptr;
			m_Left_Leg_Collider = nullptr;
			m_Right_Leg_Collider = nullptr;
			m_Left_ForeArm_Collider = nullptr;
			m_Left_Shin_Collider = nullptr;
			m_Right_Shin_Collider = nullptr;
			m_Left_Hand_Collider = nullptr;
			m_Right_Hand_Collider = nullptr;
			m_Left_Foot_Collider = nullptr;
			m_Right_Foot_Collider = nullptr;
			m_Pelvis_Collider = nullptr;
		}
	}
}

_float4 CCharacter_Controller::GetPosition_Float4()
{
	if (m_pController == nullptr)
		return _float4(0.f,0.f,0.f,1.f);

	PxExtendedVec3 position = m_pController->getPosition();

	_float4 Pos;
	Pos.x = static_cast<_float>(position.x);
	Pos.y = static_cast<_float>(position.y) - CONTROLLER_GROUND_GAP;
	Pos.z = static_cast<_float>(position.z);
	Pos.w = 1.f;
	return Pos;
}

_float4 CCharacter_Controller::GetPosition_Float4_Zombie()
{
	if (m_pController == nullptr)
		return _float4(0.f, 0.f, 0.f, 1.f);

	PxExtendedVec3 position = m_pController->getPosition();

	_float4 Pos;
	Pos.x = static_cast<_float>(position.x);
	Pos.y = static_cast<_float>(position.y) - CONTROLLER_GROUND_GAP_ZOMBIE;
	Pos.z = static_cast<_float>(position.z);
	Pos.w = 1.f;
	return Pos;
}

_bool CCharacter_Controller::IsGrounded(PxController* Controller)
{
	return _bool();
}

PxRigidDynamic* CCharacter_Controller::create_capsule_bone(uint32_t parent_idx, uint32_t child_idx, CRagdoll& ragdoll, float r, XMMATRIX rotation, COLLIDER_TYPE eType)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();
	XMVECTOR parent_pos = XMLoadFloat3(&joints[parent_idx].bind_pos_ws(XMMatrixIdentity()));
	parent_pos = XMVectorSetW(parent_pos, 1.f);
	XMVECTOR child_pos = XMLoadFloat3(&joints[child_idx].bind_pos_ws(XMMatrixIdentity()));
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
	shape->setContactOffset(0.02f);

	PxFilterData filterData_Ragdoll;
	filterData_Ragdoll.word0 = COLLISION_CATEGORY::COLLIDER;
	filterData_Ragdoll.word1 = COLLISION_CATEGORY::CCT | COLLISION_CATEGORY::RAGDOLL;
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
	XMMATRIX bind_pose_ws = bind_pose;

	PxTransform px_transform(to_vec3(body_pos), to_quat(XMQuaternionRotationMatrix(bind_pose_ws)));

	PxRigidDynamic* body = m_Physics->createRigidDynamic(px_transform);

	body->attachShape(*shape);

	m_ragdoll->m_rigid_bodies[parent_idx] = body;
	//body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	float sleepThreshold = 1.f;
	body->setSleepThreshold(sleepThreshold);
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	return body;
}

PxRigidDynamic* CCharacter_Controller::create_capsule_bone(uint32_t parent_idx, CRagdoll& ragdoll, XMVECTOR offset, float l, float r, XMMATRIX rotation, COLLIDER_TYPE eType)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	XMVECTOR parent_pos = XMLoadFloat3(&joints[parent_idx].bind_pos_ws(XMMatrixIdentity())) + offset;
	parent_pos = XMVectorSetW(parent_pos, 1.f);

#pragma region 크기 줄이는 코드
	l *= MODEL_SCALE;
	r *= (MODEL_SCALE);
#pragma endregion

	r *= SIZE_VALUE;
	PxShape* shape = m_Physics->createShape(PxCapsuleGeometry(r, l), *m_material);
	shape->setContactOffset(0.02f);

	PxFilterData filterData_Ragdoll;
	filterData_Ragdoll.word0 = COLLISION_CATEGORY::COLLIDER;
	filterData_Ragdoll.word1 = COLLISION_CATEGORY::CCT | COLLISION_CATEGORY::RAGDOLL;
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
	XMMATRIX bind_pose_ws = bind_pose;

	PxTransform px_transform(to_vec3(parent_pos), to_quat(XMQuaternionRotationMatrix(bind_pose_ws)));

	PxRigidDynamic* body = m_Physics->createRigidDynamic(px_transform);

	body->attachShape(*shape);

	m_ragdoll->m_rigid_bodies[parent_idx] = body;
	//body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	float sleepThreshold = 0.5f;
	body->setSleepThreshold(sleepThreshold);
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	return body;
}

PxRigidDynamic* CCharacter_Controller::create_sphere_bone(uint32_t parent_idx, CRagdoll& ragdoll, float r, COLLIDER_TYPE eType)
{
	Joint* joints = m_skeletal_mesh->skeleton()->joints();
	XMVECTOR parent_pos = XMLoadFloat3(&joints[parent_idx].bind_pos_ws(XMMatrixIdentity()));
	parent_pos = XMVectorSetW(parent_pos, 1.f);

#pragma region 크기 줄이는 코드
	r *= (MODEL_SCALE);
#pragma endregion

	PxShape* shape = m_Physics->createShape(PxSphereGeometry(r), *m_material);
	shape->setContactOffset(0.02f);

	PxFilterData filterData_Ragdoll;
	filterData_Ragdoll.word0 = COLLISION_CATEGORY::COLLIDER;
	filterData_Ragdoll.word1 = COLLISION_CATEGORY::CCT | COLLISION_CATEGORY::RAGDOLL;
	filterData_Ragdoll.word2 = m_iId;
	filterData_Ragdoll.word3 = eType;
	shape->setSimulationFilterData(filterData_Ragdoll);

	PxRigidDynamic* body = m_Physics->createRigidDynamic(PxTransform(to_vec3(parent_pos)));

	body->attachShape(*shape);

	m_ragdoll->m_rigid_bodies[parent_idx] = body;
	body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	float sleepThreshold = 0.5f;
	body->setSleepThreshold(sleepThreshold);
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	return body;
}

void CCharacter_Controller::Create_Collider()
{
	/*m_BodyCollider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_BodyCollider->setSleepThreshold(sleepThreshold);
	m_BodyCollider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_BodyCollider, 10.f);*/

	if (!m_vecBone)
		return;

	Joint* joints = m_skeletal_mesh->skeleton()->joints();

	m_vecBreakPartFilter.resize(m_skeletal_mesh->skeleton()->num_bones());

	m_ragdoll->m_rigid_bodies.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_relative_joint_pos.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_body_rotations.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_body_pos_relative_to_joint.resize(m_skeletal_mesh->skeleton()->num_bones());
	m_ragdoll->m_original_joint_rotations.resize(m_skeletal_mesh->skeleton()->num_bones());

	for (size_t i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
	{
		m_ragdoll->m_rigid_bodies[i] = nullptr;
		m_vecBreakPartFilter[i] = false;
	}

	// ---------------------------------------------------------------------------------------------------------------
	// Create rigid bodies for limbs
	// ---------------------------------------------------------------------------------------------------------------

	float     r = RADIUS;
	_matrix rot = XMMatrixIdentity();
	rot = XMMatrixRotationZ(XM_PI * 0.5f);
	_matrix I = XMMatrixIdentity();

	m_HeadCollider = create_capsule_bone(m_head_idx, *m_ragdoll, XMVectorSet(0.0f, 3.0f, 0.0f, 1.f), 4.0f, 6.0f, rot, COLLIDER_TYPE::HEAD);
	m_BodyCollider = create_capsule_bone(m_spine_01_idx, m_neck_01_idx, *m_ragdoll, 7.0f, rot, COLLIDER_TYPE::CHEST);
	m_Pelvis_Collider = create_capsule_bone(m_pelvis_idx, m_spine_01_idx, *m_ragdoll, 7.0f, rot, COLLIDER_TYPE::PELVIS);

	m_Left_Leg_Collider = create_capsule_bone(m_thigh_l_idx, m_calf_l_idx, *m_ragdoll, r, rot, COLLIDER_TYPE::LEG_L);
	m_Right_Leg_Collider = create_capsule_bone(m_thigh_r_idx, m_calf_r_idx, *m_ragdoll, r, rot, COLLIDER_TYPE::LEG_R);

	m_Left_Shin_Collider = create_capsule_bone(m_calf_l_idx, m_foot_l_idx, *m_ragdoll, r, rot, COLLIDER_TYPE::CALF_L);
	m_Right_Shin_Collider = create_capsule_bone(m_calf_r_idx, m_foot_r_idx, *m_ragdoll, r, rot, COLLIDER_TYPE::CALF_R);

	m_Left_Arm_Collider = create_capsule_bone(m_upperarm_l_idx, m_lowerarm_l_idx, *m_ragdoll, r, I, COLLIDER_TYPE::ARM_L);
	m_Right_Arm_Collider = create_capsule_bone(m_upperarm_r_idx, m_lowerarm_r_idx, *m_ragdoll, r, I, COLLIDER_TYPE::ARM_R);

	m_Left_ForeArm_Collider = create_capsule_bone(m_lowerarm_l_idx, m_hand_l_idx, *m_ragdoll, r, I, COLLIDER_TYPE::FOREARM_L);
	m_Right_ForeArm_Collider = create_capsule_bone(m_lowerarm_r_idx, m_hand_r_idx, *m_ragdoll, r, I, COLLIDER_TYPE::FOREARM_R);

	m_Left_Hand_Collider = create_capsule_bone(m_hand_l_idx, m_middle_01_l_idx, *m_ragdoll, r, I, COLLIDER_TYPE::HAND_L);
	m_Right_Hand_Collider = create_capsule_bone(m_hand_r_idx, m_middle_01_r_idx, *m_ragdoll, r, I, COLLIDER_TYPE::HAND_R);

	rot = XMMatrixRotationY(PxPi * 0.5f);

	m_Left_Foot_Collider = create_capsule_bone(m_foot_l_idx, m_ball_l_idx, *m_ragdoll, r, rot, COLLIDER_TYPE::FOOT_L);
	m_Right_Foot_Collider = create_capsule_bone(m_foot_r_idx, m_ball_r_idx, *m_ragdoll, r, rot, COLLIDER_TYPE::FOOT_R);

	m_ragdoll->m_rigid_bodies[1] = m_Pelvis_Collider;
	m_ragdoll->m_rigid_bodies[3] = m_Left_Leg_Collider;

#pragma endregion
	for (int i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
	{
		uint32_t        chosen_idx;
		PxRigidDynamic* body = m_ragdoll->find_recent_body(i, m_skeletal_mesh->skeleton(), chosen_idx);

		if (!body)
			continue;

		_matrix body_global_transform = to_mat4(body->getGlobalPose());
		_matrix inv_body_global_transform = XMMatrixInverse(nullptr, body_global_transform);
		_matrix bind_pose_ws = XMMatrixInverse(nullptr, joints[i].inverse_bind_pose);
		_vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(XMMatrixIdentity()));
		joint_pos_ws = XMVectorSetW(joint_pos_ws, 1.f);

		_vector p = XMVector4Transform(joint_pos_ws, inv_body_global_transform);
		m_ragdoll->m_relative_joint_pos[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
		m_ragdoll->m_original_body_rotations[i] = XMQuaternionRotationMatrix(body_global_transform);

		if (m_ragdoll->m_rigid_bodies[i])
		{
			// Rigid body position relative to the joint
			_matrix m = XMMatrixInverse(nullptr, XMMatrixInverse(nullptr, joints[i].inverse_bind_pose));
			p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies[i]->getGlobalPose().p), 1.0f), m);

			m_ragdoll->m_body_pos_relative_to_joint[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p), 1.f);
			m_ragdoll->m_original_joint_rotations[i] = XMQuaternionRotationMatrix(bind_pose_ws);
		}
	}

	// ---------------------------------------------------------------------------------------------------------------
	// Add rigid bodies to scene
	// ---------------------------------------------------------------------------------------------------------------

	// Chest and Head
	m_Scene->addActor(*m_Pelvis_Collider);
	m_Scene->addActor(*m_HeadCollider);
	m_Scene->addActor(*m_BodyCollider);

	// Left Arm
	m_Scene->addActor(*m_Left_Arm_Collider);
	m_Scene->addActor(*m_Left_ForeArm_Collider);
	m_Scene->addActor(*m_Left_Hand_Collider);

	// Right Arm
	m_Scene->addActor(*m_Right_Arm_Collider);
	m_Scene->addActor(*m_Right_ForeArm_Collider);
	m_Scene->addActor(*m_Right_Hand_Collider);

	// Left Leg
	m_Scene->addActor(*m_Left_Leg_Collider);
	m_Scene->addActor(*m_Left_Shin_Collider);
	m_Scene->addActor(*m_Left_Foot_Collider);

	// Right Leg
	m_Scene->addActor(*m_Right_Leg_Collider);
	m_Scene->addActor(*m_Right_Shin_Collider);
	m_Scene->addActor(*m_Right_Foot_Collider);

#pragma endregion
}

void CCharacter_Controller::Build_Skeleton(const std::string& name)
{
	m_ragdoll = new CRagdoll();

	load_mesh(name);

	for (int i = 0; i < MAX_BONES; i++)
		m_pose_transforms.transforms[i] = XMMatrixIdentity();
}

void CCharacter_Controller::Update_Collider()
{
	if (m_ragdoll == nullptr)
		return;

	//m_pRotationMatrix = &m_pTransform->Get_RotationMatrix_Pure();
	m_pRotationMatrix = &m_pTransform->Get_WorldMatrix_Pure();
	_float4x4* RotMat = m_pRotationMatrix;
	RotMat->_41 = m_pWorldMatrix->_41;
	RotMat->_42 = m_pWorldMatrix->_42;
	RotMat->_43 = m_pWorldMatrix->_43;

	Joint* joint = m_skeletal_mesh->skeleton()->joints();
	auto JointNum = m_skeletal_mesh->skeleton()->num_bones();

	if (m_vecBone)
	{
		for (size_t i = 0; i < JointNum; ++i)
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

				XMMATRIX transform = global_transform;

				XMVECTOR pos_with_w = XMVectorSetW(body_pos_relative_to_joint, 1.0f);

				XMVECTOR temp = XMVector4Transform(pos_with_w, transform);

				_vector world_pos = XMVectorSet(XMVectorGetX(temp), XMVectorGetY(temp), XMVectorGetZ(temp), 1.f);

				XMMATRIX final_transform = global_transform;

				XMVECTOR joint_rot = XMQuaternionRotationMatrix(final_transform);

				_vector diff_rot = XMQuaternionInverse(m_ragdoll->m_original_joint_rotations[i]) * joint_rot;

				_vector rotation = m_ragdoll->m_original_body_rotations[i] * diff_rot;

				rotation = XMQuaternionNormalize(rotation);

				PxTransform px_transform(to_vec3(world_pos), to_quat(XMQuaternionRotationMatrix(XMLoadFloat4x4((*m_vecBone)[m_vecBoneIndex[i]]->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(RotMat))));
				m_ragdoll->m_rigid_bodies[i]->setGlobalPose(px_transform);
			}
		}
	}
}

void CCharacter_Controller::load_mesh(const string& name)
{
	m_skeletal_mesh = SkeletalMesh::load(name);
}

void CCharacter_Controller::SetBoneIndex()
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
}

void CCharacter_Controller::SetPosition(_float4 vPos)
{
	PxExtendedVec3 newPosition(vPos.x, vPos.y, vPos.z);
	m_pController->setPosition(newPosition);
}

void CCharacter_Controller::Release_PartialCollider(COLLIDER_TYPE eType)
{
	switch (eType)
	{
	case COLLIDER_TYPE::PELVIS:
		//break;///////////////////////////TEMP/////////////////////////////
		if(m_Pelvis_Collider)
		{
			m_Pelvis_Collider->release();
			m_Pelvis_Collider = nullptr;
			m_vecBreakPartFilter[m_pelvis_idx] = true;
		}

		if(m_Left_Leg_Collider)
		{
			m_Left_Leg_Collider->release();
			m_Left_Leg_Collider = nullptr;
			m_vecBreakPartFilter[m_thigh_l_idx] = true;
		}

		if(m_Right_Leg_Collider)
		{
			m_Right_Leg_Collider->release();
			m_Right_Leg_Collider = nullptr;
			m_vecBreakPartFilter[m_thigh_r_idx] = true;
		}

		if(m_Left_Shin_Collider)
		{
			m_Left_Shin_Collider->release();
			m_Left_Shin_Collider = nullptr;
			m_vecBreakPartFilter[m_calf_l_idx] = true;
		}

		if(m_Right_Shin_Collider)
		{
			m_Right_Shin_Collider->release();
			m_Right_Shin_Collider = nullptr;
			m_vecBreakPartFilter[m_calf_r_idx] = true;
		}

		if(m_Left_Foot_Collider)
		{
			m_Left_Foot_Collider->release();
			m_Left_Foot_Collider = nullptr;
			m_vecBreakPartFilter[m_foot_l_idx] = true;
		}

		if(m_Right_Foot_Collider)
		{
			m_Right_Foot_Collider->release();
			m_Right_Foot_Collider = nullptr;
			m_vecBreakPartFilter[m_foot_r_idx] = true;
		}

		break;
	case COLLIDER_TYPE::ARM_L:
		if(m_Left_Arm_Collider)
		{
			m_Left_Arm_Collider->release();
			m_Left_Arm_Collider = nullptr;
			m_vecBreakPartFilter[m_upperarm_l_idx] = true;
		}

		if(m_Left_ForeArm_Collider)
		{
			m_Left_ForeArm_Collider->release();
			m_Left_ForeArm_Collider = nullptr;
			m_vecBreakPartFilter[m_lowerarm_l_idx] = true;
		}

		if(m_Left_Hand_Collider)
		{
			m_Left_Hand_Collider->release();
			m_Left_Hand_Collider = nullptr;
			m_vecBreakPartFilter[m_hand_l_idx] = true;
		}

		break;
	case COLLIDER_TYPE::ARM_R:
		if(m_Right_Arm_Collider)
		{
			m_Right_Arm_Collider->release();
			m_Right_Arm_Collider = nullptr;
			m_vecBreakPartFilter[m_upperarm_r_idx] = true;
		}

		if(m_Right_ForeArm_Collider)
		{
			m_Right_ForeArm_Collider->release();
			m_Right_ForeArm_Collider = nullptr;
			m_vecBreakPartFilter[m_lowerarm_r_idx] = true;
		}

		if(m_Right_Hand_Collider)
		{
			m_Right_Hand_Collider->release();
			m_Right_Hand_Collider = nullptr;
			m_vecBreakPartFilter[m_hand_r_idx] = true;
		}

		break;
	case COLLIDER_TYPE::FOREARM_L:

		if(m_Left_ForeArm_Collider)
		{
			m_Left_ForeArm_Collider->release();
			m_Left_ForeArm_Collider = nullptr;

			m_Left_Hand_Collider->release();
			m_Left_Hand_Collider = nullptr;

			m_vecBreakPartFilter[m_lowerarm_l_idx] = true;
			m_vecBreakPartFilter[m_hand_l_idx] = true;
		}

		break;
	case COLLIDER_TYPE::FOREARM_R:

		if(m_Right_ForeArm_Collider)
		{
			m_Right_ForeArm_Collider->release();
			m_Right_ForeArm_Collider = nullptr;

			m_Right_Hand_Collider->release();
			m_Right_Hand_Collider = nullptr;

			m_vecBreakPartFilter[m_lowerarm_r_idx] = true;
			m_vecBreakPartFilter[m_hand_r_idx] = true;
		}

		break;
	case COLLIDER_TYPE::CALF_L:
		if(m_Left_Shin_Collider)
		{
			m_Left_Shin_Collider->release();
			m_Left_Shin_Collider = nullptr;

			m_Left_Foot_Collider->release();
			m_Left_Foot_Collider = nullptr;

			m_vecBreakPartFilter[m_calf_l_idx] = true;
			m_vecBreakPartFilter[m_foot_l_idx] = true;
		}
		break;
	case COLLIDER_TYPE::CALF_R:
		if(m_Right_Shin_Collider)
		{
			m_Right_Shin_Collider->release();
			m_Right_Shin_Collider = nullptr;

			m_Right_Foot_Collider->release();
			m_Right_Foot_Collider = nullptr;

			m_vecBreakPartFilter[m_calf_r_idx] = true;
			m_vecBreakPartFilter[m_foot_r_idx] = true;
		}
		break;
	case COLLIDER_TYPE::LEG_L:
		if(m_Left_Leg_Collider)
		{
			m_Left_Leg_Collider->release();
			m_Left_Leg_Collider = nullptr;
			m_vecBreakPartFilter[m_thigh_l_idx] = true;
		}

		if(m_Left_Shin_Collider)
		{
			m_Left_Shin_Collider->release();
			m_Left_Shin_Collider = nullptr;
			m_vecBreakPartFilter[m_calf_l_idx] = true;
		}

		if(m_Left_Foot_Collider)
		{
			m_Left_Foot_Collider->release();
			m_Left_Foot_Collider = nullptr;
			m_vecBreakPartFilter[m_foot_l_idx] = true;
		}

		break;
	case COLLIDER_TYPE::LEG_R:
		if(m_Right_Leg_Collider)
		{
			m_Right_Leg_Collider->release();
			m_Right_Leg_Collider = nullptr;
			m_vecBreakPartFilter[m_calf_r_idx] = true;
		}

		if(m_Right_Shin_Collider)
		{
			m_Right_Shin_Collider->release();
			m_Right_Shin_Collider = nullptr;
			m_vecBreakPartFilter[m_thigh_r_idx] = true;
		}

		if(m_Right_Foot_Collider)
		{
			m_Right_Foot_Collider->release();
			m_Right_Foot_Collider = nullptr;
			m_vecBreakPartFilter[m_foot_r_idx] = true;
		}

		break;
	}
}

void CCharacter_Controller::Free()
{
	if (m_iRefCnt_Px != 0)
	{
		--m_iRefCnt_Px;

		if (m_BodyCollider)
		{
			m_BodyCollider->release();

			if (m_HeadCollider)
				m_HeadCollider->release();

			if (m_Pelvis_Collider)
				m_Pelvis_Collider->release();

			if (m_Left_Arm_Collider)
				m_Left_Arm_Collider->release();

			if (m_Right_Arm_Collider)
				m_Right_Arm_Collider->release();

			if (m_Left_Leg_Collider)
				m_Left_Leg_Collider->release();

			if (m_Right_Leg_Collider)
				m_Right_Leg_Collider->release();

			if (m_Left_ForeArm_Collider)
				m_Left_ForeArm_Collider->release();

			if (m_Right_ForeArm_Collider)
				m_Right_ForeArm_Collider->release();

			if (m_Left_Shin_Collider)
				m_Left_Shin_Collider->release();

			if (m_Right_Shin_Collider)
				m_Right_Shin_Collider->release();

			if (m_Left_Hand_Collider)
				m_Left_Hand_Collider->release();

			if (m_Right_Hand_Collider)
				m_Right_Hand_Collider->release();

			if (m_Left_Foot_Collider)
				m_Left_Foot_Collider->release();

			if (m_Right_Foot_Collider)
				m_Right_Foot_Collider->release();

			m_Right_ForeArm_Collider = nullptr;
			m_BodyCollider = nullptr;
			m_HeadCollider = nullptr;
			m_Left_Arm_Collider = nullptr;
			m_Right_Arm_Collider = nullptr;
			m_Left_Leg_Collider = nullptr;
			m_Right_Leg_Collider = nullptr;
			m_Left_ForeArm_Collider = nullptr;
			m_Left_Shin_Collider = nullptr;
			m_Right_Shin_Collider = nullptr;
			m_Left_Hand_Collider = nullptr;
			m_Right_Hand_Collider = nullptr;
			m_Left_Foot_Collider = nullptr;
			m_Right_Foot_Collider = nullptr;
			m_Pelvis_Collider = nullptr;
		}

		if (m_ragdoll)
		{
			delete m_ragdoll;
			delete m_skeletal_mesh->skeleton();
			delete m_skeletal_mesh;

			m_ragdoll = nullptr;
			m_skeletal_mesh = nullptr;
		}
	}
	else
	{
		if (m_pController)
		{
			m_pController->release();
			m_pController = nullptr;
		}

		if (m_BodyCollider)
		{
			m_BodyCollider->release();

			if(m_HeadCollider)
				m_HeadCollider->release();

			if (m_Pelvis_Collider)
				m_Pelvis_Collider->release();

			if(m_Left_Arm_Collider)
				m_Left_Arm_Collider->release();

			if(m_Right_Arm_Collider)
				m_Right_Arm_Collider->release();

			if(m_Left_Leg_Collider)
				m_Left_Leg_Collider->release();

			if(m_Right_Leg_Collider)
				m_Right_Leg_Collider->release();

			if(m_Left_ForeArm_Collider)
				m_Left_ForeArm_Collider->release();

			if(m_Right_ForeArm_Collider)
				m_Right_ForeArm_Collider->release();

			if(m_Left_Shin_Collider)
				m_Left_Shin_Collider->release();

			if(m_Right_Shin_Collider)
				m_Right_Shin_Collider->release();

			if(m_Left_Hand_Collider)
				m_Left_Hand_Collider->release();

			if(m_Right_Hand_Collider)
				m_Right_Hand_Collider->release();

			if(m_Left_Foot_Collider)
				m_Left_Foot_Collider->release();

			if(m_Right_Foot_Collider)
				m_Right_Foot_Collider->release();
		}

		if (m_ragdoll)
		{
			delete m_ragdoll;
			delete m_skeletal_mesh->skeleton();
			delete m_skeletal_mesh;

			m_ragdoll = nullptr;
			m_skeletal_mesh = nullptr;
		}
	}
}
