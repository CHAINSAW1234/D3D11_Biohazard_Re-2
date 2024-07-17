#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)

class CActor_SM60_033_00 : public CActor
{
public:
	enum class ACTOR_SM60_033_PART { _BODY, _END };

private:
	CActor_SM60_033_00(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_SM60_033_00(const CActor& rhs);
	virtual ~CActor_SM60_033_00() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

public:
	virtual void			Start() override;

private:
	HRESULT					Add_Components();
	virtual HRESULT			Add_PartObjects();

public:
	static CActor_SM60_033_00* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END