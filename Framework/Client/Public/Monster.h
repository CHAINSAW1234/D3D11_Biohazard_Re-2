#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CMonster final : public CGameObject
{
public:
	typedef struct tagMonsterDesc: public GAMEOBJECT_DESC
	{
		_int Index;
	}MONSTER_DESC;
public:
	enum COLLIDERTYPE { COLLIDER_HEAD, COLLIDER_BODY, COLLIDER_END };
private:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public://For AIController
	void						Init_BehaviorTree_Zombie();

private:
	CModel*						m_pModelCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };	
	CCollider*					m_pColliderCom[COLLIDER_END] = { nullptr };
	
	_int						m_iIndex = { 0 };
	MONSTER_TYPE				m_eType = { MONSTER_TYPE::MT_DEFAULT };
	
private: // For AIController
	_uint						m_iAIController_ID = { 0 };
	class CBehaviorTree*		m_pBehaviorTree = { nullptr };
private:
	HRESULT						Add_Components();
	HRESULT						Bind_ShaderResources();

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END