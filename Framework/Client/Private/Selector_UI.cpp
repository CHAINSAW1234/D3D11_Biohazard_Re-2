#include "stdafx.h"
#include "Selector_UI.h"
#include "Player.h"

#define ALHPE_ZERO              _float4(0, 0, 0, 0)

#define OUT_DISTANCE            5.f
#define INTERACTIVE_DISTANCE    2.f

#define CHECK_SIZE              50.f
#define X_TYPEY                 50.f

CSelector_UI::CSelector_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CSelector_UI::CSelector_UI(const CSelector_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CSelector_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSelector_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;
    }

    /* 부모 안에 들어있지 않다면 화살표 */
    if (true == m_IsChild)
    {
        list<CGameObject*>* pUILayer= m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

        for (auto& iter : *pUILayer)
        {
            CSelector_UI* pSelector = dynamic_cast<CSelector_UI*>(iter);

            if (nullptr != pSelector && false == pSelector->m_IsChild)
            {
                CTransform* pParentTrans = dynamic_cast<CTransform*>(pSelector->Get_Component(g_strTransformTag));
                _float4 vParentPos = pParentTrans->Get_State_Float4(CTransform::STATE_POSITION);
                _float3 fParentScaled = pParentTrans->Get_Scaled();

                _float4 vCurrentChild = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                if (vCurrentChild.y < vParentPos.y - 10.f)
                {
                    m_isArrow = true;
                    m_pParent = pSelector;
                    break;
                }
            }
        }
    }

    /* Y축 얻어오기 */
    if (false == m_isArrow)
    {
        m_vXTransform = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        m_vXTransform.y += X_TYPEY;
    }

    /* 가라로 몬스터에 붙일 거임*/
    auto pLayer_Monster = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
    
    if(pLayer_Monster)
        m_pEx = pLayer_Monster->back();

    /* Tool*/
    m_fColorTimer_Limit = 1.f;
    m_iEndingType = PLAY_BUTTON::PLAY_DEFAULT;
    m_vColor[0].fBlender_Value = m_fBlending = 0.f;
    m_isPlay = false;

    /* Client*/
    m_fOriginSize = m_pTransformCom->Get_Scaled();
    m_isRender = false;

    /* 1. Texture*/
    m_wstrInteractive_Tag = m_wstrDefaultTexturComTag;
    m_wstrNonInteractive_Tag = TEXT("Prototype_Component_Texture_Perspective_Selecter_Check");

    return S_OK;
}

void CSelector_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    m_vColor[0].isBlender = m_isBlending = true;
    m_vColor[0].vColor = m_vCurrentColor = ALHPE_ZERO;
    m_vColor[0].isColorChange = m_isColorChange = true;

    /* 상호작용이 불가능할 때 : Check */
    if(false == m_isInteractive)
    {
        Render_Selector_UI(m_pEx, fTimeDelta);
       // m_pTransformCom->Set_Scaled(CHECK_SIZE, CHECK_SIZE, m_fOriginSize.z);

        /* Texture 변경 */
        if (m_wstrDefaultTexturComTag != m_wstrNonInteractive_Tag && true == m_IsChild && false == m_isArrow)
        {
            m_wstrDefaultTexturComTag = m_wstrNonInteractive_Tag;
            Change_Texture(m_wstrNonInteractive_Tag, TEXT("Com_DefaultTexture"));
        }

        if (true == m_isArrow)
            m_isRender = false;
    }

    /* 상호작용이 가능할 때 : X */
    else if(true == m_isInteractive)
    {
        Interactive_Selector_UI(m_pEx, fTimeDelta);
       // m_pTransformCom->Set_Scaled(m_fOriginSize.x, m_fOriginSize.y, m_fOriginSize.z);

        /* Texture 변경 */
        if(m_wstrDefaultTexturComTag != m_wstrInteractive_Tag && true == m_IsChild && false == m_isArrow)
        {
            m_wstrDefaultTexturComTag = m_wstrInteractive_Tag;
            Change_Texture(m_wstrInteractive_Tag, TEXT("Com_DefaultTexture"));
        }
        
        if (true == m_isArrow)
            m_isRender = true;
    }
}

