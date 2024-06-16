#pragma once
#include "Customize_UI.h"


BEGIN(Client)

class CInventorySelect final : public CCustomize_UI
{
protected:
	CInventorySelect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventorySelect(const CInventorySelect& rhs);
	virtual ~CInventorySelect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void FirstTick_Seting();
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void ResetPosition(_float4 fResetPos);


private:
	_vector m_vOriginDiff = {};
	//_vector m_vTargetPos = {};
	CTransform* m_pCursorTranform = { nullptr };
	
public:
	static CInventorySelect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END