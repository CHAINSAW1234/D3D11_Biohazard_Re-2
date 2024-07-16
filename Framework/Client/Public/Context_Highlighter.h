#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CContext_Highlighter final : public CCustomize_UI
{
protected:
	CContext_Highlighter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContext_Highlighter(const CContext_Highlighter& rhs);
	virtual ~CContext_Highlighter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Change_Tool() override;


public:
	static CContext_Highlighter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END