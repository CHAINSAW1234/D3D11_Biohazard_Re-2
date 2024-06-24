#pragma once
#include "Customize_UI.h"


BEGIN(Client)

class CItem_DragShadow final : public CCustomize_UI
{
protected:
	CItem_DragShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_DragShadow(const CItem_DragShadow& rhs);
	virtual ~CItem_DragShadow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CItem_DragShadow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END