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
#define TOOL_INVERSEKINEMATIC_TAG       "Tool_IK"
#define TOOL_EVENTINSERTER_TAG          "Tool_EventInserter"

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
    HRESULT                     Initialize_Tools();

private:
    void                        Add_PartObject_TestObject();
    void                        Update_PartObjectsModel();

private:    /* UI 버튼 들 배치 */


private:    /* 틱이 돈 이후 모든 데이터 별도 동기화 해주는 과정 */
    void                        Update_Backend_Datas();

    void                        Update_AnimPlayerTool();
    void                        Update_PartObjectTool();
    void                        Update_BoneLayerTool();

private:
    void                        Update_TestObject();

/* For.Func 버튼 기능들.... */
private:    /* For.BoneLayer */


private:    /* For.PartObject Tool */
    HRESULT                     Initialize_PartObjectTool();

private:    /* For.Transform Tool */
    void                        Set_Transform_TransformTool();


private:    /* For.AnimPlayer Tool */
    HRESULT                     Initialize_AnimPlayer();

private:    /* For.AnimList Tool */
    HRESULT                     Initialize_AnimList();


private:    /* For.AssistRendering */
    void                        Render_BoneTags();

private:
    void                        Save_Data();


private:
    CTool_Collider*             m_pToolCollider = { nullptr };
    CModel_Selector*            m_pToolModelSelector = { nullptr };
    CTool_AnimList*             m_pToolAnimList = { nullptr };
    CTool_Transformation*       m_pToolTransformation = { nullptr };
    CTool_AnimPlayer*           m_pToolAnimPlayer = { nullptr };
    CTool_PartObject*           m_pToolPartObject = { nullptr };
    CTool_BoneLayer*            m_pToolBoneLayer = { nullptr };
    CTool_IK*                   m_pToolIK = { nullptr };
    CTool_EventInserter*        m_pToolEventInserter = { nullptr };

    string                      m_strCurrentModelTag = { "" };
    string                      m_strCurrentRootBoneTag = { "" };
    string                      m_strCurrentBoneTag = { "" };
    string                      m_strCurrentAnimTag = { "" };
    wstring                     m_strCurrentBoneLayerTag = { TEXT("") };
    wstring                     m_strCurrentPartObjectTag = { TEXT("") };
    wstring                     m_strCurrentIKTag = { TEXT("") };

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