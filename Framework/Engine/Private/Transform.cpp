#include "..\Public\Transform.h"
#include "Shader.h"

#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{	

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{

}

void CTransform::Set_Scaled(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State_Vector(STATE_RIGHT)) * fScaleX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State_Vector(STATE_UP)) * fScaleY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State_Vector(STATE_LOOK)) * fScaleZ);
}

_float4x4 CTransform::Get_WorldMatrix_Pure()
{
	_matrix identity = XMMatrixIdentity();
	_float4x4 RotationMatrix;
	XMStoreFloat4x4(&RotationMatrix, identity);

	_float3 vLenght_1 = _float3(m_WorldMatrix._11, m_WorldMatrix._12, m_WorldMatrix._13);
	XMVECTOR vector = XMLoadFloat3(&vLenght_1);
	XMVECTOR length = XMVector3Length(vector);
	float Length1;
	XMStoreFloat(&Length1, length);

	RotationMatrix._11 = m_WorldMatrix._11 / Length1;
	RotationMatrix._12 = m_WorldMatrix._12 / Length1;
	RotationMatrix._13 = m_WorldMatrix._13 / Length1;

	_float3 vLenght_2 = _float3(m_WorldMatrix._21, m_WorldMatrix._22, m_WorldMatrix._23);
	XMVECTOR vector2 = XMLoadFloat3(&vLenght_2);
	XMVECTOR length2 = XMVector3Length(vector2);
	float Length2;
	XMStoreFloat(&Length2, length2);
	RotationMatrix._21 = m_WorldMatrix._21 / Length2;
	RotationMatrix._22 = m_WorldMatrix._22 / Length2;
	RotationMatrix._23 = m_WorldMatrix._23 / Length2;

	_float3 vLenght_3 = _float3(m_WorldMatrix._31, m_WorldMatrix._32, m_WorldMatrix._33);
	XMVECTOR vector3 = XMLoadFloat3(&vLenght_3);
	XMVECTOR length3 = XMVector3Length(vector3);
	float Length3;
	XMStoreFloat(&Length3, length);
	RotationMatrix._31 = m_WorldMatrix._31 / Length3;
	RotationMatrix._32 = m_WorldMatrix._32 / Length3;
	RotationMatrix._33 = m_WorldMatrix._33 / Length3;

	RotationMatrix._41 = m_WorldMatrix._41;
	RotationMatrix._42 = m_WorldMatrix._42;
	RotationMatrix._43 = m_WorldMatrix._43;

	return RotationMatrix;
}

_matrix CTransform::Get_WorldMatrix_Pure_Mat()
{
	_matrix identity = XMMatrixIdentity();
	_float4x4 RotationMatrix;
	XMStoreFloat4x4(&RotationMatrix, identity);

	_float3 vLenght_1 = _float3(m_WorldMatrix._11, m_WorldMatrix._12, m_WorldMatrix._13);
	XMVECTOR vector = XMLoadFloat3(&vLenght_1);
	XMVECTOR length = XMVector3Length(vector);
	float Length1;
	XMStoreFloat(&Length1, length);

	RotationMatrix._11 = m_WorldMatrix._11 / Length1;
	RotationMatrix._12 = m_WorldMatrix._12 / Length1;
	RotationMatrix._13 = m_WorldMatrix._13 / Length1;

	_float3 vLenght_2 = _float3(m_WorldMatrix._21, m_WorldMatrix._22, m_WorldMatrix._23);
	XMVECTOR vector2 = XMLoadFloat3(&vLenght_2);
	XMVECTOR length2 = XMVector3Length(vector2);
	float Length2;
	XMStoreFloat(&Length2, length2);
	RotationMatrix._21 = m_WorldMatrix._21 / Length2;
	RotationMatrix._22 = m_WorldMatrix._22 / Length2;
	RotationMatrix._23 = m_WorldMatrix._23 / Length2;

	_float3 vLenght_3 = _float3(m_WorldMatrix._31, m_WorldMatrix._32, m_WorldMatrix._33);
	XMVECTOR vector3 = XMLoadFloat3(&vLenght_3);
	XMVECTOR length3 = XMVector3Length(vector3);
	float Length3;
	XMStoreFloat(&Length3, length);
	RotationMatrix._31 = m_WorldMatrix._31 / Length3;
	RotationMatrix._32 = m_WorldMatrix._32 / Length3;
	RotationMatrix._33 = m_WorldMatrix._33 / Length3;

	RotationMatrix._41 = m_WorldMatrix._41;
	RotationMatrix._42 = m_WorldMatrix._42;
	RotationMatrix._43 = m_WorldMatrix._43;

	return XMLoadFloat4x4(&RotationMatrix);
}

