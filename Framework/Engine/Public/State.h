#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CState abstract : public CBase
{
protected:
	CState();
	virtual ~CState() = default;

public:
	virtual void Enter(_float fTimeDelta);
	virtual void Excute(_float fTimeDelta);
	virtual void Exit(_float fTimeDelta);

protected:
	class CGameObject* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

END