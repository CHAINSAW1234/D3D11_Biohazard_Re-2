#pragma once
#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPhysics_Controller abstract : public CBase
{
protected:
	CPhysics_Controller();
	CPhysics_Controller(const CPhysics_Controller& rhs);
	virtual ~CPhysics_Controller() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Simulate();

protected:
	class CGameInstance* m_pGameInstance = { nullptr };
	static CPhysics_Controller* Create();

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
	PxSimulationEventCallback* m_EventCallBack = nullptr;
public:
	virtual CPhysics_Controller* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END