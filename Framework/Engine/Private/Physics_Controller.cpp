#include "Physics_Controller.h"
#include "GameInstance.h"
#include "Engine_Struct.h"
#include "Character_Controller.h"
#include "Rigid_Dynamic.h"


CPhysics_Controller::CPhysics_Controller() : m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CPhysics_Controller::CPhysics_Controller(const CPhysics_Controller& rhs)
{

}

HRESULT CPhysics_Controller::Initialize_Prototype()
{
	return S_OK;
}

/* 실제 게임내엣 사용되는 객체가 호출하는 함수다. */
HRESULT CPhysics_Controller::Initialize(void* pArg)
{
	//init physx
	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocatorCallback, m_DefaultErrorCallback);
	if (!m_Foundation)
		throw("PxCreateFoundation failed");

	m_Pvd = PxCreatePvd(*m_Foundation);
	m_Transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_Pvd->connect(*m_Transport, physx::PxPvdInstrumentationFlag::eALL);
	m_ToleranceScale.length = 100;
	m_ToleranceScale.speed = 981;
	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, m_ToleranceScale, true, m_Pvd);
	physx::PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	m_Dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_Dispatcher;

	//sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	sceneDesc.filterShader = MegamotionFilterShader;

	m_EventCallBack = new CEventCallBack();
	m_FilterCallBack = new CFilterCallBack();
	sceneDesc.filterCallback = m_FilterCallBack;
	m_Scene = m_Physics->createScene(sceneDesc);
	m_Scene->setSimulationEventCallback(m_EventCallBack);

	m_PvdClient = m_Scene->getScenePvdClient();
	if (m_PvdClient)
	{
		m_PvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		m_PvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		m_PvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}


	//create simulation
	m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);
	m_GroundPlane = PxCreatePlane(*m_Physics, physx::PxPlane(0, 1, 0, 1), *m_Material);
	m_Scene->addActor(*m_GroundPlane);

	//float halfExtent = 0.5f;
	//m_Shape = m_Physics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_Material);
	//m_Shape->setSimulationFilterData(filterData);
	//m_Shape->setContactOffset(0.1f);
	//physx::PxU32 size =30;
	//physx::PxTransform t(physx::PxVec3(0));
	//physx::PxTransform localTm(physx::PxVec3(4.f, 0.f, 4.f));
	//m_BodyCollider = m_Physics->createRigidDynamic(t.transform(localTm));
	//m_BodyCollider->attachShape(*m_Shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*m_BodyCollider, 10.f);
	//m_Scene->addActor(*m_BodyCollider);

	//m_BodyCollider->setMass(1.f);
	//m_BodyCollider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//float sleepThreshold = 0.5f; // 예시 값, 프로젝트 요구 사항에 맞게 조정
	//m_BodyCollider->setSleepThreshold(sleepThreshold);
	//m_BodyCollider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	//for (physx::PxU32 i = 0; i < size; ++i) {
	//	for (physx::PxU32 j = 0; j < size - i; ++j) {
	//		// 필요에 따라 filterData를 설정합니다.
	//		/*if (i != 0) {
	//			filterData.word0 = Category2;
	//			filterData.word1 = Category1 | Category3;
	//		}*/

	//		if (i == size - 1)
	//		{
	//			if (j == size - i - 1)
	//			{
	//				filterData.word0 = COLLISION_CATEGORY::Category2; // 이 객체의 카테고리
	//				filterData.word1 = COLLISION_CATEGORY::Category1 | COLLISION_CATEGORY::Category3; // 이 객체와 충돌해야 하는 카테고리


	//				// 각 액터마다 새로운 PxShape 인스턴스를 생성합니다.
	//				physx::PxShape* newShape = m_Physics->createShape(physx::PxBoxGeometry(halfExtent*10.f, halfExtent*10.f, halfExtent), *m_Material);
	//				newShape->setSimulationFilterData(filterData);
	//				newShape->setContactOffset(0.1f);
	//				physx::PxTransform localTm(physx::PxVec3(2.f,0.f, 2.f));
	//				physx::PxRigidDynamic* body = m_Physics->createRigidDynamic(t.transform(localTm));
	//				body->attachShape(*newShape);
	//				physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
	//				m_Scene->addActor(*body);
	//				m_vecBody.push_back(body);
	//				// 사용 후 newShape를 release합니다.
	//				newShape->release();
	//				body->setMass(1.f);
	//				body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//				body->setSleepThreshold(sleepThreshold);
	//			}
	//		}
	//		else
	//		{
	//			filterData.word0 = COLLISION_CATEGORY::Category2; // 이 객체의 카테고리
	//			filterData.word1 = COLLISION_CATEGORY::Category1 | COLLISION_CATEGORY::Category3; // 이 객체와 충돌해야 하는 카테고리

	//			// 각 액터마다 새로운 PxShape 인스턴스를 생성합니다.
	//			physx::PxShape* newShape = m_Physics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_Material);
	//			newShape->setSimulationFilterData(filterData);
	//			newShape->setContactOffset(0.1f);
	//			//physx::PxTransform localTm(physx::PxVec3(3.f,0.f, 3.f));
	//			physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * (halfExtent));
	//			physx::PxRigidDynamic* body = m_Physics->createRigidDynamic(t.transform(localTm));
	//			body->attachShape(*newShape);
	//			physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
	//			m_Scene->addActor(*body);
	//			m_vecBody.push_back(body);
	//			// 사용 후 newShape를 release합니다.
	//			newShape->release();
	//			body->setMass(1.f);
	//			body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//			body->setSleepThreshold(sleepThreshold);
	//		}
	//	}
	//}

	m_Look = PxVec3(0.f, 0.f, 1.f);

	return S_OK;
}

