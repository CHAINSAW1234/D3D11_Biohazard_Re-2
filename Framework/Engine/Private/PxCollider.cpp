#include "PxCollider.h"
#include "Transform.h"

PxQuat               to_quat(const _fvector& q)
{
	return PxQuat(XMVectorGetX(q), XMVectorGetY(q), XMVectorGetZ(q), XMVectorGetW(q));
}

PxCollider::PxCollider()
{
}

PxCollider::PxCollider(const PxCollider& rhs)
{
}

HRESULT PxCollider::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT PxCollider::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

PxCollider* PxCollider::Create()
{
	return nullptr;
}

void PxCollider::Free()
{
	for (int i = 0; i < m_vecCollider.size(); ++i)
	{
		m_vecCollider[i]->release();
	}
}
