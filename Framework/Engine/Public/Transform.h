#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec = { 0.0f };
		_float		fRotationPerSec = { 0.0f };
	}TRANSFORM_DESC;

public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	void					Set_State(STATE eState, _fvector vState) {
		_float4		vTemp;
		XMStoreFloat4(&vTemp, vState);
		memcpy(&m_WorldMatrix.m[eState], &vTemp, sizeof(_float4));
	}

	void					Set_State(STATE eState, const _float4& vState)
	{
		memcpy(&m_WorldMatrix.m[eState], &vState, sizeof(_float4));
	}

	void					Set_Scaled(_float fScaleX, _float fScaleY, _float fScaleZ);

	inline void				Set_WorldMatrix(_float4x4 WorldMatrix) {
		m_WorldMatrix = WorldMatrix;
	}

	inline void				Set_WorldMatrix(_matrix WorldMatrix) {
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}

	_vector					Get_State_Vector(STATE eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_float4					Get_State_Float4(STATE eState) {
		_float4		vTemp;
		XMStoreFloat4(&vTemp, XMLoadFloat4x4(&m_WorldMatrix).r[eState]);
		return vTemp;
	}

	_float3					Get_Scaled() const {
		_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
		return _float3(XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_RIGHT])), 
			XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_UP])), 
			XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_LOOK])));
	}

	_float4x4				Get_WorldFloat4x4() const {
		return m_WorldMatrix;
	}
	_matrix					Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_float4x4				Get_WorldFloat4x4_Inverse() const 
	{
		_float4x4	WorldMatrixInverse;
		XMStoreFloat4x4(&WorldMatrixInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
		return WorldMatrixInverse;
	}

	const _float4x4*		Get_WorldFloat4x4_Ptr() const
	{
		return &m_WorldMatrix;
	}
	_matrix					Get_WorldMatrix_Inverse() const 
	{
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	//스케일을 1로 만든 월드 행렬 반환 -XMFLOAT4X4
	_float4x4				Get_WorldMatrix_Pure();
	//스케일을 1로 만든 월드 행렬 반환 -XMMATRIX
	_matrix					Get_WorldMatrix_Pure_Mat();
	//회전 행렬만을 뽑아서 반환 - XMFLOAT4X4
	_float4x4				Get_RotationMatrix_Pure();
	//회전 행렬만을 뽑아서 반환 - XMMATRIX
	_matrix					Get_RotationMatrix_Pure_Mat();
	//회전 행렬만을 설정 - XMFLOAT4X4
	void					Set_RotationMatrix_Pure(_float4x4 Mat);
	//회전 행렬만을 설정 - XMMATRIX
	void					Set_RotationMatrix_Pure(_matrix Mat);
	// 현재 행렬을 이전 Tick의 행렬로써  PrevMatrix에 세팅 -> 이번 Tick의 모은 행동이 종료 된 이후에 실행 할 것

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

public:
	HRESULT					Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

public:
	void					Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void					Go_Backward(_float fTimeDelta);
	void					Go_Left(_float fTimeDelta);
	void					Go_Right(_float fTimeDelta);
	void					Go_Up(_float fTimeDelta);
	void					Go_Down(_float fTimeDelta);
	void					Look_At(_fvector vPosition);
	void					Look_At_ForLandObject(_fvector vPosition);
	void					Move_toTarget(_fvector vTargetPos, _float fTimeDelta, _float fMinDistance = 0.f);	
	void					Turn(_fvector vAxis, _float fTimeDelta);
	void					Rotation(_fvector vAxis, _float fRadian);
	void					SetRotationPerSec(_float fRotationPerSec)
	{
		m_fRotationPerSec = fRotationPerSec;
	}
private:
	_float4x4				m_WorldMatrix;
	_float					m_fSpeedPerSec = { 0.0f };
	_float					m_fRotationPerSec = { 0.0f };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

};

END