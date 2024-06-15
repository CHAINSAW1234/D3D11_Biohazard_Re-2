#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CFace_Zombie final : public CPartObject
{
public:
	typedef struct tagFaceMonsterDesc : public CPartObject::PARTOBJECT_DESC
	{
	}FACE_MONSTER_DESC;

private:
	CFace_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFace_Zombie(const CFace_Zombie& rhs);
	virtual ~CFace_Zombie() = default;

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

private:
	HRESULT					Add_Components();
	HRESULT					Bind_ShaderResources();

public:
	static CFace_Zombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END