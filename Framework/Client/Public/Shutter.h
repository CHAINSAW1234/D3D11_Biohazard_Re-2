#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CShutter final : public CInteractProps
{
public:
	enum Shutter_Type
	{
		SHUTTER_NORMAL,
		SHUTTER_033,
		SHUTTER_034,
	};
	enum SHUTTER_NORMAL_ANIM
	{
		SHUTTER_OPEN,
		SHUTTER_OPEN_STATIC,
		SHUTTER_STATIC,

	};
	enum SHUTTER_033_ANIM
	{
		SHTTER_033_CLOSED,
		SHTTER_033_FULL_CLOSED,
		SHTTER_033_FULL_OPEN,
		SHTTER_033_FULL_OPENED,
		SHTTER_033_HALF_CLOSED,
		SHTTER_033_HALF_OPEN,
		SHTTER_033_HALF_OPENED,
	};
	enum SHUTTER_034_ANIM
	{
		SHTTER_034_CLOSED,
		SHTTER_034_END,
		SHTTER_034_OPEN,
		SHTTER_034_OPENED,
		SHTTER_034_START,
	};
	enum SHUTTER_PART
	{
		PART_BODY,
		PART_LEVER,
		PART_END,
	};
private:
	CShutter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShutter(const CShutter& rhs);
	virtual ~CShutter() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;
	virtual HRESULT				Bind_ShaderResources() override;
public:
	virtual _float4 Get_Object_Pos() override;
private:
	void Active();



private:
	_bool				m_bActive = { false };
	_float			m_fTime = { 0.f };
	_ubyte			m_eType = { SHUTTER_NORMAL };
	_ubyte			m_eNormalState = { SHUTTER_STATIC };
	_ubyte			m_e033State = { SHTTER_033_FULL_OPENED };
	_ubyte			m_e034State = { SHTTER_034_OPEN };


public:
	static CShutter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END