CPhysics_Controller* CPhysics_Controller::Create()
{
	CPhysics_Controller* pInstance = new CPhysics_Controller();

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CPhysics_Controller"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_Controller::Simulate(_float fTimeDelta)
{
	if (m_pGameInstance->GetSimulate() == false)
		return;

	//if (PRESSING == m_pGameInstance->Get_KeyState('H'))
	//{
	//	// y축을 중심으로 회전할 쿼터니언 생성
	//	PxQuat rotation(-0.0075f, PxVec3(0, 1, 0));

	//	//// 현재 글로벌 포즈 가져오기
	//	//PxTransform currentPose = m_BodyCollider->getGlobalPose();

	//	//// 새로운 회전 적용
	//	//currentPose.q = rotation * currentPose.q; // 현재 회전에 추가 회전 적용

	//	//// 변경된 포즈를 객체에 적용
	//	//m_BodyCollider->setGlobalPose(currentPose);

	//	// 생성된 회전으로 벡터 회전
	//	m_Look = rotation.rotate(m_Look);
	//}

	//if (PRESSING == m_pGameInstance->Get_KeyState('K'))
	//{
	//	// y축을 중심으로 회전할 쿼터니언 생성
	//	PxQuat rotation(0.0075f, PxVec3(0, 1, 0));

	//	//// 현재 글로벌 포즈 가져오기
	//	//PxTransform currentPose = m_BodyCollider->getGlobalPose();

	//	//// 새로운 회전 적용
	//	//currentPose.q = rotation * currentPose.q; // 현재 회전에 추가 회전 적용

	//	//// 변경된 포즈를 객체에 적용
	//	//m_BodyCollider->setGlobalPose(currentPose);

	//	// 생성된 회전으로 벡터 회전
	//	m_Look = rotation.rotate(m_Look);
	//}


	//if (UP == m_pGameInstance->Get_KeyState(VK_SPACE))
	//{
	//	if (m_Controller)
	//	{
	//		if (IsGrounded(m_Controller))
	//		{
	//			m_bJump = true;

	//			m_JumpVel = PxVec3(0.f, 4.f, 0.f);
	//		}
	//	}


	//	//m_BodyCollider->addForce(m_JumpVel*2.5f, PxForceMode::eIMPULSE);
	//}

	//if (PRESSING == m_pGameInstance->Get_KeyState('U'))
	//{
	//	// 이동 속도 설정
	//	PxVec3 velocity = m_Look * 10.f * fTimeDelta;

	//	// 캐릭터 이동
	//	if (m_Controller)
	//		PxControllerCollisionFlags flags = m_Controller->move(velocity, 0.0f, fTimeDelta, PxControllerFilters());

	//	//m_BodyCollider->addForce(velocity * 5.f, PxForceMode::eIMPULSE);
	//}

	PxVec3 gravity(0.0f, -9.81f * fTimeDelta, 0.0f);

	//if (PRESSING == m_pGameInstance->Get_KeyState('J'))
	//{
	//	// 이동 속도 설정
	//	PxVec3 velocity = -m_Look * 10.f * fTimeDelta;

	//	// 캐릭터 이동
	//	if (m_Controller)
	//		PxControllerCollisionFlags flags = m_Controller->move(velocity, 0.0f, fTimeDelta, PxControllerFilters());

	//	//m_BodyCollider->addForce(velocity * 5.f,PxForceMode::eIMPULSE);
	//}

	//if (m_bJump)
	//{
	//	m_JumpVel += gravity; // 중력 적용

	//	// 이동 및 중력 적용
	//	PxVec3 displacement = m_JumpVel * fTimeDelta;
	//	PxControllerFilters filters;
	//	PxControllerCollisionFlags flags = m_Controller->move(displacement, 0.0f, fTimeDelta, filters);

	//	if (m_Controller)
	//	{
	//		if (IsGrounded(m_Controller))
	//		{
	//			m_bJump = false;
	//			m_JumpVel = PxVec3(0.f, 0.f, 0.f); // 점프 속도 초기화
	//		}
	//	}
	//}
	//else
	//{
	//	if (m_Controller)
	//		PxControllerCollisionFlags flags = m_Controller->move(gravity, 0.0f, fTimeDelta, PxControllerFilters());
	//}

	//	if (m_Controller)
	//		PxControllerCollisionFlags flags = m_Controller->move(gravity, 0.0f, fTimeDelta, PxControllerFilters());

	//auto Pos = m_Controller->getPosition();
	//// 현재 글로벌 포즈 가져오기
	//PxTransform currentPose;
	//currentPose.p.x = Pos.x;
	//currentPose.p.y = Pos.y;
	//currentPose.p.z = Pos.z;
	//// 변경된 포즈를 객체에 적용
	//m_BodyCollider->setGlobalPose(currentPose);

	//PxExtendedVec3 characterPosition = m_Controller->getPosition();
	//PxVec3 newPosition(characterPosition.x, characterPosition.y, characterPosition.z);

	//// `RigidDynamic` 객체를 새 위치로 이동시킵니다.
	//m_BodyCollider->setKinematicTarget(PxTransform(newPosition));

	for (int i = 0; i < m_vecRigid_Dynamic.size(); ++i)
	{
		m_vecRigid_Dynamic[i]->Update();
	}

	m_Scene->simulate(fTimeDelta);
	m_Scene->fetchResults(true);
}

void CPhysics_Controller::Create_Controller(_float4 Pos)
{
	//Character Controller Init
	m_Manager = PxCreateControllerManager(*m_Scene);

	m_Controll_Desc.height = 3.0f; // 캐릭터의 높이
	m_Controll_Desc.radius = 2.5f; // 캐릭터의 반지름
	m_Controll_Desc.position = PxExtendedVec3(0.0, 5.0, 0.0); // 초기 위치 설정
	m_Controll_Desc.material = m_Physics->createMaterial(0.f, 0.f, 0.f); // 마찰계수와 반발력 설정
	m_Controller = m_Manager->createController(m_Controll_Desc);


	//auto Controller = m_Manager->createController(m_Controll_Desc);


	PxShape* shapes[1];
	m_Controller->getActor()->getShapes(shapes, 1);

	// 충돌 필터 데이터 생성
	PxFilterData filterData_Character;
	filterData_Character.word0 = COLLISION_CATEGORY::Category1; // 이 객체의 카테고리
	filterData_Character.word1 = COLLISION_CATEGORY::Category2 | COLLISION_CATEGORY::Category3; // 이 객체와 충돌해야 하는 카테고리
	filterData_Character.word3 = m_iCharacter_Controller_Count;

	// 캐릭터 컨트롤러의 PxShape에 충돌 필터 데이터 설정
	shapes[0]->setSimulationFilterData(filterData_Character);
	shapes[0]->setContactOffset(0.1f);

	auto Character_Controller = new CCharacter_Controller(m_Controller);
	Character_Controller->SetIndex(m_iCharacter_Controller_Count);
	m_vecCharacter_Controller.push_back(Character_Controller);

	if (m_vecCharacter_Controller.size() >= 2)
	{
		std::sort(m_vecCharacter_Controller.begin(), m_vecCharacter_Controller.end(), [](CCharacter_Controller* a, CCharacter_Controller* b)
			{
				return a->GetIndex() < b->GetIndex();
			});
	}

	++m_iCharacter_Controller_Count;
}

void CPhysics_Controller::Create_Rigid_Dynamic(_float4 Pos)
{
	PxFilterData filterData;
	filterData.word0 = COLLISION_CATEGORY::Category2; // 이 객체의 카테고리
	filterData.word1 = COLLISION_CATEGORY::Category1 | COLLISION_CATEGORY::Category3; // 이 객체와 충돌해야 하는 카테고리

	float halfExtent = 0.5f;
	m_Shape = m_Physics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_Material);
	m_Shape->setSimulationFilterData(filterData);
	m_Shape->setContactOffset(0.1f);
	physx::PxU32 size = 0;
	physx::PxTransform t(physx::PxVec3(0));
	physx::PxTransform localTm(physx::PxVec3(4.f, 0.f, 4.f));
	//m_BodyCollider = m_Physics->createRigidDynamic(t.transform(localTm));
	//m_BodyCollider->attachShape(*m_Shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*m_BodyCollider, 10.f);
	//m_Scene->addActor(*m_BodyCollider);

	//m_BodyCollider->setMass(1.f);
	//m_BodyCollider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//float sleepThreshold = 0.5f;
	//m_BodyCollider->setSleepThreshold(sleepThreshold);
	//m_BodyCollider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);


	float sleepThreshold = 0.5f;

	for (physx::PxU32 i = 0; i < size; ++i) {
		for (physx::PxU32 j = 0; j < size - i; ++j) {
			// 필요에 따라 filterData를 설정합니다.
			/*if (i != 0) {
				filterData.word0 = Category2;
				filterData.word1 = Category1 | Category3;
			}*/

			if (i == size - 1)
			{
				if (j == size - i - 1)
				{
					filterData.word0 = COLLISION_CATEGORY::Category2; // 이 객체의 카테고리
					filterData.word1 = COLLISION_CATEGORY::Category1 | COLLISION_CATEGORY::Category3; // 이 객체와 충돌해야 하는 카테고리
					filterData.word3 = m_iRigid_Dynamic_Count;

					// 각 액터마다 새로운 PxShape 인스턴스를 생성합니다.
					physx::PxShape* newShape = m_Physics->createShape(physx::PxBoxGeometry(halfExtent * 10.f, halfExtent * 10.f, halfExtent), *m_Material);
					newShape->setSimulationFilterData(filterData);
					newShape->setContactOffset(1.1f);
					physx::PxTransform localTm(physx::PxVec3(10.f, 5.f, 10.f));
					physx::PxRigidDynamic* body = m_Physics->createRigidDynamic(t.transform(localTm));
					body->attachShape(*newShape);
					physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
					m_Scene->addActor(*body);
					m_vecBody.push_back(body);
					// 사용 후 newShape를 release합니다.
					newShape->release();
					body->setMass(1.f);
					body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
					body->setSleepThreshold(sleepThreshold);

					auto RigidDynamic = new CRigid_Dynamic(body);
					RigidDynamic->SetIndex(m_iRigid_Dynamic_Count);
					++m_iRigid_Dynamic_Count;
					m_vecRigid_Dynamic.push_back(RigidDynamic);

					if (m_vecRigid_Dynamic.size() >= 2)
					{
						std::sort(m_vecRigid_Dynamic.begin(), m_vecRigid_Dynamic.end(), [](CRigid_Dynamic* a, CRigid_Dynamic* b)
							{
								return a->GetIndex() < b->GetIndex();
							});
					}
				}
			}
			else
			{
				filterData.word0 = COLLISION_CATEGORY::Category2; // 이 객체의 카테고리
				filterData.word1 = COLLISION_CATEGORY::Category1 | COLLISION_CATEGORY::Category3; // 이 객체와 충돌해야 하는 카테고리
				filterData.word3 = m_iRigid_Dynamic_Count;

				// 각 액터마다 새로운 PxShape 인스턴스를 생성합니다.
				physx::PxShape* newShape = m_Physics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_Material);
				newShape->setSimulationFilterData(filterData);
				newShape->setContactOffset(0.1f);
				//physx::PxTransform localTm(physx::PxVec3(3.f,0.f, 3.f));
				physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i) + 10.f, physx::PxReal(i * 2 + 1) + 5.f, 10.f) * (halfExtent));
				physx::PxRigidDynamic* body = m_Physics->createRigidDynamic(t.transform(localTm));
				body->attachShape(*newShape);
				physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
				m_Scene->addActor(*body);
				m_vecBody.push_back(body);
				// 사용 후 newShape를 release합니다.
				newShape->release();
				body->setMass(1.f);
				body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
				body->setSleepThreshold(sleepThreshold);

				auto RigidDynamic = new CRigid_Dynamic(body);
				RigidDynamic->SetIndex(m_iRigid_Dynamic_Count);
				++m_iRigid_Dynamic_Count;
				m_vecRigid_Dynamic.push_back(RigidDynamic);

				if (m_vecRigid_Dynamic.size() >= 2)
				{
					std::sort(m_vecRigid_Dynamic.begin(), m_vecRigid_Dynamic.end(), [](CRigid_Dynamic* a, CRigid_Dynamic* b)
						{
							return a->GetIndex() < b->GetIndex();
						});
				}
			}
		}
	}

	PxInitExtensions(*m_Physics, m_Pvd);

	PxReal radius = 1.0f;
	PxVec3 offset(0, 0.5f, 0);

	// Head
	PxVec3 headPos = PxVec3(0, 25, 0);
	m_pHead = PxCreateDynamic(*m_Physics, PxTransform(headPos), PxSphereGeometry(radius), *m_Material, 1.0f);
	m_Scene->addActor(*m_pHead);

	// Body (longer)
	PxVec3 bodyPos = headPos - PxVec3(0, 4 * radius, 0);
	//PxRigidDynamic* body = PxCreateDynamic(*m_Physics, PxTransform(bodyPos), PxBoxGeometry(PxVec3(radius, 3 * radius, radius)), *m_Material, 1.0f);
	m_pBody = PxCreateDynamic(*m_Physics, PxTransform(bodyPos), PxBoxGeometry(PxVec3(radius, 2.f * radius, radius)), *m_Material, 1.0f);
	m_Scene->addActor(*m_pBody);
	//m_pBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	PxVec3 PelvisPos = bodyPos - PxVec3(3 * radius, 0, 0);
	m_pPelvis = PxCreateDynamic(*m_Physics, PxTransform(PelvisPos), PxBoxGeometry(PxVec3(radius, radius, radius)), *m_Material, 1.0f);
	m_Scene->addActor(*m_pPelvis);
	//m_pPelvis->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	// Connect head and body
	m_pPelvis_Joint= PxSphericalJointCreate(*m_Physics, m_pBody, PxTransform(PxVec3(0, -radius * 1.5f, 0)), m_pPelvis, PxTransform(PxVec3(0, radius * 1.f, 0)));
	m_pPelvis_Joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	// Connect head and body
	m_pNeck_Joint = PxSphericalJointCreate(*m_Physics, m_pHead, PxTransform(PxVec3(0, -radius, 0)), m_pBody, PxTransform(PxVec3(0, 2 * radius, 0)));
	m_pNeck_Joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	// Arms (shorter)
	PxVec3 leftArmPos = bodyPos - PxVec3(3 * radius, 0, 0);
	m_pLeftArm = PxCreateDynamic(*m_Physics, PxTransform(leftArmPos), PxCapsuleGeometry(radius, 1.5 * radius), *m_Material, 1.0f);
	m_Scene->addActor(*m_pLeftArm);
	//m_pLeftArm->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	PxVec3 rightArmPos = bodyPos + PxVec3(3 * radius, 0, 0);
	m_pRightArm = PxCreateDynamic(*m_Physics, PxTransform(rightArmPos), PxCapsuleGeometry(radius, 1.5 * radius), *m_Material, 1.0f);
	m_Scene->addActor(*m_pRightArm);
	//m_pRightArm->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	// Connect arms to body
	m_pLeft_Shoulder_Joint = PxSphericalJointCreate(*m_Physics, m_pBody, PxTransform(PxVec3(-radius * 1.5f, radius, 0)), m_pLeftArm, PxTransform(PxVec3(radius * 1.5f, 0, 0)));
	m_pLeft_Shoulder_Joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	m_pRight_Shoulder_Joint = PxSphericalJointCreate(*m_Physics, m_pBody, PxTransform(PxVec3(radius * 1.5f, radius, 0)), m_pRightArm, PxTransform(PxVec3(-radius * 1.5f, 0, 0)));
	m_pRight_Shoulder_Joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	// Legs
	PxVec3 leftLegPos = PelvisPos - PxVec3(5 * radius, 5.f, 0);
	m_pLeftLeg = PxCreateDynamic(*m_Physics, PxTransform(leftLegPos), PxCapsuleGeometry(radius, 2 * radius), *m_Material, 1.0f);
	m_Scene->addActor(*m_pLeftLeg);
	//m_pLeftLeg->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	PxVec3 rightLegPos = PelvisPos + PxVec3(5 * radius, -5.f, 0);
	m_pRightLeg = PxCreateDynamic(*m_Physics, PxTransform(rightLegPos), PxCapsuleGeometry(radius, 2 * radius), *m_Material, 1.0f);
	m_Scene->addActor(*m_pRightLeg);
	m_pRightLeg->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	// Connect legs to body
	m_pLeft_Hip_Joint = PxSphericalJointCreate(*m_Physics, m_pPelvis, PxTransform(PxVec3(-radius * 1.5f, -radius, 0)), m_pLeftLeg, PxTransform(PxVec3(radius * 1.5f, 0, 0)));
	m_pLeft_Hip_Joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	m_pRight_Hip_Joint = PxSphericalJointCreate(*m_Physics, m_pPelvis, PxTransform(PxVec3(radius * 1.5f, -radius, 0)), m_pRightLeg, PxTransform(PxVec3(-radius * 1.5f, 0, 0)));
	m_pRight_Hip_Joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	// Elbows
	PxVec3 leftForearmPos = leftArmPos - PxVec3(12 * radius, 12.f, 0); // Adjust for shorter arms
	m_pLeftForeArm = PxCreateDynamic(*m_Physics, PxTransform(leftForearmPos), PxCapsuleGeometry(radius, radius), *m_Material, 1.0f);
	m_Scene->addActor(*m_pLeftForeArm);
	m_pLeftForeArm->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	PxVec3 rightForearmPos = rightArmPos + PxVec3(12 * radius, 12.f, 0); // Adjust for shorter arms
	m_pRightForeArm = PxCreateDynamic(*m_Physics, PxTransform(rightForearmPos), PxCapsuleGeometry(radius, radius), *m_Material, 1.0f);
	m_Scene->addActor(*m_pRightForeArm);
	m_pRightForeArm->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	// Connect forearms to upper arms (elbows)
	m_pLeft_Elbow_Joint= PxSphericalJointCreate(*m_Physics, m_pLeftArm, PxTransform(PxVec3(-radius * 2.f, 0.f, 0)), m_pLeftForeArm, PxTransform(PxVec3(radius * 1.5f, 0, 0)));
	m_pLeft_Elbow_Joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	m_pRight_Elbow_Joint = PxSphericalJointCreate(*m_Physics, m_pRightArm, PxTransform(PxVec3(radius * 2.f, 0.f, 0)), m_pRightForeArm, PxTransform(PxVec3(-radius * 1.5f, 0, 0)));
	m_pRight_Elbow_Joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	// Knees
	PxVec3 leftShinPos = leftLegPos - PxVec3(0, 3 * radius, 0); // Adjust for body length
	m_pLeftShin = PxCreateDynamic(*m_Physics, PxTransform(leftShinPos), PxCapsuleGeometry(radius, 2 * radius), *m_Material, 1.0f);
	m_Scene->addActor(*m_pLeftShin);
	m_pLeftShin->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	PxVec3 rightShinPos = rightLegPos + PxVec3(0, 3 * radius, 0); // Adjust for body length
	m_pRightShin = PxCreateDynamic(*m_Physics, PxTransform(rightShinPos), PxCapsuleGeometry(radius, 2 * radius), *m_Material, 1.0f);
	m_Scene->addActor(*m_pRightShin);
	m_pRightShin->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	// Connect shins to thighs (knees)
	m_pLeft_Knee_Joint = PxSphericalJointCreate(*m_Physics, m_pLeftLeg, PxTransform(PxVec3(-radius * 3.f, 0.f, 0)), m_pLeftShin, PxTransform(PxVec3(radius * 2.f, 0, 0)));
	m_pLeft_Knee_Joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	m_pRight_Knee_Joint = PxSphericalJointCreate(*m_Physics, m_pRightLeg, PxTransform(PxVec3(radius * 3.f, 0.f, 0)), m_pRightShin, PxTransform(PxVec3(-radius * 2.f, 0, 0)));
	m_pRight_Knee_Joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
}

