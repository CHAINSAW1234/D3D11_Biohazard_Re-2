#pragma once

#include "Physics_Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigid_Static : public CPhysics_Component
{
public:
	CRigid_Static();
	CRigid_Static(const CRigid_Static& rhs);
	virtual ~CRigid_Static() = default;

public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize(void* pArg);
public:
	void								Set_Hit(_bool boolean)
	{
		m_bHit = boolean;
	}
	_bool								Is_Hit()
	{
		return m_bHit;
	}
public:
	static CRigid_Static*				Create();

private:
	_bool		m_bHit = { false };

public:
	virtual void Free() override;
};

END