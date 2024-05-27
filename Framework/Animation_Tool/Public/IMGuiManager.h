#pragma once

#include "Tool_Defines.h"
#include "Base.h"

#include "ImGuizmo.h"

BEGIN(Tool)

class CIMGuiManager : public CBase
{
private:
    enum EDITOR_TYPE { EDITOR_ANIMATION, EDITOR_END };
    enum DEBUGER_TYPE { DEBUGER_OBJECT, DEBUGER_END };

private:
    CIMGuiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CIMGuiManager(const CIMGuiManager& rhs);
    ~CIMGuiManager() = default;

public:
    HRESULT Initialize();
    void Tick(_float fTimeDelta);
    HRESULT Render();

    void OnOffImgui() { m_isEnable = !m_isEnable; }
    _bool IsEnable() { return m_isEnable; }

private:
    HRESULT Reset_All();
    _bool   Check_LevelChange();

public:
    void Update_CurrentEditor(class CEditor* pEditor, EDITOR_TYPE eType);

private:
    _bool m_isEnable = { false };

    ID3D11Device*               m_pDevice = { nullptr };
    ID3D11DeviceContext*        m_pContext = { nullptr };

    EDITOR_TYPE                 m_CurrentEditor = { EDITOR_TYPE::EDITOR_END };

    class CEditor*              m_Editors[EDITOR_TYPE::EDITOR_END];
    class CDebuger*             m_Debugers[DEBUGER_TYPE::DEBUGER_END];

private: /* For.FrameMeter */
    _float                      m_fAccTimeForFrame = { 0.f };
    _uint                       m_iFrameCnt = { 0 };
    _uint                       m_iPreFrame = { 0 };

public:
    static CIMGuiManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

END