#include "Character_Controller.h"
#include "Event_Call_Back.h"
#include "GameObject.h"

CCharacter_Controller::CCharacter_Controller(PxController* Controller,class CGameObject* pCharacter, PxScene* pScene,
	PxPhysics* pPhysics)
{
	m_pController = Controller;
	m_pCharacter =pCharacter;
	++m_iRefCnt_Px;
	m_Scene = pScene;
	m_Physics = pPhysics;
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
#pragma region BodyCollider 위치 설정 코드
	m_BodyCollider->setGlobalPose(ColliderTransform_Body);
	m_HeadCollider->setGlobalPose(ColliderTransform_Head);
	m_Left_Arm_Collider->setGlobalPose(ColliderTransform_Left_Arm);
	m_Right_Arm_Collider->setGlobalPose(ColliderTransform_Right_Arm);
	m_Right_ForeArm_Collider->setGlobalPose(ColliderTransform_Right_ForeArm);
	m_Left_ForeArm_Collider->setGlobalPose(ColliderTransform_Left_ForeArm);
	m_Pelvis_Collider->setGlobalPose(ColliderTransform_Pelvis);
	m_Left_Leg_Collider->setGlobalPose(ColliderTransform_Left_Leg);
	m_Right_Leg_Collider->setGlobalPose(ColliderTransform_Right_Leg);
	m_Right_Shin_Collider->setGlobalPose(ColliderTransform_Right_Shin);
	m_Left_Shin_Collider->setGlobalPose(ColliderTransform_Left_Shin);
#pragma endregion
}

CCharacter_Controller* CCharacter_Controller::Create()
{
	return nullptr;
}

void CCharacter_Controller::Move(_float4 Dir, _float fTimeDelta)
{
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
	QueryFilterCallback filterCallback;
	PxControllerFilters controllerFilters;
	controllerFilters.mFilterCallback = &filterCallback;
	// 캐릭터 이동
	if (m_pController)
		PxControllerCollisionFlags flags = m_pController->move(Dir, 0.0f, fTimeDelta, controllerFilters);
}

_float4 CCharacter_Controller::GetTranslation()
{
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
		m_pCharacter->Release_Controller();
	}
}

_float4 CCharacter_Controller::GetPosition_Float4()
{
	PxExtendedVec3 position = m_pController->getPosition();

	_float4 Pos;
	Pos.x = static_cast<_float>(position.x);
	Pos.y = static_cast<_float>(position.y) - CONTROLLER_GROUND_GAP;
	Pos.z = static_cast<_float>(position.z);
	Pos.w = 1.f;
	return Pos;
}

_bool CCharacter_Controller::IsGrounded(PxController* Controller)
{
	return _bool();
}

void CCharacter_Controller::SetColliderTransform_Body(_float4x4 Transform)
{
	_matrix Mat = XMLoadFloat4x4(&Transform);

	_vector Quaternion = XMQuaternionRotationMatrix(Mat);
	_float4 dxQuat;
	XMStoreFloat4(&dxQuat, Quaternion);

	// PhysX의 PxQuat에 값을 대입
	PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.y, dxQuat.z, dxQuat.w);

	_float4 fourthRow;
	XMStoreFloat4(&fourthRow, Mat.r[3]);
	PxVec3 Result(fourthRow.x, fourthRow.y, fourthRow.z);

	PxTransform newTransform(Result);

	ColliderTransform_Body = newTransform;
}

void CCharacter_Controller::SetColliderTransform_Head(_float4x4 Transform)
{
	_matrix Mat = XMLoadFloat4x4(&Transform);

	_vector Quaternion = XMQuaternionRotationMatrix(Mat);
	_float4 dxQuat;
	XMStoreFloat4(&dxQuat, Quaternion);

	// PhysX의 PxQuat에 값을 대입
	PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.y, dxQuat.z, dxQuat.w);

	_float4 fourthRow;
	XMStoreFloat4(&fourthRow, Mat.r[3]);
	PxVec3 Result(fourthRow.x, fourthRow.y, fourthRow.z);

	PxTransform newTransform(Result);

	ColliderTransform_Head = newTransform;
}

