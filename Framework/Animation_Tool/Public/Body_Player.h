#pragma once

#include "Tool_Defines.h"
#include "PartObject.h"

BEGIN(Tool)

class CBody_Player final : public CPartObject
{
public:
	typedef struct tagBodyDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _ubyte* pState;
		_float3* pRootTranslation = { nullptr };
	}BODY_DESC;

private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& rhs);
	virtual ~CBody_Player() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;


private:
	void					Update_PartColliders_Debug();

private:
	HRESULT					Render_LightDepth_Dir()override;
	HRESULT					Render_LightDepth_Spot()override;
	HRESULT					Render_LightDepth_Point() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	_float3* m_pRootTranslation = { nullptr };

	vector<CCollider*>		m_PartColliders;

	const _ubyte* m_pState;

	_bool					m_bRagdoll = { false };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END