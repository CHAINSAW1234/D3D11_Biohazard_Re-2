#pragma once

#include "Physics_Component.h"

BEGIN(Engine)

class ENGINE_DLL CCharacter_Controller : public CPhysics_Component
{
public:
	CCharacter_Controller(PxController* Controller,class CGameObject* pCharacter);
	CCharacter_Controller(const CCharacter_Controller& rhs);
	virtual ~CCharacter_Controller() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
public:
	virtual void Update() override;
public:
	static CCharacter_Controller* Create();

	virtual void Move(_float4 Dir, _float fTimeDelta) override;
	virtual void Move(PxVec3 Dir, _float fTimeDelta) override;
	virtual _float4 GetTranslation() override;
	virtual void	Release_Px() override;
	virtual _float4 GetPosition_Float4() override; 

	class CGameObject* m_pCharacter = { nullptr };
protected:

private:

public:
	virtual void Free() override;
};

END