void CCharacter_Controller::SetColliderTransform_Left_Arm(_float4x4 Transform)
{
	_matrix Mat = XMLoadFloat4x4(&Transform);

	_vector Quaternion = XMQuaternionRotationMatrix(Mat);
	_float4 dxQuat;
	XMStoreFloat4(&dxQuat, Quaternion);

	// PhysX의 PxQuat에 값을 대입
	//PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.y, dxQuat.z, dxQuat.w);
	PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.z, dxQuat.y, dxQuat.w);

	_float4 fourthRow;
	XMStoreFloat4(&fourthRow, Mat.r[3]);
	PxVec3 Result(fourthRow.x, fourthRow.y, fourthRow.z);

	PxTransform newTransform(Result);

	ColliderTransform_Left_Arm = newTransform;
}

void CCharacter_Controller::SetColliderTransform_Right_Arm(_float4x4 Transform)
{
	_matrix Mat = XMLoadFloat4x4(&Transform);

	_vector Quaternion = XMQuaternionRotationMatrix(Mat);
	_float4 dxQuat;
	XMStoreFloat4(&dxQuat, Quaternion);

	// PhysX의 PxQuat에 값을 대입
	//PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.y, dxQuat.z, dxQuat.w);
	PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.z, dxQuat.y, dxQuat.w);

	_float4 fourthRow;
	XMStoreFloat4(&fourthRow, Mat.r[3]);
	PxVec3 Result(fourthRow.x, fourthRow.y, fourthRow.z);

	PxTransform newTransform(Result);

	ColliderTransform_Right_Arm = newTransform;
}

void CCharacter_Controller::SetColliderTransform_Left_ForeArm(_float4x4 Transform)
{
	_matrix Mat = XMLoadFloat4x4(&Transform);

	_vector Quaternion = XMQuaternionRotationMatrix(Mat);
	_float4 dxQuat;
	XMStoreFloat4(&dxQuat, Quaternion);

	// PhysX의 PxQuat에 값을 대입
	//PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.y, dxQuat.z, dxQuat.w);
	PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.z, dxQuat.y, dxQuat.w);

	_float4 fourthRow;
	XMStoreFloat4(&fourthRow, Mat.r[3]);
	PxVec3 Result(fourthRow.x, fourthRow.y, fourthRow.z);

	PxTransform newTransform(Result);

	ColliderTransform_Left_ForeArm = newTransform;
}

void CCharacter_Controller::SetColliderTransform_Right_ForeArm(_float4x4 Transform)
{
	_matrix Mat = XMLoadFloat4x4(&Transform);

	_vector Quaternion = XMQuaternionRotationMatrix(Mat);
	_float4 dxQuat;
	XMStoreFloat4(&dxQuat, Quaternion);

	// PhysX의 PxQuat에 값을 대입
	//PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.y, dxQuat.z, dxQuat.w);
	PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.z, dxQuat.y, dxQuat.w);

	_float4 fourthRow;
	XMStoreFloat4(&fourthRow, Mat.r[3]);
	PxVec3 Result(fourthRow.x, fourthRow.y, fourthRow.z);

	PxTransform newTransform(Result);

	ColliderTransform_Right_ForeArm = newTransform;
}

void CCharacter_Controller::SetColliderTransform_Pelvis(_float4x4 Transform)
{
	_matrix Mat = XMLoadFloat4x4(&Transform);

	_vector Quaternion = XMQuaternionRotationMatrix(Mat);
	_float4 dxQuat;
	XMStoreFloat4(&dxQuat, Quaternion);

	// PhysX의 PxQuat에 값을 대입
	PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.y, dxQuat.z, dxQuat.w);

	_float4 fourthRow;
	XMStoreFloat4(&fourthRow, Mat.r[3]);
	PxVec3 Result(fourthRow.x, fourthRow.y, fourthRow.z);

	PxTransform newTransform(Result);

	ColliderTransform_Pelvis = newTransform;
}

