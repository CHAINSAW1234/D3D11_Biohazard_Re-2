#pragma once
#include "Client_Defines.h"
#include "Customize_UI.h"

BEGIN(Client)

class CInteract_UI abstract : public CCustomize_UI
{
protected :
	CInteract_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteract_UI(const CInteract_UI& rhs);
	virtual ~CInteract_UI() = default;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


protected :
	// class CProp_Manager*		m_pPropManager = { nullptr };


public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

END;
