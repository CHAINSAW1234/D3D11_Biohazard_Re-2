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

	void			Create_Controller(_float4 Pos);
	void			Create_Rigid_Dynamic(_float4 Pos);
	void			Create_Rigid_Static(_float4 Pos);
	void			Create_Plane(_float4 Pos);

	class CCharacter_Controller* GetCharacter_Controller(_int Index);
	class CRigid_Dynamic*		 GetRigid_Dynamic(_int Index);
	_float4						 GetTranslation_Rigid_Dynamic(_int Index);
public:
	_bool			IsGrounded(PxController* Controller);
	_float4			GetPosition();

	void			InitTerrain();

	_matrix						 GetWorldMatrix_Rigid_Dynamic(_int Index);
	void			Cook_Mesh(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum);
	void			Move_CCT(_float4 Dir, _float fTimeDelta,_int Index);
public:
	static CPhysics_Controller* Create();

protected:
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	PxDefaultAllocator m_DefaultAllocatorCallback;
	PxDefaultErrorCallback m_DefaultErrorCallback;
	PxDefaultCpuDispatcher* m_Dispatcher = nullptr;
	PxTolerancesScale		m_ToleranceScale;
	PxFoundation* m_Foundation = nullptr;
	PxPhysics* m_Physics = nullptr;
	PxScene* m_Scene = nullptr;
	PxMaterial* m_Material = nullptr;
	PxPvd* m_Pvd = nullptr;
	class CEventCallBack* m_EventCallBack = { nullptr };
	class CFilterCallBack* m_FilterCallBack = { nullptr };
	vector<PxRigidDynamic*> m_vecBody;
	PxRigidDynamic* m_BodyCollider = { nullptr };
	PxControllerManager* m_Manager = { nullptr };
	PxController* m_Controller = { nullptr };

	PxVec3 m_Look;
	PxVec3 m_JumpVel;

	_float3* m_pVerticesPos = { nullptr };
	_uint* m_pIndices = { nullptr };

	PxTriangleMesh* m_TerrainMesh = { nullptr };
	PxRigidStatic* m_GroundPlane = { nullptr };
	PxRigidStatic* m_TerrainActor = { nullptr };


	PxPvdSceneClient* m_PvdClient = { nullptr };
	PxPvdTransport* m_Transport = { nullptr };

	PxCapsuleControllerDesc m_Controll_Desc;

	PxShape* m_Shape = { nullptr };

	vector<class CCharacter_Controller*> m_vecCharacter_Controller;
	vector<class CRigid_Dynamic*> m_vecRigid_Dynamic;


	_int						m_iCharacter_Controller_Count = { 0 };
	_int						m_iRigid_Dynamic_Count = { 0 };
	_int						m_iRigid_Static_Count = { 0 };

	_int						m_iMapMeshCount = { 0 };
	vector<PxRigidStatic*>			m_vecFullMapObject;

private:
	_bool			m_bJump = { false };
public:
	virtual void Free() override;
};

END