void CCharacter_Controller::SetColliderTransform_Left_Leg(_float4x4 Transform)
{
	_matrix Mat = XMLoadFloat4x4(&Transform);

	_vector Quaternion = XMQuaternionRotationMatrix(Mat);
	_float4 dxQuat;
	XMStoreFloat4(&dxQuat, Quaternion);

	// PhysX의 PxQuat에 값을 대입
	PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.y, dxQuat.z, dxQuat.w);

	_float4 fourthRow;
	XMStoreFloat4(&fourthRow, Mat.r[3]);
	PxVec3 Result(fourthRow.x, fourthRow.y, fourthRow.z);

	PxTransform newTransform(Result);

	ColliderTransform_Left_Leg = newTransform;
}

void CCharacter_Controller::SetColliderTransform_Right_Leg(_float4x4 Transform)
{
	_matrix Mat = XMLoadFloat4x4(&Transform);

	_vector Quaternion = XMQuaternionRotationMatrix(Mat);
	_float4 dxQuat;
	XMStoreFloat4(&dxQuat, Quaternion);

	// PhysX의 PxQuat에 값을 대입
	PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.y, dxQuat.z, dxQuat.w);

	_float4 fourthRow;
	XMStoreFloat4(&fourthRow, Mat.r[3]);
	PxVec3 Result(fourthRow.x, fourthRow.y, fourthRow.z);

	PxTransform newTransform(Result);

	ColliderTransform_Right_Leg = newTransform;
}

void CCharacter_Controller::SetColliderTransform_Left_Shin(_float4x4 Transform)
{
	_matrix Mat = XMLoadFloat4x4(&Transform);

	_vector Quaternion = XMQuaternionRotationMatrix(Mat);
	_float4 dxQuat;
	XMStoreFloat4(&dxQuat, Quaternion);

	// PhysX의 PxQuat에 값을 대입
	PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.y, dxQuat.z, dxQuat.w);

	_float4 fourthRow;
	XMStoreFloat4(&fourthRow, Mat.r[3]);
	PxVec3 Result(fourthRow.x, fourthRow.y, fourthRow.z);

	PxTransform newTransform(Result);

	ColliderTransform_Left_Shin = newTransform;
}

void CCharacter_Controller::SetColliderTransform_Right_Shin(_float4x4 Transform)
{
	_matrix Mat = XMLoadFloat4x4(&Transform);

	_vector Quaternion = XMQuaternionRotationMatrix(Mat);
	_float4 dxQuat;
	XMStoreFloat4(&dxQuat, Quaternion);

	// PhysX의 PxQuat에 값을 대입
	PxQuat pxQuat = PxQuat(dxQuat.x, dxQuat.y, dxQuat.z, dxQuat.w);

	_float4 fourthRow;
	XMStoreFloat4(&fourthRow, Mat.r[3]);
	PxVec3 Result(fourthRow.x, fourthRow.y, fourthRow.z);

	PxTransform newTransform(Result);

	ColliderTransform_Right_Shin = newTransform;
}

void CCharacter_Controller::Create_Collider()
{
	PxMaterial* Material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);
	float sleepThreshold = 0.5f;
	PxReal radius = 0.1f;

	PxFilterData filterData;
	filterData.word0 = COLLISION_CATEGORY::Category2; // 이 객체의 카테고리
	filterData.word1 = COLLISION_CATEGORY::Category1 | COLLISION_CATEGORY::Category3; // 이 객체와 충돌해야 하는 카테고리

	PxShape* Shape = m_Physics->createShape(PxSphereGeometry(radius), *Material);
	Shape->setSimulationFilterData(filterData);
	Shape->setContactOffset(0.1f);
	physx::PxU32 size = 0;
	physx::PxTransform t(physx::PxVec3(0));
	physx::PxTransform localTm(physx::PxVec3(0.f, 0.f, 0.f));