void CPhysics_Controller::Cook_Mesh(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum)
{
	// Cooking 파라미터 설정
	PxCookingParams cookingParams(m_Physics->getTolerancesScale());

	vector<PxVec3> vertices;
	vector<PxU32> IndicesVec;

	for (int i = 0; i < VertexNum; ++i)
	{
		PxVec3 Ver = PxVec3(pVertices[i].x, pVertices[i].y, pVertices[i].z);
		vertices.push_back(Ver);
	}

	for (int i = 0; i < IndexNum; ++i)
	{
		PxU32 Ind = pIndices[i];
		IndicesVec.push_back(Ind);
	}

	// Triangle Mesh 설명자 생성
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<PxU32>(VertexNum);
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertices.data();

	meshDesc.triangles.count = static_cast<PxU32>(IndexNum / 3);
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = IndicesVec.data();


	auto Mesh = PxCreateTriangleMesh(cookingParams, meshDesc);

	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f)); // 지형의 위치
	auto Actor = m_Physics->createRigidStatic(transform);

	PxTriangleMeshGeometry meshGeometry(Mesh);
	PxShape* Shape = m_Physics->createShape(meshGeometry, *m_Physics->createMaterial(0.5f, 0.5f, 0.5f));

	//PxFilterData filterData_Character;
	//filterData_Character.word0 = COLLISION_CATEGORY::Category1;
	//filterData_Character.word1 = Category1 | Category2 | Category3;

	//// 캐릭터 컨트롤러의 PxShape에 충돌 필터 데이터 설정
	//terrainShape->setSimulationFilterData(filterData_Character);

	Actor->attachShape(*Shape);
	Shape->release();


	m_Scene->addActor(*Actor);

	m_vecFullMapObject.push_back(Actor);
	++m_iMapMeshCount;

	Mesh->release();
}

