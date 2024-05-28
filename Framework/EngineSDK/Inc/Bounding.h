#pragma once

#include "Collider.h"

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct
	{
		_float3		vCenter;
	}BOUNDING_DESC;

protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	virtual void* Get_BoundingDesc() = 0;

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Tick(_fmatrix WorldMatrix) = 0;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding* pBoundingDesc) = 0;

public:
	void Active_Color(_bool isActive);
	void Set_Color(_float4 vColor);

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch);
#endif

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	_bool					m_isCollision = { false };

	_bool					m_isSetColor = { false };
	_float4					m_vColor = { 1.f, 1.f, 1.f ,1.f };

public:
	virtual void Free() override;
};

END