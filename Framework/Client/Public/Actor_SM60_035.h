#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)

class CActor_SM60_035 : public CActor
{
public:
	enum class ACTOR_SM60_035_PART { _BODY, _END };

private:
	CActor_SM60_035(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_SM60_035(const CActor& rhs);
	virtual ~CActor_SM60_035() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

private:
	HRESULT					Add_Components();
	virtual HRESULT			Add_PartObjects();

public:
	static CActor_SM60_035* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END