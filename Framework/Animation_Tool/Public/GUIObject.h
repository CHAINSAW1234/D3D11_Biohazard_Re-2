#pragma once

#include "Tool_Defines.h"
#include "Base.h"

/* For.ToolType */
#include "Model_Selector.h"
#include "Tool_Transformation.h"
#include "Tool_Collider.h"
#include "Tool_AnimList.h"

BEGIN(Tool)

class CGUIObject abstract: public CBase
{
public:
    typedef struct tagComponentInfo
    {
        CComponent*         pComponent = { nullptr };
        wstring             strPrototypeTag = { TEXT("") };
    }COMPONENT_INFO;

protected:
    CGUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CGUIObject() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual HRESULT Render();

protected:
    HRESULT Add_Component(LEVEL eLevel, const wstring& strPrototypeTag, const wstring& strComponentTag, void* pArg = nullptr);
    //  virtual HRESULT Add_Tool(CTool** ppTool, _uint iToolType, const string& strToolTag, void* pArg = nullptr);
    virtual HRESULT Add_Components() = 0;
    virtual HRESULT Add_Tools() = 0;

    virtual HRESULT Update_Tools(_float fTimeDelta);

protected:
    void Set_ChildSize(const string& strToolTag, CTool* pTool);

protected:
    void Hoverd_Check();

public:
    virtual HRESULT    Reset();

    void Set_Focus(_bool isFocus) {
        m_isFocus = isFocus;
    }

    _bool Get_Focus() {
        return m_isFocus;
    }

protected:
    CGameInstance*                                  m_pGameInstance = { nullptr };
    ID3D11Device*                                   m_pDevice = { nullptr };
    ID3D11DeviceContext*                            m_pContext = { nullptr };

    _bool                                           m_isFocus = { false };
    _bool                                           m_isHovered = { false };

    //  텍스처등.. 새로운 컴포넌트를 추가하면 해당 태그에 쭉 추가하게 됌
    //  각 하위 클래스에서 enum으로 정의해서 벡터 인덱스 사용하기
    map<const wstring, vector<COMPONENT_INFO>>      m_ComponentInfos;

    //  ImGui 창들 기본적인 툴들을 컴포넌트화 해서 조립식으로 작성하기
    _uint                                           m_iNumTools = { 0 };
    map<const string, CTool*>                       m_Tools;


public:
    virtual void Free() override;
};

END