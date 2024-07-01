#pragma once
#include "Customize_UI.h"
#include "Observer.h"
BEGIN(Client)

class CBullet_UI final : public CCustomize_UI, public CObserver
{
private:
	enum class BULLET_TEXT_TYPE { CURRENT_BULLET, STORE_BULLET, END_BULLET };
	struct Text
	{
		CTextBox*	pText = {  };
		_bool		isFull = { false };
		_float4		vOriginTextColor = {};
		_int		iBulletCnt = {};
	};

private:
	CBullet_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBullet_UI(const CBullet_UI& rhs);
	virtual ~CBullet_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private :
	void					Mission_Complete();
	virtual void Start() override;
	virtual void OnNotify() override;

private:
	void					Change_BulletUI();
	void					Render_Bullet_UI(_float fTimeDelta);

	void					Find_Crosshair();

private:
	class CCrosshair_UI*	m_pCrosshair = { nullptr };

private:
	Text					m_pTextUI[2] = {};

	_int					m_iStoreBullet = { 0 };
	_int					m_iCurrentBullet = { 0 };
	_int					m_iMaxBullet = { 12 };
	_float					m_fOrigin_Blending = {};

	/* 원본 위치*/
	_float4					m_fFull_CurrentBullet_Transform = {  };
	_float4					m_fFull_StoreBullet_Transform = {  };
	_float4					m_fOrigin_TextColor = {};

	/* 시간*/
	_float					m_fBulletTimer = {};
	
	_bool					m_isKeepCross = { false };
	_bool					m_isTutiorial = { false };


public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END