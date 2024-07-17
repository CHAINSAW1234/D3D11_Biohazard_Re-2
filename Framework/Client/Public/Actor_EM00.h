#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)

class CActor_EM00 : public CActor
{
public:
	enum class ACTOR_EM00_PART { _BODY, _HEAD, _SHIRTS, _PANTS, _END };
	enum class ACTOR_EM00_FACE_TYPE { _DEFAULT, _BROKEN, _END };

public:
	typedef struct tagActorEm00Desc : public ACTOR_DESC
	{
		ACTOR_EM00_FACE_TYPE			eFaceType = { ACTOR_EM00_FACE_TYPE::_END };
	}ACTOR_EM00_DESC;

private:
	CActor_EM00(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_EM00(const CActor& rhs);
	virtual ~CActor_EM00() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

private:
	HRESULT					Add_Components();
	virtual HRESULT			Add_PartObjects();

private:
	ACTOR_EM00_FACE_TYPE			m_eFaceType = { ACTOR_EM00_FACE_TYPE::_END };

public:
	static CActor_EM00* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END