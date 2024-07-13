#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CThrowing_Weapon_Pin final : public CGameObject
{
public:
	typedef struct Throwing_Weapon_Pin_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		CPlayer::EQUIP eEquip;
		class CTransform* pParentsTransform;
	} THROWING_WEAPON_PIN_DESC;

private:
	CThrowing_Weapon_Pin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CThrowing_Weapon_Pin(const CThrowing_Weapon_Pin& rhs);
	virtual ~CThrowing_Weapon_Pin() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	HRESULT					Render_LightDepth_Dir()override;
	HRESULT					Render_LightDepth_Point() override;
	HRESULT					Render_LightDepth_Spot()override;

public:
	void					Initiate(_float4 vPos, _float4 vDir, _float4 vLook);
private:
	CPlayer::EQUIP m_eEquip = { CPlayer::NONE };

	class CTransform* m_pParentsTransform;
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_float4 m_vDir;

	ULONGLONG				m_Explode_Time;
	ULONGLONG				m_Explode_Delay;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CThrowing_Weapon_Pin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
