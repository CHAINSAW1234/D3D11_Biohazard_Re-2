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
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
public:
	_int			GetIndex()
	{
		return m_iIndex;
	}
	void			SetIndex(_int Index)
	{
		m_iIndex = Index;
	}
	virtual void			SetPosition(PxVec3 Pos) {}

	virtual PxVec3 GetPosition() { return PxVec3(0.f, 0.f, 0.f); }
	void			SetUpdated(_bool boolean)
	{
		m_bUpdated = boolean;
	}
	virtual void			Update() {}
	virtual _float4			GetTranslation() { return _float4(0.f, 0.f, 0.f, 1.f); }
public:
	static CPhysics_Component* Create();

protected:
	class CGameInstance* m_pGameInstance = { nullptr };

	PxController*		m_pController = { nullptr };
	PxRigidDynamic*		m_pRigid_Dynamic = { nullptr };

	_int				m_iIndex = { 0 };

	PxVec3				m_UpdatePos;
	_bool				m_bUpdated = { false };
private:
	
public:
	virtual void Free() override;
};

END