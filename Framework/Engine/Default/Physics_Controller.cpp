#include "Physics_Controller.h"
#include "GameInstance.h"
#include "Event_Call_Back.h"
#include "Engine_Struct.h"


CPhysics_Controller::CPhysics_Controller()
{

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
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_Pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
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
	m_Scene = m_Physics->createScene(sceneDesc);
	m_Scene->setSimulationEventCallback(m_EventCallBack);


	physx::PxPvdSceneClient* pvdClient = m_Scene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}


	PxFilterData filterData;
	filterData.word0 = COLLISION_CATEGORY::Category1; // 이 객체의 카테고리
	filterData.word1 = COLLISION_CATEGORY::Category2 | COLLISION_CATEGORY::Category3; // 이 객체와 충돌해야 하는 카테고리


	//create simulation
	m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*m_Physics, physx::PxPlane(0, 1, 0, 1), *m_Material);
	m_Scene->addActor(*groundPlane);

	float halfExtent = 0.5f;
	physx::PxShape* shape = m_Physics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_Material);
	shape->setSimulationFilterData(filterData);
	physx::PxU32 size = 30;
	physx::PxTransform t(physx::PxVec3(0));
	//for (physx::PxU32 i = 0; i < size; ++i)
	//{
	//	for (physx::PxU32 j = 0; j < size - i; ++j)
	//	{
	//		if (i != 0)
	//		{
	//			filterData.word0 = Category2;
	//			filterData.word1 = Category1 | Category3; // 이 객체와 충돌해야 하는 카테고리
	//		}
	//		
	//		physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * (halfExtent*0.5f));
	//		physx::PxRigidDynamic* body = mPhysics->createRigidDynamic(t.transform(localTm));
	//		body->attachShape(*shape);
	//		physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
	//		mScene->addActor(*body);
	//	}
	//}

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
					filterData.word0 = Category2;
					filterData.word1 = Category1 | Category3;
				}
			}

			// 각 액터마다 새로운 PxShape 인스턴스를 생성합니다.
			physx::PxShape* newShape = m_Physics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_Material);
			newShape->setSimulationFilterData(filterData);
			newShape->setContactOffset(0.1f);
			physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * (halfExtent * 0.5f));
			physx::PxRigidDynamic* body = m_Physics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*newShape);
			physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
			m_Scene->addActor(*body);

			// 사용 후 newShape를 release합니다.
			newShape->release();
		}
	}
	shape->release();

	return S_OK;
}

void CPhysics_Controller::Simulate()
{
	m_Scene->simulate(1.f / 60.f);
	m_Scene->fetchResults(true);
}

void CPhysics_Controller::Free()
{
	__super::Free();

	m_Scene->release();
	m_Dispatcher->release();
	m_Foundation->release();
	m_Physics->release();
	m_Material->release();
	m_Material->release();
	delete m_EventCallBack;
}

CPhysics_Controller* CPhysics_Controller::Create()
{
	return nullptr;
}