void CPhysics_Controller::Move_CCT(_float4 Dir, _float fTimeDelta, _int Index)
{
	PxVec3 PxDir;
	PxDir.x = Dir.x;
	PxDir.y = Dir.y;
	PxDir.z = Dir.z;

	m_vecCharacter_Controller[Index]->Move(PxDir, fTimeDelta);
}

_float4 CPhysics_Controller::GetHeadPos()
{
	auto Pos = m_pHead->getGlobalPose();
	_float4 Result;
	Result.x = Pos.p.x;
	Result.y = Pos.p.y;
	Result.z = Pos.p.z;
	Result.w = 1.f;

	return Result;
}

_float4 CPhysics_Controller::GetBodyPos()
{
	auto Pos = m_pBody->getGlobalPose();
	_float4 Result;
	Result.x = Pos.p.x;
	Result.y = Pos.p.y;
	Result.z = Pos.p.z;
	Result.w = 1.f;

	return Result;
}

_float4 CPhysics_Controller::GetPelvisPos()
{
	auto Pos = m_pPelvis->getGlobalPose();
	_float4 Result;
	Result.x = Pos.p.x;
	Result.y = Pos.p.y;
	Result.z = Pos.p.z;
	Result.w = 1.f;

	return Result;
}

_float4 CPhysics_Controller::GetLeft_Arm_Pos()
{
	auto Pos = m_pLeftArm->getGlobalPose();
	_float4 Result;
	Result.x = Pos.p.x;
	Result.y = Pos.p.y;
	Result.z = Pos.p.z;
	Result.w = 1.f;

	return Result;
}

