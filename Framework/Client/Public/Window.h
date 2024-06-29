#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CWindow final : public CInteractProps
{
public:
	enum WINDOW_PART
	{
		PART_BODY,
		PART_PANNEL,
		PART_END,
	};
	enum WINDOW_STATE
	{
		WINDOW_BREAK,
		WINDOW_STATIC,
		WINDOW_END
	};
	enum BARRIGATE_STATE
	{
		BARRIGATE_NO,
		BARRIGATE_BREAK,
		BARRIGATE_STATIC,
		BARRIGATE_END
	};
private:
	CWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWindow(const CWindow& rhs);
	virtual ~CWindow() = default;

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

public:
	void		Attack_Window() { 
		if(m_bBarrigate)
			m_iHP[PART_PANNEL] -= 1;
		else
			m_iHP[PART_BODY] -= 1;

	}
	
	virtual _float4 Get_Object_Pos() override;

	_bool		Get_Installable_Barrigate() { return m_bBarrigateInstallable; } // 설치 가능?
	void		Set_Barrigate() { m_bBarrigate = true; m_bBarrigateInstallable = false; m_iHP[PART_PANNEL] = 10; } // 설치할때 호출
private:
	_bool				m_bBarrigateInstallable = { true };
	_bool				m_bBarrigate = { false };
	_int				m_iHP[PART_END] = {10,0};
	_bool				m_bActive = { false };
	_float			m_fTime = { 0.f };
	_ubyte			m_eState = { WINDOW_STATIC };



public:
	static CWindow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END