_float4x4 CTransform::Get_RotationMatrix_Pure()
{
	_matrix identity = XMMatrixIdentity();
	_float4x4 RotationMatrix;
	XMStoreFloat4x4(&RotationMatrix, identity);

	_float3 vLenght_1 = _float3(m_WorldMatrix._11, m_WorldMatrix._12, m_WorldMatrix._13);
	XMVECTOR vector = XMLoadFloat3(&vLenght_1);
	XMVECTOR length = XMVector3Length(vector);
	float Length1;
	XMStoreFloat(&Length1, length);

	RotationMatrix._11 = m_WorldMatrix._11 / Length1;
	RotationMatrix._12 = m_WorldMatrix._12 / Length1;
	RotationMatrix._13 = m_WorldMatrix._13 / Length1;

	_float3 vLenght_2 = _float3(m_WorldMatrix._21, m_WorldMatrix._22, m_WorldMatrix._23);
	XMVECTOR vector2 = XMLoadFloat3(&vLenght_2);
	XMVECTOR length2 = XMVector3Length(vector2);
	float Length2;
	XMStoreFloat(&Length2, length2);
	RotationMatrix._21 = m_WorldMatrix._21 / Length2;
	RotationMatrix._22 = m_WorldMatrix._22 / Length2;
	RotationMatrix._23 = m_WorldMatrix._23 / Length2;

	_float3 vLenght_3 = _float3(m_WorldMatrix._31, m_WorldMatrix._32, m_WorldMatrix._33);
	XMVECTOR vector3 = XMLoadFloat3(&vLenght_3);
	XMVECTOR length3 = XMVector3Length(vector3);
	float Length3;
	XMStoreFloat(&Length3, length);
	RotationMatrix._31 = m_WorldMatrix._31 / Length3;
	RotationMatrix._32 = m_WorldMatrix._32 / Length3;
	RotationMatrix._33 = m_WorldMatrix._33 / Length3;

	return RotationMatrix;
}

_matrix CTransform::Get_RotationMatrix_Pure_Mat()
{
	_matrix identity = XMMatrixIdentity();
	_float4x4 RotationMatrix;
	XMStoreFloat4x4(&RotationMatrix, identity);

	_float3 vLenght_1 = _float3(m_WorldMatrix._11, m_WorldMatrix._12, m_WorldMatrix._13);
	XMVECTOR vector = XMLoadFloat3(&vLenght_1);
	XMVECTOR length = XMVector3Length(vector);
	float Length1;
	XMStoreFloat(&Length1, length);

	RotationMatrix._11 = m_WorldMatrix._11 / Length1;
	RotationMatrix._12 = m_WorldMatrix._12 / Length1;
	RotationMatrix._13 = m_WorldMatrix._13 / Length1;

	_float3 vLenght_2 = _float3(m_WorldMatrix._21, m_WorldMatrix._22, m_WorldMatrix._23);
	XMVECTOR vector2 = XMLoadFloat3(&vLenght_2);
	XMVECTOR length2 = XMVector3Length(vector2);
	float Length2;
	XMStoreFloat(&Length2, length2);
	RotationMatrix._21 = m_WorldMatrix._21 / Length2;
	RotationMatrix._22 = m_WorldMatrix._22 / Length2;
	RotationMatrix._23 = m_WorldMatrix._23 / Length2;

	_float3 vLenght_3 = _float3(m_WorldMatrix._31, m_WorldMatrix._32, m_WorldMatrix._33);
	XMVECTOR vector3 = XMLoadFloat3(&vLenght_3);
	XMVECTOR length3 = XMVector3Length(vector3);
	float Length3;
	XMStoreFloat(&Length3, length);
	RotationMatrix._31 = m_WorldMatrix._31 / Length3;
	RotationMatrix._32 = m_WorldMatrix._32 / Length3;
	RotationMatrix._33 = m_WorldMatrix._33 / Length3;

	return XMLoadFloat4x4(&RotationMatrix);
}

