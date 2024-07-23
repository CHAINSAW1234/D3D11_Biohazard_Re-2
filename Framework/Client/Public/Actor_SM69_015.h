#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)

class CActor_SM69_015 : public CActor
{
public:
	enum class ACTOR_SM69_015_PART { _HEAD, _END };

private:
	CActor_SM69_015(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_SM69_015(const CActor& rhs);
	virtual ~CActor_SM69_015() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

private:
	HRESULT					Add_Components();
	virtual HRESULT			Add_PartObjects();

public:
	static CActor_SM69_015* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END