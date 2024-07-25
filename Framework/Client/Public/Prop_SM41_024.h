#pragma once

#include "Client_Defines.h"
#include "Prop_Controller.h"

BEGIN(Client)

class CProp_SM41_024 : public CProp_Controller
{
public:
	enum class ACTOR_SM41_024_PART { _BODY, _END };

private:
	CProp_SM41_024(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProp_SM41_024(const CProp_SM41_024& rhs);
	virtual ~CProp_SM41_024() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

public:
	virtual void			Late_Tick(_float fTimeDelta) override;

public:
	virtual void			Start() override;

public:
	void					Set_Transformation_Origin();
	void					Reset_Transformation();

private:
	_float4x4				m_OriginalTransformation = {};
	_float4x4*				m_pTempIdentity = { nullptr };

public:
	static CProp_SM41_024* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END