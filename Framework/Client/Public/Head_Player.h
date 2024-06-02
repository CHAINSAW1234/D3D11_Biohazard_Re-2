#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CHead_Player final : public CPartObject
{
public:
	typedef struct tagHeadDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _ubyte* pState;
	}HEAD_DESC;

private:
	CHead_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHead_Player(const CHead_Player& rhs);
	virtual ~CHead_Player() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Priority_Tick(_float fTimeDelta) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

	HRESULT							Render_LightDepth_Dir()override;
	HRESULT							Render_LightDepth_Spot()override;
	HRESULT							Render_LightDepth_Point() override;

private:
	CModel*							m_pModelCom = { nullptr };
	CShader*						m_pShaderCom = { nullptr };
	CCollider*						m_pColliderCom = { nullptr };

	const _ubyte*					m_pState;

	_bool							m_bRagdoll = { false };
private:
	HRESULT							Add_Components();
	HRESULT							Bind_ShaderResources();

public:
	static CHead_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END