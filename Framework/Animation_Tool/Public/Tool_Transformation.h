#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tool)

class CTool_Transformation final : public CTool
{
public:
	enum MODE { MODE_PERSPECTIVE, MODE_ORTHO, MODE_END };

protected:
	CTool_Transformation();
	virtual ~CTool_Transformation() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	void Set_Target(CTransform* pTransform);
	void Set_Target(CGameObject* pGameObject);
	void ReSet_Target();

public:
	CTransform* Get_Target() { return m_pTargetTransform; }

private:
	void Update_Transform();

public:
	void Set_Mode(MODE eMode) { m_eMode = eMode; }

private:
	void Update_WorldMatrix();

private:
	_float4x4			m_WorldMatrix;
	MODE				m_eMode = { MODE_END };

	_float3				m_vScale = { 0.f, 0.f, 0.f };
	_float3				m_vRotation = { 0.f ,0.f, 0.f };
	_float4				m_vTranslation = { 0.f, 0.f,0.f, 1.f };

	CTransform*			m_pTargetTransform = { nullptr };

public:
	static CTool_Transformation* Create(void* pArg);
	virtual void Free() override;
};

END