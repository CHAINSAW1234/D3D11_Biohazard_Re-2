#pragma once

#include "Tool_Defines.h"
#include "GUIObject.h"

BEGIN(Tool)

class CEditor abstract : public CGUIObject
{
public:
    typedef struct tagEditorDesc
    {
        class CBluePrint*       pBluePrint = { nullptr };
    }EDITOR_DESC;
protected:
    CEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CEditor() = default;

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(_float fTimeDelta) override;
    virtual HRESULT Render() override;
        
public:
    virtual HRESULT Reset() override;

protected:
    virtual HRESULT Add_Components() = 0;
    virtual HRESULT Add_Tools() override; 
    HRESULT Add_Tool(CTool** ppTool, _uint iToolType, const string& strToolTag, void* pArg = nullptr);

    //  해당 컴포넌트 태그의 컴포넌트 타입을 찾아줌
    //  컴포넌트 타입은 에디터 구현층에서 직접 enum으로 정의하기.
    COMPONENT_INFO Find_ComponentInfo(const wstring& strComponentTag, _uint iComponentType);

protected:

public:
    virtual void Free() override;
};

END