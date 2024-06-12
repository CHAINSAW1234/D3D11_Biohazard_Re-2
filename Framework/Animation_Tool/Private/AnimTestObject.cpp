#include "stdafx.h"
#include "AnimTestObject.h"
#include "AnimTestPartObject.h"

#include "Character_Controller.h"

CAnimTestObject::CAnimTestObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CAnimTestObject::CAnimTestObject(const CAnimTestObject& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CAnimTestObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAnimTestObject::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {

    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    //  m_pController = m_pGameInstance->Create_Controller(_float4(0.f, 0.f, 0.f, 1.f), &m_iIndex_CCT, this);

    return S_OK;
}

void CAnimTestObject::Priority_Tick(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        Pair.second->Priority_Tick(fTimeDelta);
    }
}

void CAnimTestObject::Tick(_float fTimeDelta)
{
    //  m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pController->GetPosition_Float4());

    for (auto& Pair : m_PartObjects)
    {
        Pair.second->Tick(fTimeDelta);
    }
}

void CAnimTestObject::Late_Tick(_float fTimeDelta)
{
   /* _vector         vMovedDirection = { XMLoadFloat3(&m_vRootTranslation) };
    m_pController->Move(vMovedDirection, fTimeDelta);*/

    _vector         vCurrentPosition = { m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) };
    _vector         vMovedDirection = { XMLoadFloat3(&m_vRootTranslation) };
    _vector         vResultPosition = { vCurrentPosition + vMovedDirection };

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vResultPosition);

    m_vRootTranslation = { 0.f, 0.f, 0.f, 0.f };

    for (auto& Pair : m_PartObjects)
    {
        Pair.second->Late_Tick(fTimeDelta);
    }
}

HRESULT CAnimTestObject::Render()
{
    return S_OK;
}

void CAnimTestObject::Priority_Tick_PartObjects(_float fTimeDelta)
{
}

void CAnimTestObject::Tick_PartObjects(_float fTimeDelta)
{
}

void CAnimTestObject::Late_Tick_PartObjects(_float fTimeDelta)
{    
}

CAnimTestPartObject* CAnimTestObject::Get_PartObject(const wstring& strPartTag)
{
    map<wstring, CAnimTestPartObject*>::iterator        iter = { m_PartObjects.find(strPartTag) };
    if (iter == m_PartObjects.end())
        return nullptr;

    return m_PartObjects[strPartTag];
}

map<wstring, class CAnimTestPartObject*>& CAnimTestObject::Get_PartObjects()
{
    return m_PartObjects;
}

HRESULT CAnimTestObject::Chanage_Componenet_PartObject(const wstring& strPartTag, CComponent* pComponent, _uint iType)
{
    if (nullptr == pComponent)
        return E_FAIL;

    map<wstring, CAnimTestPartObject*>::iterator        iter = { m_PartObjects.find(strPartTag) };
    if (iter == m_PartObjects.end())
        return E_FAIL;

    if(FAILED(m_PartObjects[strPartTag]->Chanage_Componenet(pComponent, static_cast<CAnimTestPartObject::COMPONENT_TYPE>(iType))))
        return E_FAIL;

    return S_OK;
}

HRESULT CAnimTestObject::Add_PartObject(const wstring& strPartTag)
{
    map<wstring, CAnimTestPartObject*>::iterator        iter = { m_PartObjects.find(strPartTag) };
    if (iter != m_PartObjects.end())
        return E_FAIL;

    CAnimTestPartObject::ANIMTESTPART_DESC      PartDesc = {};
    PartDesc.pRootTranslation = &m_vRootTranslation;
    PartDesc.pParentsTransform = m_pTransformCom;

    CAnimTestPartObject* pPartObject = { dynamic_cast<CAnimTestPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_AnimTestPartObject"), &PartDesc)) };
    if (nullptr == pPartObject)
        return E_FAIL;
        
    m_PartObjects[strPartTag] = pPartObject;
    Safe_AddRef(m_PartObjects[strPartTag]);

    return S_OK;
}

HRESULT CAnimTestObject::Erase_PartObject(const wstring& strPartTag)
{
    map<wstring, CAnimTestPartObject*>::iterator        iter = { m_PartObjects.find(strPartTag) };
    if (iter == m_PartObjects.end())
        return E_FAIL;

    Safe_Release(m_PartObjects[strPartTag]);
    m_PartObjects[strPartTag] = nullptr;

    m_PartObjects.erase(iter);

    return S_OK;
}

HRESULT CAnimTestObject::Link_Bone_PartObject(const wstring& strSrcPartTag, const wstring& strDstPartTag, const string& strSrcBoneTag, const string& strDstBoneTag)
{
    map<wstring, CAnimTestPartObject*>::iterator        iterSrc = { m_PartObjects.find(strSrcPartTag) };
    if (iterSrc == m_PartObjects.end())
        return E_FAIL;
    map<wstring, CAnimTestPartObject*>::iterator        iterDst = { m_PartObjects.find(strDstPartTag) };
    if (iterDst == m_PartObjects.end())
        return E_FAIL;

    CModel*             pSrcModel = { dynamic_cast<CModel*>(m_PartObjects[strSrcPartTag]->Get_Component(TEXT("Com_Model"))) };
    CModel*             pDstModel = { dynamic_cast<CModel*>(m_PartObjects[strDstPartTag]->Get_Component(TEXT("Com_Model"))) };

    if (nullptr == pSrcModel || nullptr == pDstModel)
        return E_FAIL;

    _float4x4*          pSrcBoneCombinedMatrix = { const_cast<_float4x4*>(pSrcModel->Get_CombinedMatrix(strSrcBoneTag)) };
    pDstModel->Set_Surbodinate(strDstBoneTag, true);
    pDstModel->Set_Parent_CombinedMatrix_Ptr(strDstBoneTag, pSrcBoneCombinedMatrix);

    return S_OK;
}

void CAnimTestObject::Set_RootActivePart(const wstring& strRootActivePartTag)
{
    _bool       isIncluded = { false };
    for (auto& Pair: m_PartObjects)
    {
        wstring         strPartTag = { Pair.first };
        if (strPartTag == strRootActivePartTag)
        {
            isIncluded = true;
            break;
        }
    }

    if (true == isIncluded)
    {
        m_strRootActivePartTag = strRootActivePartTag;
    }
}

HRESULT CAnimTestObject::Add_Components()
{
    return S_OK;
}

CAnimTestObject* CAnimTestObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAnimTestObject*        pInstance = { new CAnimTestObject(pDevice, pContext) };

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CAnimTestObject"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAnimTestObject::Clone(void* pArg)
{
    CAnimTestObject*        pInstance = { new CAnimTestObject(*this) };

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CAnimTestObject"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAnimTestObject::Free()
{
    __super::Free();

    for (auto& Pair : m_PartObjects)
    {
        Safe_Release(Pair.second);
        Pair.second = nullptr;
    }
    m_PartObjects.clear();
}
