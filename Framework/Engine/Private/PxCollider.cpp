#include "PxCollider.h"
#include "Transform.h"
#include "Bone.h"

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

		Transform = Mat * Transform;

		m_vecCollider[i]->setGlobalPose(XMMATRIXToPxTransform(Transform));
	}
}

void CPxCollider::Update_Transform_Divided_NotRoot(CBone* pRoot, CBone* pTarget,CTransform* pTransform, _int iIndex)
{
	auto Temp = pTarget->Get_TrasformationMatrix();
	_float4x4 TargetTransform;
	XMStoreFloat4x4(&TargetTransform, Temp);
	TargetTransform._41 = 0.f;
	TargetTransform._42 = 0.f;
	TargetTransform._43 = 0.f;
	_matrix Mat = XMLoadFloat4x4(&TargetTransform);

	_matrix RotMat = XMMatrixRotationY(PxPi);
	_matrix WorldMat = pTransform->Get_WorldMatrix();
	_matrix RootMat= RotMat*XMLoadFloat4x4(pRoot->Get_CombinedTransformationMatrix())* WorldMat;
	_matrix TargetMat= RotMat*XMLoadFloat4x4(pTarget->Get_CombinedTransformationMatrix())* WorldMat;

	_vector vRootPos = RootMat.r[3];
	_vector vTargetPos = TargetMat.r[3];
	_vector vDelta = vRootPos - vTargetPos;
	_vector vDir = XMVector4Normalize(vDelta);
	_float	fDelta = XMVectorGetX(XMVector3Length(vDelta));

	_matrix TransformMat = PxTransformToXMMATRIX(m_vecCollider_Transform[iIndex]);

	_vector ColliderTransform = TransformMat.r[3];
	_vector PureTransform = ColliderTransform;
	ColliderTransform += vDelta;
	ColliderTransform = XMVectorSetW(ColliderTransform,1.f);

	TransformMat.r[3] = ColliderTransform;
	TransformMat = Mat * TransformMat;
	//TransformMat.r[3] = PureTransform;

	m_vecCollider[iIndex]->setGlobalPose(XMMATRIXToPxTransform(TransformMat));
}

void CPxCollider::Update_Transform_Divided_Double_Door_L(_float4x4* Transform)
{
	auto Temp = Transform;
	Temp->_41 = 0.f;
	Temp->_42 = 0.f;
	Temp->_43 = 0.f;
	_matrix Mat = XMLoadFloat4x4(Temp);

	for(int i = 0;i<3;++i)
	{
		_matrix TransformMat = PxTransformToXMMATRIX(m_vecCollider_Transform[i]);

		TransformMat = Mat * TransformMat;

		m_vecCollider[i]->setGlobalPose(XMMATRIXToPxTransform(TransformMat));
	}
}

void CPxCollider::Update_Transform_Divided_Double_Door_R(_float4x4* Transform)
{
	auto Temp = Transform;
	Temp->_41 = 0.f;
	Temp->_42 = 0.f;
	Temp->_43 = 0.f;
	_matrix Mat = XMLoadFloat4x4(Temp);

	for (int i = 3; i < 6; ++i)
	{
		_matrix TransformMat = PxTransformToXMMATRIX(m_vecCollider_Transform[i]);

		TransformMat = Mat * TransformMat;

		m_vecCollider[i]->setGlobalPose(XMMATRIXToPxTransform(TransformMat));
	}
}


void CPxCollider::Update_Transform_Divided(_float4x4* Transform, _int iIndex)
{
	auto Temp = Transform;
	Temp->_41 = 0.f;
	Temp->_42 = 0.f;
	Temp->_43 = 0.f;
	_matrix Mat = XMLoadFloat4x4(Temp);

	_matrix TransformMat = PxTransformToXMMATRIX(m_vecCollider_Transform[iIndex]);

	TransformMat = Mat * TransformMat;

	m_vecCollider[iIndex]->setGlobalPose(XMMATRIXToPxTransform(TransformMat));
}

void CPxCollider::Free()
{
	for (int i = 0; i < m_vecCollider.size(); ++i)
	{
		m_vecCollider[i]->release();
	}
}