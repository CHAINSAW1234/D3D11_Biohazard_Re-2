#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CLever final : public CInteractProps
{
public:
	enum LEVER_STATE
	{
		LEVER_RESET,
		LEVER_DOWN,
		LEVER_STATIC,
		LEVER_END,
	};
	enum LEVER_PART
	{
		PART_BODY,
		PART_END
	};
private:
	CLever(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLever(const CLever& rhs);
	virtual ~CLever() = default;

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

private:
	void Active();

public:
	virtual _float4 Get_Object_Pos() override;


private:
	_bool				m_bReonDesk = { false };
	_bool				m_bItemDead = { true };
	_bool				m_bObtain = { false };
	_bool				m_bOpened = { false };
	_bool				m_bLock =	{ false };
	_bool				m_bActivity = { false };
	_bool				m_bRightCol = { false };

	_ubyte			m_eState = { LEVER_RESET };

	class CShutter* m_pShutter = { nullptr };

public:
	static CLever* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END