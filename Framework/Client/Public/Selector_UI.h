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
	virtual HRESULT Change_Tool() override;

	
public :
	CGameObject*						Destroy_Selector(_bool _selectCull = false); /* �ش� ��ü�� ������� ���� ���̴�. */
	void								Select_Type(_bool _Interact, _float4 _objPos);  /* �ش� ��ü�� � ���¸� ����ϰ��� �ϴ°�? */
	
	_bool*								Get_Using() { return &m_isUsing; }
	CGameObject*						Get_Supervise() { return m_pSupervise;  } 

private :
	void								Exception_Handle();
	void								Reset();
	void								Operate_Selector(_float fTimeDelta);


public:
	void								Calc_Position();
	void								Rendering(_float fTimeDelta);
	void								Find_TabWindow();

private : /* ���� ���� */
	_bool								m_isOutDistance				= { false }; /* ����ΰ�? */
	_float4								m_vTargetPos				= {}; /* Ÿ���� ��ġ */

private :
	class CInteractProps*				m_pInteractObj				= { nullptr }; /* ��ǥ�� */
	class CTab_Window*					m_pTab_Window				= { nullptr };
	CSelector_UI*						m_pSelectorParent			= { nullptr };

	SELECTOR_TYPE						m_eSelectorType				= { SELECTOR_TYPE::END_SELECTOR };

	_float3								m_fOriginSize				= {};
	_bool								m_isInteractive				= { false };
	_bool								m_isCull					= { false };
private :
	wstring								m_wstrInteractive_Tag		= { TEXT("") }; /* Interactive �� �� */
	wstring								m_wstrNonInteractive_Tag	= { TEXT("") }; /* Interactive ���� ���� �� */

private : // X ǥ��
	_float3								m_fXSize					= {};
	_bool								m_isArrow					= { false };
	_float4								m_vXTransform				= {};
	
	_bool								m_isUsing					= { false };
	CGameObject*						m_pSupervise				= { nullptr }; /* �ڽĵ��� �ʴ� �θ� */

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
