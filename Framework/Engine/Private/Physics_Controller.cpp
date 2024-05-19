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

/* ���� ���ӳ��� ���Ǵ� ��ü�� ȣ���ϴ� �Լ���. */
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
	//float sleepThreshold = 0.5f; // ���� ��, ������Ʈ �䱸 ���׿� �°� ����
	//m_BodyCollider->setSleepThreshold(sleepThreshold);
	//m_BodyCollider->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	//for (physx::PxU32 i = 0; i < size; ++i) {
	//	for (physx::PxU32 j = 0; j < size - i; ++j) {
	//		// �ʿ信 ���� filterData�� �����մϴ�.
	//		/*if (i != 0) {
	//			filterData.word0 = Category2;
	//			filterData.word1 = Category1 | Category3;
	//		}*/

	//		if (i == size - 1)
	//		{
	//			if (j == size - i - 1)
	//			{
	//				filterData.word0 = COLLISION_CATEGORY::Category2; // �� ��ü�� ī�װ�
	//				filterData.word1 = COLLISION_CATEGORY::Category1 | COLLISION_CATEGORY::Category3; // �� ��ü�� �浹�ؾ� �ϴ� ī�װ�


	//				// �� ���͸��� ���ο� PxShape �ν��Ͻ��� �����մϴ�.
	//				physx::PxShape* newShape = m_Physics->createShape(physx::PxBoxGeometry(halfExtent*10.f, halfExtent*10.f, halfExtent), *m_Material);
	//				newShape->setSimulationFilterData(filterData);
	//				newShape->setContactOffset(0.1f);
	//				physx::PxTransform localTm(physx::PxVec3(2.f,0.f, 2.f));
	//				physx::PxRigidDynamic* body = m_Physics->createRigidDynamic(t.transform(localTm));
	//				body->attachShape(*newShape);
	//				physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
	//				m_Scene->addActor(*body);
	//				m_vecBody.push_back(body);
	//				// ��� �� newShape�� release�մϴ�.
	//				newShape->release();
	//				body->setMass(1.f);
	//				body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//				body->setSleepThreshold(sleepThreshold);
	//			}
	//		}
	//		else
	//		{
	//			filterData.word0 = COLLISION_CATEGORY::Category2; // �� ��ü�� ī�װ�
	//			filterData.word1 = COLLISION_CATEGORY::Category1 | COLLISION_CATEGORY::Category3; // �� ��ü�� �浹�ؾ� �ϴ� ī�װ�

	//			// �� ���͸��� ���ο� PxShape �ν��Ͻ��� �����մϴ�.
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
	//			// ��� �� newShape�� release�մϴ�.
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

	if (PRESSING == m_pGameInstance->Get_KeyState('H'))
	{
		// y���� �߽����� ȸ���� ���ʹϾ� ����
		PxQuat rotation(-0.0075f, PxVec3(0, 1, 0));

		//// ���� �۷ι� ���� ��������
		//PxTransform currentPose = m_BodyCollider->getGlobalPose();

		//// ���ο� ȸ�� ����
		//currentPose.q = rotation * currentPose.q; // ���� ȸ���� �߰� ȸ�� ����

		//// ����� ��� ��ü�� ����
		//m_BodyCollider->setGlobalPose(currentPose);

		// ������ ȸ������ ���� ȸ��
		m_Look = rotation.rotate(m_Look);
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('K'))
	{
		// y���� �߽����� ȸ���� ���ʹϾ� ����
		PxQuat rotation(0.0075f, PxVec3(0, 1, 0));

		//// ���� �۷ι� ���� ��������
		//PxTransform currentPose = m_BodyCollider->getGlobalPose();

		//// ���ο� ȸ�� ����
		//currentPose.q = rotation * currentPose.q; // ���� ȸ���� �߰� ȸ�� ����

		//// ����� ��� ��ü�� ����
		//m_BodyCollider->setGlobalPose(currentPose);

		// ������ ȸ������ ���� ȸ��
		m_Look = rotation.rotate(m_Look);
	}


	if (UP == m_pGameInstance->Get_KeyState(VK_SPACE))
	{
		if (m_Controller)
		{
			if (IsGrounded(m_Controller))
			{
				m_bJump = true;

				m_JumpVel = PxVec3(0.f, 4.f, 0.f);
			}
		}


		//m_BodyCollider->addForce(m_JumpVel*2.5f, PxForceMode::eIMPULSE);
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('U'))
	{
		// �̵� �ӵ� ����
		PxVec3 velocity = m_Look * 10.f * fTimeDelta;

		// ĳ���� �̵�
		if (m_Controller)
			PxControllerCollisionFlags flags = m_Controller->move(velocity, 0.0f, fTimeDelta, PxControllerFilters());

		//m_BodyCollider->addForce(velocity * 5.f, PxForceMode::eIMPULSE);
	}

	PxVec3 gravity(0.0f, -9.81f * fTimeDelta, 0.0f);

	if (PRESSING == m_pGameInstance->Get_KeyState('J'))
	{
		// �̵� �ӵ� ����
		PxVec3 velocity = -m_Look * 10.f * fTimeDelta;

		// ĳ���� �̵�
		if (m_Controller)
			PxControllerCollisionFlags flags = m_Controller->move(velocity, 0.0f, fTimeDelta, PxControllerFilters());

		//m_BodyCollider->addForce(velocity * 5.f,PxForceMode::eIMPULSE);
	}

	if (m_bJump)
	{
		m_JumpVel += gravity; // �߷� ����

		// �̵� �� �߷� ����
		PxVec3 displacement = m_JumpVel * fTimeDelta;
		PxControllerFilters filters;
		PxControllerCollisionFlags flags = m_Controller->move(displacement, 0.0f, fTimeDelta, filters);

		if (m_Controller)
		{
			if (IsGrounded(m_Controller))
			{
				m_bJump = false;
				m_JumpVel = PxVec3(0.f, 0.f, 0.f); // ���� �ӵ� �ʱ�ȭ
			}
		}
	}
	else
	{
		if (m_Controller)
			PxControllerCollisionFlags flags = m_Controller->move(gravity, 0.0f, fTimeDelta, PxControllerFilters());
	}

	//auto Pos = m_Controller->getPosition();
	//// ���� �۷ι� ���� ��������
	//PxTransform currentPose;
	//currentPose.p.x = Pos.x;
	//currentPose.p.y = Pos.y;
	//currentPose.p.z = Pos.z;
	//// ����� ��� ��ü�� ����
	//m_BodyCollider->setGlobalPose(currentPose);

	//PxExtendedVec3 characterPosition = m_Controller->getPosition();
	//PxVec3 newPosition(characterPosition.x, characterPosition.y, characterPosition.z);

	//// `RigidDynamic` ��ü�� �� ��ġ�� �̵���ŵ�ϴ�.
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

	m_Controll_Desc.height = 1.0f; // ĳ������ ����
	m_Controll_Desc.radius = 0.5f; // ĳ������ ������
	m_Controll_Desc.position = PxExtendedVec3(1.0,5.0, 1.0); // �ʱ� ��ġ ����
	m_Controll_Desc.material = m_Physics->createMaterial(0.f, 0.f, 0.f); // ��������� �ݹ߷� ����
	m_Controller = m_Manager->createController(m_Controll_Desc);


	//auto Controller = m_Manager->createController(m_Controll_Desc);


	PxShape* shapes[1];
	m_Controller->getActor()->getShapes(shapes, 1);

	// �浹 ���� ������ ����
	PxFilterData filterData_Character;
	filterData_Character.word0 = COLLISION_CATEGORY::Category1; // �� ��ü�� ī�װ�
	filterData_Character.word1 = COLLISION_CATEGORY::Category2 | COLLISION_CATEGORY::Category3; // �� ��ü�� �浹�ؾ� �ϴ� ī�װ�
	filterData_Character.word3 = m_iCharacter_Controller_Count;

	// ĳ���� ��Ʈ�ѷ��� PxShape�� �浹 ���� ������ ����
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
	filterData.word0 = COLLISION_CATEGORY::Category2; // �� ��ü�� ī�װ�
	filterData.word1 = COLLISION_CATEGORY::Category1 | COLLISION_CATEGORY::Category3; // �� ��ü�� �浹�ؾ� �ϴ� ī�װ�

	float halfExtent = 0.5f;
	m_Shape = m_Physics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_Material);
	m_Shape->setSimulationFilterData(filterData);
	m_Shape->setContactOffset(0.1f);
	physx::PxU32 size = 10;
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
			// �ʿ信 ���� filterData�� �����մϴ�.
			/*if (i != 0) {
				filterData.word0 = Category2;
				filterData.word1 = Category1 | Category3;
			}*/

			if (i == size - 1)
			{
				if (j == size - i - 1)
				{
					filterData.word0 = COLLISION_CATEGORY::Category2; // �� ��ü�� ī�װ�
					filterData.word1 = COLLISION_CATEGORY::Category1 | COLLISION_CATEGORY::Category3; // �� ��ü�� �浹�ؾ� �ϴ� ī�װ�
					filterData.word3 = m_iRigid_Dynamic_Count;

					// �� ���͸��� ���ο� PxShape �ν��Ͻ��� �����մϴ�.
					physx::PxShape* newShape = m_Physics->createShape(physx::PxBoxGeometry(halfExtent * 10.f, halfExtent * 10.f, halfExtent), *m_Material);
					newShape->setSimulationFilterData(filterData);
					newShape->setContactOffset(1.1f);
					physx::PxTransform localTm(physx::PxVec3(10.f, 5.f, 10.f));
					physx::PxRigidDynamic* body = m_Physics->createRigidDynamic(t.transform(localTm));
					body->attachShape(*newShape);
					physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
					m_Scene->addActor(*body);
					m_vecBody.push_back(body);
					// ��� �� newShape�� release�մϴ�.
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
				filterData.word0 = COLLISION_CATEGORY::Category2; // �� ��ü�� ī�װ�
				filterData.word1 = COLLISION_CATEGORY::Category1 | COLLISION_CATEGORY::Category3; // �� ��ü�� �浹�ؾ� �ϴ� ī�װ�
				filterData.word3 = m_iRigid_Dynamic_Count;

				// �� ���͸��� ���ο� PxShape �ν��Ͻ��� �����մϴ�.
				physx::PxShape* newShape = m_Physics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_Material);
				newShape->setSimulationFilterData(filterData);
				newShape->setContactOffset(0.1f);
				//physx::PxTransform localTm(physx::PxVec3(3.f,0.f, 3.f));
				physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i)+10.f, physx::PxReal(i * 2 + 1)+5.f, 10.f) * (halfExtent));
				physx::PxRigidDynamic* body = m_Physics->createRigidDynamic(t.transform(localTm));
				body->attachShape(*newShape);
				physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
				m_Scene->addActor(*body);
				m_vecBody.push_back(body);
				// ��� �� newShape�� release�մϴ�.
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
		Pos.y = static_cast<_float>(position.y-1.f);
		Pos.z = static_cast<_float>(position.z);
		Pos.w = 1.f;
		return Pos;
	}

	return _float4(0.f, 0.f, 0.f, 1.f);
}

