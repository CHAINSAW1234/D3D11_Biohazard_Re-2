#pragma once

#include "Customize_UI.h"

BEGIN(Client)

class CHint_Display final : public CCustomize_UI
{
protected:
	CHint_Display(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHint_Display(const CHint_Display& rhs);
	virtual ~CHint_Display() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Start() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Set_Dead(_bool bDead) override;

	void Set_Display(ITEM_READ_TYPE eItemReadType, _uint TextureNum, _float2 fPos, _float2 fSize);
	_uint Get_CurTypeTexCount() const { return m_iCurTypeTexCount; }
	_uint Get_CurTexNum() const { return m_iCurTexNum; }
	ITEM_READ_TYPE Get_CurIRT() { return m_eCurIRT;	}
	void Set_CurTexNum(_uint  iTexNum) {
		m_iCurTexNum = iTexNum;
		m_pTextureCom = m_mapDocumentTextures[m_eCurIRT][m_iCurTexNum];
	}


private:
	class CHint_Display_Background* m_pBackground = { nullptr };

private:
	map<ITEM_READ_TYPE, vector<CTexture*>> m_mapDocumentTextures;
	CTexture* m_pOriginalTexture = { nullptr };
	ITEM_READ_TYPE m_eCurIRT = { ITEM_READ_TYPE::END_NOTE };
	_uint m_iCurTypeTexCount = { 0 };
	_uint m_iCurTexNum = { 0 };
	
private:
	HRESULT Load_Texture();
	HRESULT Create_DisplayBackground();

public:
	static CHint_Display* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END