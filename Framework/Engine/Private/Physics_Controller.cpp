#include "Physics_Controller.h"
#include "GameInstance.h"
#include "Engine_Struct.h"
#include "Character_Controller.h"
#include "Rigid_Dynamic.h"
#include "RagDoll_Physics.h"

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
	m_ToleranceScale.length = 1.f;
	m_ToleranceScale.speed = 9.81;
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
	m_GroundPlane = PxCreatePlane(*m_Physics, physx::PxPlane(0, 1, 0, 0.f), *m_Material);
	m_Scene->addActor(*m_GroundPlane);

	m_Look = PxVec3(0.f, 0.f, 1.f);

	PxInitExtensions(*m_Physics, m_Pvd);

	m_pGameInstance->SetSimulate(true);

	m_pRagdoll_Physics = new CRagdoll_Physics(m_Scene,m_Physics, m_DefaultAllocatorCallback,m_DefaultErrorCallback,m_Foundation,
		m_Dispatcher,m_Material);

	//	m_pRagdoll_Physics->Init();

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

	PxVec3 gravity(0.0f, -9.81f * fTimeDelta, 0.0f);

	QueryFilterCallback filterCallback;
	PxControllerFilters controllerFilters;
	controllerFilters.mFilterCallback = &filterCallback;

	for (int i = 0; i < m_vecCharacter_Controller.size(); ++i)
	{
		if (m_vecCharacter_Controller[i])
			m_vecCharacter_Controller[i]->Move(gravity, fTimeDelta);
	}

	for (int i = 0; i < m_vecRigid_Dynamic.size(); ++i)
	{
		m_vecRigid_Dynamic[i]->Update();
	}

#pragma region BodyCollider 위치 설정 코드
	/*m_BodyCollider->setGlobalPose(ColliderTransform);
	m_HeadCollider->setGlobalPose(ColliderTransform_Head);
	m_Left_Arm_Collider->setGlobalPose(ColliderTransform_Left_Arm);
	m_Right_Arm_Collider->setGlobalPose(ColliderTransform_Right_Arm);
	m_Right_ForeArm_Collider->setGlobalPose(ColliderTransform_Right_ForeArm);
	m_Left_ForeArm_Collider->setGlobalPose(ColliderTransform_Left_ForeArm);
	m_Pelvis_Collider->setGlobalPose(ColliderTransform_Pelvis);
	m_Left_Leg_Collider->setGlobalPose(ColliderTransform_Left_Leg);
	m_Right_Leg_Collider->setGlobalPose(ColliderTransform_Right_Leg);
	m_Right_Shin_Collider->setGlobalPose(ColliderTransform_Right_Shin);
	m_Left_Shin_Collider->setGlobalPose(ColliderTransform_Left_Shin);*/
#pragma endregion

	static bool Temp = false;

	if (UP == m_pGameInstance->Get_KeyState(VK_SPACE))
	{
		Temp = !Temp;
		m_pRagdoll_Physics->Set_Kinematic(Temp);
	}

	if (UP == m_pGameInstance->Get_KeyState(VK_BACK))
	{
		m_pRagdoll_Physics->Init_Ragdoll();

		if (m_vecCharacter_Controller[0])
		{
			Safe_Release(m_vecCharacter_Controller[0]);
		}
	}

	//	m_pRagdoll_Physics->Update(fTimeDelta);

	m_Scene->simulate(fTimeDelta);
	m_Scene->fetchResults(true);
}

