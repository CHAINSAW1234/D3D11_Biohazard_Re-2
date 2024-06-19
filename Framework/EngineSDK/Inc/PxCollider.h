#pragma once

#include "Physics_Component.h"

BEGIN(Engine)

class ENGINE_DLL CPxCollider : public CPhysics_Component
{
public:
	CPxCollider();
	CPxCollider(const CPxCollider& rhs);
	virtual ~CPxCollider() = default;

public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize(void* pArg);
public:
	static CPxCollider*					Create();

public:
	vector<PxRigidDynamic*>*			GetCollider_Container()
	{
		return &m_vecCollider;
	}
	void								Update_Transform(_float4x4* Transform);
private:
	vector<PxRigidDynamic*>				m_vecCollider;
	_int								m_iColliderCount = { 0 };
public:
	virtual void Free() override;
};

END