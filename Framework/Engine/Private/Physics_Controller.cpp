#pragma once
#include "Event_Call_Back.h"
#include "Physics_Controller.h"
#include "GameInstance.h"
#include "Engine_Struct.h"
#include "Character_Controller.h"
#include "Rigid_Dynamic.h"
#include "RagDoll_Physics.h"
#include "Transform.h"
#include "PxCollider.h"
#include "Model.h"
#include "SoftBody.h"
#include "Rigid_Static.h"

//#define OLD_RAYCAST
#define NEW_RAYCAST

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
	m_ToleranceScale.length = PxReal(1.f);
	m_ToleranceScale.speed = PxReal(9.81f);
	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, m_ToleranceScale, true, m_Pvd);
	physx::PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	m_Dispatcher = physx::PxDefaultCpuDispatcherCreate(4);
	sceneDesc.cpuDispatcher = m_Dispatcher;
	sceneDesc.filterShader = MegamotionFilterShader;

#pragma region GPU 가속 설정 - RigidBody
	//PxCudaContextManagerDesc cudaContextManagerDesc;
	//m_CudaContextManager = PxCreateCudaContextManager(*m_Foundation, cudaContextManagerDesc, PxGetProfilerCallback());

	//sceneDesc.cudaContextManager = m_CudaContextManager;

	//sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	//sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
#pragma endregion

	//Call Back
	m_EventCallBack = new CEventCallBack();
	m_FilterCallBack = new CFilterCallBack();
	sceneDesc.filterCallback = m_FilterCallBack;
	sceneDesc.simulationEventCallback = m_EventCallBack;
	m_Scene = m_Physics->createScene(sceneDesc);

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

	m_vecBlockNormals_Props.clear();
	m_vecBlockPoints_Props.clear();

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
	if (m_pGameInstance->GetSimulate() == false || m_pGameInstance->IsPaused())
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
		if (m_vecCharacter_Controller[i] && m_vecCharacter_Controller[i]->GetbGravity())
			m_vecCharacter_Controller[i]->Move(gravity, fTimeDelta);
	}

	for (int i = 0; i < m_vecRigid_Dynamic.size(); ++i)
	{
		m_vecRigid_Dynamic[i]->Update();
	}

	//for (int i = 0; i < m_vecRagdoll.size(); ++i)
	//{
	//	m_vecRagdoll[i]->Update(fTimeDelta);
	//}

	//Simulate
	m_Scene->simulate(fTimeDelta);
	m_Scene->fetchResults(true);
}

void CPhysics_Controller::Simulate_Ragdoll(_float fTimeDelta)
{
	for (int i = 0; i < m_vecRagdoll.size(); ++i)
	{
		m_vecRagdoll[i]->Update(fTimeDelta);
	}
}

CCharacter_Controller* CPhysics_Controller::Create_Controller(_float4 Pos, _int* Index, CGameObject* pCharacter, _float fHeight, _float fRadius, CTransform* pTransform, vector<CBone*>* pBones, const std::string& name)
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
	filterData_Character.word0 = COLLISION_CATEGORY::CCT;
	//filterData_Character.word0 = COLLISION_CATEGORY::CCT_NO_COLLISION;
	filterData_Character.word1 = COLLISION_CATEGORY::COLLIDER | COLLISION_CATEGORY::RAGDOLL;
	filterData_Character.word3 = m_iCharacter_Controller_Count;

	shapes[0]->setSimulationFilterData(filterData_Character);
	shapes[0]->setContactOffset(0.1f);

	auto Character_Controller = new CCharacter_Controller(Controller, pCharacter, m_Scene, m_Physics, pTransform, pBones, m_iCharacter_Controller_Count, name);
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

CRigid_Dynamic* CPhysics_Controller::Create_Rigid_Dynamic(CModel* pModel, CTransform* pTransform, _int* iId, CGameObject* pObj)
{
	auto pRigid_Dynamic = new CRigid_Dynamic();
	pRigid_Dynamic->SetIndex(m_iRigid_Dynamic_Count);
	pRigid_Dynamic->SetObject(pObj);
	m_vecRigid_Dynamic.push_back(pRigid_Dynamic);
	pModel->Convex_Mesh_Cooking_RigidDynamic(pRigid_Dynamic->GetRigidDynamic_DoublePtr(), m_iRigid_Dynamic_Count, pTransform);

	if (m_vecRigid_Dynamic.size() >= 2)
	{
		std::sort(m_vecRigid_Dynamic.begin(), m_vecRigid_Dynamic.end(), [](CRigid_Dynamic* a, CRigid_Dynamic* b)
			{
				return a->GetIndex() < b->GetIndex();
			});
	}

	*iId = m_iRigid_Dynamic_Count;

	Safe_AddRef(m_vecRigid_Dynamic[m_iRigid_Dynamic_Count]);
	auto ReturnPtr = m_vecRigid_Dynamic[m_iRigid_Dynamic_Count];
	++m_iRigid_Dynamic_Count;

	return ReturnPtr;
}

CRigid_Dynamic* CPhysics_Controller::Create_Rigid_Dynamic_NoConvex(_float fRadius,_int* iId, CGameObject* pObj)
{
	auto pRigid_Dynamic = new CRigid_Dynamic();
	pRigid_Dynamic->SetIndex(m_iRigid_Dynamic_Count);
	pRigid_Dynamic->SetObject(pObj);
	m_vecRigid_Dynamic.push_back(pRigid_Dynamic);

	PxShape* shape = m_Physics->createShape(PxBoxGeometry(fRadius, fRadius, fRadius), *m_Physics->createMaterial(0.5f, 0.5f, 0.1f));

	PxTransform local(PxVec3(0.f,0.f,0.f));
	shape->setLocalPose(local);

	PxRigidDynamic* dynamicSphere = m_Physics->createRigidDynamic(local);
	dynamicSphere->setMass(1.f);

	dynamicSphere->attachShape(*shape);

	if (dynamicSphere) {
		// 구체를 물리 씬에 추가
		m_Scene->addActor(*dynamicSphere);
	}
	*pRigid_Dynamic->GetRigidDynamic_DoublePtr() = dynamicSphere;

	if (m_vecRigid_Dynamic.size() >= 2)
	{
		std::sort(m_vecRigid_Dynamic.begin(), m_vecRigid_Dynamic.end(), [](CRigid_Dynamic* a, CRigid_Dynamic* b)
			{
				return a->GetIndex() < b->GetIndex();
			});
	}

	*iId = m_iRigid_Dynamic_Count;

	Safe_AddRef(m_vecRigid_Dynamic[m_iRigid_Dynamic_Count]);
	auto ReturnPtr = m_vecRigid_Dynamic[m_iRigid_Dynamic_Count];
	++m_iRigid_Dynamic_Count;

	return ReturnPtr;
}