_int CPhysics_Controller::Create_Controller(_float4 Pos)
{
	//Character Controller Init
	m_Manager = PxCreateControllerManager(*m_Scene);

	m_Controll_Desc.height = 1.f; // 캐릭터의 높이
	m_Controll_Desc.radius = 0.5f; // 캐릭터의 반지름
	m_Controll_Desc.position = PxExtendedVec3(0.0, 0.0, 0.0); // 초기 위치 설정
	m_Controll_Desc.material = m_Physics->createMaterial(0.f, 0.f, 0.f); // 마찰계수와 반발력 설정
	//m_Controller = m_Manager->createController(m_Controll_Desc);


	auto Controller = m_Manager->createController(m_Controll_Desc);


	PxShape* shapes[1];
	Controller->getActor()->getShapes(shapes, 1);

	// 충돌 필터 데이터 생성
	PxFilterData filterData_Character;
	filterData_Character.word0 = COLLISION_CATEGORY::Category1; // 이 객체의 카테고리
	filterData_Character.word1 = COLLISION_CATEGORY::Category2 | COLLISION_CATEGORY::Category3; // 이 객체와 충돌해야 하는 카테고리
	filterData_Character.word3 = m_iCharacter_Controller_Count;

	// 캐릭터 컨트롤러의 PxShape에 충돌 필터 데이터 설정
	shapes[0]->setSimulationFilterData(filterData_Character);
	shapes[0]->setContactOffset(0.1f);

	auto Character_Controller = new CCharacter_Controller(Controller);
	Character_Controller->SetIndex(m_iCharacter_Controller_Count);
	m_vecCharacter_Controller.push_back(Character_Controller);

	if (m_vecCharacter_Controller.size() >= 2)
	{
		std::sort(m_vecCharacter_Controller.begin(), m_vecCharacter_Controller.end(), [](CCharacter_Controller* a, CCharacter_Controller* b)
			{
				return a->GetIndex() < b->GetIndex();
			});
	}

	auto Index = m_iCharacter_Controller_Count;
	++m_iCharacter_Controller_Count;
	return Index;
}

void CPhysics_Controller::Create_Rigid_Dynamic(_float4 Pos)
{
	float sleepThreshold = 0.5f;
	PxReal radius = 0.3f;

	PxFilterData filterData;
	filterData.word0 = COLLISION_CATEGORY::Category2; // 이 객체의 카테고리
	filterData.word1 = COLLISION_CATEGORY::Category1 | COLLISION_CATEGORY::Category3; // 이 객체와 충돌해야 하는 카테고리

	float halfExtent = 0.25f;
	//m_Shape = m_Physics->createShape(PxCapsuleGeometry(halfExtent,halfExtent*2.f), *m_Material);
	m_Shape = m_Physics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_Material);
	m_Shape->setSimulationFilterData(filterData);
	m_Shape->setContactOffset(0.1f);
	physx::PxU32 size = 10;
	physx::PxTransform t(physx::PxVec3(0));
	physx::PxTransform localTm(physx::PxVec3(4.f, 0.f, 4.f));

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
	return;

