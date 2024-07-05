#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CDamage_UI final : public CCustomize_UI
{
private:
	CDamage_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDamage_UI(const CDamage_UI& rhs);
	virtual ~CDamage_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	void				Set_Damage_Scene_Start(_bool _damage) { m_isDamage_Start = _damage; }

private :
	void				Damage_Reset();
	void				Operate_Damage(_float fTimeDelta);

private :
	_bool				m_isDamage_Start = { false }; /* 데미지를 입기 시작*/
	_float				m_fDamager_Timer = { 0.f }; /* Damage를 입는 시간 */

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END