void CSelector_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CSelector_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CSelector_UI::Render_Selector_UI(CGameObject* _obj, _float fTimeDelta)
{
    _float fDistance = Distance_Player(_obj);

    if (fDistance >= OUT_DISTANCE)
    {
        if (m_fBlending >= 1.f)
        {
            m_fBlending = 1.f;
            m_isRender = false;
        }
        else
            m_fBlending += fTimeDelta;
        return;
    }

    else
    {
        /* 1. 만약 Render가 false 라면 fBlending를 점점 내린다. */
        m_isRender = true;
        if (m_fBlending <= 0.f)
        {
            m_fBlending = 0.f;
        }
        else
            m_fBlending -= fTimeDelta;

        /* 2. 뷰에 따라 위치 조정 */
        CTransform* pObjTrans = dynamic_cast<CTransform*>(_obj->Get_Component(g_strTransformTag));
        _float3 fCurrentSize = {};

        _float4 vTargetPosition = pObjTrans->Get_State_Float4(CTransform::STATE_POSITION);
        _vector vViewPos = XMVector3TransformCoord(vTargetPosition, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
        _vector vProjPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

        _float2 vProjPosition = { (XMVectorGetX(vProjPos) + 1.f) * 0.5f * g_iWinSizeX, (1.f - XMVectorGetY(vProjPos)) * 0.5f * g_iWinSizeY };

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + vProjPosition.x, g_iWinSizeY * 0.5f - vProjPosition.y, 0.8f, 1.f });

        /* 3. 거리에서 멀어질 수록 Size가 커진다. */
        if (fDistance <= INTERACTIVE_DISTANCE)
        {
            /*4. 상호작용이 가능하면 이 함수를 멈추고 다른 함수로 간다*/
            fCurrentSize = m_fOriginSize;
            m_isInteractive = true;
        }

        else
            fCurrentSize *= fDistance;
    }

    /* y 축 조정*/
    if(false == m_isArrow)
    {
        if (m_vXTransform.y - X_TYPEY > m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).y)
        {
            _float4 vPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
            vPos.y = m_vXTransform.y - X_TYPEY;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        }
    }
   
}

void CSelector_UI::Interactive_Selector_UI(CGameObject* _obj, _float fTimeDelta)
{
    _float fDistance = Distance_Player(_obj);

    if (fDistance >= INTERACTIVE_DISTANCE)
        m_isInteractive = false;

    /* y 축 조정*/
    if (false == m_isArrow)
    {
        if (m_vXTransform.y < m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).y)
        {
            _float4 vPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
            vPos.y = m_vXTransform.y;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        }
    }
    else
    {
        CTransform* pParentTrans = dynamic_cast<CTransform*>(m_pParent->Get_Component(g_strTransformTag));
        _float4 vParentTrans = pParentTrans->Get_State_Float4(CTransform::STATE_POSITION);

        vParentTrans.y -= X_TYPEY;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vParentTrans);
    }
}

_float CSelector_UI::Distance_Player(CGameObject* _obj)
{
    /* Player 찾기 */
    CGameObject* pPlayerObj = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->back();
    auto pPlayer = dynamic_cast<CPlayer*>(pPlayerObj);

    if (nullptr == pPlayer)
        return _float();

    CTransform* pPlayerTrans = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag));
    CTransform* pTargetTrans = dynamic_cast<CTransform*>(_obj->Get_Component(g_strTransformTag));

    _vector vDistanceVector = pTargetTrans->Get_State_Vector(CTransform::STATE_POSITION) - pPlayerTrans->Get_State_Vector(CTransform::STATE_POSITION);
    _float fPlayer_Distance = XMVectorGetX(XMVector3Length(vDistanceVector));

    return fPlayer_Distance;
}

CCustomize_UI* CSelector_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSelector_UI* pInstance = new CSelector_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CSelector_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSelector_UI::Clone(void* pArg)
{
    CSelector_UI* pInstance = new CSelector_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CSelector_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSelector_UI::Free()
{
    __super::Free();

}
