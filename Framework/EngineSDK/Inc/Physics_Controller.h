
#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPhysics_Controller : public CBase
{
protected:
	CPhysics_Controller();
	CPhysics_Controller(const CPhysics_Controller& rhs);
	virtual ~CPhysics_Controller() = default;
public:
	virtual HRESULT										Initialize_Prototype();
	virtual HRESULT										Initialize(void* pArg);
	virtual void										Simulate(_float fTimeDelta);
	void												Create_Plane(_float4 Pos);
public:
	static CPhysics_Controller*							Create();
protected:
	class CGameInstance*								m_pGameInstance = { nullptr };
public:
	virtual void Free() override;
private:
	//Init
	PxDefaultAllocator									m_DefaultAllocatorCallback;
	PxDefaultErrorCallback								m_DefaultErrorCallback;
	PxDefaultCpuDispatcher*								m_Dispatcher = nullptr;
	PxTolerancesScale									m_ToleranceScale;
	PxFoundation*										m_Foundation = nullptr;
	PxPhysics*											m_Physics = nullptr;
	PxScene*											m_Scene = nullptr;
	PxMaterial*											m_Material = nullptr;
	PxPvd*												m_Pvd = nullptr;
	class CEventCallBack*								m_EventCallBack = { nullptr };
	class CFilterCallBack*								m_FilterCallBack = { nullptr };
	PxControllerManager*								m_Manager = { nullptr };
	PxPvdSceneClient*									m_PvdClient = { nullptr };
	PxPvdTransport*										m_Transport = { nullptr };
	PxRigidStatic*										m_GroundPlane = { nullptr };
	PxCapsuleControllerDesc								m_Controll_Desc;
	PxShape*											m_Shape = { nullptr };

#pragma region Physics_Component 
public:
	class CCharacter_Controller*						Create_Controller(_float4 Pos, _int* Index,class CGameObject* pCharacter,_float fHeight,_float fRadius, class CTransform* pTransform, vector<class CBone*>* pBones, const std::string& name = "");
	void												Create_Rigid_Dynamic(_float4 Pos);
	void												Create_Rigid_Static(_float4 Pos);
	class PxCollider*									Create_Px_Collider(class CModel* pModel,class CTransform* pTransform,_int* iId);
	class CCharacter_Controller*						GetCharacter_Controller(_int Index);
	class CRigid_Dynamic*								GetRigid_Dynamic(_int Index);
	_float4												GetTranslation_Rigid_Dynamic(_int Index);
	_matrix												GetWorldMatrix_Rigid_Dynamic(_int Index);
private:
	//Physics_Component
	vector<class CCharacter_Controller*>				m_vecCharacter_Controller;
	vector<class CRigid_Dynamic*>						m_vecRigid_Dynamic;
	_int												m_iCharacter_Controller_Count = { 0 };
	_int												m_iRigid_Dynamic_Count = { 0 };
	_int												m_iRigid_Static_Count = { 0 };
	//Map Physics_Component
	_int												m_iMapMeshCount = { 0 };
	vector<PxRigidStatic*>								m_vecFullMapObject;
	vector<class PxCollider*>							m_vecCollider;
	_int												m_iCollider_Count = { 0 };
#pragma endregion

#pragma region Ray Cast
public:
	_bool												RayCast(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint, _float fMaxDist = 1000.f);
	_bool												RayCast_Shoot(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint, _float fMaxDist = 1000.f);
	_bool												SphereCast_Shoot(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint, _float fMaxDist = 1000.f);
#pragma endregion

#pragma region Sphere Cast
public:
	_bool												SphereCast(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint, _float fMaxDist = 1000.f);
#pragma endregion

#pragma region For Mesh Cooking
public://For Mesh Cooking
	void												Cook_Mesh(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum,class CTransform* pTransform = nullptr);
	void												Cook_Mesh_Dynamic(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum,class CTransform* pTransform = nullptr);
	void												Cook_Mesh_Convex(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum,vector<PxRigidDynamic*>* pColliders,class CTransform* pTransform = nullptr);
public://For Terrain Cooking
	void												InitTerrain();
private:
	//For Cooking
	_float3*											m_pVerticesPos = { nullptr };
	_uint*												m_pIndices = { nullptr };
	PxTriangleMesh*										m_TerrainMesh = { nullptr };
	PxRigidStatic*										m_TerrainActor = { nullptr };
#pragma endregion

#pragma region Ragdoll
public:
	class CRagdoll_Physics*								Create_Ragdoll(vector<class CBone*>* vecBone,class CTransform* pTransform, const string& name);
	void												Start_Ragdoll(class CRagdoll_Physics* pRagdoll,_uint iId);
	void												SetBone_Ragdoll(vector<class CBone*>* vecBone);
	void												SetWorldMatrix_Ragdoll(_float4x4 WorldMatrix);
	void												SetRotationMatrix_Ragdoll(_float4x4 WorldMatrix);
private://RagDoll
	_bool												m_bRagdoll = { false };
	vector<class CRagdoll_Physics*>						m_vecRagdoll = { nullptr };
#pragma endregion

#pragma region 편의성 함수
public:
	FORCEINLINE PxVec3									Vector_To_PxVec(_vector vSrc)
	{
		PxVec3 PxvDst;
		PxvDst.x = XMVectorGetX(vSrc);
		PxvDst.y = XMVectorGetY(vSrc);
		PxvDst.z = XMVectorGetZ(vSrc);

		return PxvDst;
	}
	FORCEINLINE PxVec3									Float4_To_PxVec(_float4 vSrc)
	{
		PxVec3 PxvDst;
		PxvDst.x = vSrc.x;
		PxvDst.y = vSrc.y;
		PxvDst.z = vSrc.z;

		return PxvDst;
	}
	FORCEINLINE PxVec3									Float3_To_PxVec(_float3 vSrc)
	{
		PxVec3 PxvDst;
		PxvDst.x = vSrc.x;
		PxvDst.y = vSrc.y;
		PxvDst.z = vSrc.z;

		return PxvDst;
	}
	FORCEINLINE _vector									PxVec_To_Vector_Coord(PxVec3 PxvSrc)
	{
		_vector vDst = XMVectorSet(PxvSrc.x, PxvSrc.y, PxvSrc.z, 1.f);

		return vDst;
	}
	FORCEINLINE _vector									PxVec_To_Vector_Dir(PxVec3 PxvSrc)
	{
		_vector vDst = XMVectorSet(PxvSrc.x, PxvSrc.y, PxvSrc.z, 0.f);

		return vDst;
	}
	FORCEINLINE _float4									PxVec_To_Float4_Dir(PxVec3 PxvSrc)
	{
		_float4 vDst;
		vDst.x = PxvSrc.x;
		vDst.y = PxvSrc.y;
		vDst.z = PxvSrc.z;
		vDst.w = 0.f;

		return vDst;
	}
	FORCEINLINE _float4									PxVec_To_Float4_Coord(PxVec3 PxvSrc)
	{
		_float4 vDst;
		vDst.x = PxvSrc.x;
		vDst.y = PxvSrc.y;
		vDst.z = PxvSrc.z;
		vDst.w = 1.f;

		return vDst;
	}
	FORCEINLINE _float3									PxVec_To_Float3(PxVec3 PxvSrc)
	{
		_float3 vDst;
		vDst.x = PxvSrc.x;
		vDst.y = PxvSrc.y;
		vDst.z = PxvSrc.z;

		return vDst;
	}
#pragma endregion
};

END