void CTransform::Set_RotationMatrix_Pure(_float4x4 Mat)
{
	//_matrix RotationMatrix;
	_float4 Position = Get_State_Float4(STATE::STATE_POSITION);
	_float3 Scale = Get_Scaled();
	m_WorldMatrix = Mat;
	m_WorldMatrix._11 *= Scale.x;
	m_WorldMatrix._12 *= Scale.x;
	m_WorldMatrix._13 *= Scale.x;

	m_WorldMatrix._21 *= Scale.x;
	m_WorldMatrix._22 *= Scale.x;
	m_WorldMatrix._23 *= Scale.x;

	m_WorldMatrix._31 *= Scale.x;
	m_WorldMatrix._32 *= Scale.x;
	m_WorldMatrix._33 *= Scale.x;

	m_WorldMatrix._41 = Position.x;
	m_WorldMatrix._42 = Position.y;
	m_WorldMatrix._43 = Position.z;
	m_WorldMatrix._44 = 1.f;
}

void CTransform::Set_RotationMatrix_Pure(_matrix Mat)
{
	_float4x4 RotationMatrix;
	XMStoreFloat4x4(&RotationMatrix, Mat);
	_float4 Position = Get_State_Float4(STATE::STATE_POSITION);
	_float3 Scale = Get_Scaled();
	m_WorldMatrix = RotationMatrix;
	m_WorldMatrix._11 *= Scale.x;
	m_WorldMatrix._12 *= Scale.x;
	m_WorldMatrix._13 *= Scale.x;

	m_WorldMatrix._21 *= Scale.x;
	m_WorldMatrix._22 *= Scale.x;
	m_WorldMatrix._23 *= Scale.x;

	m_WorldMatrix._31 *= Scale.x;
	m_WorldMatrix._32 *= Scale.x;
	m_WorldMatrix._33 *= Scale.x;

	m_WorldMatrix._41 = Position.x;
	m_WorldMatrix._42 = Position.y;
	m_WorldMatrix._43 = Position.z;
	m_WorldMatrix._44 = 1.f;
}

