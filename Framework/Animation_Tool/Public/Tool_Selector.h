#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

BEGIN(Tool)

class CTool_Selector abstract : public CTool
{
protected:
	CTool_Selector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_Selector() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	virtual void Free() override;
};

END