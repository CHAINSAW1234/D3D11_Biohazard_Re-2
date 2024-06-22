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

static PxPBDParticleSystem* gParticleSystem = NULL;
static PxParticleClothBuffer* gClothBuffer = NULL;
static bool							gIsRunning = true;
PxRigidDynamic* sphere;
static _float				SimTime = 0.f;

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
	m_Dispatcher = physx::PxDefaultCpuDispatcherCreate(4);
	sceneDesc.cpuDispatcher = m_Dispatcher;
	sceneDesc.filterShader = MegamotionFilterShader;

#pragma region GPU RigidBody
	//PxCudaContextManagerDesc cudaContextManagerDesc;
	//m_CudaContextManager = PxCreateCudaContextManager(*m_Foundation, cudaContextManagerDesc, PxGetProfilerCallback());

	//sceneDesc.cudaContextManager = m_CudaContextManager;

	//sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	//sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
#pragma endregion

#pragma region GPU 가속 설정 - SoftBody
	//sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;

	//sceneDesc.sceneQueryUpdateMode = PxSceneQueryUpdateMode::eBUILD_ENABLED_COMMIT_DISABLED;
	//sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	//sceneDesc.gpuMaxNumPartitions = 8;

	//PxCudaContextManagerDesc cudaContextManagerDesc;
	//m_CudaContextManager = PxCreateCudaContextManager(*m_Foundation, cudaContextManagerDesc);
	//if (!m_CudaContextManager->contextIsValid())
	//{
	//	m_CudaContextManager->release();
	//	m_CudaContextManager = NULL;
	//}

	//sceneDesc.solverType = PxSolverType::ePGS;
	//sceneDesc.cudaContextManager = m_CudaContextManager;
#pragma endregion

#pragma region GPU 가속 설정 - Cloth
	//if (PxGetSuggestedCudaDeviceOrdinal(m_Foundation->getErrorCallback()) >= 0)
	//{
	//	// initialize CUDA
	//	PxCudaContextManagerDesc cudaContextManagerDesc;
	//	m_CudaContextManager = PxCreateCudaContextManager(*m_Foundation, cudaContextManagerDesc, PxGetProfilerCallback());
	//	if (m_CudaContextManager && !m_CudaContextManager->contextIsValid())
	//	{
	//		m_CudaContextManager->release();
	//		m_CudaContextManager = NULL;
	//	}
	//}
	//if (m_CudaContextManager == NULL)
	//{
	//	PxGetFoundation().error(PxErrorCode::eINVALID_OPERATION, PX_FL, "Failed to initialize CUDA!\n");
	//}

	//sceneDesc.cudaContextManager = m_CudaContextManager;
	//sceneDesc.staticStructure = PxPruningStructureType::eDYNAMIC_AABB_TREE;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	//sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	//sceneDesc.solverType = PxSolverType::eTGS;
#pragma endregion

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


#pragma region SoftBody 생성 임시 코드
	/*PxCookingParams cookingParams(m_Physics->getTolerancesScale());
	cookingParams.meshWeldTolerance = 0.001f;
	cookingParams.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
	cookingParams.buildTriangleAdjacencies = false;
	cookingParams.buildGPUData = true;

	PxArray<PxVec3> triVerts;
	PxArray<PxU32> triIndices;
	PxReal maxEdgeLength = 1;

	createCube(triVerts, triIndices, PxVec3(0, 9.5, 0), 2.5);
	PxRemeshingExt::limitMaxEdgeLength(triIndices, triVerts, maxEdgeLength);
	PxSoftBody* softBodyCube = Create_SoftBody_Debug(cookingParams, triVerts, triIndices);

	createSphere(triVerts, triIndices, PxVec3(0, 4.5, 0), 2.5, maxEdgeLength);
	PxSoftBody* softBodySphere = Create_SoftBody_Debug(cookingParams, triVerts, triIndices);

	PxReal halfExtent = 1;
	PxVec3 cubePosA(0, 7.25, 0);
	PxVec3 cubePosB(0, 11.75, 0);
	PxRigidDynamic* rigidCubeA = Create_RigidCube(halfExtent, cubePosA);
	PxRigidDynamic* rigidCubeB = Create_RigidCube(halfExtent, cubePosB);

	connectCubeToSoftBody(rigidCubeA, 2 * halfExtent, cubePosA, softBodySphere);
	connectCubeToSoftBody(rigidCubeA, 2 * halfExtent, cubePosA, softBodyCube);*/
