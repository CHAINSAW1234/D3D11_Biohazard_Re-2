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

class CMonster abstract : public CGameObject
{
public:
	typedef struct tagMonsterDesc: public GAMEOBJECT_DESC
	{
		_int Index;
	}MONSTER_DESC;

	typedef struct tagMonsterStatus
	{
		_float				fSpeed = { 0.f };

		_float				fRecognitionRange = { 0.f };		//	시간 누적하기 시작하는 거리
		_float				fAccRecognitionTime = { 0.f };		//	Hold 누적 시간 ( 시도하기 전 )
		_float				fMaxRecognitionTime = { 0.f };		//	Hold 누적 최대 시간 ( 아직 시도하지 못할 경우 시간 누적 감소 => 미리 더 쌓기 가능하게 )

		_float				fBiteRange = { 0.f };

		_float				fLightlyHoldRange = { 0.f };		//	시간 누적하기 시작하는 거리
		_float				fAccLightlyHoldTime = { 0.f };		//	Lightly Hold 누적 시간 ( 시도하기 전 )
		_float				fTryLightlyHoldTime = { 0.f };		//	Lightly Hold 충분 조건

		_float				fTryHoldRange = { 0.f };			//	시간 누적하기 시작하는 거리
		_float				fAccHoldTime = { 0.f };				//	Hold 누적 시간 ( 시도하기 전 )
		_float				fTryHoldTime = { 0.f };				//	Hold 충분 조건

		_float				fAccCreepTime = { 0.f };
		_float				fTryStandUpTime = { 0.f };			//	Creep에서 일어나기 위한 모션을 진행할때 필요한 시간
																//	하체가 없을 시 => TurnOver로 대체

		_float				fStamina = { 0.f };
		_float				fStaminaChargingPerSec = { 0.f };
		_float				fMaxStamina = { 0.f };

		_float				fViewAngle = { 0.f };
		_float				fAttack = { 0.f };
		_float				fHealth = { 0.f };
	}MONSTER_STATUS;

public:
	enum COLLIDERTYPE { COLLIDER_HEAD, COLLIDER_BODY, COLLIDER_END };
	enum PART_ID { PART_BODY, PART_FACE, PART_FACE2, PART_FACE3, PART_HAT, PART_PANTS, PART_SHIRTS, PART_SHIRTS2, PART_SHIRTS3, PART_END };

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

public:
	_bool								Distance_Culling();

	//Action
public:
	void								Move(_float4 vDir, _float fTimeDelta);

public:
	void								Add_Root_Translation(_fvector vAdditionalTranslation);

public:		/* For.Access */
	MONSTER_STATUS*						Get_Status_Ptr() { return m_pStatus; }

protected:	/* Update_PartObjects */
	void								Priority_Tick_PartObjects(_float fTimeDelta);
	void								Tick_PartObjects(_float fTimeDelta);
	void								Late_Tick_PartObjects(_float fTimeDelta);

public:		/* Access_PartObjects */
	CModel*								Get_Model_From_PartObject(PART_ID eID);
	CPartObject*						Get_PartObject(PART_ID eID);

public://For AIController
	virtual void						Init_BehaviorTree_Zombie() {}
	void								Set_State(MONSTER_STATE eState) { m_eState = eState; }
	MONSTER_STATE						GetState() { return m_eState; }
protected:
	CModel*								m_pModelCom = { nullptr };
	CShader*							m_pShaderCom = { nullptr };	
	CCollider*							m_pColliderCom[COLLIDER_END] = { nullptr };
	CNavigation*						m_pNavigationCom = { nullptr };
	
	_int								m_iIndex = { 0 };
	MONSTER_TYPE						m_eType = { MONSTER_TYPE::MT_DEFAULT };

	//For Decal
	CCollider*							m_pColliderCom_Bounding = { nullptr };
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

	class CPlayer*						m_pPlayer = { nullptr };
	_bool								m_bRoomCulling = { false };
	_float4x4							m_WorldMat_To_Decal;
public://For Decal
	virtual void						Perform_Skinning() {}
	virtual void						Ready_Decal() {}

protected:
	MONSTER_STATUS*						m_pStatus = { nullptr };


protected:
	virtual HRESULT						Add_Components() = 0;
	virtual HRESULT						Bind_ShaderResources();
	virtual HRESULT						Add_PartObjects() = 0;
	virtual HRESULT						Initialize_Status() = 0;

protected:	/* Initialize_PartObjects_Models */
	virtual HRESULT 					Initialize_PartModels() = 0;

	private:	/* Test*/

		_int								m_iCurrentShirts = { 0 };


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END