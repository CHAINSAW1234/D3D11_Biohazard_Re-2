#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Zombie.h"

BEGIN(Client)

class CClothes_Zombie final : public CPartObject
{
public:
public:
	typedef struct tagClothesMonsterDesc : public CPartObject::PARTOBJECT_DESC
	{
		ZOMBIE_BODY_TYPE			eBodyType = { ZOMBIE_BODY_TYPE::_END };
		ZOMBIE_CLOTHES_TYPE			eClothesType = { ZOMBIE_CLOTHES_TYPE::_END };
		_int						iClothesModelID = { -1 };
	}CLOTHES_MONSTER_DESC;

private:
	CClothes_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CClothes_Zombie(const CClothes_Zombie& rhs);
	virtual ~CClothes_Zombie() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	HRESULT					Render_LightDepth_Dir()override;
	HRESULT					Render_LightDepth_Point() override;
	HRESULT					Render_LightDepth_Spot()override;
	virtual void			Create_Cloth() override;
	virtual void			Add_RenderGroup() override;
private:
	HRESULT					Initialize_Model();

private:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };

	ZOMBIE_BODY_TYPE		m_eBodyType = { ZOMBIE_BODY_TYPE::_END };
	ZOMBIE_CLOTHES_TYPE		m_eClothesType = { ZOMBIE_CLOTHES_TYPE::_END };
	_int					m_iClothesModelID = { -1 };

private:
	HRESULT					Add_Components();
	HRESULT					Bind_ShaderResources();

public:
	static CClothes_Zombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END