CRigid_Dynamic* CPhysics_Controller::Create_Rigid_Dynamic_Grenade(CModel* pModel, CTransform* pTransform, _int* iId, CGameObject* pObj)
{
	auto pRigid_Dynamic = new CRigid_Dynamic();
	pRigid_Dynamic->SetIndex(m_iRigid_Dynamic_Count);
	pRigid_Dynamic->SetObject(pObj);
	m_vecRigid_Dynamic.push_back(pRigid_Dynamic);
  	pModel->Convex_Mesh_Cooking_RigidDynamic_Grenade(pRigid_Dynamic->GetRigidDynamic_DoublePtr(), pTransform);

	if (m_vecRigid_Dynamic.size() >= 2)
	{
		std::sort(m_vecRigid_Dynamic.begin(), m_vecRigid_Dynamic.end(), [](CRigid_Dynamic* a, CRigid_Dynamic* b)
			{
				return a->GetIndex() < b->GetIndex();
			});
	}

	*iId = m_iRigid_Dynamic_Count;

	Safe_AddRef(m_vecRigid_Dynamic[m_iRigid_Dynamic_Count]);
	auto ReturnPtr = m_vecRigid_Dynamic[m_iRigid_Dynamic_Count];
	++m_iRigid_Dynamic_Count;

	return ReturnPtr;
}

void CPhysics_Controller::Cook_Mesh(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, CTransform* pTransform, _int* pIndex)
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

	for (_uint i = 0; i < VertexNum; ++i)
	{
		_vector VertexPos = XMLoadFloat3(&pVertices[i]);
		VertexPos = XMVector3TransformCoord(VertexPos, WorldMat);

		_float4 vPos;
		XMStoreFloat4(&vPos, VertexPos);

		PxVec3 Ver = PxVec3(vPos.x, vPos.y, vPos.z);
		vertices.push_back(Ver);
	}

	for (_uint i = 0; i < IndexNum; ++i)
	{
		PxU32 Ind = pIndices[i];
		IndicesVec.push_back(Ind);
	}

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<PxU32>(VertexNum);
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertices.data();

	meshDesc.triangles.count = static_cast<PxU32>(IndexNum / 3);
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = IndicesVec.data();


	auto Mesh = PxCreateTriangleMesh(cookingParams, meshDesc);

	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f));
	auto Actor = m_Physics->createRigidStatic(transform);

	PxTriangleMeshGeometry meshGeometry(Mesh);
	PxShape* Shape = m_Physics->createShape(meshGeometry, *m_Physics->createMaterial(0.5f, 0.5f, 0.5f));

	PxFilterData StaticMeshFilter;
	StaticMeshFilter.word0 = COLLISION_CATEGORY::STATIC_MESH;
	StaticMeshFilter.word3 = *pIndex;

	Shape->setSimulationFilterData(StaticMeshFilter);

	Actor->attachShape(*Shape);
	Shape->release();

	m_Scene->addActor(*Actor);

	m_vecFullMapObject.push_back(Actor);
	++m_iMapMeshCount;

	Mesh->release();
}

void CPhysics_Controller::Cook_Mesh_NoRotation(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, CTransform* pTransform)
{
	// Init Cooking Params 
	PxCookingParams cookingParams(m_Physics->getTolerancesScale());

	vector<PxVec3> vertices;
	vector<PxU32> IndicesVec;

	_matrix WorldMat = XMMatrixIdentity();
	if (pTransform != nullptr)
	{
		WorldMat = XMMatrixRotationY(PxPi) * pTransform->Get_WorldMatrix();
	}

	for (_uint i = 0; i < VertexNum; ++i)
	{
		_vector VertexPos = XMLoadFloat3(&pVertices[i]);
		VertexPos = XMVector3TransformCoord(VertexPos, WorldMat);

		_float4 vPos;
		XMStoreFloat4(&vPos, VertexPos);

		PxVec3 Ver = PxVec3(vPos.x, vPos.y, vPos.z);
		vertices.push_back(Ver);
	}

	for (_uint i = 0; i < IndexNum; ++i)
	{
		PxU32 Ind = pIndices[i];
		IndicesVec.push_back(Ind);
	}

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<PxU32>(VertexNum);
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertices.data();

	meshDesc.triangles.count = static_cast<PxU32>(IndexNum / 3);
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = IndicesVec.data();


	auto Mesh = PxCreateTriangleMesh(cookingParams, meshDesc);

	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f));
	auto Actor = m_Physics->createRigidStatic(transform);

	PxTriangleMeshGeometry meshGeometry(Mesh);
	PxShape* Shape = m_Physics->createShape(meshGeometry, *m_Physics->createMaterial(0.5f, 0.5f, 0.5f));

	PxFilterData StaticMeshFilter;
	StaticMeshFilter.word0 = COLLISION_CATEGORY::STATIC_MESH;
	StaticMeshFilter.word3 = IndexNum;
	Shape->setSimulationFilterData(StaticMeshFilter);

	Actor->attachShape(*Shape);
	Shape->release();

	m_Scene->addActor(*Actor);

	m_vecFullMapObject.push_back(Actor);
	++m_iMapMeshCount;

	Mesh->release();

	//Start Simulate
	//	m_pGameInstance->SetSimulate(true);
}

void CPhysics_Controller::Cook_Mesh_Dynamic(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, class CTransform* pTransform)
{
	PxCookingParams cookingParams(m_Physics->getTolerancesScale());

	vector<PxVec3> vertices;
	vector<PxU32> IndicesVec;

	auto vScale = pTransform->Get_Scaled();
	auto ScaleMatrix = XMMatrixScaling(vScale.x, vScale.y, vScale.z);

	auto RotationMatrix = pTransform->Get_RotationMatrix_Pure_Mat();
	auto PxQuat = to_quat(XMQuaternionRotationMatrix(RotationMatrix));
	RotationMatrix = XMMatrixRotationY(PxPi) * RotationMatrix * ScaleMatrix;

	auto WorldMat = pTransform->Get_WorldMatrix();
	WorldMat = XMMatrixRotationY(PxPi) * WorldMat;

	for (_uint i = 0; i < VertexNum; ++i)
	{
		_vector VertexPos = XMLoadFloat3(&pVertices[i]);
		VertexPos = XMVector3TransformCoord(VertexPos, RotationMatrix);

		_float4 vPos;
		XMStoreFloat4(&vPos, VertexPos);

		PxVec3 Ver = PxVec3(vPos.x, vPos.y, vPos.z);
		vertices.push_back(Ver);
	}

	for (_uint i = 0; i < IndexNum; ++i)
	{
		PxU32 Ind = pIndices[i];
		IndicesVec.push_back(Ind);
	}

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<PxU32>(VertexNum);
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertices.data();

	meshDesc.triangles.count = static_cast<PxU32>(IndexNum / 3);
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = IndicesVec.data();

	auto Mesh = PxCreateTriangleMesh(cookingParams, meshDesc);

	_float4 vPos;
	if (pTransform)
		vPos = pTransform->Get_State_Float4(CTransform::STATE_POSITION);

	PxTransform transform(PxVec3(vPos.x, vPos.y, vPos.z));

	PxTriangleMeshGeometry meshGeometry(Mesh);
	PxMaterial* material = m_Physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxRigidDynamic* body = m_Physics->createRigidDynamic(transform);
	PxShape* shape = m_Physics->createShape(meshGeometry, *material);
	body->attachShape(*shape);
	m_Scene->addActor(*body);
	shape->release();
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	Mesh->release();
	material->release();

	pColliders->push_back(body);
	pTransforms->push_back(transform);
}

