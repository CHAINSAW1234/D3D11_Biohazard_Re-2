#pragma once

#include "Tool_Defines.h"
#include "Editor.h"

BEGIN(Tool)

class CAnimationEditor final : public CEditor
{
private:
    CAnimationEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CAnimationEditor() = default;

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Add_Tools() override;

public:
    static CAnimationEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free() override;
};

END