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
	CTool_Collider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_Collider() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:

private:

public:
	static CTool_Collider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END