void CPhysics_Controller::Cook_Mesh_Convex(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, CTransform* pTransform)
{
	PxCookingParams cookingParams(m_Physics->getTolerancesScale());

	vector<PxVec3> vertices;
	vector<PxU32> IndicesVec;

	auto vScale = pTransform->Get_Scaled();
	auto ScaleMatrix = XMMatrixScaling(vScale.x, vScale.y, vScale.z);

	auto RotationMatrix = pTransform->Get_RotationMatrix_Pure_Mat();
	auto PxQuat = to_quat(XMQuaternionRotationMatrix(RotationMatrix));
	RotationMatrix = XMMatrixRotationY(PxPi) * RotationMatrix * ScaleMatrix;

	auto WorldMat = pTransform->Get_WorldMatrix();
	WorldMat = XMMatrixRotationY(PxPi) * WorldMat;

	for (_uint i = 0; i < VertexNum; ++i)
	{
		_vector VertexPos = XMLoadFloat3(&pVertices[i]);
		VertexPos = XMVector3TransformCoord(VertexPos, RotationMatrix);

		_float4 vPos;
		XMStoreFloat4(&vPos, VertexPos);

		PxVec3 Ver = PxVec3(vPos.x, vPos.y, vPos.z);
		vertices.push_back(Ver);
	}

	for (_uint i = 0; i < IndexNum; ++i)
	{
		PxU32 Ind = pIndices[i];
		IndicesVec.push_back(Ind);
	}

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = static_cast<PxU32>(vertices.size());
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = vertices.data();
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxConvexMesh* convexMesh = PxCreateConvexMesh(cookingParams, convexDesc);

	_float4 vPos;
	if (pTransform)
		vPos = pTransform->Get_State_Float4(CTransform::STATE_POSITION);

	PxTransform transform(PxVec3(vPos.x, vPos.y, vPos.z));

	PxConvexMeshGeometry geometry(convexMesh);
	PxMaterial* material = m_Physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxRigidDynamic* body = m_Physics->createRigidDynamic(transform);
	PxShape* shape = m_Physics->createShape(geometry, *material);

	PxFilterData StaticMeshFilter;
	StaticMeshFilter.word0 = COLLISION_CATEGORY::STATIC_MESH;
	StaticMeshFilter.word3 = IndexNum;
	shape->setSimulationFilterData(StaticMeshFilter);

	body->attachShape(*shape);
	m_Scene->addActor(*body);

	shape->release();
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	convexMesh->release();
	material->release();

	pColliders->push_back(body);
	pTransforms->push_back(transform);
}

void CPhysics_Controller::Cook_Mesh_Convex_RigidDynamic(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, _int iId,PxRigidDynamic** pCollider, CTransform* pTransform)
{
	PxCookingParams cookingParams(m_Physics->getTolerancesScale());

	vector<PxVec3> vertices;
	vector<PxU32> IndicesVec;

	auto vScale = pTransform->Get_Scaled();
	auto ScaleMatrix = XMMatrixScaling(vScale.x, vScale.y, vScale.z);

	auto RotationMatrix = pTransform->Get_RotationMatrix_Pure_Mat();
	auto PxQuat = to_quat(XMQuaternionRotationMatrix(RotationMatrix));
	RotationMatrix = XMMatrixRotationY(PxPi) * RotationMatrix * ScaleMatrix;

	auto WorldMat = pTransform->Get_WorldMatrix();
	WorldMat = XMMatrixRotationY(PxPi) * WorldMat;

	for (_uint i = 0; i < VertexNum; ++i)
	{
		_vector VertexPos = XMLoadFloat3(&pVertices[i]);
		VertexPos = XMVector3TransformCoord(VertexPos, RotationMatrix);

		_float4 vPos;
		XMStoreFloat4(&vPos, VertexPos);

		PxVec3 Ver = PxVec3(vPos.x, vPos.y, vPos.z);
		vertices.push_back(Ver);
	}

	for (_uint i = 0; i < IndexNum; ++i)
	{
		PxU32 Ind = pIndices[i];
		IndicesVec.push_back(Ind);
	}

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = static_cast<PxU32>(vertices.size());
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = vertices.data();
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxConvexMesh* convexMesh = PxCreateConvexMesh(cookingParams, convexDesc);

	_float4 vPos;
	if (pTransform)
		vPos = pTransform->Get_State_Float4(CTransform::STATE_POSITION);

	PxTransform transform(PxVec3(vPos.x, vPos.y, vPos.z));

	PxConvexMeshGeometry geometry(convexMesh);
	PxMaterial* material = m_Physics->createMaterial(1.5f, 1.5f, 0.1f);
	PxRigidDynamic* body = m_Physics->createRigidDynamic(transform);
	PxShape* shape = m_Physics->createShape(geometry, *material);

	PxFilterData StaticMeshFilter;
	StaticMeshFilter.word0 = COLLISION_CATEGORY::EFFECT;
	StaticMeshFilter.word3 = iId;
	shape->setSimulationFilterData(StaticMeshFilter);

	body->attachShape(*shape);
	m_Scene->addActor(*body);

	shape->release();
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	convexMesh->release();
	material->release();

	float sleepThreshold = 2.f;
	body->setSleepThreshold(sleepThreshold);
	body->setMassSpaceInertiaTensor(PxVec3(1.0f, 1.0f, 1.0f));

	PxVec3 inertiaTensor = PxVec3(1.0f, 1.0f, 1.0f);
	body->setMassSpaceInertiaTensor(inertiaTensor);

	*pCollider = body;
}

void CPhysics_Controller::Cook_Mesh_Convex_Convert_Root(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, CTransform* pTransform, _float4 vDelta)
{
	PxCookingParams cookingParams(m_Physics->getTolerancesScale());

	vector<PxVec3> vertices;
	vector<PxU32> IndicesVec;

	auto vScale = pTransform->Get_Scaled();
	auto ScaleMatrix = XMMatrixScaling(vScale.x, vScale.y, vScale.z);

	auto RotationMatrix = pTransform->Get_RotationMatrix_Pure_Mat();
	auto PxQuat = to_quat(XMQuaternionRotationMatrix(RotationMatrix));
	RotationMatrix = XMMatrixRotationY(PxPi) * RotationMatrix * ScaleMatrix;

	auto WorldMat = pTransform->Get_WorldMatrix();
	WorldMat = XMMatrixRotationY(PxPi) * WorldMat;

	_vector RootDelta = XMLoadFloat4(&vDelta);
	RootDelta = XMVector4Transform(RootDelta, RotationMatrix);
	RootDelta = XMVectorSetW(RootDelta, 0.f);
	XMStoreFloat4(&vDelta, RootDelta);

	for (_uint i = 0; i < VertexNum; ++i)
	{
		_vector VertexPos = XMLoadFloat3(&pVertices[i]);
		VertexPos = XMVector3TransformCoord(VertexPos, RotationMatrix);

		_float4 vPos;
		XMStoreFloat4(&vPos, VertexPos);
		vPos += vDelta;

		PxVec3 Ver = PxVec3(vPos.x, vPos.y, vPos.z);
		vertices.push_back(Ver);
	}

	for (_uint i = 0; i < IndexNum; ++i)
	{
		PxU32 Ind = pIndices[i];
		IndicesVec.push_back(Ind);
	}

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = static_cast<PxU32>(vertices.size());
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = vertices.data();
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxConvexMesh* convexMesh = PxCreateConvexMesh(cookingParams, convexDesc);

	_float4 vPos;
	if (pTransform)
		vPos = pTransform->Get_State_Float4(CTransform::STATE_POSITION);

	PxTransform transform(PxVec3(vPos.x - vDelta.x, vPos.y - vDelta.y, vPos.z - vDelta.z));

	PxConvexMeshGeometry geometry(convexMesh);
	PxMaterial* material = m_Physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxRigidDynamic* body = m_Physics->createRigidDynamic(transform);
	PxShape* shape = m_Physics->createShape(geometry, *material);
	body->attachShape(*shape);
	m_Scene->addActor(*body);
	shape->release();
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	convexMesh->release();
	material->release();

	pColliders->push_back(body);
	pTransforms->push_back(transform);
}

