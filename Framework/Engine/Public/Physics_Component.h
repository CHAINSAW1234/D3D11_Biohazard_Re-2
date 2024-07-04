#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPhysics_Component : public CBase
{
protected:
	CPhysics_Component();
	CPhysics_Component(const CPhysics_Component& rhs);
	virtual ~CPhysics_Component() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);
public:
	virtual void					SetPosition(PxVec3 Pos) {}
	virtual PxVec3					GetPosition() { return PxVec3(0.f, 0.f, 0.f); }
	virtual _float4					GetPosition_Float4() { return _float4(0.f, 0.f, 0.f,1.f); }
	virtual _float4					GetPosition_Float4_Zombie() { return _float4(0.f, 0.f, 0.f,1.f); }
	virtual void					Update() {}
	virtual _float4					GetTranslation() { return _float4(0.f, 0.f, 0.f, 1.f); }
	virtual PxTransform				GetTransform_Px() { return PxTransform(); }
	virtual void					Move(_float4 Dir, _float fTimeDelta) {};
	virtual void					Move(PxVec3 Dir, _float fTimeDelta) {};
	virtual void					Release_Px() {}
	_matrix							GetWorldMatrix(_int Index) { return XMMatrixIdentity(); }
	FORCEINLINE void				SetUpdated(_bool boolean)
	{
		m_bUpdated = boolean;
	}
	FORCEINLINE _int				GetIndex()
	{
		return m_iIndex;
	}
	FORCEINLINE void				SetIndex(_int Index)
	{
		m_iIndex = Index;
	}
	virtual void					Update(_float fTimeDelta) {}
	virtual void					Build_Skeleton(const std::string& name) {}
	void							SetReleased(_bool boolean)
	{
		m_bReleased = boolean;
	}
	_bool							IsReleased()
	{
		return m_bReleased;
	}
	_float4							GetBlockPoint()
	{
		return m_vBlockPoint;
	}
	void							SetBlockPoint(_float4 vPoint)
	{
		m_vBlockPoint = vPoint;
	}
	_float4							GetHitNormal()
	{
		return m_vHitNormal;
	}
	void							SetHitNormal(_float4 HitNormal)
	{
		m_vHitNormal = HitNormal;
		m_vHitNormal.w = 0.f;
	}
	void							Increase_Hit_Count()
	{
		++m_iHitCount;
	}
	_uint							Get_Hit_Count()
	{
		return m_iHitCount;
	}
	void							SetDead(_bool boolean)
	{
		m_bDead = boolean;
	}
	_bool							GetDead()
	{
		return m_bDead;
	}
	virtual void					SetPosition(_float4 vPos) {}
	void							SetObject(class CGameObject* pObject)
	{
		m_pObject = pObject;
	}
	void							SetGravity(_bool boolean)
	{
		m_bGravity = boolean;
	}
	_bool							GetbGravity()
	{
		return m_bGravity;
	}
public:
	static CPhysics_Component*		Create();

protected:
	class CGameInstance*			m_pGameInstance = { nullptr };
	PxController*					m_pController = { nullptr };
	PxRigidDynamic*					m_pRigid_Dynamic = { nullptr };
	_int							m_iIndex = { 0 };
	PxVec3							m_UpdatePos;
	_bool							m_bUpdated = { false };

	//Init
	PxPhysics*						m_Physics = nullptr;
	PxScene*						m_Scene = nullptr;

	class CEventCallBack*			m_pEventCallBack = { nullptr };
	class CFilterCallBack*			m_pFilterCallBack = { nullptr };

	_bool							m_bReleased = { false };

	_float4							m_vBlockPoint;
	_float4							m_vHitNormal;
	_uint							m_iHitCount = { 0 };
	_bool							m_bDead = { false };

	class CGameObject*				m_pObject = { nullptr };

	_bool							m_bGravity = { true };
protected:
	_int m_iRefCnt_Px = { 0 };
public:
	virtual void Free() override;
};

END