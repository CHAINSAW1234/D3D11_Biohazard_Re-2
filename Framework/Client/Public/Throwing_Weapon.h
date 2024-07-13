#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CThrowing_Weapon final : public CGameObject
{
public:
	typedef struct Throwing_Weapon_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		CPlayer::EQUIP eEquip;
		class CTransform*	   pParentsTransform;
	} THROWING_WEAPON_DESC;

private:
	CThrowing_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CThrowing_Weapon(const CThrowing_Weapon& rhs);
	virtual ~CThrowing_Weapon() = default;

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

	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_float4 m_vDir;
	class CTransform* m_pParentsTransform;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CThrowing_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
