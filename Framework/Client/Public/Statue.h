#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CStatue final : public CInteractProps
{
public:
	enum STATUE_STATE
	{
		STATE_PLAY,
		STATE_STATIC
	};
	enum STATUE_PART
	{
		PART_BODY,
		PART_PART,
		PART_END,
	};

private:
	CStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatue(const CStatue& rhs);
	virtual ~CStatue() = default;

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

private:
	void Active();



private:
	_bool				m_bActive = { false };
	_float			m_fTime = { 0.f };
	_ubyte			m_eState = { STATE_STATIC };


public:
	static CStatue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END