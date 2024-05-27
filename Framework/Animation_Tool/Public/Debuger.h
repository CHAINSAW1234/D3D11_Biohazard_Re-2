#pragma once

#include "Tool_Defines.h"
#include "GUIObject.h"

BEGIN(Tool)

class CDebuger final : public CGUIObject
{
protected:
    CDebuger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CDebuger() = default;

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    void Test_Code();

private:
    virtual HRESULT Add_Components() { return S_OK; }
    virtual HRESULT Add_Tools() { return S_OK; }

public:
    static CDebuger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free() override;
};

END