void CPhysics_Controller::Cook_Mesh_Convex_Convert_Root_No_Rotate(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, CTransform* pTransform, _float4 vDelta)
{
	PxCookingParams cookingParams(m_Physics->getTolerancesScale());

	vector<PxVec3> vertices;
	vector<PxU32> IndicesVec;

	auto vScale = pTransform->Get_Scaled();
	auto ScaleMatrix = XMMatrixScaling(vScale.x, vScale.y, vScale.z);

	auto RotationMatrix = pTransform->Get_RotationMatrix_Pure_Mat();
	auto PxQuat = to_quat(XMQuaternionRotationMatrix(RotationMatrix));
	RotationMatrix = XMMatrixRotationY(PxPi) * RotationMatrix * ScaleMatrix;

	auto WorldMat = pTransform->Get_WorldMatrix();
	WorldMat = XMMatrixRotationY(PxPi) * WorldMat;

	_vector RootDelta = XMLoadFloat4(&vDelta);
	RootDelta = XMVector4Transform(RootDelta, RotationMatrix);
	RootDelta = XMVectorSetW(RootDelta, 0.f);
	XMStoreFloat4(&vDelta, RootDelta);

	for (_uint i = 0; i < VertexNum; ++i)
	{
		_vector VertexPos = XMLoadFloat3(&pVertices[i]);
		VertexPos = XMVector3TransformCoord(VertexPos, RotationMatrix);

		_float4 vPos;
		XMStoreFloat4(&vPos, VertexPos);
		vPos += vDelta;

		PxVec3 Ver = PxVec3(vPos.x, vPos.y, vPos.z);
		vertices.push_back(Ver);
	}

	for (_uint i = 0; i < IndexNum; ++i)
	{
		PxU32 Ind = pIndices[i];
		IndicesVec.push_back(Ind);
	}

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = static_cast<PxU32>(vertices.size());
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = vertices.data();
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxConvexMesh* convexMesh = PxCreateConvexMesh(cookingParams, convexDesc);

	_float4 vPos;
	if (pTransform)
		vPos = pTransform->Get_State_Float4(CTransform::STATE_POSITION);

	PxTransform transform(PxVec3(vPos.x - vDelta.x, vPos.y - vDelta.y, vPos.z - vDelta.z));
	//PxTransform transform(PxVec3(vPos.x, vPos.y, vPos.z));

	PxConvexMeshGeometry geometry(convexMesh);
	PxMaterial* material = m_Physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxRigidDynamic* body = m_Physics->createRigidDynamic(transform);
	PxShape* shape = m_Physics->createShape(geometry, *material);
	body->attachShape(*shape);
	m_Scene->addActor(*body);
	shape->release();
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	convexMesh->release();
	material->release();

	pColliders->push_back(body);
	pTransforms->push_back(transform);
}

void CPhysics_Controller::Create_SoftBody(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, _bool bHasCollider)
{
	PxCookingParams cookingParams(m_Physics->getTolerancesScale());
	cookingParams.meshWeldTolerance = 0.001f;
	cookingParams.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
	cookingParams.buildTriangleAdjacencies = false;
	cookingParams.buildGPUData = true;

	PxArray<PxVec3> triVerts;
	PxArray<PxU32> triIndices;

	/*for (int i = 0; i < VertexNum; ++i)
	{
		_vector VertexPos = XMLoadFloat3(&pVertices[i]);

		_float4 vPos;
		XMStoreFloat4(&vPos, VertexPos);

		PxVec3 Ver = PxVec3(vPos.x, vPos.y, vPos.z);
		triVerts.pushBack(Ver);
	}

	for (int i = 0; i < IndexNum; ++i)
	{
		PxU32 Ind = pIndices[i];
		triIndices.pushBack(Ind);
	}*/

	PxReal maxEdgeLength = 1;

	createCube(triVerts, triIndices, PxVec3(0, 9.5f, 0), 2.5f);
	PxRemeshingExt::limitMaxEdgeLength(triIndices, triVerts, maxEdgeLength);

	PxFEMSoftBodyMaterial* material = PxGetPhysics().createFEMSoftBodyMaterial(1e+6f, 0.45f, 0.5f);
	material->setDamping(0.005f);

	PxSoftBodyMesh* softBodyMesh;

	PxU32 numVoxelsAlongLongestAABBAxis = 8;

	PxSimpleTriangleMesh surfaceMesh;
	surfaceMesh.points.count = triVerts.size();
	surfaceMesh.points.data = triVerts.begin();
	surfaceMesh.triangles.count = triIndices.size() / 3;
	surfaceMesh.triangles.data = triIndices.begin();

	if (!bHasCollider)
	{
		softBodyMesh = PxSoftBodyExt::createSoftBodyMeshNoVoxels(cookingParams, surfaceMesh, m_Physics->getPhysicsInsertionCallback());
	}
	else
	{
		softBodyMesh = PxSoftBodyExt::createSoftBodyMesh(cookingParams, surfaceMesh, numVoxelsAlongLongestAABBAxis, m_Physics->getPhysicsInsertionCallback());
	}

	//Alternatively one can cook a softbody mesh in a single step
	//tetMesh = cooking.createSoftBodyMesh(simulationMeshDesc, collisionMeshDesc, softbodyDesc, physics.getPhysicsInsertionCallback());
	PX_ASSERT(softBodyMesh);

	if (!m_CudaContextManager)
		return;

	PxSoftBody* softBody = m_Physics->createSoftBody(*m_CudaContextManager);
	if (softBody)
	{
		PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE;

		PxFEMSoftBodyMaterial* materialPtr = m_Physics->createFEMSoftBodyMaterial(1e+6f, 0.45f, 0.5f);
		PxTetrahedronMeshGeometry geometry(softBodyMesh->getCollisionMesh());
		PxShape* shape = m_Physics->createShape(geometry, &materialPtr, 1, true, shapeFlags);
		if (shape)
		{
			softBody->attachShape(*shape);
			shape->setSimulationFilterData(PxFilterData(0, 0, 2, 0));
		}
		softBody->attachSimulationMesh(*softBodyMesh->getSimulationMesh(), *softBodyMesh->getSoftBodyAuxData());

		m_Scene->addActor(*softBody);

		PxFEMParameters femParams;
		Config_SoftBody(softBody, femParams, material, PxTransform(PxVec3(0.f, 5.f, 0.f), PxQuat(PxIdentity)), 100.f, 1.0f, 30);
		softBody->setSoftBodyFlag(PxSoftBodyFlag::eDISABLE_SELF_COLLISION, true);


		PxReal halfExtent = 1;
		PxVec3 cubePosA(0, 7.25, 0);
		PxVec3 cubePosB(0, 11.75, 0);
		PxRigidDynamic* rigidCubeA = Create_RigidCube(halfExtent, cubePosA);

		connectCubeToSoftBody(rigidCubeA, 2 * halfExtent, cubePosA, softBody);
	}
}

