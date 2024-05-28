
#pragma once

#include "Base.h"
#include "Event_Call_Back.h"

BEGIN(Engine)

class ENGINE_DLL CPhysics_Controller : public CBase
{
protected:
	CPhysics_Controller();
	CPhysics_Controller(const CPhysics_Controller& rhs);
	virtual ~CPhysics_Controller() = default;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Simulate(_float fTimeDelta);
	void			Create_Plane(_float4 Pos);
public:
	static CPhysics_Controller* Create();
protected:
	class CGameInstance* m_pGameInstance = { nullptr };
public:
	virtual void Free() override;
private:
	//Init
	PxDefaultAllocator						m_DefaultAllocatorCallback;
	PxDefaultErrorCallback					m_DefaultErrorCallback;
	PxDefaultCpuDispatcher*					m_Dispatcher = nullptr;
	PxTolerancesScale						m_ToleranceScale;
	PxFoundation*							m_Foundation = nullptr;
	PxPhysics*								m_Physics = nullptr;
	PxScene*								m_Scene = nullptr;
	PxMaterial*								m_Material = nullptr;
	PxPvd*									m_Pvd = nullptr;
	class CEventCallBack*					m_EventCallBack = { nullptr };
	class CFilterCallBack*					m_FilterCallBack = { nullptr };
	PxControllerManager*					m_Manager = { nullptr };
	PxPvdSceneClient*						m_PvdClient = { nullptr };
	PxPvdTransport*							m_Transport = { nullptr };
	PxRigidStatic*							m_GroundPlane = { nullptr };
	PxCapsuleControllerDesc					m_Controll_Desc;
	PxShape*								m_Shape = { nullptr };

#pragma region Physics_Component 
public:
	class CCharacter_Controller*			Create_Controller(_float4 Pos, _int* Index,class CGameObject* pCharacter);
	void									Create_Rigid_Dynamic(_float4 Pos);
	void									Create_Rigid_Static(_float4 Pos);
private:
	//Physics_Component
	vector<class CCharacter_Controller*>	m_vecCharacter_Controller;
	vector<class CRigid_Dynamic*>			m_vecRigid_Dynamic;
	_int									m_iCharacter_Controller_Count = { 0 };
	_int									m_iRigid_Dynamic_Count = { 0 };
	_int									m_iRigid_Static_Count = { 0 };
	//Map Physics_Component
	_int									m_iMapMeshCount = { 0 };
	vector<PxRigidStatic*>					m_vecFullMapObject;
#pragma endregion


#pragma region For Mesh Cooking
public://For Mesh Cooking
	void									Cook_Mesh(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum);
public://For Terrain Cooking
	void									InitTerrain();
private:
	//For Cooking
	_float3*								m_pVerticesPos = { nullptr };
	_uint*									m_pIndices = { nullptr };
	PxTriangleMesh*							m_TerrainMesh = { nullptr };
	PxRigidStatic*							m_TerrainActor = { nullptr };
#pragma endregion

#pragma region Ragdoll
public://RagDoll
	void			SetBone_Ragdoll(vector<class CBone*>* vecBone);
	void			SetWorldMatrix(_float4x4 WorldMatrix);
	void			SetRotationMatrix(_float4x4 WorldMatrix);
public://RagDoll
	_bool	 m_bRagdoll = { false };
	void			SetBone_Ragdoll(vector<class CBone*>* vecBone);
	void			SetWorldMatrix(_float4x4 WorldMatrix);
	void			SetRotationMatrix(_float4x4 WorldMatrix);
	class CRagdoll_Physics* m_pRagdoll_Physics = { nullptr };
#pragma endregion
};

END