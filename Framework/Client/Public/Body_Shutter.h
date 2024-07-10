#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CBody_Shutter final : public CPart_InteractProps
{
public:
	typedef struct tag_Shutter_desc :public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
		_ubyte* eShutter_Type = { nullptr };
		_ubyte* eNormalState = { nullptr };
		_ubyte* e033State = { nullptr };
		_ubyte* e034State = { nullptr };

	}BODY_SHUTTER_DESC;
private:
	CBody_Shutter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Shutter(const CBody_Shutter& rhs);
	virtual ~CBody_Shutter() = default;

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
private:
	void Shutter_Normal_Late_Tick(_float fTimeDelta);
	void Shutter_033_Late_Tick(_float fTimeDelta);
	void Shutter_034_Late_Tick(_float fTimeDelta);




private:
	_ubyte*			m_eType = { nullptr };
	_ubyte*			m_eNormalState = { nullptr };
	_ubyte*			m_e033State = { nullptr };
	_ubyte*			m_e034State = { nullptr };

public:
	static CBody_Shutter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END