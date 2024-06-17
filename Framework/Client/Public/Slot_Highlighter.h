#pragma once
#include "Customize_UI.h"


BEGIN(Client)

class CSlot_Highlighter final : public CCustomize_UI
{
protected:
	CSlot_Highlighter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSlot_Highlighter(const CSlot_Highlighter& rhs);
	virtual ~CSlot_Highlighter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void FirstTick_Seting();
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void ResetPosition(_float4 fResetPos);


private:
	_vector			m_vOriginDiff = {};
	CTransform*		m_pCursorTranform = { nullptr };
	
public:
	static CSlot_Highlighter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END