_float4 CPhysics_Controller::GetRight_Arm_Pos()
{
	auto Pos = m_pRightArm->getGlobalPose();
	_float4 Result;
	Result.x = Pos.p.x;
	Result.y = Pos.p.y;
	Result.z = Pos.p.z;
	Result.w = 1.f;

	return Result;
}

_float4 CPhysics_Controller::GetLeft_Fore_Arm_Pos()
{
	auto Pos = m_pLeftForeArm->getGlobalPose();
	_float4 Result;
	Result.x = Pos.p.x;
	Result.y = Pos.p.y;
	Result.z = Pos.p.z;
	Result.w = 1.f;

	return Result;
}

_float4 CPhysics_Controller::GetRight_Fore_Arm_Pos()
{
	auto Pos = m_pRightForeArm->getGlobalPose();
	_float4 Result;
	Result.x = Pos.p.x;
	Result.y = Pos.p.y;
	Result.z = Pos.p.z;
	Result.w = 1.f;

	return Result;
}

_float4 CPhysics_Controller::GetLeft_Leg_Pos()
{
	auto Pos = m_pLeftLeg->getGlobalPose();
	_float4 Result;
	Result.x = Pos.p.x;
	Result.y = Pos.p.y;
	Result.z = Pos.p.z;
	Result.w = 1.f;

	return Result;
}

