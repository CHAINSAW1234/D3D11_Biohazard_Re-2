#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Tool)

class CTool abstract : public CBase
{
public:
	enum TOOL_TYPE {
		TRANSFORMATION,
		COLLIDER,
		MODEL_SELECTOR,
		ANIM_LIST,
		ANIM_PLAYER,
		TOOL_END
	};

protected:
	CTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool() = default;

public:
	virtual HRESULT			Initialize(void* pArg);
	virtual void			Tick(_float fTimeDelta);

public:
	_float					Get_ChildHeight() {
		return m_fHeight;
	}

	void					Set_ChildHeight(_float fHeight) {
		if (0.f > fHeight)
			fHeight = 0.f;

		m_fHeight = fHeight;
	}

protected: 
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	_float                  m_fHeight = { 0.f };
	string					m_strCollasingTag = { "" };

public:
	virtual void Free();
};

END