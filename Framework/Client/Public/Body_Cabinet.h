#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CBody_Cabinet final : public CPart_InteractProps
{
private:
	CBody_Cabinet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Cabinet(const CBody_Cabinet& rhs);
	virtual ~CBody_Cabinet() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT					Render_LightDepth_Dir()override;
	virtual HRESULT					Render_LightDepth_Point() override;
	virtual HRESULT					Render_LightDepth_Spot()override;

private:
	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;
	HRESULT						Initialize_Model_i44();
	HRESULT						Initialize_Model();
public:
	virtual _float4				Get_Pos(_int iArg = 0) override;
	virtual void					Get_SpecialBone_Rotation() override;
private:
	list<_uint>					m_NonHideIndices;
public:
	static CBody_Cabinet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END