#include "stdafx.h"
#include "Actor_WP4530.h"
#include "Actor_PartObject.h"

#include "Bone.h"
#include "Call_Center.h"

CActor_WP4530::CActor_WP4530(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CActor_WP4530::CActor_WP4530(const CActor& rhs)
	: CActor{ rhs }
{
}

HRESULT CActor_WP4530::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_WP4530::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	LIGHT_DESC eDesc;

	eDesc.eType = LIGHT_DESC::TYPE_SPOT;
	eDesc.bShadow = true;
	eDesc.bRender = false;

	eDesc.fRange = 7.f;
	eDesc.fCutOff = XMConvertToRadians(20.f);
	eDesc.fOutCutOff = XMConvertToRadians(50.f);

	eDesc.BelongNumVec = vector<_int>(50);

	for (size_t i = 0; i < 50; i++)
	{
		eDesc.BelongNumVec[i] = i;
	}

	eDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	eDesc.vDirection = _float4(0.f, 0.f, 1.f, 1.f);

	eDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	eDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	eDesc.vSpecular = _float4(0.4f, 0.4f, 0.4f, 1.f);


	m_pGameInstance->Add_Light(TEXT("Light_Flash2"), eDesc);

	return S_OK;
}

void CActor_WP4530::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if(true == m_PartObjects[static_cast<_uint>(ACTOR_WP4530_PART::_BODY)]->Is_Active_Sequence(m_iCurSeqLev))
		m_PartObjects[static_cast<_uint>(ACTOR_WP4530_PART::_BODY)]->Set_Render(true);
	else
		m_PartObjects[static_cast<_uint>(ACTOR_WP4530_PART::_BODY)]->Set_Render(false);
}

void CActor_WP4530::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Render_Light();
}

void CActor_WP4530::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CActor_WP4530::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_WP4530::Add_PartObjects()
{
	m_PartObjects.resize(static_cast<size_t>(ACTOR_WP4530_PART::_END));

	CActor_PartObject::ACTOR_PART_DESC			Body_Desc;
	Body_Desc.pParentsTransform = m_pTransformCom;
	Body_Desc.pRootTranslation = &m_vRootTranslation;
	Body_Desc.isBaseObject = true;
	Body_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_WP4530");
	//	Body_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_FlashLight");
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF92_WP4530_00"));
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF94_WP4530_00"));
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF95_WP4530_00"));
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF190_WP4530"));

	CActor_PartObject* pPartObject_Body = { CActor_PartObject::Create(m_pDevice, m_pContext, &Body_Desc) };
	if (nullptr == pPartObject_Body)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_WP4530_PART::_BODY)] = pPartObject_Body;

	return S_OK;
}

void CActor_WP4530::Render_Light()
{
	CModel*				pModel = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(CActor_WP4530::ACTOR_WP4530_PART::_BODY)]->Get_Component(TEXT("Com_Model"))) };

	_float4x4			pMatrix = { XMLoadFloat4x4(pModel->Get_CombinedMatrix("root")) * XMMatrixScaling(0.01f, 0.01f, 0.01f) };
	//	const _float4x4 pMatrix = pModel->Get_BonePtr(0)->Get_CombinedTransformationMatrix_Var();

	//_matrix					CurrentCombinedMatrix = { pModel->Get_CurrentKeyFrame_Root_CombinedMatrix(0) * XMMatrixScaling(0.01f, 0.01f, 0.01) * XMMatrixRotationY(XMConvertToRadians(180.f))};

	_float4x4 fMatrix;
	XMStoreFloat4x4(&fMatrix, pMatrix);
	_float4 vPos = _float4(fMatrix._41, fMatrix._42, fMatrix._43, 1.f);


	const LIGHT_DESC* eDesc = m_pGameInstance->Get_LightDesc(TEXT("Light_Flash2"), 0);
	LIGHT_DESC eNewDesc = *eDesc;
	eNewDesc.vDirection = XMVectorSetW(XMVector3TransformNormal(XMVectorSet(0.f, -1.f, 0.f, 0.f), fMatrix), 0.f);
	eNewDesc.vPosition = vPos + eNewDesc.vDirection * 10;
	//eNewDesc.vDirection = XMVectorSetW(-m_WorldMatrix.Forward(), 0.f);


	eNewDesc.bRender = true;
	eNewDesc.bShadow = true;

	m_pGameInstance->Update_Light(TEXT("Light_Flash2"), eNewDesc, 0);
	m_pGameInstance->Add_ShadowLight(CPipeLine::SPOT, TEXT("Light_Flash2"));
}

CActor_WP4530* CActor_WP4530::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActor_WP4530* pInstance = new CActor_WP4530(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CActor_WP4530"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CActor_WP4530::Clone(void* pArg)
{
	CActor_WP4530* pInstance = new CActor_WP4530(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CActor_WP4530"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_WP4530::Free()
{
	__super::Free();
}
