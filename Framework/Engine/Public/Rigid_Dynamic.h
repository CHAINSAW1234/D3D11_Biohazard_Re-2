#pragma once

#include "Physics_Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigid_Dynamic : public CPhysics_Component
{
public:
	CRigid_Dynamic();
	CRigid_Dynamic(const CRigid_Dynamic& rhs);
	virtual ~CRigid_Dynamic() = default;

public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize(void* pArg);
public:
	virtual void						SetPosition(PxVec3 Pos) override;
	virtual void						SetPosition(_float4 vPos) override;
	virtual PxVec3						GetPosition() override;
	virtual void						Update() override;

	virtual _float4						GetTranslation() override;
	virtual PxTransform					GetTransform_Px() override;
	void								SetKinematic(_bool boolean);

	void								AddForce(_float4 vDir);
	void								ClearForce();
public:
	static CRigid_Dynamic*				Create();

public:
	_matrix								GetWorldMatrix_Rigid_Dynamic(_float3 vScale);

public:
	virtual void Free() override;
};

END