void CPhysics_Controller::InitTerrain()
{
	//����

	// Cooking �Ķ���� ����
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

	// Triangle Mesh ������ ����
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<PxU32>(Num);
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertices.data();

	meshDesc.triangles.count = static_cast<PxU32>(Indices / 3);
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = IndicesVec.data();


	m_TerrainMesh = PxCreateTriangleMesh(cookingParams, meshDesc);

	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f)); // ������ ��ġ
	m_TerrainActor = m_Physics->createRigidStatic(transform);

	PxTriangleMeshGeometry meshGeometry(m_TerrainMesh);
	PxShape* terrainShape = m_Physics->createShape(meshGeometry, *m_Physics->createMaterial(0.5f, 0.5f, 0.5f));

	//PxFilterData filterData_Character;
	//filterData_Character.word0 = COLLISION_CATEGORY::Category1;
	//filterData_Character.word1 = Category1 | Category2 | Category3;

	//// ĳ���� ��Ʈ�ѷ��� PxShape�� �浹 ���� ������ ����
	//terrainShape->setSimulationFilterData(filterData_Character);

	m_TerrainActor->attachShape(*terrainShape);
	terrainShape->release();

	// PhysX ���� ���� �߰�
	PxScene* scene = m_Scene; // �̹� �ʱ�ȭ�� PxScene �ν��Ͻ�
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

	if(m_Shape)
		m_Shape->release();
	if (m_BodyCollider)
		m_BodyCollider->release();
	m_TerrainActor->release();
	m_GroundPlane->release();
	m_TerrainMesh->release();
	m_Material->release();
	m_Controller->release();
	m_Manager->release();
	m_Scene->release();
	m_Dispatcher->release();
	m_Physics->release();
	m_Pvd->release();
	m_Transport->release();
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
