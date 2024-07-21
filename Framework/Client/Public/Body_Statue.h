#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CBody_Statue final : public CPart_InteractProps
{
public:
	typedef struct StatueBody_desc :public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
		_bool* pResetCamera = { nullptr };
	}STATUE_BODY_DESC;
private:
	CBody_Statue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Statue(const CBody_Statue& rhs);
	virtual ~CBody_Statue() = default;

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
private:
	_bool* m_pResetCamera = { nullptr };


public:
	static CBody_Statue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END