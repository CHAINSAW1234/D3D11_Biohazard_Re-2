#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Tool)

class CTool abstract : public CBase
{
public:
	enum TOOL_TYPE {
		MODEL_SELECTOR,
		TRANSFORMATION,
		COLLIDER,
		ANIM_LIST,
		TOOL_END
	};

protected:
	CTool();
	virtual ~CTool() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);

public:
	_float Get_ChildHeight() {
		return m_fHeight;
	}

	void Set_ChildHeight(_float fHeight) {
		if (0.f > fHeight)
			fHeight = 0.f;

		m_fHeight = fHeight;
	}

protected: 
	CGameInstance*			m_pGameInstance = { nullptr };
	_float                  m_fHeight = { 0.f };

public:
	virtual void Free();
};

END