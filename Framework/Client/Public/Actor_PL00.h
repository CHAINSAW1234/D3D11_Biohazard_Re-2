#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)

class CActor_PL00 : public CActor
{
public:
	enum class ACTOR_PL00_PART { _BODY, _HEAD, _HAIR, _END };

private:
	CActor_PL00(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_PL00(const CActor& rhs);
	virtual ~CActor_PL00() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

private:
	HRESULT					Add_Components();
	virtual HRESULT			Add_PartObjects();

public:
	static CActor_PL00* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END