#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CBody_BigStatue final : public CPart_InteractProps
{
private:
	CBody_BigStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_BigStatue(const CBody_BigStatue& rhs);
	virtual ~CBody_BigStatue() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;

	virtual HRESULT				Render() override;
	virtual HRESULT				Render_LightDepth_Dir()override;
	virtual HRESULT				Render_LightDepth_Point() override;
	virtual HRESULT				Render_LightDepth_Spot()override;

private:
	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;



public:
	static CBody_BigStatue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END