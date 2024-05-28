#pragma once

#include "Physics_Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigid_Dynamic : public CPhysics_Component
{
public:
	CRigid_Dynamic(PxRigidDynamic* RigidDynamic);
	CRigid_Dynamic(const CRigid_Dynamic& rhs);
	virtual ~CRigid_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
public:
	virtual void SetPosition(PxVec3 Pos) override;
	virtual PxVec3 GetPosition() override;
	virtual void Update() override;

	virtual _float4 GetTranslation() override;
	virtual PxTransform	GetTransform_Px() override;
public:
	static CRigid_Dynamic* Create();

protected:
	_matrix			GetWorldMatrix_Rigid_Dynamic(_int Index);
private:

public:
	virtual void Free() override;
};

END