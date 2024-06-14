#pragma once
#include "Event_Call_Back.h"
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
	sceneDesc.filterShader = MegamotionFilterShader;
	
	//Call Back
	m_EventCallBack = new CEventCallBack();
	m_FilterCallBack = new CFilterCallBack();
	sceneDesc.filterCallback = m_FilterCallBack;
	m_Scene = m_Physics->createScene(sceneDesc);
	m_Scene->setSimulationEventCallback(m_EventCallBack);

	//Init - For Pvd
	m_PvdClient = m_Scene->getScenePvdClient();
	if (m_PvdClient)
	{
		m_PvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		m_PvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		m_PvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	//Create simulation
	m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);
	m_GroundPlane = PxCreatePlane(*m_Physics, physx::PxPlane(0, 1, 0, 0.f), *m_Material);
	m_Scene->addActor(*m_GroundPlane);

	//Init - For Joint
	PxInitExtensions(*m_Physics, m_Pvd);

	//Character Controller Init
	m_Manager = PxCreateControllerManager(*m_Scene);

	m_vecRagdoll.clear();

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

	//Gravity Setting
	PxVec3 gravity(0.0f, -9.81f * fTimeDelta, 0.0f);

	//CCT_Filter
	QueryFilterCallback filterCallback;
	PxControllerFilters controllerFilters;
	controllerFilters.mFilterCallback = &filterCallback;

	//Apply Gravity
	for (int i = 0; i < m_vecCharacter_Controller.size(); ++i)
	{
		if (m_vecCharacter_Controller[i])
			m_vecCharacter_Controller[i]->Move(gravity, fTimeDelta);
	}

	for (int i = 0; i < m_vecRigid_Dynamic.size(); ++i)
	{
		m_vecRigid_Dynamic[i]->Update();
	}
	
	for (int i = 0; i < m_vecRagdoll.size(); ++i)
	{
		m_vecRagdoll[i]->Update(fTimeDelta);
	}

	//Simulate
	m_Scene->simulate(fTimeDelta);
	m_Scene->fetchResults(true);
}

CCharacter_Controller* CPhysics_Controller::Create_Controller(_float4 Pos, _int* Index,CGameObject* pCharacter,_float fHeight,_float fRadius)
{
	//Init Shape
	m_Controll_Desc.height = fHeight;
	m_Controll_Desc.radius = fRadius;
	m_Controll_Desc.position = PxExtendedVec3(Pos.x, Pos.y, Pos.z);
	m_Controll_Desc.material = m_Physics->createMaterial(0.f, 0.f, 0.f);
	m_Controll_Desc.stepOffset = 0.1f;

	auto Controller = m_Manager->createController(m_Controll_Desc);

	PxShape* shapes[1];
	Controller->getActor()->getShapes(shapes, 1);

	//Colliision Filter
	PxFilterData filterData_Character;
	filterData_Character.word0 = COLLISION_CATEGORY::Category1;
	filterData_Character.word1 = COLLISION_CATEGORY::Category2 | COLLISION_CATEGORY::Category3; 
	filterData_Character.word3 = m_iCharacter_Controller_Count;

	shapes[0]->setSimulationFilterData(filterData_Character);
	shapes[0]->setContactOffset(0.1f);

	auto Character_Controller = new CCharacter_Controller(Controller, pCharacter,m_Scene,m_Physics);
	Character_Controller->SetIndex(m_iCharacter_Controller_Count);
	m_vecCharacter_Controller.push_back(Character_Controller);

	if (m_vecCharacter_Controller.size() >= 2)
	{
		std::sort(m_vecCharacter_Controller.begin(), m_vecCharacter_Controller.end(), [](CCharacter_Controller* a, CCharacter_Controller* b)
			{
				return a->GetIndex() < b->GetIndex();
			});
	}

	*Index = m_iCharacter_Controller_Count;

	Safe_AddRef(m_vecCharacter_Controller[m_iCharacter_Controller_Count]);
	auto ReturnPtr = m_vecCharacter_Controller[m_iCharacter_Controller_Count];
	++m_iCharacter_Controller_Count;

	return ReturnPtr;
}

void CPhysics_Controller::Create_Rigid_Dynamic(_float4 Pos)
{

	return;
}

void CPhysics_Controller::Cook_Mesh(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, CTransform* pTransform)
{
	// Init Cooking Params 
	PxCookingParams cookingParams(m_Physics->getTolerancesScale());

	vector<PxVec3> vertices;
	vector<PxU32> IndicesVec;

	_matrix WorldMat = XMMatrixIdentity();
	if (pTransform != nullptr)
	{
		WorldMat = pTransform->Get_WorldMatrix();
	}

	for (int i = 0; i < VertexNum; ++i)
	{
		_vector VertexPos = XMLoadFloat3(&pVertices[i]);
		VertexPos = XMVector3TransformCoord(VertexPos, WorldMat);

		_float4 vPos;
		XMStoreFloat4(&vPos, VertexPos);

		PxVec3 Ver = PxVec3(vPos.x, vPos.y, vPos.z);
		vertices.push_back(Ver);
	}

	for (int i = 0; i < IndexNum; ++i)
	{
		PxU32 Ind = pIndices[i];
		IndicesVec.push_back(Ind);
	}

	// Init Triangle Mesh
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

	//Start Simulate
	m_pGameInstance->SetSimulate(true);
}

