#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tool)

class CTool_EventInserter final :
    public CTool
{
public:
	typedef struct tagEventInserterDesc
	{

	}EVENTINSERTER_DESC;

private:
	CTool_EventInserter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_EventInserter() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:

private:


public:
	static CTool_EventInserter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END