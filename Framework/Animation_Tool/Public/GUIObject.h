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

    //  �ؽ�ó��.. ���ο� ������Ʈ�� �߰��ϸ� �ش� �±׿� �� �߰��ϰ� ��
    //  �� ���� Ŭ�������� enum���� �����ؼ� ���� �ε��� ����ϱ�
    map<const wstring, vector<COMPONENT_INFO>>      m_ComponentInfos;

    //  ImGui â�� �⺻���� ������ ������Ʈȭ �ؼ� ���������� �ۼ��ϱ�
    _uint                                           m_iNumTools = { 0 };
    map<const string, CTool*>                       m_Tools;


public:
    virtual void Free() override;
};

END