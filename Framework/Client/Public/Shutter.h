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
		SHUTTER_STATIC,
	};
	enum SHUTTER_033_ANIM
	{
		SHUTTER_033_CLOSED,
		SHUTTER_033_FULL_CLOSED,
		SHUTTER_033_FULL_OPEN,
		SHUTTER_033_FULL_OPENED,
		SHUTTER_033_HALF_CLOSED,
		SHUTTER_033_HALF_OPEN,
		SHUTTER_033_HALF_OPENED,
	};
	enum SHUTTER_034_ANIM
	{
		SHUTTER_034_CLOSED,
		SHUTTER_034_END,
		SHUTTER_034_OPEN,
		SHUTTER_034_OPENED,
		SHUTTER_034_START,
	};
	enum SHUTTER_PART
	{
		PART_BODY,
		PART_END,
	};
private:
	CShutter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShutter(const CShutter& rhs);
	virtual ~CShutter() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					Start() override;
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
	void								Set_Shutter_Open_State();


private:
	void Active();



private:
	_bool				m_bActivity = { false };
	_float			m_fTime = { 0.f };
	_ubyte			m_eType = { SHUTTER_NORMAL };
	_ubyte			m_eNormalState = { SHUTTER_STATIC };
	_ubyte			m_e033State = { SHUTTER_033_FULL_CLOSED };
	_ubyte			m_e034State = { SHUTTER_034_CLOSED };


public:
	static CShutter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END