#pragma region Collider 코드
	//BodyCollider 코드
	m_BodyCollider = m_Physics->createRigidDynamic(t.transform(localTm));
	m_BodyCollider->attachShape(*Shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_BodyCollider, 10.f);
	m_Scene->addActor(*m_BodyCollider);

	m_BodyCollider->setMass(1.f);
	m_BodyCollider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_BodyCollider->setSleepThreshold(sleepThreshold);
	m_BodyCollider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	//Head
	m_HeadCollider = m_Physics->createRigidDynamic(t.transform(localTm));
	m_HeadCollider->attachShape(*Shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_HeadCollider, 10.f);
	m_Scene->addActor(*m_HeadCollider);

	m_HeadCollider->setMass(1.f);
	m_HeadCollider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_HeadCollider->setSleepThreshold(sleepThreshold);
	m_HeadCollider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);


	//Left Arm
	m_Left_Arm_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	m_Left_Arm_Collider->attachShape(*Shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_Left_Arm_Collider, 10.f);
	m_Scene->addActor(*m_Left_Arm_Collider);

	m_Left_Arm_Collider->setMass(1.f);
	m_Left_Arm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_Left_Arm_Collider->setSleepThreshold(sleepThreshold);
	m_Left_Arm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);



	//Right Arm
	m_Right_Arm_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	m_Right_Arm_Collider->attachShape(*Shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_Right_Arm_Collider, 10.f);
	m_Scene->addActor(*m_Right_Arm_Collider);

	m_Right_Arm_Collider->setMass(1.f);
	m_Right_Arm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_Right_Arm_Collider->setSleepThreshold(sleepThreshold);
	m_Right_Arm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);


	//Right ForeArm
	m_Right_ForeArm_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	m_Right_ForeArm_Collider->attachShape(*Shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_Right_ForeArm_Collider, 10.f);
	m_Scene->addActor(*m_Right_ForeArm_Collider);

	m_Right_ForeArm_Collider->setMass(1.f);
	m_Right_ForeArm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_Right_ForeArm_Collider->setSleepThreshold(sleepThreshold);
	m_Right_ForeArm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	//Left ForeArm
	m_Left_ForeArm_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	m_Left_ForeArm_Collider->attachShape(*Shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_Left_ForeArm_Collider, 10.f);
	m_Scene->addActor(*m_Left_ForeArm_Collider);

	m_Left_ForeArm_Collider->setMass(1.f);
	m_Left_ForeArm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_Left_ForeArm_Collider->setSleepThreshold(sleepThreshold);
	m_Left_ForeArm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	//Pelvis
	m_Pelvis_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	m_Pelvis_Collider->attachShape(*Shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_Pelvis_Collider, 10.f);
	m_Scene->addActor(*m_Pelvis_Collider);

	m_Pelvis_Collider->setMass(1.f);
	m_Pelvis_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_Pelvis_Collider->setSleepThreshold(sleepThreshold);
	m_Pelvis_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	//Left Leg
	m_Left_Leg_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	m_Left_Leg_Collider->attachShape(*Shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_Left_Leg_Collider, 10.f);
	m_Scene->addActor(*m_Left_Leg_Collider);

	m_Left_Leg_Collider->setMass(1.f);
	m_Left_Leg_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_Left_Leg_Collider->setSleepThreshold(sleepThreshold);
	m_Left_Leg_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	//Right Leg
	m_Right_Leg_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	m_Right_Leg_Collider->attachShape(*Shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_Right_Leg_Collider, 10.f);
	m_Scene->addActor(*m_Right_Leg_Collider);

	m_Right_Leg_Collider->setMass(1.f);
	m_Right_Leg_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_Right_Leg_Collider->setSleepThreshold(sleepThreshold);
	m_Right_Leg_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);


	//Left Shin
	m_Left_Shin_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	m_Left_Shin_Collider->attachShape(*Shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_Left_Shin_Collider, 10.f);
	m_Scene->addActor(*m_Left_Shin_Collider);

	m_Left_Shin_Collider->setMass(1.f);
	m_Left_Shin_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_Left_Shin_Collider->setSleepThreshold(sleepThreshold);
	m_Left_Shin_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);


	//Right Shin
	m_Right_Shin_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	m_Right_Shin_Collider->attachShape(*Shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_Right_Shin_Collider, 10.f);
	m_Scene->addActor(*m_Right_Shin_Collider);

	m_Right_Shin_Collider->setMass(1.f);
	m_Right_Shin_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_Right_Shin_Collider->setSleepThreshold(sleepThreshold);
	m_Right_Shin_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
#pragma endregion
}

void CCharacter_Controller::Free()
{
	if (m_iRefCnt_Px != 0)
	{
		--m_iRefCnt_Px;
	}
	else
	{
		if (m_pController)
		{
			m_pController->release();
			m_pController = nullptr;
		}
	}
}
