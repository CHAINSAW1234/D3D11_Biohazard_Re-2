#pragma once
#include "Customize_UI.h"


BEGIN(Client)

class CHint_Display_Background final : public CCustomize_UI
{
protected:
	CHint_Display_Background(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHint_Display_Background(const CHint_Display_Background& rhs);
	virtual ~CHint_Display_Background() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Start() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_BackGround(ITEM_READ_TYPE eIRT, _bool bDead);

public:
	static _bool IsCanChangeBG_Tex(ITEM_READ_TYPE eIRT);

private:
	map<ITEM_READ_TYPE, CTexture*>	m_mapDocumentTextures;
	CTexture*						m_pOriginalTexture = { nullptr };

private:
	HRESULT Load_Texture();

public:
	static CHint_Display_Background* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END