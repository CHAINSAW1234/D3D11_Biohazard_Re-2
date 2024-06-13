#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CBody_Monster final : public CPartObject
{
public:
	typedef struct tagBodyMonsterDesc : public CPartObject::PARTOBJECT_DESC
	{
		_float3*			pRootTranslation = { nullptr };
	}BODY_MONSTER_DESC;

private:
	CBody_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Monster(const CBody_Monster& rhs);
	virtual ~CBody_Monster() = default;

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

	_float3*				m_pRootTranslation = { nullptr };

	_bool					m_bRagdoll = { false };
private:
	HRESULT					Add_Components();
	HRESULT					Bind_ShaderResources();

public:
	static CBody_Monster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END