_float4 CPhysics_Controller::GetRight_Leg_Pos()
{
	auto Pos = m_pRightLeg->getGlobalPose();
	_float4 Result;
	Result.x = Pos.p.x;
	Result.y = Pos.p.y;
	Result.z = Pos.p.z;
	Result.w = 1.f;

	return Result;
}

_float4 CPhysics_Controller::GetRight_Shin_Pos()
{
	auto Pos = m_pRightShin->getGlobalPose();
	_float4 Result;
	Result.x = Pos.p.x;
	Result.y = Pos.p.y;
	Result.z = Pos.p.z;
	Result.w = 1.f;

	return Result;
}

_float4 CPhysics_Controller::GetLeft_Shin_Pos()
{
	auto Pos = m_pLeftShin->getGlobalPose();
	_float4 Result;
	Result.x = Pos.p.x;
	Result.y = Pos.p.y;
	Result.z = Pos.p.z;
	Result.w = 1.f;

	return Result;
}


_matrix CPhysics_Controller::GetWorldMatrix_Rigid_Dynamic(_int Index)
{
	PxTransform globalPose = m_vecRigid_Dynamic[Index]->GetTransform_Px();

	PxVec3 position = globalPose.p;

	PxQuat rotation = globalPose.q;

	XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);

	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w));

	XMMATRIX worldMatrix = rotationMatrix * translationMatrix;

	return worldMatrix;
}


