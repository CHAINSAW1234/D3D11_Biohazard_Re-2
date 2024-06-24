#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CSelector_UI final : public CCustomize_UI
{
private :
	enum class SELECTOR_TYPE { BACKGROUND_SELECTOR, INCHECK_SELECTOR, OUTCHECK_SELECTOR, END_SELECTOR };

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

private :
	void								Find_InteractObj();
	void								Find_Selector_Obj();

	void								Operate_Selector(_float fTimeDelta);
	void								Attach_InteractObj();

public :
	void								Render_Selector_UI(CGameObject* _obj, _float fTimeDelta);
	void								Interactive_Selector_UI(CGameObject* _obj, _float fTimeDelta);
	
private :
	class CInteractProps*				m_pInteractObj				= { nullptr }; /* 목표물 */
	CSelector_UI*						m_pParent					= { nullptr };	/* 부모 */

	SELECTOR_TYPE						m_eSelectorType				= { SELECTOR_TYPE::END_SELECTOR };

	_float3								m_fOriginSize				= {};
	_bool								m_isInteractive				= { false };

private :
	wstring								m_wstrInteractive_Tag		= { TEXT("") };
	wstring								m_wstrNonInteractive_Tag	= { TEXT("") };

private : // X 표시
	_float3								m_fXSize					= {};
	_bool								m_isArrow					= { false };
	_float4								m_vXTransform				= {};
	
private :
	list<class CInteractProps*>			m_InteractPropsList; /* 목표물 모음*/
	vector<CSelector_UI*>				m_SelectorObj_Vec;

	_bool								m_isUsing					= { false };
	CGameObject*						m_pSupervise				= { nullptr }; /* 자식들의 초대 부모 */

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
