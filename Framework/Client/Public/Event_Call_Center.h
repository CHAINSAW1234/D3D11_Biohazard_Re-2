#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CEvent_Call_Center final : public CBase
{
	DECLARE_SINGLETON(CEvent_Call_Center)

public:
	enum class CALLER {
		_ZOMBIE_HIDE_LOCKER,
		/* For.CutScene */
		_PL00,
		_SM60_033, _SM60_034,
		_END
	};

private:
	CEvent_Call_Center();
	virtual ~CEvent_Call_Center() = default;

public:
	CGameObject* Get_Caller(CALLER eCaller) {
		if (eCaller >= CALLER::_END)
			return nullptr;

		return m_Callers[static_cast<_uint>(eCaller)];
	}

	HRESULT							Add_Caller(CGameObject* pGameObject, CALLER eCaller)
	{
		if (eCaller >= CALLER::_END)
			return E_FAIL;

		if (nullptr == pGameObject)
			return E_FAIL;

		Safe_Release(m_Callers[static_cast<_uint>(eCaller)]);
		m_Callers[static_cast<_uint>(eCaller)] = pGameObject;
		Safe_AddRef(pGameObject);

		return S_OK;
	}

private:
	vector<CGameObject*>			m_Callers;

public:
	virtual void Free() override;
};

END