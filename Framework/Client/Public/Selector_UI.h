#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CSelector_UI final : public CCustomize_UI
{
private:
	CSelector_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSelector_UI(const CSelector_UI& rhs);
	virtual ~CSelector_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	void			Render_Selector_UI(CGameObject* _obj, _float fTimeDelta);
	void			Interactive_Selector_UI(CGameObject* _obj, _float fTimeDelta);

	_float			Distance_Player(CGameObject* _obj);

private :
	CGameObject*		m_pEx = { nullptr }; /* 목표물 */
	CSelector_UI*		m_pParent = { nullptr }; /* 부모 */

	_float3				m_fOriginSize = {};
	_bool				m_isInteractive = { false };

private :
	wstring				m_wstrInteractive_Tag = { TEXT("") };
	wstring				m_wstrNonInteractive_Tag = { TEXT("") };

private : // X
	_float3				m_fXSize = {};
	_bool				m_isArrow = { false };
	_float4				m_vXTransform = {};

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
