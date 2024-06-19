#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CClothes_Zombie final : public CPartObject
{
public:
	enum CLOTHES_TYPE{ TYPE_SHIRTS, TYPE_SHIRTS2, TYPE_SHIRTS3, TYPE_HAT, TYPE_PANTS, TYPE_END };
public:
	typedef struct tagClothesMonsterDesc : public CPartObject::PARTOBJECT_DESC
	{
		CLOTHES_TYPE			eType = { TYPE_END };
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

private:
	HRESULT					Initialize_Model();

private:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };

	CLOTHES_TYPE			m_eType = { CLOTHES_TYPE::TYPE_END };

private:
	HRESULT					Add_Components();
	HRESULT					Bind_ShaderResources();

public:
	static CClothes_Zombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END