void CPhysics_Controller::Config_SoftBody(PxSoftBody* softBody, const PxFEMParameters& femParams, PxFEMSoftBodyMaterial* femMaterial, const PxTransform& transform, const PxReal density, const PxReal scale, const PxU32 iterCount)
{
	PxShape* shape = softBody->getShape();

	PxVec4* simPositionInvMassPinned;
	PxVec4* simVelocityPinned;
	PxVec4* collPositionInvMassPinned;
	PxVec4* restPositionPinned;

	PxSoftBodyExt::allocateAndInitializeHostMirror(*softBody, m_CudaContextManager, simPositionInvMassPinned, simVelocityPinned, collPositionInvMassPinned, restPositionPinned);

	const PxReal maxInvMassRatio = 50.f;

	softBody->setParameter(femParams);
	shape->setSoftBodyMaterials(&femMaterial, 1);
	softBody->setSolverIterationCounts(iterCount);

	PxSoftBodyExt::transform(*softBody, transform, scale, simPositionInvMassPinned, simVelocityPinned, collPositionInvMassPinned, restPositionPinned);
	PxSoftBodyExt::updateMass(*softBody, density, maxInvMassRatio, simPositionInvMassPinned);
	PxSoftBodyExt::copyToDevice(*softBody, PxSoftBodyDataFlag::eALL, simPositionInvMassPinned, simVelocityPinned, collPositionInvMassPinned, restPositionPinned);

	SoftBody* sBody = new SoftBody(softBody, m_CudaContextManager);
	m_vecSoftBodies.push_back(sBody);

	PX_PINNED_HOST_FREE(m_CudaContextManager, simPositionInvMassPinned);
	PX_PINNED_HOST_FREE(m_CudaContextManager, simVelocityPinned);
	PX_PINNED_HOST_FREE(m_CudaContextManager, collPositionInvMassPinned);
	PX_PINNED_HOST_FREE(m_CudaContextManager, restPositionPinned);
}

CRagdoll_Physics* CPhysics_Controller::Create_Ragdoll(vector<class CBone*>* vecBone, CTransform* pTransform, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& name)
{
	//Ragdoll Init
	auto pRagdoll = new CRagdoll_Physics(m_Scene, m_Physics, m_DefaultAllocatorCallback, m_DefaultErrorCallback, m_Foundation,
		m_Dispatcher, m_Material);

	pRagdoll->SetBone_Ragdoll(vecBone);
	pRagdoll->SetTransform(pTransform);
	pRagdoll->Init(name,pDevice,pContext);

	m_vecRagdoll.push_back(pRagdoll);

	return pRagdoll;
}

void CPhysics_Controller::Start_Ragdoll(CRagdoll_Physics* pRagdoll, _uint iId)
{
	if (nullptr != pRagdoll)
	{
		if (m_vecCharacter_Controller[iId])
		{
			m_vecCharacter_Controller[iId]->Release_Px();
		}

		pRagdoll->Set_Index_CCT(iId);
		pRagdoll->Init_Ragdoll();
		pRagdoll->SetSimulate(true);
	}
}

void CPhysics_Controller::Start_PartialRagdoll(CRagdoll_Physics* pRagdoll, _uint iId, COLLIDER_TYPE eType)
{
	if (nullptr != pRagdoll)
	{
		if (m_vecCharacter_Controller[iId])
		{
			m_vecCharacter_Controller[iId]->Release_PartialCollider(eType);
		}

		pRagdoll->Set_Index_CCT(iId);
		pRagdoll->Init_PartialRagdoll(eType);
		pRagdoll->SetSimulate_Partial(true);
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

CRagdoll_Physics* CPhysics_Controller::GetRagdoll(_uint iId)
{
	return m_vecRagdoll[iId-1];
}

CRigid_Static* CPhysics_Controller::Create_Rigid_Static(_float4 Pos, _int* Index, class CGameObject* pStaticMesh)
{
	auto pRigid_Static = new CRigid_Static();
	pRigid_Static->SetIndex(m_iRigid_Static_Count);
	pRigid_Static->SetObject(pStaticMesh);
	m_vecRigid_Static.push_back(pRigid_Static);

	if (m_vecRigid_Static.size() >= 2)
	{
		std::sort(m_vecRigid_Static.begin(), m_vecRigid_Static.end(), [](CRigid_Static* a, CRigid_Static* b)
			{
				return a->GetIndex() < b->GetIndex();
			});
	}

	*Index = m_iRigid_Static_Count;

	Safe_AddRef(m_vecRigid_Static[m_iRigid_Static_Count]);
	auto ReturnPtr = m_vecRigid_Static[m_iRigid_Static_Count];
	++m_iRigid_Static_Count;

	return ReturnPtr;
}

CPxCollider* CPhysics_Controller::Create_Px_Collider(CModel* pModel, CTransform* pTransform, _int* iId)
{
	CPxCollider* pCollider = new CPxCollider();
	auto pColliders = pCollider->GetCollider_Container();
	auto pTransforms = pCollider->GetCollider_Transform_Container();
	pModel->Convex_Mesh_Cooking(pColliders, pTransforms, pTransform);

	m_vecCollider.push_back(pCollider);

	*iId = m_iCollider_Count;
	++m_iCollider_Count;

	return pCollider;
}

CPxCollider* CPhysics_Controller::Create_Px_Collider_Convert_Root(CModel* pModel, CTransform* pTransform, _int* iId)
{
	CPxCollider* pCollider = new CPxCollider();
	auto pColliders = pCollider->GetCollider_Container();
	auto pTransforms = pCollider->GetCollider_Transform_Container();
	pModel->Convex_Mesh_Cooking_Convert_Root(pColliders, pTransforms, pTransform);

	m_vecCollider.push_back(pCollider);

	*iId = m_iCollider_Count;
	++m_iCollider_Count;

	return pCollider;
}

CPxCollider* CPhysics_Controller::Create_Px_Collider_Convert_Root_Double_Door(CModel* pModel, CTransform* pTransform, _int* iId)
{
	CPxCollider* pCollider = new CPxCollider();
	auto pColliders = pCollider->GetCollider_Container();
	auto pTransforms = pCollider->GetCollider_Transform_Container();
	pModel->Convex_Mesh_Cooking_Convert_Root_Double_Door_No_Rotate(pColliders, pTransforms, pTransform);

	m_vecCollider.push_back(pCollider);

	*iId = m_iCollider_Count;
	++m_iCollider_Count;

	return pCollider;
}

CPxCollider* CPhysics_Controller::Create_Px_Collider_Cabinet(CModel* pModel, CTransform* pTransform, _int* iId)
{
	CPxCollider* pCollider = new CPxCollider();
	auto pColliders = pCollider->GetCollider_Container();
	auto pTransforms = pCollider->GetCollider_Transform_Container();
	pModel->Convex_Mesh_Cooking_Cabinet(pColliders, pTransforms, pTransform);

	m_vecCollider.push_back(pCollider);

	*iId = m_iCollider_Count;
	++m_iCollider_Count;

	return pCollider;
}

CPxCollider* CPhysics_Controller::Create_Px_Collider_Toilet(CModel* pModel, CTransform* pTransform, _int* iId)
{
	CPxCollider* pCollider = new CPxCollider();
	auto pColliders = pCollider->GetCollider_Container();
	auto pTransforms = pCollider->GetCollider_Transform_Container();
	pModel->Convex_Mesh_Cooking_Toilet(pColliders, pTransforms, pTransform);

	m_vecCollider.push_back(pCollider);

	*iId = m_iCollider_Count;
	++m_iCollider_Count;

	return pCollider;
}

void CPhysics_Controller::Create_Plane(_float4 Pos)
{
}

PxRigidDynamic* CPhysics_Controller::Create_RigidCube(PxReal halfExtent, const PxVec3& position)
{
	PxShape* shape = m_Physics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_Material);

	shape->setSimulationFilterData(PxFilterData(0, 0, 1, 0));

	PxTransform localTm(position);
	PxRigidDynamic* body = m_Physics->createRigidDynamic(localTm);
	body->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	m_Scene->addActor(*body);

	shape->release();

	return body;
}

PxSoftBody* CPhysics_Controller::Create_SoftBody_Debug(const PxCookingParams& params, const PxArray<PxVec3>& triVerts, const PxArray<PxU32>& triIndices, bool useCollisionMeshForSimulation)
{
	PxFEMSoftBodyMaterial* material = PxGetPhysics().createFEMSoftBodyMaterial(1e+6f, 0.45f, 0.5f);
	material->setDamping(0.005f);

	PxSoftBodyMesh* softBodyMesh;

	PxU32 numVoxelsAlongLongestAABBAxis = 8;

	PxSimpleTriangleMesh surfaceMesh;
	surfaceMesh.points.count = triVerts.size();
	surfaceMesh.points.data = triVerts.begin();
	surfaceMesh.triangles.count = triIndices.size() / 3;
	surfaceMesh.triangles.data = triIndices.begin();

	softBodyMesh = PxSoftBodyExt::createSoftBodyMesh(params, surfaceMesh, numVoxelsAlongLongestAABBAxis, m_Physics->getPhysicsInsertionCallback());

	//Alternatively one can cook a softbody mesh in a single step
	//tetMesh = cooking.createSoftBodyMesh(simulationMeshDesc, collisionMeshDesc, softbodyDesc, physics.getPhysicsInsertionCallback());
	PX_ASSERT(softBodyMesh);

	if (!m_CudaContextManager)
		return nullptr;

	PxSoftBody* softBody = m_Physics->createSoftBody(*m_CudaContextManager);
	if (softBody)
	{
		PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE;

		PxFEMSoftBodyMaterial* materialPtr = m_Physics->createFEMSoftBodyMaterial(1e+6f, 0.45f, 0.5f);
		PxTetrahedronMeshGeometry geometry(softBodyMesh->getCollisionMesh());
		PxShape* shape = m_Physics->createShape(geometry, &materialPtr, 1, true, shapeFlags);
		if (shape)
		{
			softBody->attachShape(*shape);
			shape->setSimulationFilterData(PxFilterData(0, 0, 2, 0));
		}
		softBody->attachSimulationMesh(*softBodyMesh->getSimulationMesh(), *softBodyMesh->getSoftBodyAuxData());

		m_Scene->addActor(*softBody);

		PxFEMParameters femParams;
		Config_SoftBody(softBody, femParams, material, PxTransform(PxVec3(10.f, 5.f, 10.f), PxQuat(PxIdentity)), 100.f, 10.0f, 30);
		softBody->setSoftBodyFlag(PxSoftBodyFlag::eDISABLE_SELF_COLLISION, true);
	}

	return softBody;
}

void CPhysics_Controller::initCloth(const PxU32 numX, const PxU32 numZ, const PxVec3& position, const PxReal particleSpacing, const PxReal totalClothMass)
{

}

void CPhysics_Controller::Create_Cloth(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, CTransform* pTransform)
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
	filterData_Character.word0 = COLLISION_CATEGORY::CCT;
	filterData_Character.word1 = CCT | COLLIDER | RAGDOLL;

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

	return Status;
}

