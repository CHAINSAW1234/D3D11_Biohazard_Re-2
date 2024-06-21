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
	vector<PxTransform>*				GetCollider_Transform_Container()
	{
		return &m_vecCollider_Transform;
	}
	void								Update_Transform(_float4x4* Transform);
	void								Update_Transform_Divided(_float4x4* Transform, _int iIndex);
	void								Update_Transform_Divided_NotRoot(class CBone* pRoot,class CBone* pTarget,class CTransform* pTransform,_int iIndex);
	void								Update_Transform_Divided_Double_Door_L(_float4x4* Transform);
	void								Update_Transform_Divided_Double_Door_R(_float4x4* Transform);
private:
	vector<PxRigidDynamic*>				m_vecCollider;
	vector<PxTransform>					m_vecCollider_Transform;
	_int								m_iColliderCount = { 0 };
public:
	virtual void Free() override;
};

END