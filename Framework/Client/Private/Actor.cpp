#include "stdafx.h"
#include "Actor.h"
#include "Actor_PartObject.h"

#include "Character_Controller.h"

CActor::CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CActor::CActor(const CActor& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CActor::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CActor::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    ACTOR_DESC*             pDesc = { static_cast<ACTOR_DESC*>(pArg) };
    m_iNumParts = pDesc->iNumParts;
    m_iBasePartIndex = pDesc->iBasePartIndex;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Initialize_PartObjects()))
        return E_FAIL;

    m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);

    return S_OK;
}

void CActor::Priority_Tick(_float fTimeDelta)
{
    Priority_Tick_PartObjects(fTimeDelta);
}

void CActor::Tick(_float fTimeDelta)
{
    _vector         vCurrentPosition = { m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) };
    _vector         vMovedDirection = { XMLoadFloat3(&m_vRootTranslation) };
    _vector         vResultPosition = { vCurrentPosition + vMovedDirection };

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vResultPosition);
    m_vRootTranslation = { 0.f, 0.f, 0.f, 0.f };

    Tick_PartObjects(fTimeDelta);
}

void CActor::Late_Tick(_float fTimeDelta)
{
    Late_Tick_PartObjects(fTimeDelta);
}

HRESULT CActor::Render()
{
    return S_OK;
}

void CActor::Priority_Tick_PartObjects(_float fTimeDelta)
{
    for (auto& pPartObject : m_PartObjects)
    {
        if (nullptr == pPartObject)
            continue;

        pPartObject->Priority_Tick(fTimeDelta);
    }
}

void CActor::Tick_PartObjects(_float fTimeDelta)
{
    for (auto& pPartObject : m_PartObjects)
    {
        if (nullptr == pPartObject)
            continue;

        pPartObject->Tick(fTimeDelta);
    }
}

void CActor::Late_Tick_PartObjects(_float fTimeDelta)
{
    for (auto& pPartObject : m_PartObjects)
    {
        if (nullptr == pPartObject)
            continue;

        pPartObject->Late_Tick(fTimeDelta);
    }
}

HRESULT CActor::Set_Animation(_uint iPartType, const wstring& strAnimLayerTag, _uint iAnimIndex)
{
    if (iPartType >= m_iNumParts)
        return E_FAIL;

    return m_PartObjects[iPartType]->Set_Animation(0, strAnimLayerTag, iAnimIndex);
}

void CActor::Set_Next_Animation_Sequence()
{
    ++m_iCurSeqLev;

    for(_uint i = 0; i < m_iNumParts; ++i)
        m_PartObjects[i]->Set_Animation_Seq(0, m_iCurSeqLev);
}

void CActor::Set_Loop(_uint iPartType, _bool isLoop)
{
    if (iPartType >= m_iNumParts)
        return;

    m_PartObjects[iPartType]->Set_Loop(0, isLoop);
}

void CActor::Reset_Animations()
{
    m_iCurSeqLev = 0;

    for (auto& pPartObject : m_PartObjects)
    {
        pPartObject->Set_Animation_Seq(0, 0);
    }
}

void CActor::Set_Pause_Anim_All_Part(_bool isPause)
{
    for (_uint i = 0; i < m_iNumParts; ++i)
    {
        m_PartObjects[i]->Set_Pause_Anim(isPause);
    }
}

void CActor::Set_Render_All_Part(_bool isRender)
{
    for (_uint i = 0; i < m_iNumParts; ++i)
    {
        m_PartObjects[i]->Set_Render(isRender);
    }
}

_bool CActor::Is_Finished_Animation_Part(_uint iPartType)
{
    if (m_iNumParts <= iPartType)
        return false;

    CModel*             pPart_Model = { dynamic_cast<CModel*>(m_PartObjects[iPartType]->Get_Component(TEXT("Com_Model"))) };
    if (nullptr == pPart_Model)
        return false;

    return pPart_Model->isFinished(0);
}

_bool CActor::Is_Finished_Animation_All_Part()
{
    for (auto& pPartObject : m_PartObjects)
    {
        if (nullptr == pPartObject)
            continue;

        CModel* pPart_Model = { dynamic_cast<CModel*>(pPartObject->Get_Component(TEXT("Com_Model"))) };
        if (nullptr == pPart_Model)
            continue;

        if (false == pPart_Model->isFinished(0))
            return false;
    }

    return true;
}

void CActor::Play_Pose_FirstTick(_uint iPartIndex)
{
    m_PartObjects[iPartIndex]->Play_Pose_FirstTick();
}

void CActor::Play_Animation_PartObject(_uint iPartIndex)
{
    m_PartObjects[iPartIndex]->Play_Animation_Light(0.01f);
}

_bool CActor::Is_Finished_All_Animation_All()
{
    for (auto& pPartObject : m_PartObjects)
    {
        if (nullptr == pPartObject)
            continue;

        CModel*             pPart_Model = { dynamic_cast<CModel*>(pPartObject->Get_Component(TEXT("Com_Model"))) };
        if (nullptr == pPart_Model)
            continue;

        wstring             strCurrentAnimLayerTag = { pPart_Model->Get_CurrentAnimLayerTag(0) };
        _int                iAnimIndex = { pPart_Model->Get_CurrentAnimIndex(0) };
        _uint               iNumAnims = { static_cast<_uint>(pPart_Model->Get_Animations(strCurrentAnimLayerTag).size()) };

        if (strCurrentAnimLayerTag == TEXT("Default"))
            continue;

        //  마지막 애니메이션이 아니라면
        if (iNumAnims - 1 != iAnimIndex)
            return false;   

        if (false == pPart_Model->isFinished(0))
            return false;
    }

    return true;
}

CActor_PartObject* CActor::Get_PartObject(_uint iPartType)
{
    if (m_iNumParts <= iPartType)
        return nullptr;

    return m_PartObjects[iPartType];
}

HRESULT CActor::Initialize_PartObjects()
{
    if (m_iBasePartIndex >= m_iNumParts)
        return E_FAIL;

    CActor_PartObject*          pPartObject = { m_PartObjects[m_iBasePartIndex] };
    CModel*                     pBase_Model = { dynamic_cast<CModel*>(pPartObject->Get_Component(TEXT("Com_Model"))) };
    for (_uint i = 0; i < m_iNumParts; ++i)
    {
        if (i == m_iBasePartIndex)
            continue;

        CModel*                 pPart_Model = { dynamic_cast<CModel*>(m_PartObjects[i]->Get_Component(TEXT("Com_Model"))) };
        if (nullptr == pPart_Model)
            return E_FAIL;

        if (true == m_PartObjects[i]->Is_LinkAuto())
        {
            if (FAILED(pPart_Model->Link_Bone_Auto(pBase_Model)))
                return E_FAIL;

        }
    }

    return S_OK;
}

HRESULT CActor::Add_Components()
{
    return S_OK;
}

void CActor::Free()
{
    __super::Free();

    for (auto& pPartObject : m_PartObjects)
    {
        Safe_Release(pPartObject);
        pPartObject = nullptr;
    }
    m_PartObjects.clear();
}