void CTransform::SetWorldMatrix_With_UpVector(_vector upVector)
{
	_vector Scale = Get_Scaled();
	_vector Position = Get_State_Vector(CTransform::STATE_POSITION);

	upVector = XMVector3Normalize(upVector);

	_vector forwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	if (XMVector3NearEqual(upVector, forwardVector, XMVectorSplatEpsilon()))
	{
		forwardVector = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	}

	_vector rightVector = XMVector3Cross(upVector, forwardVector);
	rightVector = XMVector3Normalize(rightVector);

	forwardVector = XMVector3Cross(rightVector, upVector);
	forwardVector = XMVector3Normalize(forwardVector);

	_matrix rotationMatrix = _matrix(
		rightVector,
		upVector,
		forwardVector,
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)
	);

	_matrix scaleMatrix = XMMatrixScalingFromVector(Scale);

	m_WorldMatrix = scaleMatrix * rotationMatrix * XMMatrixTranslationFromVector(Position);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
	{
		TRANSFORM_DESC*		pDesc = (TRANSFORM_DESC*)pArg;

		m_fSpeedPerSec = pDesc->fSpeedPerSec;
		m_fRotationPerSec = pDesc->fRotationPerSec;
	}

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShader, const _char * pConstantName)
{

	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _float4x4 MoveMatrix)
{
	//Lo x word
	//이게 만약에 m_WorldMatrix 에 스케일이 있으면 (1이아니면) 
	_float3 vScale = Get_Scaled();
	if (1.f != vScale.x || 1.f != vScale.y || 1.f != vScale.z)
	{
		_float4x4 wordpure = Get_WorldMatrix_Pure();

		//_float4x4 TempMatrix = MoveMatrix * wordpure;
		_float4x4 TempMatrix = MoveMatrix * m_WorldMatrix;

		_matrix Resultmat = XMLoadFloat4x4(&TempMatrix);

		_vector vRight = Resultmat.r[0];
		vRight = XMVectorScale(XMVector3Normalize(vRight), vScale.x);
		_vector vUp = Resultmat.r[1];
		vUp = XMVectorScale(XMVector3Normalize(vUp), vScale.y);
		_vector vLook = Resultmat.r[2];
		vLook = XMVectorScale(XMVector3Normalize(vLook), vScale.z);

		Resultmat.r[0] = vRight;
		Resultmat.r[1] = vUp;
		Resultmat.r[2] = vLook;

		XMStoreFloat4x4(&TempMatrix, Resultmat);

		return pShader->Bind_Matrix(pConstantName, &TempMatrix);
	}

	else
	{
		_float4x4 TempMatrix = MoveMatrix * m_WorldMatrix;
		return pShader->Bind_Matrix(pConstantName, &TempMatrix);
	}
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vLook = Get_State_Vector(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if(nullptr == pNavigation)
		Set_State(STATE_POSITION, vPosition);
	else
	{
		if (true == pNavigation->isMove(vPosition))
			Set_State(STATE_POSITION, vPosition);
	}	
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vLook = Get_State_Vector(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vRight = Get_State_Vector(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vRight = Get_State_Vector(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vUp = Get_State_Vector(STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vUp = Get_State_Vector(STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Look_At(_fvector vAt)
{
	_vector		vLook = vAt - Get_State_Vector(STATE_POSITION);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Look_At_ForLandObject(_fvector vAt)
{
	// XMMatrixDecompose();

	_vector		vUp = Get_State_Vector(STATE_UP);
	_vector		vLook = vAt - Get_State_Vector(STATE_POSITION);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vLook = XMVector3Cross(vRight, vUp);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);	
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Move_toTarget(_fvector vTargetPos, _float fTimeDelta, _float fMinDistance)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);

	_vector		vLook = vTargetPos - vPosition;

	if(fMinDistance <= XMVector3Length(vLook).m128_f32[0])
		vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
}

_bool CTransform::Move_toTargetUI(_float4 vTargetPos, _float fTimeDelta, _float fMinDistance)
{
	_float4		vPosition = Get_State_Float4(STATE_POSITION);
	_float		fZ = vPosition.z;

	_float4		vLook = vTargetPos - vPosition;

	if (fMinDistance < XMVector3Length(vLook).m128_f32[0])
	{
		vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
		vPosition.z = fZ;
		Set_State(STATE_POSITION, vPosition);

		return false;
	}

	Set_State(STATE_POSITION, vTargetPos);

	return true;
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix			RotationMatrix  = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	for (size_t i = 0; i < STATE_POSITION; i++)
	{
		Set_State(STATE(i), 
			XMVector4Transform(Get_State_Vector((STATE)i), RotationMatrix));
		/*Set_State(STATE(i),
			XMVector3TransformNormal(Get_State_Vector((STATE)i), RotationMatrix));*/
	}

}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_vector		vState[] = {
		XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x,
		XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y, 
		XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z, 		
	};

	
	// XMConvertToRadians(Degree);

	_matrix			RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	for (size_t i = 0; i < STATE_POSITION; i++)
	{
		Set_State(STATE(i),
			XMVector4Transform(vState[(STATE)i], RotationMatrix));
	}
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTransform"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTransform"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}