CRagdoll_Physics* CPhysics_Controller::Create_Ragdoll(vector<class CBone*>* vecBone, CTransform* pTransform, const string& name)
{
	//Ragdoll Init
	auto pRagdoll = new CRagdoll_Physics(m_Scene, m_Physics, m_DefaultAllocatorCallback, m_DefaultErrorCallback, m_Foundation,
		m_Dispatcher, m_Material);

	pRagdoll->SetBone_Ragdoll(vecBone);
	//pRagdoll->SetRotationMatrix(RotationMatrix);
	//pRagdoll->SetWorldMatrix(WorldMatrix);
	pRagdoll->SetTransform(pTransform);
	pRagdoll->Init(name);

	m_vecRagdoll.push_back(pRagdoll);
	pRagdoll->Init_Ragdoll();

	return pRagdoll;
}

void CPhysics_Controller::Start_Ragdoll(CRagdoll_Physics* pRagdoll,_uint iId)
{
	if (nullptr != pRagdoll)
	{
		pRagdoll->Set_Kinematic(false);
		pRagdoll->Init_Ragdoll();
		
		if (m_vecCharacter_Controller[iId])
		{
			m_vecCharacter_Controller[iId]->Release_Px();
		}
	}
}

void CPhysics_Controller::SetBone_Ragdoll(vector<class CBone*>* vecBone)
{
	//if(nullptr != m_pRagdoll_Physics)
	//	m_pRagdoll_Physics->SetBone_Ragdoll(vecBone);
}

void CPhysics_Controller::SetWorldMatrix_Ragdoll(_float4x4 WorldMatrix)
{
	//if (nullptr != m_pRagdoll_Physics)
	//	m_pRagdoll_Physics->SetWorldMatrix(WorldMatrix);
}

void CPhysics_Controller::SetRotationMatrix_Ragdoll(_float4x4 WorldMatrix)
{
	/*if (nullptr != m_pRagdoll_Physics)
		m_pRagdoll_Physics->SetRotationMatrix(WorldMatrix);*/
}

void CPhysics_Controller::Create_Rigid_Static(_float4 Pos)
{
}

void CPhysics_Controller::Create_Plane(_float4 Pos)
{
}

void CPhysics_Controller::InitTerrain()
{
	// Init Cookint Params
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

	// Init Triangle Mesh
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

	PxFilterData filterData_Character;
	filterData_Character.word0 = COLLISION_CATEGORY::Category1;
	filterData_Character.word1 = Category1 | Category2 | Category3;

	terrainShape->setSimulationFilterData(filterData_Character);

	m_TerrainActor->attachShape(*terrainShape);
	terrainShape->release();

	PxScene* scene = m_Scene;
	scene->addActor(*m_TerrainActor);

	m_pGameInstance->SetSimulate(true);
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

_bool CPhysics_Controller::RayCast(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint, _float fMaxDist)
{
	PxVec3 PxvOrigin = Float4_To_PxVec(vOrigin);
	PxVec3 PxvDir = Float4_To_PxVec(vDir);

	PxRaycastBuffer hit;

	bool Status = m_Scene->raycast(PxvOrigin, PxvDir.getNormalized(), fMaxDist, hit, PxHitFlag::eDEFAULT, PxQueryFilterData(PxQueryFlag::eSTATIC /*| PxQueryFlag::eDYNAMIC*/));

	if (Status)
	{
		*pBlockPoint = PxVec_To_Float4_Coord(hit.block.position);
	}

	/*PxSweepBuffer hit;

	bool Status = m_Scene->sweep(
		PxSphereGeometry(0.1f), 
		PxTransform(PxvOrigin), 
		PxvDir, 
		fMaxDist,
		hit, 
		PxHitFlag::eDEFAULT, PxQueryFilterData(PxQueryFlag::eSTATIC)
	);

	if (Status)
	{
		*pBlockPoint = PxVec_To_Float4_Coord(hit.block.position);
	}*/

	return Status;
}

_bool CPhysics_Controller::SphereCast(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint, _float fMaxDist)
{
	PxVec3 PxvOrigin = Float4_To_PxVec(vOrigin);
	PxVec3 PxvDir = Float4_To_PxVec(vDir);

	PxSweepBuffer hit;

	bool Status = m_Scene->sweep(
		PxSphereGeometry(0.1f),
		PxTransform(PxvOrigin),
		PxvDir.getNormalized(),
		fMaxDist,
		hit,
		PxHitFlag::ePRECISE_SWEEP, PxQueryFilterData(PxQueryFlag::eSTATIC)
	);

	if (Status)
	{
		*pBlockPoint = PxVec_To_Float4_Coord(hit.block.position);

		if (hit.block.position.x == 0 && hit.block.position.y == 0 && hit.block.position.z == 0)
		{
			return false;
		}
	}

	return Status;
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

void CPhysics_Controller::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	for (int i = 0; i < m_vecFullMapObject.size(); ++i)
	{
		m_vecFullMapObject[i]->release();
	}

	PxCloseExtensions();

	for (int i = 0; i < m_vecRagdoll.size(); ++i)
	{
		Safe_Release(m_vecRagdoll[i]);
	}

	if (m_Shape)
		m_Shape->release();

	if (m_TerrainActor)
		m_TerrainActor->release();

	if (m_GroundPlane)
		m_GroundPlane->release();

	if (m_TerrainMesh)
		m_TerrainMesh->release();

	if (m_Material)
		m_Material->release();

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