#pragma region Collider 코드
	////BodyCollider Temp 코드
	//m_BodyCollider = m_Physics->createRigidDynamic(t.transform(localTm));
	//m_BodyCollider->attachShape(*m_Shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*m_BodyCollider, 10.f);
	//m_Scene->addActor(*m_BodyCollider);

	//m_BodyCollider->setMass(1.f);
	//m_BodyCollider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//m_BodyCollider->setSleepThreshold(sleepThreshold);
	//m_BodyCollider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	////Head
	//m_HeadCollider = m_Physics->createRigidDynamic(t.transform(localTm));
	//m_HeadCollider->attachShape(*m_Shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*m_HeadCollider, 10.f);
	//m_Scene->addActor(*m_HeadCollider);

	//m_HeadCollider->setMass(1.f);
	//m_HeadCollider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//m_HeadCollider->setSleepThreshold(sleepThreshold);
	//m_HeadCollider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);


	////Left Arm
	//m_Left_Arm_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	//m_Left_Arm_Collider->attachShape(*m_Shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*m_Left_Arm_Collider, 10.f);
	//m_Scene->addActor(*m_Left_Arm_Collider);

	//m_Left_Arm_Collider->setMass(1.f);
	//m_Left_Arm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//m_Left_Arm_Collider->setSleepThreshold(sleepThreshold);
	//m_Left_Arm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);



	////Right Arm
	//m_Right_Arm_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	//m_Right_Arm_Collider->attachShape(*m_Shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*m_Right_Arm_Collider, 10.f);
	//m_Scene->addActor(*m_Right_Arm_Collider);

	//m_Right_Arm_Collider->setMass(1.f);
	//m_Right_Arm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//m_Right_Arm_Collider->setSleepThreshold(sleepThreshold);
	//m_Right_Arm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);


	////Right ForeArm
	//m_Right_ForeArm_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	//m_Right_ForeArm_Collider->attachShape(*m_Shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*m_Right_ForeArm_Collider, 10.f);
	//m_Scene->addActor(*m_Right_ForeArm_Collider);

	//m_Right_ForeArm_Collider->setMass(1.f);
	//m_Right_ForeArm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//m_Right_ForeArm_Collider->setSleepThreshold(sleepThreshold);
	//m_Right_ForeArm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);




	////Left ForeArm
	//m_Left_ForeArm_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	//m_Left_ForeArm_Collider->attachShape(*m_Shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*m_Left_ForeArm_Collider, 10.f);
	//m_Scene->addActor(*m_Left_ForeArm_Collider);

	//m_Left_ForeArm_Collider->setMass(1.f);
	//m_Left_ForeArm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//m_Left_ForeArm_Collider->setSleepThreshold(sleepThreshold);
	//m_Left_ForeArm_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);


	////Pelvis
	//m_Pelvis_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	//m_Pelvis_Collider->attachShape(*m_Shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*m_Pelvis_Collider, 10.f);
	////	m_Scene->addActor(*m_Pelvis_Collider);

	//m_Pelvis_Collider->setMass(1.f);
	//m_Pelvis_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//m_Pelvis_Collider->setSleepThreshold(sleepThreshold);
	//m_Pelvis_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);



	////Left Leg
	//m_Left_Leg_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	//m_Left_Leg_Collider->attachShape(*m_Shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*m_Left_Leg_Collider, 10.f);
	//m_Scene->addActor(*m_Left_Leg_Collider);

	//m_Left_Leg_Collider->setMass(1.f);
	//m_Left_Leg_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//m_Left_Leg_Collider->setSleepThreshold(sleepThreshold);
	//m_Left_Leg_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	////Right Leg
	//m_Right_Leg_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	//m_Right_Leg_Collider->attachShape(*m_Shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*m_Right_Leg_Collider, 10.f);
	//m_Scene->addActor(*m_Right_Leg_Collider);

	//m_Right_Leg_Collider->setMass(1.f);
	//m_Right_Leg_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//m_Right_Leg_Collider->setSleepThreshold(sleepThreshold);
	//m_Right_Leg_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);


	////Left Shin
	//m_Left_Shin_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	//m_Left_Shin_Collider->attachShape(*m_Shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*m_Left_Shin_Collider, 10.f);
	//m_Scene->addActor(*m_Left_Shin_Collider);

	//m_Left_Shin_Collider->setMass(1.f);
	//m_Left_Shin_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//m_Left_Shin_Collider->setSleepThreshold(sleepThreshold);
	//m_Left_Shin_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);


	////Right Shin
	//m_Right_Shin_Collider = m_Physics->createRigidDynamic(t.transform(localTm));
	//m_Right_Shin_Collider->attachShape(*m_Shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*m_Right_Shin_Collider, 10.f);
	//m_Scene->addActor(*m_Right_Shin_Collider);

	//m_Right_Shin_Collider->setMass(1.f);
	//m_Right_Shin_Collider->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//m_Right_Shin_Collider->setSleepThreshold(sleepThreshold);
	//m_Right_Shin_Collider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
#pragma endregion
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

	if(m_vecCharacter_Controller[Index])
		m_vecCharacter_Controller[Index]->Move(PxDir, fTimeDelta);
}

