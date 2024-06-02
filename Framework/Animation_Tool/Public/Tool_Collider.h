#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

#include "Collider.h"

BEGIN(Engine)
class CGameObject;
END

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
	CGameObject* Get_CurrentSelectCollider();
	void Set_CurrentSelectCollider(CGameObject* pStaticCollider) { m_pCurrentSelectCollider = pStaticCollider; }

private:
	CGameObject*						m_pCurrentSelectCollider = { nullptr };

public:
	static CTool_Collider* Create( void* pArg);
	virtual void Free() override;
};

END