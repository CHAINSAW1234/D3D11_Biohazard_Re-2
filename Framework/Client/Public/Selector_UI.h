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

public :
	/* For. ��*/
	void								Destroy_Selector(); /* �ش� ��ü�� ������� ���� ���̴�. */
	void								Select_Type(_bool _Interact); /* �ش� ��ü�� � ���¸� ����ϰ��� �ϴ°�? */

	/* For. Player Call */
	void								Set_SelectorObj_Collection(CSelector_UI* pSelect)
	{
		m_SelectorObj_Vec.push_back(pSelect);
	}
	_bool*								Get_Using() { return &m_isUsing; }
	CGameObject*						Get_Supervise() { return m_pSupervise;  } 

private :
	void								Exception_Handle();
	void								Find_Selector_Obj();
	void								Reset();
	void								Operate_Selector(_float fTimeDelta);

public :
	void								Render_Selector_UI(CGameObject* _obj, _float fTimeDelta);
	
private :
	class CInteractProps*				m_pInteractObj				= { nullptr }; /* ��ǥ�� */
	CSelector_UI*						m_pParent					= { nullptr };	/* �θ� */

	SELECTOR_TYPE						m_eSelectorType				= { SELECTOR_TYPE::END_SELECTOR };

	_float3								m_fOriginSize				= {};
	_bool								m_isInteractive				= { false };

private :
	wstring								m_wstrInteractive_Tag		= { TEXT("") }; /* Interactive �� �� */
	wstring								m_wstrNonInteractive_Tag	= { TEXT("") }; /* Interactive ���� ���� �� */

private : // X ǥ��
	_float3								m_fXSize					= {};
	_bool								m_isArrow					= { false };
	_float4								m_vXTransform				= {};
	
private :
	vector<CSelector_UI*>				m_SelectorObj_Vec;

	_bool								m_isUsing					= { false };
	CGameObject*						m_pSupervise				= { nullptr }; /* �ڽĵ��� �ʴ� �θ� */

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
