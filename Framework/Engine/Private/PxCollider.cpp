#include "PxCollider.h"
#include "Transform.h"

PxQuat               to_quat(const _fvector& q)
{
	return PxQuat(XMVectorGetX(q), XMVectorGetY(q), XMVectorGetZ(q), XMVectorGetW(q));
}

CPxCollider::CPxCollider()
{
}

CPxCollider::CPxCollider(const CPxCollider& rhs)
{
}

HRESULT CPxCollider::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CPxCollider::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

CPxCollider* CPxCollider::Create()
{
	return nullptr;
}

void CPxCollider::Update_Transform(_float4x4* Transform)
{
	_vector Rotquat =XMQuaternionRotationMatrix(XMLoadFloat4x4(Transform));

	PxQuat pxQuat = to_quat(Rotquat);

	for (int i = 0; i < m_vecCollider.size(); ++i)
	{
		m_vecCollider[i]->setGlobalPose(PxTransform(PxVec3(Transform->_41, Transform->_42, Transform->_43), pxQuat));
	}
}

void CPxCollider::Free()
{
	for (int i = 0; i < m_vecCollider.size(); ++i)
	{
		m_vecCollider[i]->release();
	}
}
