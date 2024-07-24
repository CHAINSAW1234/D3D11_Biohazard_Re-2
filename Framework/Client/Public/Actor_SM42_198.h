#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)

class CActor_SM42_198 : public CActor
{
public:
	enum class ACTOR_SM42_198_PART { _BODY, _END };

private:
	CActor_SM42_198(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_SM42_198(const CActor& rhs);
	virtual ~CActor_SM42_198() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

private:
	HRESULT					Add_Components();
	virtual HRESULT			Add_PartObjects();

public:
	static CActor_SM42_198* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END