#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Engine)
class CRagdoll_Physics;
END

BEGIN(Client)

class CActor_PL78 : public CActor
{
public:
	enum class ACTOR_PL78_PART { _BODY, _HEAD, _HAIR, _GUTS, _RAGDOLL, _END };
	enum class ACTOR_PL78_MESH_BRANCH { _UPPER_BODY, _LOWER_BODY, _END };

private:
	CActor_PL78(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_PL78(const CActor& rhs);
	virtual ~CActor_PL78() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;

private:
	HRESULT					Add_Components();
	virtual HRESULT			Add_PartObjects();
	virtual HRESULT			Initialize_Models();

public:
	void					Cut_Body();

private:
	void					SetRagdoll();
	void					Apply_LastAnimCombined_RagDoll();

private:
	CRagdoll_Physics*		m_pRagDoll = { nullptr };
	_bool					m_isCutBody = { false };

public:
	static CActor_PL78* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END