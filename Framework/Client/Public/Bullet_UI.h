#pragma once
#include "Customize_UI.h"

class CBullet_UI final : public CCustomize_UI
{
private :
	enum class BULLET_TEXT_TYPE { CURRENT_BULLET, STORE_BULLET, END_BULLET };
	struct Text
	{
		CTextBox* pText = {  };
		_bool		isFull = { false };
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
	void					Control_BulletU();
	void					Change_BulletUI();
private :
	Text					m_pTextUI[2] = {};
	_int					m_iStoreBullet = { 0 };
	_int					m_iCurrentBullet = { 0 };
	_float4					m_fFull_CurrentBullet_Transform = {  };
	_float4					m_fFull_StoreBullet_Transform = {  };
	_float4					m_fOrigin_TextColor = {};

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

