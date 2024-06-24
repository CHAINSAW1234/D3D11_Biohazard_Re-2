#pragma once

#include "Client_Defines.h"
#include "Player.h"
#include "PartObject.h"

BEGIN(Engine)
class CBone;
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CWeapon final : public CPartObject
{
public:
	enum RENDERLOCATION { MOVE, MOVE_LIGHT, HOLD, HOLSTER, NONE };

public:
	typedef struct tagWeaponDesc : public CPartObject::PARTOBJECT_DESC
	{
		CPlayer::EQUIP	eEquip;
		_float4x4		fTransformationMatrices[NONE] = { XMMatrixIdentity() };
		_float4x4*		pSocket[NONE] = { nullptr };
	}WEAPON_DESC;

private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

	virtual HRESULT					Render_LightDepth_Dir()override;
	virtual HRESULT					Render_LightDepth_Point() override;
	virtual HRESULT					Render_LightDepth_Spot()override;

	RENDERLOCATION					Get_RenderLocation() { return m_eRenderLocation; }

	void							Set_RenderLocation(RENDERLOCATION eRenderLocation) { m_eRenderLocation = eRenderLocation; }

private:
	CModel*							m_pModelCom = { nullptr };
	CShader*						m_pShaderCom = { nullptr };	
	_float4x4*						m_pSocketMatrix[NONE];
	CPlayer::EQUIP					m_eEquip = { CPlayer::NONE };
	RENDERLOCATION					m_eRenderLocation = { NONE };
	_float4x4						m_fTransformationMatrices[NONE];

private:
	HRESULT							Add_Components();
	HRESULT							Bind_ShaderResources();

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END