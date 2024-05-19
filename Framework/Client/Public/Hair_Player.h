#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CHair_Player final : public CPartObject
{
public:
	typedef struct tagHairDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _ubyte* pState;
	}HAIR_DESC;

private:
	CHair_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHair_Player(const CHair_Player& rhs);
	virtual ~CHair_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	const _ubyte* m_pState;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHair_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END