_bool CPhysics_Controller::RayCast_Shoot(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint,_float4* pBlockNormal, _bool bBigAttack, _bool bDecalRay, _bool* IsHit_Props, _float fMaxDist)
{
	PxVec3 PxvOrigin = Float4_To_PxVec(vOrigin);
	PxVec3 PxvDir = Float4_To_PxVec(vDir);

	const PxU32 maxHits = 10;
	PxRaycastHit hitBuffer[maxHits];
	PxRaycastBufferN<maxHits> hit;

	PxQueryFilterData filterData;
	filterData.flags = PxQueryFlag::eDYNAMIC /*| PxQueryFlag::eSTATIC*/;

	auto Filter = QueryFilterCallback_Ray();

	bool Status = m_Scene->raycast(PxvOrigin, PxvDir.getNormalized(), 50.f, hit,PxHitFlag::eDEFAULT | PxHitFlag::eNORMAL, filterData, &Filter);

	_vector OriginVector = XMLoadFloat4(&vOrigin);

#ifdef NEW_RAYCAST
#pragma region 신버전 (거리 비교)
	if (Status)
	{
		for (PxU32 i = 0; i < hit.getNbTouches(); ++i)
		{
			const PxRaycastHit& hit_Obj = hit.getTouch(i);

			PxShape* shape = hit_Obj.shape;
			PxRigidActor* actor = hit_Obj.actor;

			PxFilterData filterData = shape->getSimulationFilterData();

			if (filterData.word0 & COLLISION_CATEGORY::COLLIDER)
			{
				COLLIDER_TYPE eType = (COLLIDER_TYPE)(_int)filterData.word3;

				_float fDist = XMVectorGetX(XMVector3Length(OriginVector - XMLoadFloat4(&PxVec_To_Float4_Coord(hit_Obj.position))));
				if (fDist < m_fMinDist)
				{
					m_fMinDist = fDist;
					m_iMinPointIndex = i;
					m_bRagdollHit = true;
					m_bHit = true;
				}
			}

			if (filterData.word0 & COLLISION_CATEGORY::RAGDOLL)
			{
				COLLIDER_TYPE eType = (COLLIDER_TYPE)(_int)filterData.word3;

				_float fDist = XMVectorGetX(XMVector3Length(OriginVector - XMLoadFloat4(&PxVec_To_Float4_Coord(hit_Obj.position))));
				if (fDist < m_fMinDist)
				{
					m_fMinDist = fDist;
					m_iMinPointIndex = i;
					m_bRagdollHit = true;
					m_bHit = true;
				}
			}
		}

		if (m_bHit == false)
		{
			m_fMinDist = 9999.f;
			m_bHit = false;
			m_bRagdollHit = false;
			return false;
		}

		const PxRaycastHit& hit_Obj = hit.getTouch(m_iMinPointIndex);

		PxShape* shape = hit_Obj.shape;
		PxRigidActor* actor = hit_Obj.actor;

		PxFilterData filterData = shape->getSimulationFilterData();

		if (filterData.word0 & COLLISION_CATEGORY::COLLIDER)
		{
			COLLIDER_TYPE eType = (COLLIDER_TYPE)(_int)filterData.word3;

			*pBlockPoint = PxVec_To_Float4_Coord(hit_Obj.position);

			if (m_vecCharacter_Controller[filterData.word2] && m_vecCharacter_Controller[filterData.word2]->IsReleased() == false)
			{
				auto vDelta = Float4_Normalize(*pBlockPoint - vOrigin);
				vDelta.y = 0.f;

				m_vecCharacter_Controller[filterData.word2]->Set_Hit(true);

				if (bDecalRay)
				{
					m_vecCharacter_Controller[filterData.word2]->Set_Force(vDelta, eType);
					m_vecCharacter_Controller[filterData.word2]->SetBlockPoint(*pBlockPoint);
					m_vecCharacter_Controller[filterData.word2]->SetHitNormal(PxVec_To_Float4_Coord(hit_Obj.normal));
					m_vecCharacter_Controller[filterData.word2]->Set_Hit_Decal_Ray(true);
				}

				if (bBigAttack)
				{
					m_vecCharacter_Controller[filterData.word2]->SetBigAttack(true);
					m_vecCharacter_Controller[filterData.word2]->Increase_Hit_Count_STG();
					m_vecCharacter_Controller[filterData.word2]->Insert_Hit_Point_STG(*pBlockPoint);
					m_vecCharacter_Controller[filterData.word2]->Insert_Hit_Normal_STG(PxVec_To_Float4_Coord(hit_Obj.normal));
					m_vecCharacter_Controller[filterData.word2]->Insert_Collider_Type(eType);
				}
				else
				{
					m_vecCharacter_Controller[filterData.word2]->SetBigAttack(false);
				}

				//if (eType == COLLIDER_TYPE::HEAD)
				{
					m_vecCharacter_Controller[filterData.word2]->Increase_Hit_Count();
				}

				if (bBigAttack)
				{
					if (m_vecCharacter_Controller[filterData.word2]->Get_Hit_Count() >= 15)
					{
						/*Ragdoll을 구동하려면 살려야 함.*/
						m_vecCharacter_Controller[filterData.word2]->SetReleased(true);
						m_vecCharacter_Controller[filterData.word2]->SetDead(true);
						m_vecCharacter_Controller[filterData.word2]->Set_Force(vDelta, eType);
					}
				}
				else
				{
					if (m_vecCharacter_Controller[filterData.word2]->Get_Hit_Count() >= 15)
					{
						/*Ragdoll을 구동하려면 살려야 함.*/
						m_vecCharacter_Controller[filterData.word2]->SetReleased(true);
						m_vecCharacter_Controller[filterData.word2]->SetDead(true);
						m_vecCharacter_Controller[filterData.word2]->Set_Force(vDelta, eType);
					}
				}

				m_fMinDist = 9999.f;
				m_bHit = false;
				m_bRagdollHit = false;
				return true;
			}


			m_fMinDist = 9999.f;
			m_bHit = false;
			m_bRagdollHit = false;
			return false;
		}

		if (filterData.word0 & COLLISION_CATEGORY::RAGDOLL)
		{
			COLLIDER_TYPE eType = (COLLIDER_TYPE)(_int)filterData.word3;

			*pBlockPoint = PxVec_To_Float4_Coord(hit_Obj.position);

			PxRigidDynamic* dynamicActor = actor->is<PxRigidDynamic>();
			if (dynamicActor)
			{
				auto vDelta = Float4_Normalize(*pBlockPoint - vOrigin);
				vDelta.y = 0.f;

				auto fPower = rand() % 100;

				vDelta = vDelta * (fPower + 50.f);

				PxVec3 pxForce = PxVec3(vDelta.x, vDelta.y, vDelta.z);
				dynamicActor->addForce(pxForce, PxForceMode::eIMPULSE);
				m_vecCharacter_Controller[filterData.word2]->Set_Hit(true);

				if (bDecalRay)
				{
					m_vecCharacter_Controller[filterData.word2]->SetBlockPoint(*pBlockPoint);
					m_vecCharacter_Controller[filterData.word2]->SetHitNormal(PxVec_To_Float4_Coord(hit_Obj.normal));
					m_vecCharacter_Controller[filterData.word2]->Set_Hit_Decal_Ray(true);
				}

				if (bBigAttack)
				{
					m_vecCharacter_Controller[filterData.word2]->SetBigAttack(true);
					m_vecCharacter_Controller[filterData.word2]->Increase_Hit_Count_STG();
					m_vecCharacter_Controller[filterData.word2]->Insert_Hit_Point_STG(*pBlockPoint);
					m_vecCharacter_Controller[filterData.word2]->Insert_Hit_Normal_STG(PxVec_To_Float4_Coord(hit_Obj.normal));
					m_vecCharacter_Controller[filterData.word2]->Insert_Collider_Type(eType);
				}
				else
				{
					m_vecCharacter_Controller[filterData.word2]->SetBigAttack(false);
				}
			}

			m_fMinDist = 9999.f;
			m_bHit = false;
			m_bRagdollHit = false;

			return true;
		}

		m_fMinDist = 9999.f;
		m_bHit = false;
		m_bRagdollHit = false;
		return false;
	}

	m_fMinDist = 9999.f;
	m_bHit = false;
	m_bRagdollHit = false;
	return false;

#pragma endregion
#endif

#ifdef OLD_RAYCAST
#pragma region 구버전
	if (Status)
	{
		for (PxU32 i = 0; i < hit.getNbTouches(); ++i)
		{
			const PxRaycastHit& hit_Obj = hit.getTouch(i);

			PxShape* shape = hit_Obj.shape;
			PxRigidActor* actor = hit_Obj.actor;

			PxFilterData filterData = shape->getSimulationFilterData();

			if (filterData.word0 & COLLISION_CATEGORY::COLLIDER)
			{
				COLLIDER_TYPE eType = (COLLIDER_TYPE)(_int)filterData.word3;

				*pBlockPoint = PxVec_To_Float4_Coord(hit_Obj.position);
				
				if (m_vecCharacter_Controller[filterData.word2] && m_vecCharacter_Controller[filterData.word2]->IsReleased() == false)
				{
					auto vDelta = Float4_Normalize(*pBlockPoint - vOrigin);
					vDelta.y = 0.f;

					m_vecCharacter_Controller[filterData.word2]->Set_Hit(true);

					if (bDecalRay)
					{
						m_vecCharacter_Controller[filterData.word2]->Set_Force(vDelta, eType);
						m_vecCharacter_Controller[filterData.word2]->SetBlockPoint(*pBlockPoint);
						m_vecCharacter_Controller[filterData.word2]->SetHitNormal(PxVec_To_Float4_Coord(hit_Obj.normal));
						m_vecCharacter_Controller[filterData.word2]->Set_Hit_Decal_Ray(true);
					}

					if (bBigAttack)
					{
						m_vecCharacter_Controller[filterData.word2]->SetBigAttack(true);
						m_vecCharacter_Controller[filterData.word2]->Increase_Hit_Count_STG();
						m_vecCharacter_Controller[filterData.word2]->Insert_Hit_Point_STG(*pBlockPoint);
						m_vecCharacter_Controller[filterData.word2]->Insert_Hit_Normal_STG(PxVec_To_Float4_Coord(hit_Obj.normal));
						m_vecCharacter_Controller[filterData.word2]->Insert_Collider_Type(eType);
					}
					else
					{
						m_vecCharacter_Controller[filterData.word2]->SetBigAttack(false);
					}

					//if (eType == COLLIDER_TYPE::HEAD)
					{
							m_vecCharacter_Controller[filterData.word2]->Increase_Hit_Count();
					}

					if (bBigAttack)
					{
						if (m_vecCharacter_Controller[filterData.word2]->Get_Hit_Count() >= 15)
						{
							/*Ragdoll을 구동하려면 살려야 함.*/
							m_vecCharacter_Controller[filterData.word2]->SetReleased(true);
							m_vecCharacter_Controller[filterData.word2]->SetDead(true);
							m_vecCharacter_Controller[filterData.word2]->Set_Force(vDelta, eType);
						}
					}
					else
					{
						if (m_vecCharacter_Controller[filterData.word2]->Get_Hit_Count() >= 15)
						{
							/*Ragdoll을 구동하려면 살려야 함.*/
							m_vecCharacter_Controller[filterData.word2]->SetReleased(true);
							m_vecCharacter_Controller[filterData.word2]->SetDead(true);
							m_vecCharacter_Controller[filterData.word2]->Set_Force(vDelta, eType);
						}
					}
	

					return true;
				}

				return false;
			}

			if (filterData.word0 & COLLISION_CATEGORY::RAGDOLL)
			{
				COLLIDER_TYPE eType = (COLLIDER_TYPE)(_int)filterData.word3;

				*pBlockPoint = PxVec_To_Float4_Coord(hit_Obj.position);

				PxRigidDynamic* dynamicActor = actor->is<PxRigidDynamic>();
				if (dynamicActor)
				{
					auto vDelta = Float4_Normalize(*pBlockPoint - vOrigin);
					vDelta.y = 0.f;

					auto fPower = rand() % 100;

					vDelta = vDelta * (fPower + 50.f);

					PxVec3 pxForce = PxVec3(vDelta.x, vDelta.y, vDelta.z);
					dynamicActor->addForce(pxForce, PxForceMode::eIMPULSE);
					m_vecCharacter_Controller[filterData.word2]->Set_Hit(true);

					if (bDecalRay)
					{
						m_vecCharacter_Controller[filterData.word2]->SetBlockPoint(*pBlockPoint);
						m_vecCharacter_Controller[filterData.word2]->SetHitNormal(PxVec_To_Float4_Coord(hit_Obj.normal));
						m_vecCharacter_Controller[filterData.word2]->Set_Hit_Decal_Ray(true);
					}

					if (bBigAttack)
					{
						m_vecCharacter_Controller[filterData.word2]->SetBigAttack(true);
						m_vecCharacter_Controller[filterData.word2]->Increase_Hit_Count_STG();
						m_vecCharacter_Controller[filterData.word2]->Insert_Hit_Point_STG(*pBlockPoint);
						m_vecCharacter_Controller[filterData.word2]->Insert_Hit_Normal_STG(PxVec_To_Float4_Coord(hit_Obj.normal));
						m_vecCharacter_Controller[filterData.word2]->Insert_Collider_Type(eType);
					}
					else
					{
						m_vecCharacter_Controller[filterData.word2]->SetBigAttack(false);
					}
				}

				return true;
			}
		}

		return false;
	}
#pragma endregion
#endif

	return false;
}