#pragma endregion

#pragma region Cloth 설정 임시 코드
	//// Setup Cloth
	//const PxReal totalClothMass = 10.0f;

	//PxU32 numPointsX = 250;
	//PxU32 numPointsZ = 250;
	//PxReal particleSpacing = 0.05f;
	//initCloth(numPointsX, numPointsZ, PxVec3(-0.5f * numPointsX * particleSpacing, 8.f, -0.5f * numPointsZ * particleSpacing), particleSpacing, totalClothMass);

	//initObstacles();


	//// Setup rigid bodies
	//const PxReal boxSize = 1.0f;
	//const PxReal boxMass = 1.0f;
	//PxShape* shape = m_Physics->createShape(PxBoxGeometry(0.5f * boxSize, 0.5f * boxSize, 0.5f * boxSize), *m_Material);
	//for (int i = 0; i < 500; ++i)
	//{
	//	PxRigidDynamic* body = m_Physics->createRigidDynamic(PxTransform(PxVec3(i - 3.0f, 10, 4.0f)));
	//	body->attachShape(*shape);
	//	PxRigidBodyExt::updateMassAndInertia(*body, boxMass);
	//	m_Scene->addActor(*body);
	//}
	//shape->release();


	//m_Scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	//m_Scene->setVisualizationParameter(PxVisualizationParameter::eSIMULATION_MESH, 2.0f);
