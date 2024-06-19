#pragma once

#include "Physics_Component.h"

BEGIN(Engine)

class ENGINE_DLL PxCollider : public CPhysics_Component
{
public:
	PxCollider();
	PxCollider(const PxCollider& rhs);
	virtual ~PxCollider() = default;

public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize(void* pArg);
public:
	static PxCollider*					Create();

public:
	vector<PxRigidDynamic*>*			GetCollider_Container()
	{
		return &m_vecCollider;
	}
private:
	vector<PxRigidDynamic*>				m_vecCollider;
	_int								m_iColliderCount = { 0 };
public:
	virtual void Free() override;
};

END