void CPhysics_Controller::Create_Rigid_Static(_float4 Pos)
{
}

void CPhysics_Controller::Create_Plane(_float4 Pos)
{
}

CCharacter_Controller* CPhysics_Controller::GetCharacter_Controller(_int Index)
{
	return m_vecCharacter_Controller[Index];
}

CRigid_Dynamic* CPhysics_Controller::GetRigid_Dynamic(_int Index)
{
	return m_vecRigid_Dynamic[Index];
}

_float4 CPhysics_Controller::GetTranslation_Rigid_Dynamic(_int Index)
{
	return m_vecRigid_Dynamic[Index]->GetTranslation();
}

_bool CPhysics_Controller::IsGrounded(PxController* Controller)
{
	PxControllerState state;
	Controller->getState(state);
	return (state.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN) != 0;
}

_float4 CPhysics_Controller::GetPosition()
{
	if (m_Controller)
	{
		PxExtendedVec3 position = m_Controller->getPosition();

		_float4 Pos;
		Pos.x = static_cast<_float>(position.x);
		Pos.y = static_cast<_float>(position.y - 4.f);
		Pos.z = static_cast<_float>(position.z);
		Pos.w = 1.f;
		return Pos;
	}

	return _float4(0.f, 0.f, 0.f, 1.f);
}

