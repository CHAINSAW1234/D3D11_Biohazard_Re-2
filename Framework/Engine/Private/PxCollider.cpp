#include "PxCollider.h"
#include "Transform.h"

PxQuat               to_quat(const _fvector& q)
{
	return PxQuat(XMVectorGetX(q), XMVectorGetY(q), XMVectorGetZ(q), XMVectorGetW(q));
}

_matrix PxTransformToXMMATRIX(const PxTransform& transform)
{

	XMVECTOR quaternion = XMVectorSet(transform.q.x, transform.q.y, transform.q.z, transform.q.w);
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quaternion);

	//XMVECTOR translation = XMVectorSet(transform.p.x, transform.p.y, transform.p.z, 1.0f);
	XMVECTOR translation = XMVectorSet(transform.p.x, transform.p.y, transform.p.z, 1.0f);
	XMMATRIX translationMatrix = XMMatrixTranslationFromVector(translation);

	XMMATRIX finalMatrix = rotationMatrix * translationMatrix;

	return finalMatrix;
}

PxTransform XMMATRIXToPxTransform(const XMMATRIX& matrix) 
{
	XMFLOAT4X4 matrixFloat4x4;
	XMStoreFloat4x4(&matrixFloat4x4, matrix);
	PxVec3 position(matrixFloat4x4._41, matrixFloat4x4._42, matrixFloat4x4._43);

	XMMATRIX rotationMatrix = matrix;
	rotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1);
	XMVECTOR quat = XMQuaternionRotationMatrix(rotationMatrix);
	PxQuat rotation;
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&rotation), quat);

	return PxTransform(position, rotation);
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
	/*_vector Rotquat =XMQuaternionRotationMatrix(XMLoadFloat4x4(Transform));

	PxQuat pxQuat = to_quat(Rotquat);

	for (int i = 0; i < m_vecCollider.size(); ++i)
	{
		m_vecCollider[i]->setGlobalPose(PxTransform(PxVec3(Transform->_41, Transform->_42, Transform->_43), pxQuat));
	}*/

	//_vector Rotquat = XMQuaternionRotationMatrix(XMLoadFloat4x4(Transform));

	//PxQuat pxQuat = to_quat(Rotquat);
	auto Temp = Transform;
	Temp->_41 = 0.f;
	Temp->_42 = 0.f;
	Temp->_43 = 0.f;
	_matrix Mat = XMLoadFloat4x4(Temp);

	for (int i = 0; i < m_vecCollider.size(); ++i)
	{
		_matrix Transform = PxTransformToXMMATRIX(m_vecCollider_Transform[i]);

		Transform = Mat * Transform ;

		m_vecCollider[i]->setGlobalPose(XMMATRIXToPxTransform(Transform));
	}
}

void CPxCollider::Free()
{
	for (int i = 0; i < m_vecCollider.size(); ++i)
	{
		m_vecCollider[i]->release();
	}
}
