#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CHold_UI final : public CCustomize_UI
{
private:
	CHold_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHold_UI(const CHold_UI& rhs);
	virtual ~CHold_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Change_Tool() override;

private :
	void Calc_Position();

private :
	_float4		m_vTargetPos	= {};
	_bool		m_isCull		 = { false };
	_float2		m_fDistance		= {};
	CTransform* m_pParentTransform = { nullptr };
	CHold_UI* m_pParentObj = { nullptr };

	_bool		m_isHold = { false };

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
