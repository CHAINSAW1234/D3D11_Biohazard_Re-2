#pragma once

#include "Client_Defines.h"
#include "Prop_Controller.h"

BEGIN(Client)

class CProp_SM60_034 : public CProp_Controller
{
public:
	enum class ACTOR_SM60_034_PART { _BODY, _END };

private:
	CProp_SM60_034(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProp_SM60_034(const CProp_SM60_034& rhs);
	virtual ~CProp_SM60_034() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

public:
	virtual void			Late_Tick(_float fTimeDelta) override;

public:
	virtual void			Start() override;

public:
	static CProp_SM60_034* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END