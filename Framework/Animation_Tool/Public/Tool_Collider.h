#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

#include "Collider.h"

BEGIN(Tool)

class CTool_Collider final : public CTool
{
protected:
	CTool_Collider();
	virtual ~CTool_Collider() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	class CGameObject* Get_CurrentSelectCollider();
	void Set_CurrentSelectCollider(class CGameObject* pStaticCollider) { m_pCurrentSelectCollider = pStaticCollider; }

private:
	class CGameObject*						m_pCurrentSelectCollider = { nullptr };

public:
	static CTool_Collider* Create( void* pArg);
	virtual void Free() override;
};

END