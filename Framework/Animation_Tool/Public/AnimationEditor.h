#pragma once

#include "Tool_Defines.h"
#include "Editor.h"

#define TOOL_COLLIDER_TAG               "Tool_Collider"
#define TOOL_ANIMLIST_TAG               "Tool_AnimList"
#define TOOL_MODELSELECTOR_TAG          "Tool_ModelSelector"
#define TOOL_TRANSFORMATION_TAG         "Tool_Transformation"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tool)

class CAnimationEditor final : public CEditor
{
private:
    CAnimationEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CAnimationEditor() = default;

public:
    virtual HRESULT             Initialize(void* pArg) override;
    virtual void                Tick(_float fTimeDelta) override;
    virtual HRESULT             Render() override;

private:
    virtual HRESULT             Add_Components() override;
    virtual HRESULT             Add_Tools() override;

public:
    void                        Set_Context(CGameObject* pGameObject);

private:    /* For.AssistRendering */
    void                        Render_BoneTags();

private:
    CGameObject*                m_pContext = { nullptr };

    CTool_Collider*             m_pToolCollider = { nullptr };
    CModel_Selector*            m_pToolModelSelector = { nullptr };
    CTool_AnimList*             m_pToolAnimList = { nullptr };
    CTool_Transformation*       m_pToolTransformation = { nullptr };

    _bool                       m_isActiveRoot_XZ = { false };
    _bool                       m_isActiveRoot_Y = { false };
    _bool                       m_isActiveRoot_Rotate = { false };

    _bool                       m_isRenderBoneTags = { false };

public:
    static CAnimationEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free() override;
};

END