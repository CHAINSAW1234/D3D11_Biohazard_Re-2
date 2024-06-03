#pragma once

#include "Tool_Defines.h"
#include "Editor.h"

#define TOOL_COLLIDER_TAG               "Tool_Collider"
#define TOOL_ANIMLIST_TAG               "Tool_AnimList"
#define TOOL_MODELSELECTOR_TAG          "Tool_ModelSelector"
#define TOOL_TRANSFORMATION_TAG         "Tool_Transformation"
#define TOOL_ANIMPLAYER_TAG             "Tool_AnimPlayer"

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
    HRESULT                     Add_TestObject();

private:
    void                        Add_PartObject_TestObject();
    void                        Change_Model_TestObject();
    void                        Update_Root_Active();

private:    /* For.Transform Tool */
    void                        Set_Transform_TransformTool();

private:    /* For.AnimPlayer Tool */
    void                        Update_AnimPlayer();

private:    /* For.AnimList Tool */
    HRESULT                     Initialize_AnimList();

private:    /* For.AssistRendering */
    void                        Render_BoneTags();

private:
    CTool_Collider*             m_pToolCollider = { nullptr };
    CModel_Selector*            m_pToolModelSelector = { nullptr };
    CTool_AnimList*             m_pToolAnimList = { nullptr };
    CTool_Transformation*       m_pToolTransformation = { nullptr };
    CTool_AnimPlayer*           m_pToolAnimPlayer = { nullptr };

    class CAnimTestObject*      m_pTestObject = { nullptr };

    _bool                       m_isActiveRoot_XZ = { false };
    _bool                       m_isActiveRoot_Y = { false };
    _bool                       m_isActiveRoot_Rotate = { false };

    _bool                       m_isRenderBoneTags = { false };

public:
    static CAnimationEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free() override;
};

END