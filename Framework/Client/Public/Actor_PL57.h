#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)

class CActor_PL57 : public CActor
{
public:
	enum class ACTOR_PL57_PART { _BODY, _HEAD, _END };

private:
	CActor_PL57(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_PL57(const CActor& rhs);
	virtual ~CActor_PL57() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

private:
	HRESULT					Add_Components();
	virtual HRESULT			Add_PartObjects();

public:
	void					Set_Idle_Loop();

public:
	static CActor_PL57* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END