#pragma endregion

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

	//const PxReal dt = fTimeDelta;

	//bool rotatingSphere = true;
	//if (rotatingSphere)
	//{
	//	const PxReal speed = 2.0f;
	//	PxTransform pose = sphere->getGlobalPose();
	//	sphere->setKinematicTarget(PxTransform(pose.p, PxQuat(PxCos(SimTime * speed), PxVec3(0, 1, 0))));
	//}

	//SimTime += fTimeDelta;

	//Simulate
	m_Scene->simulate(fTimeDelta);
	m_Scene->fetchResults(true);
	/*m_Scene->fetchResultsParticleSystem();

	for (PxU32 i = 0; i < m_vecSoftBodies.size(); i++)
	{
		SoftBody* sb = m_vecSoftBodies[i];
		sb->copyDeformedVerticesFromGPU();
	}*/
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

	Actor->attachShape(*Shape);
	Shape->release();

	m_Scene->addActor(*Actor);

	m_vecFullMapObject.push_back(Actor);
	++m_iMapMeshCount;

	Mesh->release();

	//Start Simulate
	//	m_pGameInstance->SetSimulate(true);
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

	for (int i = 0; i < VertexNum; ++i)
	{
		_vector VertexPos = XMLoadFloat3(&pVertices[i]);
		VertexPos = XMVector3TransformCoord(VertexPos, RotationMatrix);

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

	for (int i = 0; i < VertexNum; ++i)
	{
		_vector VertexPos = XMLoadFloat3(&pVertices[i]);
		VertexPos = XMVector3TransformCoord(VertexPos, RotationMatrix);

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
	body->attachShape(*shape);
	m_Scene->addActor(*body);
	shape->release();
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	convexMesh->release();
	material->release();

	pColliders->push_back(body);
	pTransforms->push_back(transform);
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

	for (int i = 0; i < VertexNum; ++i)
	{
		_vector VertexPos = XMLoadFloat3(&pVertices[i]);
		VertexPos = XMVector3TransformCoord(VertexPos, RotationMatrix);

		_float4 vPos;
		XMStoreFloat4(&vPos, VertexPos);
		vPos += vDelta;

		PxVec3 Ver = PxVec3(vPos.x, vPos.y, vPos.z);
		vertices.push_back(Ver);
	}

	for (int i = 0; i < IndexNum; ++i)
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

	for (int i = 0; i < VertexNum; ++i)
	{
		_vector VertexPos = XMLoadFloat3(&pVertices[i]);
		VertexPos = XMVector3TransformCoord(VertexPos, RotationMatrix);

		_float4 vPos;
		XMStoreFloat4(&vPos, VertexPos);
		vPos += vDelta;

		PxVec3 Ver = PxVec3(vPos.x, vPos.y, vPos.z);
		vertices.push_back(Ver);
	}

	for (int i = 0; i < IndexNum; ++i)
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

CRagdoll_Physics* CPhysics_Controller::Create_Ragdoll(vector<class CBone*>* vecBone, CTransform* pTransform, const string& name)
{
	//Ragdoll Init
	auto pRagdoll = new CRagdoll_Physics(m_Scene, m_Physics, m_DefaultAllocatorCallback, m_DefaultErrorCallback, m_Foundation,
		m_Dispatcher, m_Material);

	pRagdoll->SetBone_Ragdoll(vecBone);
	pRagdoll->SetTransform(pTransform);
	pRagdoll->Init(name);

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

		pRagdoll->Init_Ragdoll();
		pRagdoll->SetSimulate(true);
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
	const PxU32 numParticles = numX * numZ;
	const PxU32 numSprings = (numX - 1) * (numZ - 1) * 4 + (numX - 1) + (numZ - 1);
	const PxU32 numTriangles = (numX - 1) * (numZ - 1) * 2;

	const PxReal restOffset = particleSpacing;

	const PxReal stretchStiffness = 10000.f;
	const PxReal shearStiffness = 100.f;
	const PxReal springDamping = 0.001f;

	// Material setup
	PxPBDMaterial* defaultMat = m_Physics->createPBDMaterial(0.8f, 0.05f, 1e+6f, 0.001f, 0.5f, 0.005f, 0.05f, 0.f, 0.f);

	PxPBDParticleSystem* particleSystem = m_Physics->createPBDParticleSystem(*m_CudaContextManager);
	gParticleSystem = particleSystem;

	// General particle system setting

	const PxReal particleMass = totalClothMass / numParticles;
	particleSystem->setRestOffset(restOffset);
	particleSystem->setContactOffset(restOffset + 0.02f);
	particleSystem->setParticleContactOffset(restOffset + 0.02f);
	particleSystem->setSolidRestOffset(restOffset);
	particleSystem->setFluidRestOffset(0.0f);

	m_Scene->addActor(*particleSystem);

	// Create particles and add them to the particle system
	const PxU32 particlePhase = particleSystem->createPhase(defaultMat, PxParticlePhaseFlags(PxParticlePhaseFlag::eParticlePhaseSelfCollideFilter | PxParticlePhaseFlag::eParticlePhaseSelfCollide));

	ExtGpu::PxParticleClothBufferHelper* clothBuffers = ExtGpu::PxCreateParticleClothBufferHelper(1, numTriangles, numSprings, numParticles, m_CudaContextManager);

	PxU32* phase = m_CudaContextManager->allocPinnedHostBuffer<PxU32>(numParticles);
	PxVec4* positionInvMass = m_CudaContextManager->allocPinnedHostBuffer<PxVec4>(numParticles);
	PxVec4* velocity = m_CudaContextManager->allocPinnedHostBuffer<PxVec4>(numParticles);

	PxReal x = position.x;
	PxReal y = position.y;
	PxReal z = position.z;

	// Define springs and triangles
	PxArray<PxParticleSpring> springs;
	springs.reserve(numSprings);
	PxArray<PxU32> triangles;
	triangles.reserve(numTriangles * 3);

	for (PxU32 i = 0; i < numX; ++i)
	{
		for (PxU32 j = 0; j < numZ; ++j)
		{
			const PxU32 index = i * numZ + j;

			PxVec4 pos(x, y, z, 1.0f / particleMass);
			phase[index] = particlePhase;
			positionInvMass[index] = pos;
			velocity[index] = PxVec4(0.0f);

			if (i > 0)
			{
				PxParticleSpring spring = { id(i - 1, j, numZ), id(i, j, numZ), particleSpacing, stretchStiffness, springDamping, 0 };
				springs.pushBack(spring);
			}
			if (j > 0)
			{
				PxParticleSpring spring = { id(i, j - 1, numZ), id(i, j, numZ), particleSpacing, stretchStiffness, springDamping, 0 };
				springs.pushBack(spring);
			}

			if (i > 0 && j > 0)
			{
				PxParticleSpring spring0 = { id(i - 1, j - 1, numZ), id(i, j, numZ), PxSqrt(2.0f) * particleSpacing, shearStiffness, springDamping, 0 };
				springs.pushBack(spring0);
				PxParticleSpring spring1 = { id(i - 1, j, numZ), id(i, j - 1, numZ), PxSqrt(2.0f) * particleSpacing, shearStiffness, springDamping, 0 };
				springs.pushBack(spring1);

				//Triangles are used to compute approximated aerodynamic forces for cloth falling down
				triangles.pushBack(id(i - 1, j - 1, numZ));
				triangles.pushBack(id(i - 1, j, numZ));
				triangles.pushBack(id(i, j - 1, numZ));

				triangles.pushBack(id(i - 1, j, numZ));
				triangles.pushBack(id(i, j - 1, numZ));
				triangles.pushBack(id(i, j, numZ));
			}

			z += particleSpacing;
		}
		z = position.z;
		x += particleSpacing;
	}

	PX_ASSERT(numSprings == springs.size());
	PX_ASSERT(numTriangles == triangles.size() / 3);

	clothBuffers->addCloth(0.0f, 0.0f, 0.0f, triangles.begin(), numTriangles, springs.begin(), numSprings, positionInvMass, numParticles);

	ExtGpu::PxParticleBufferDesc bufferDesc;
	bufferDesc.maxParticles = numParticles;
	bufferDesc.numActiveParticles = numParticles;
	bufferDesc.positions = positionInvMass;
	bufferDesc.velocities = velocity;
	bufferDesc.phases = phase;

	const PxParticleClothDesc& clothDesc = clothBuffers->getParticleClothDesc();
	PxParticleClothPreProcessor* clothPreProcessor = PxCreateParticleClothPreProcessor(m_CudaContextManager);

	PxPartitionedParticleCloth output;
	clothPreProcessor->partitionSprings(clothDesc, output);
	clothPreProcessor->release();

	gClothBuffer = physx::ExtGpu::PxCreateAndPopulateParticleClothBuffer(bufferDesc, clothDesc, output, m_CudaContextManager);
	gParticleSystem->addParticleBuffer(gClothBuffer);

	clothBuffers->release();

	m_CudaContextManager->freePinnedHostBuffer(positionInvMass);
	m_CudaContextManager->freePinnedHostBuffer(velocity);
	m_CudaContextManager->freePinnedHostBuffer(phase);
}

void CPhysics_Controller::initObstacles()
{
	PxShape* shape = m_Physics->createShape(PxSphereGeometry(3.0f), *m_Material);
	sphere = m_Physics->createRigidDynamic(PxTransform(PxVec3(0.f, 5.0f, 0.f)));
	sphere->attachShape(*shape);
	sphere->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	m_Scene->addActor(*sphere);
	shape->release();
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

_bool CPhysics_Controller::RayCast_Shoot(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint, _float fMaxDist)
{
	PxVec3 PxvOrigin = Float4_To_PxVec(vOrigin);
	PxVec3 PxvDir = Float4_To_PxVec(vDir);

	const PxU32 maxHits = 10;
	PxRaycastHit hitBuffer[maxHits];
	PxRaycastBufferN<maxHits> hit;

	PxQueryFilterData filterData;
	filterData.flags = PxQueryFlag::eDYNAMIC;

	bool Status = m_Scene->raycast(PxvOrigin, PxvDir.getNormalized(), 50.f, hit, PxHitFlag::eDEFAULT, filterData);

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
					m_vecCharacter_Controller[filterData.word2]->Set_Force(vDelta, eType);
					m_vecCharacter_Controller[filterData.word2]->SetReleased(true);

					return true;
				}

				return false;
			}
		}

		return false;
	}

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
