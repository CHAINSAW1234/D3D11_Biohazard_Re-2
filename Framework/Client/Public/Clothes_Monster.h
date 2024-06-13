#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CClothes_Monster final : public CPartObject
{
public:
	enum CLOTHES_TYPE{ TYPE_SHIRTHS, TYPE_HAT, TYPE_PANTS, TYPE_END };
public:
	typedef struct tagClothesMonsterDesc : public CPartObject::PARTOBJECT_DESC
	{
		CLOTHES_TYPE			eType = { TYPE_END };
	}CLOTHES_MONSTER_DESC;

private:
	CClothes_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CClothes_Monster(const CClothes_Monster& rhs);
	virtual ~CClothes_Monster() = default;

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

	_bool					m_bRagdoll = { false };

	CLOTHES_TYPE			m_eType = { CLOTHES_TYPE::TYPE_END };

private:
	HRESULT					Add_Components();
	HRESULT					Bind_ShaderResources();

public:
	static CClothes_Monster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END