void CPhysics_Controller::SetColliderTransform(_float4x4 Transform)
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

	ColliderTransform = newTransform;
}

void CPhysics_Controller::SetColliderTransform_Head(_float4x4 Transform)
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

void CPhysics_Controller::SetColliderTransform_Left_Arm(_float4x4 Transform)
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

void CPhysics_Controller::SetColliderTransform_Right_Arm(_float4x4 Transform)
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

void CPhysics_Controller::SetColliderTransform_Left_ForeArm(_float4x4 Transform)
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

void CPhysics_Controller::SetColliderTransform_Right_ForeArm(_float4x4 Transform)
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

void CPhysics_Controller::SetColliderTransform_Pelvis(_float4x4 Transform)
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

void CPhysics_Controller::SetColliderTransform_Left_Leg(_float4x4 Transform)
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

void CPhysics_Controller::SetColliderTransform_Right_Leg(_float4x4 Transform)
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

void CPhysics_Controller::SetColliderTransform_Left_Shin(_float4x4 Transform)
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

void CPhysics_Controller::SetColliderTransform_Right_Shin(_float4x4 Transform)
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

void CPhysics_Controller::SetBone_Ragdoll(vector<class CBone*>* vecBone)
{
	m_pRagdoll_Physics->SetBone_Ragdoll(vecBone);
}

void CPhysics_Controller::SetWorldMatrix(_float4x4 WorldMatrix)
{
	m_pRagdoll_Physics->SetWorldMatrix(WorldMatrix);
}

void CPhysics_Controller::SetRotationMatrix(_float4x4 WorldMatrix)
{
	m_pRagdoll_Physics->SetRotationMatrix(WorldMatrix);
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
	if (m_vecRigid_Dynamic.empty() == false)
		return m_vecRigid_Dynamic[Index];
	else
		return nullptr;
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

_float4 CPhysics_Controller::GetPosition_CCT(_int Index)
{
	if (m_vecCharacter_Controller[Index])
	{
		return m_vecCharacter_Controller[Index]->GetTranslation();
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

	m_pRagdoll_Physics->Release();

	if (m_HeadCollider)
		m_HeadCollider->release();

	if (m_Left_Arm_Collider)
		m_Left_Arm_Collider->release();

	if (m_Right_Arm_Collider)
		m_Right_Arm_Collider->release();

	if (m_Left_ForeArm_Collider)
		m_Left_ForeArm_Collider->release();

	if (m_Right_ForeArm_Collider)
		m_Right_ForeArm_Collider->release();

	if (m_Pelvis_Collider)
		m_Pelvis_Collider->release();

	if (m_Left_Leg_Collider)
		m_Left_Leg_Collider->release();

	if (m_Right_Leg_Collider)
		m_Right_Leg_Collider->release();

	if (m_Left_Shin_Collider)
		m_Left_Shin_Collider->release();

	if (m_Right_Shin_Collider)
		m_Right_Shin_Collider->release();

	if (m_Shape)
		m_Shape->release();

	if (m_Shape_Head)
		m_Shape_Head->release();

	if (m_Shape_Body)
		m_Shape_Body->release();

	if (m_Shape_Pelvis)
		m_Shape_Pelvis->release();

	if (m_Shape_Left_Arm)
		m_Shape_Left_Arm->release();

	if (m_Shape_Right_Arm)
		m_Shape_Right_Arm->release();

	if (m_Shape_Left_ForeArm)
		m_Shape_Left_ForeArm->release();

	if (m_Shape_Right_ForeArm)
		m_Shape_Right_ForeArm->release();

	if (m_Shape_Left_Leg)
		m_Shape_Left_Leg->release();

	if (m_Shape_Right_Leg)
		m_Shape_Right_Leg->release();

	if (m_Shape_Left_Shin)
		m_Shape_Left_Shin->release();

	if (m_Shape_Right_Shin)
		m_Shape_Right_Shin->release();

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
