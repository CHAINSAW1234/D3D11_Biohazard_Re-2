#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CPartObject;
END

BEGIN(Client)

class CMonster : public CGameObject
{
public:
	typedef struct tagMonsterDesc: public GAMEOBJECT_DESC
	{
		_int Index;
	}MONSTER_DESC;
public:
	enum COLLIDERTYPE { COLLIDER_HEAD, COLLIDER_BODY, COLLIDER_END };
	enum PART_ID { PART_BODY, PART_FACE, PART_HAT, PART_PANTS, PART_SHIRTS, PART_END };

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;
	virtual void						Priority_Tick(_float fTimeDelta);
	virtual void						Tick(_float fTimeDelta) override;
	virtual void						Late_Tick(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	//Action
public:
	void								Move(_float4 vDir, _float fTimeDelta);

protected:	/* Update_PartObjects */
	void								Priority_Tick_PartObjects(_float fTimeDelta);
	void								Tick_PartObjects(_float fTimeDelta);
	void								Late_Tick_PartObjects(_float fTimeDelta);

public:		/* Access_PartObjects */
	CModel*								Get_Model_From_PartObject(PART_ID eID);


public://For AIController
	virtual void						Init_BehaviorTree_Zombie() {}
	void								SetState(MONSTER_STATE eState)
	{
		m_eState = eState;
	}
protected:
	CModel*								m_pModelCom = { nullptr };
	CShader*							m_pShaderCom = { nullptr };	
	CCollider*							m_pColliderCom[COLLIDER_END] = { nullptr };
	CNavigation*						m_pNavigationCom = { nullptr };
	
	_int								m_iIndex = { 0 };
	MONSTER_TYPE						m_eType = { MONSTER_TYPE::MT_DEFAULT };
	
protected: // For AIController
	_uint								m_iAIController_ID = { 0 };
	class CBehaviorTree*				m_pBehaviorTree = { nullptr };
	class CPathFinder*					m_pPathFinder = { nullptr };

	//For Move
	_bool								m_bArrived = { false };
	_float4								m_vNextTarget;
	_float4								m_vDir;

	_float3								m_vRootTranslation = {};		/* For RootMotion */

	vector<class CPartObject*>			m_PartObjects;
	
	MONSTER_STATE						m_eState = { MST_DEFAULT };

protected:
	virtual HRESULT						Add_Components();
	virtual HRESULT						Bind_ShaderResources();
	virtual HRESULT						Add_PartObjects();

protected:	/* Initialize_PartObjects_Models */
	virtual HRESULT 					Initialize_PartModels();

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END