void CPhysics_Controller::InitTerrain()
{
	//지형

	// Cooking 파라미터 설정
	PxCookingParams cookingParams(m_Physics->getTolerancesScale());

	vector<PxVec3> vertices;
	vector<PxU32> IndicesVec;

	m_pVerticesPos = m_pGameInstance->GetTerrainPos();
	m_pIndices = m_pGameInstance->GetIndices();
	_int	Num = m_pGameInstance->GetNumVertices();
	_int	Indices = m_pGameInstance->GetNumIndicies();


	for (int i = 0; i < Num; ++i)
	{
		PxVec3 Ver = PxVec3(m_pVerticesPos[i].x, m_pVerticesPos[i].y, m_pVerticesPos[i].z);
		vertices.push_back(Ver);
	}

	for (int i = 0; i < Indices; ++i)
	{
		PxU32 Ind = m_pIndices[i];
		IndicesVec.push_back(Ind);
	}

	// Triangle Mesh 설명자 생성
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<PxU32>(Num);
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertices.data();

	meshDesc.triangles.count = static_cast<PxU32>(Indices / 3);
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = IndicesVec.data();


	m_TerrainMesh = PxCreateTriangleMesh(cookingParams, meshDesc);

	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f)); // 지형의 위치
	m_TerrainActor = m_Physics->createRigidStatic(transform);

	PxTriangleMeshGeometry meshGeometry(m_TerrainMesh);
	PxShape* terrainShape = m_Physics->createShape(meshGeometry, *m_Physics->createMaterial(0.5f, 0.5f, 0.5f));

	//PxFilterData filterData_Character;
	//filterData_Character.word0 = COLLISION_CATEGORY::Category1;
	//filterData_Character.word1 = Category1 | Category2 | Category3;

	//// 캐릭터 컨트롤러의 PxShape에 충돌 필터 데이터 설정
	//terrainShape->setSimulationFilterData(filterData_Character);

	m_TerrainActor->attachShape(*terrainShape);
	terrainShape->release();

	// PhysX 씬에 지형 추가
	PxScene* scene = m_Scene; // 이미 초기화된 PxScene 인스턴스
	scene->addActor(*m_TerrainActor);

	m_pGameInstance->SetSimulate(true);
}

void CPhysics_Controller::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	for (int i = 0; i < m_vecBody.size(); ++i)
	{
		m_vecBody[i]->release();
	}

	for (int i = 0; i < m_vecFullMapObject.size(); ++i)
	{
		m_vecFullMapObject[i]->release();
	}

	PxCloseExtensions();

	//Ragdoll
	if (m_pHead)
		m_pHead->release();

	if (m_pBody)
		m_pBody->release();

	if (m_BodyCollider)
		m_BodyCollider->release();

	if (m_pPelvis)
		m_pPelvis->release();

	if (m_pLeftArm)
		m_pLeftArm->release();

	if (m_pRightArm)
		m_pRightArm->release();

	if (m_pLeftForeArm)
		m_pLeftForeArm->release();

	if (m_pRightForeArm)
		m_pRightForeArm->release();

	if (m_pLeftLeg)
		m_pLeftLeg->release();

	if (m_pRightLeg)
		m_pRightLeg->release();

	if (m_pRightShin)
		m_pRightShin->release();

	if (m_pLeftShin)
		m_pLeftShin->release();

	if (m_pNeck_Joint)
		m_pNeck_Joint->release();

	if (m_pLeft_Shoulder_Joint)
		m_pLeft_Shoulder_Joint->release();

	if (m_pRight_Shoulder_Joint)
		m_pRight_Shoulder_Joint->release();

	if (m_pLeft_Elbow_Joint)
		m_pLeft_Elbow_Joint->release();

	if (m_pRight_Elbow_Joint)
		m_pRight_Elbow_Joint->release();

	if (m_pPelvis_Joint)
		m_pPelvis_Joint->release();

	if (m_pLeft_Hip_Joint)
		m_pLeft_Hip_Joint->release();

	if (m_pRight_Hip_Joint)
		m_pRight_Hip_Joint->release();

	if (m_pLeft_Knee_Joint)
		m_pLeft_Knee_Joint->release();

	if (m_pRight_Knee_Joint)
		m_pRight_Knee_Joint->release();

	if (m_Shape)
		m_Shape->release();
	if (m_BodyCollider)
		m_BodyCollider->release();

	if (m_TerrainActor)
		m_TerrainActor->release();

	if (m_GroundPlane)
		m_GroundPlane->release();

	if (m_TerrainMesh)
		m_TerrainMesh->release();

	if (m_Material)
		m_Material->release();

	if (m_Controller)
		m_Controller->release();

	if (m_Manager)
		m_Manager->release();

	if (m_Scene)
		m_Scene->release();

	if (m_Dispatcher)
		m_Dispatcher->release();

	if (m_Physics)
		m_Physics->release();

	if (m_Pvd)
		m_Pvd->release();

	if (m_Transport)
		m_Transport->release();

	if (m_Foundation)
		m_Foundation->release();

	delete m_EventCallBack;
	delete m_FilterCallBack;

	for (int i = 0; i < m_vecCharacter_Controller.size(); ++i)
	{
		Safe_Release(m_vecCharacter_Controller[i]);
	}

	for (int i = 0; i < m_vecRigid_Dynamic.size(); ++i)
	{
		Safe_Release(m_vecRigid_Dynamic[i]);
	}
}
