#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CBody_Chair final : public CPart_InteractProps
{
public:
	typedef struct Body_Chair_Desc : public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
		_ubyte eType = { 0 };
	}BODY_CHAIR_DESC;
private:
	CBody_Chair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Chair(const CBody_Chair& rhs);
	virtual ~CBody_Chair() = default;

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
	HRESULT						Initialize_Model();
public:
	virtual _float4				Get_Pos(_int iArg = 0) override;
	//virtual void					Get_SpecialBone_Rotation() override;
private:
	_ubyte							m_eType = { 0 };
public:
	static CBody_Chair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END