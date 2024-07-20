#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Zombie.h"

BEGIN(Engine)

class CRagdoll_Physics;

END

BEGIN(Client)

class CClothes_Zombie final : public CPartObject
{
public:
public:
	typedef struct tagClothesMonsterDesc : public CPartObject::PARTOBJECT_DESC
	{
		class CPart_Breaker_Zombie** ppPart_Breaker = { nullptr };
		_bool*						pRender = { nullptr };
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
	virtual void			SetRagdoll(_int iId, _float4 vForce, COLLIDER_TYPE eType) override;
private:
	HRESULT					Initialize_Model();

public:
	void					Set_RagDoll_Ptr(CRagdoll_Physics* pRagDoll);

private:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };

	ZOMBIE_BODY_TYPE		m_eBodyType = { ZOMBIE_BODY_TYPE::_END };
	ZOMBIE_CLOTHES_TYPE		m_eClothesType = { ZOMBIE_CLOTHES_TYPE::_END };
	_int					m_iClothesModelID = { -1 };

	CRagdoll_Physics*		m_pRagdoll = { nullptr };

private:
	_bool*					m_pRender = { nullptr };
	_bool					m_bDecal_Player = { false };

private:
	class CPart_Breaker_Zombie** m_ppPart_Breaker = { nullptr };

private:
	HRESULT					Add_Components();
	HRESULT					Bind_ShaderResources();
	HRESULT					Bind_WorldMatrix(_uint iIndex);

public:
	static CClothes_Zombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END