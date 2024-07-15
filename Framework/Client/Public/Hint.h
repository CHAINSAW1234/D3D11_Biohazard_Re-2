#pragma once

#include "UI.h"

#include "Hint_Highliter.h"
#include "Hint_Directory.h"
#include "Hint_Display.h"
#include "Hint_Blind.h"

BEGIN(Client)

class CHint final : public CUI
{
protected:
	CHint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHint(const CHint& rhs);
	virtual ~CHint() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Start() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_Dead(_bool bDead) override;
	void Acquire_Document(ITEM_NUMBER eAcquire_Document);

private:
	CHint_Highliter* m_pHighlighter = { nullptr };
	vector<CHint_Directory*> m_vecDirectory;
	CHint_Display* m_pDisplay = { nullptr };
	CHint_Blind* m_pDisplay_Blinde = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	HRESULT	Create_Display_Blinde();
	HRESULT	Create_Display();
	HRESULT Create_Directory();
	HRESULT Create_Directory_Highlighter();

public:
	static CHint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END