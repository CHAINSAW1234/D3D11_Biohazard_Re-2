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
		BARRIGATE_BREAK,
		BARRIGATE_STATIC,
		BARRIGATE_NO,
		BARRIGATE_NEW,
		BARRIGATE_END
	};
private:
	CWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWindow(const CWindow& rhs);
	virtual ~CWindow() = default;

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
	inline _int					Get_HP_Body() { return m_iHP[PART_BODY]; }
	inline _bool				Is_Set_Barrigate() { return m_bBarrigate; }
	inline class CCustomCollider*		Get_CustomCollider_Ptr() { return m_pMyCustomCollider; }

private:
	void Active();

public:

	virtual _bool		Attack_Prop(class CTransform* pTransfromCom = nullptr) override
	{
		if (m_bBarrigate &&  BARRIGATE_STATIC == m_eBarrigateState )
		{
			if (m_iHP[PART_PANNEL] <= 0)
				return true; // 좀비가 들어올 수 있는
			else
			{
				m_iHP[PART_PANNEL] -= 1;
				return false; // 좀비가 못들어오는
			}
		}
		else
		{
			if (m_iHP[PART_BODY] <= 0)
				return true;
			else
			{
				m_iHP[PART_BODY] -= 1;
				return false;
			}
			
		}


	}
	virtual _float4 Get_Object_Pos() override;

	_bool		Get_Installable_Barrigate() { return m_bBarrigateInstallable; } // 설치 가능?
	void		Set_Barrigate() { m_bBarrigate = true; m_bBarrigateInstallable = false; m_iHP[PART_PANNEL] = 10; m_eBarrigateState = BARRIGATE_NEW; } // 설치할때 호출


	virtual void Do_Interact_Props() override;


private:
	_bool				m_bBarrigateInstallable = { true };
	_bool				m_bBarrigate = { false };
	_bool				m_bCamera = { false };
	_float			m_fBarrigateOldTime = { 0.f };
	_int				m_iHP[PART_END] = {5,0};
	_bool				m_bActivity = { false };
	_float			m_fTime = { 0.f };
	_float			m_fDelayLockTime = { 0.f };
	_ubyte			m_eState = { WINDOW_STATIC };
	_ubyte			m_eBarrigateState = { BARRIGATE_NO };

	class CCustomCollider*		m_pMyCustomCollider = { nullptr };


public:
	static CWindow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END