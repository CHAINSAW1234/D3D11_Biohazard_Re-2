#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CPartObject;
END

#define	STATUS_ZOMBIE_DEFAULT_RECOGNIZE_DISTANCE		3.f
#define	STATUS_ZOMBIE_MAX_RECOGNIZE_DISTANCE			5.f
#define	STATUS_ZOMBIE_VIEW_ANGLE						XMConvertToRadians(180.f)
#define	STATUS_ZOMBIE_HEALTH							100.f
#define	STATUS_ZOMBIE_ATTACK							10.f

BEGIN(Client)

class CZombie final : public CMonster
{
public:
	typedef struct tagMonsterDesc: public GAMEOBJECT_DESC
	{
		_int Index;
	}MONSTER_DESC;

	typedef struct tagZombieStatus : public MONSTER_STATUS
	{

	}ZOMBIE_STATUS;
public:
	enum COLLIDERTYPE { COLLIDER_HEAD, COLLIDER_BODY, COLLIDER_END };

private:
	CZombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CZombie(const CZombie& rhs);
	virtual ~CZombie() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;
	virtual void						Priority_Tick(_float fTimeDelta) override;
	virtual void						Tick(_float fTimeDelta) override;
	virtual void						Late_Tick(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

public://For AIController;
	virtual void						Init_BehaviorTree_Zombie() override;
	
private:
	virtual HRESULT						Add_Components() override;
	virtual HRESULT						Bind_ShaderResources() override;
	virtual HRESULT						Add_PartObjects() override;
	virtual HRESULT						Initialize_Status() override;

public:		/* For.Collision Part */
	COLLIDER_TYPE						Get_Current_IntersectCollider() { return m_eCurrentHitCollider; }

private:	/* Initialize_PartObjects_Models */
	virtual HRESULT 					Initialize_PartModels() override;
	
private: // For AIController
	class CBlackBoard_Zombie*			m_pBlackBoard = { nullptr };

private:
	class CModel*						m_pBodyModel = { nullptr };
	COLLIDER_TYPE						m_eCurrentHitCollider = { _END };

public:
	static CZombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END