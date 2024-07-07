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

	
public :/* For. 옌*/
	CGameObject*						Destroy_Selector(); /* 해당 객체를 사용하지 않을 것이다. */
	void								Select_Type(_bool _Interact, _float4 _objPos);  /* 해당 객체의 어떤 상태를 사용하고자 하는가? */
	
	/* For. Player Call */
	void								Set_SelectorObj_Collection(CSelector_UI* pSelect)
	{
		m_SelectorObj_Vec.push_back(pSelect);
	} 
	_bool*								Get_Using() { return &m_isUsing; }
	CGameObject*						Get_Supervise() { return m_pSupervise;  } 
	vector<CSelector_UI*>*				Get_Collection() { return &m_SelectorObj_Vec;  }
private :
	void								Exception_Handle();
	void								Find_Selector_Obj();
	void								Reset();
	void								Operate_Selector(_float fTimeDelta);


public:
	void								NonInteractive_Rendering(_float fTimeDelta);

	
private : /* 연동 관련 */
	_bool								m_isOutDistance				= { false }; /* 논외인가? */
	_float4								m_vTargetPos				= {}; /* 타겟의 위치 */

private :
	class CInteractProps*				m_pInteractObj				= { nullptr }; /* 목표물 */
	CSelector_UI*						m_pParent					= { nullptr };	/* 부모 */

	SELECTOR_TYPE						m_eSelectorType				= { SELECTOR_TYPE::END_SELECTOR };

	_float3								m_fOriginSize				= {};
	_bool								m_isInteractive				= { false };

private :
	wstring								m_wstrInteractive_Tag		= { TEXT("") }; /* Interactive 할 때 */
	wstring								m_wstrNonInteractive_Tag	= { TEXT("") }; /* Interactive 하지 않을 때 */

private : // X 표시
	_float3								m_fXSize					= {};
	_bool								m_isArrow					= { false };
	_float4								m_vXTransform				= {};
	
private :
	vector<CSelector_UI*>				m_SelectorObj_Vec;

	_bool								m_isUsing					= { false };
	CGameObject*						m_pSupervise				= { nullptr }; /* 자식들의 초대 부모 */

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
