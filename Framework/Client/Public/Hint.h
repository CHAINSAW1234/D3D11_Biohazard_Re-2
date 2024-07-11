#pragma once

#include "UI.h"

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

private:
	class CHint_Highliter* m_pHint_Highlighter = { nullptr };

	unordered_map<wstring, vector<CTexture*>> m_mapDocuments;

	//vector<>;



private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Create_Hint_Highlighter();

public:
	static CHint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END