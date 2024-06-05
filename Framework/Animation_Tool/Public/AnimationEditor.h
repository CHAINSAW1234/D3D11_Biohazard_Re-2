#pragma once

#include "Tool_Defines.h"
#include "Editor.h"

#define TOOL_COLLIDER_TAG               "Tool_Collider"
#define TOOL_ANIMLIST_TAG               "Tool_AnimList"
#define TOOL_MODELSELECTOR_TAG          "Tool_ModelSelector"
#define TOOL_TRANSFORMATION_TAG         "Tool_Transformation"
#define TOOL_ANIMPLAYER_TAG             "Tool_AnimPlayer"
#define TOOL_PARTOBJECT_TAG             "Tool_PartObject"
#define TOOL_BONELAYER_TAG              "Tool_BoneLayer"

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

private:
    void                        Show_PartObjectTags();

private:    /* UI 버튼 들 배치 */


private:    /* 틱이 돈 이후 모든 데이터 별도 동기화 해주는 과정 */
    void                        Update_Datas();

    void                        Update_AnimPlayer();

    void                        Set_Model_BoneLayer();

/* For.Func 버튼 기능들.... */
private:    /* For.BoneLayer */
    void                        Add_BoneLayer_AllBone();
    void                        Add_BoneLayer_ChildBones();
    void                        Add_BoneLayer_BetweenIndices();


private:    /* For.Transform Tool */
    void                        Set_Transform_TransformTool();


private:    /* For.AnimPlayer Tool */


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
    CTool_PartObject*           m_pToolPartObject = { nullptr };
    CTool_BoneLayer*            m_pToolBoneLayer = { nullptr };

    string                      m_strCurrentModelTag = { "" };
    string                      m_strCurrentBoneTag = { "" };
    string                      m_strCurrentAnimTag = { "" };
    wstring                     m_strCurrentBoneLayerTag = { TEXT("") };

    _bool                       m_isActiveRootXZ = { false };
    _bool                       m_isActiveRootY = { false };
    _bool                       m_isActiveRootRotate = { false };

    class CAnimTestObject*      m_pTestObject = { nullptr };

    _bool                       m_isRenderBoneTags = { false };

public:
    static CAnimationEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free() override;
};

END