_bool CPhysics_Controller::SphereCast_Shoot(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint, _float fMaxDist)
{
	PxVec3 PxvOrigin = Float4_To_PxVec(vOrigin);
	PxVec3 PxvDir = Float4_To_PxVec(vDir);

	PxSweepHit hitBuffer[256];
	PxHitBuffer<PxSweepHit> sweepBuffer(hitBuffer, 256);

	PxQueryFilterData filterData;
	filterData.flags = PxQueryFlag::eDYNAMIC;

	bool status = m_Scene->sweep(PxSphereGeometry(0.1f), PxTransform(PxvOrigin),
		PxvDir.getNormalized(),
		fMaxDist,
		sweepBuffer,
		PxHitFlag::ePRECISE_SWEEP, filterData
	);

	if (status)
	{
		for (PxU32 i = 0; i < sweepBuffer.getNbTouches(); ++i)
		{
			const PxSweepHit& hit = sweepBuffer.getTouch(i);
			PxVec3 hitPosition = hit.position;
			PxShape* shape = hit.shape;
			PxRigidActor* actor = hit.actor;

			PxFilterData filterData = shape->getSimulationFilterData();

			if (filterData.word0 & COLLISION_CATEGORY::COLLIDER)
			{
				COLLIDER_TYPE eType = (COLLIDER_TYPE)(_int)filterData.word3;

				*pBlockPoint = PxVec_To_Float4_Coord(hit.position);

				if (hit.position.x == 0 && hit.position.y == 0 && hit.position.z == 0)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			else
			{
				*pBlockPoint = _float4(0.f, 0.f, 0.f, 1.f);
			}
		}

		return false;
	}

	return false;
}

_bool CPhysics_Controller::RayCast_Decal(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint, _float4* pBlockNormal, _float fMaxDist)
{
	PxVec3 PxvOrigin = Float4_To_PxVec(vOrigin);
	PxVec3 PxvDir = Float4_To_PxVec(vDir);

	PxRaycastBuffer hit;

	bool Status = m_Scene->raycast(PxvOrigin, PxvDir.getNormalized(), fMaxDist, hit, PxHitFlag::eDEFAULT, PxQueryFilterData(PxQueryFlag::eSTATIC));

	if (Status)
	{
		PxShape* shape = hit.block.shape;
		PxRigidActor* actor = hit.block.actor;

		PxFilterData filterData = shape->getSimulationFilterData();

		if (filterData.word0 & COLLISION_CATEGORY::STATIC_MESH)
		{
			m_vecRigid_Static[filterData.word3]->Set_Hit(true);
			m_vecRigid_Static[filterData.word3]->SetBlockPoint(*pBlockPoint);
			m_vecRigid_Static[filterData.word3]->SetHitNormal(*pBlockNormal);
		}

		*pBlockPoint = PxVec_To_Float4_Coord(hit.block.position);
		*pBlockNormal = PxVec_To_Float4_Dir(hit.block.normal);
	}

	return Status;
}

_bool CPhysics_Controller::RayCast_Effect(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint, _float4* pBlockNormal,_bool bBigAttack, _bool* pDynamic, _float fMaxDist)
{
	PxVec3 PxvOrigin = Float4_To_PxVec(vOrigin);
	PxVec3 PxvDir = Float4_To_PxVec(vDir);

	PxRaycastBuffer hit;

	bool Status = m_Scene->raycast(PxvOrigin, PxvDir.getNormalized(), fMaxDist, hit, PxHitFlag::eDEFAULT, PxQueryFilterData(PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC));

	if (Status)
	{
		PxShape* shape = hit.block.shape;
		PxRigidActor* actor = hit.block.actor;

		PxFilterData filterData = shape->getSimulationFilterData();

		if (filterData.word0 & COLLISION_CATEGORY::STATIC_MESH)
		{
			if (bBigAttack == false)
			{
				*pBlockPoint = PxVec_To_Float4_Coord(hit.block.position);
				*pBlockNormal = PxVec_To_Float4_Dir(hit.block.normal);
			}
			else
			{
				m_vecBlockPoints_Props.push_back(PxVec_To_Float4_Coord(hit.block.position));
				m_vecBlockNormals_Props.push_back(PxVec_To_Float4_Dir(hit.block.normal));
			}

			if (actor->is<PxRigidDynamic>())
			{
				*pDynamic = true;
			}
			else
			{
				*pDynamic = false;
			}
		}
		else
		{
			return false;
		}
	}

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

	for (int i = 0; i < m_vecCharacter_Controller.size(); ++i)
	{
		Safe_Release(m_vecCharacter_Controller[i]);
	}

	for (int i = 0; i < m_vecRigid_Static.size(); ++i)
	{
		Safe_Release(m_vecRigid_Static[i]);
	}

	for (int i = 0; i < m_vecRigid_Dynamic.size(); ++i)
	{
		Safe_Release(m_vecRigid_Dynamic[i]);
	}

	for (int i = 0; i < m_vecCollider.size(); ++i)
	{
		Safe_Release(m_vecCollider[i]);
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

	if (m_CudaContextManager)
		m_CudaContextManager->release();

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

}
