#include "stdafx.h"
#include "BigStatue.h"
#include"Player.h"

#include "Body_BigStatue.h"
#include"Mini_BigStatue.h"
#include"Medal_BigStatue.h"
#include "Camera_Gimmick.h"


CBigStatue::CBigStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CBigStatue::CBigStatue(const CBigStatue& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CBigStatue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBigStatue::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (m_tagPropDesc.strObjectPrototype.find(TEXT("182")) != wstring::npos)
		m_eType = BIGSTATUE_WOMEN;
	else if(m_tagPropDesc.strObjectPrototype.find(TEXT("183")) != wstring::npos)
		m_eType = BIGSTATUE_LION;
	else
		m_eType = BIGSTATUE_UNICON;



	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Initialize_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CBigStatue::Tick(_float fTimeDelta)
{
	__super::Tick_Col();

	if (!m_bVisible)
		return;

	__super::Tick(fTimeDelta);

}

void CBigStatue::Late_Tick(_float fTimeDelta)
{
	if (m_pPlayer == nullptr)
		return;
	if (!Visible())
		return;

	if (m_bRender == false)
		return;
	else
	{
		for (auto& it : m_PartObjects)
		{
			if (it != nullptr)
				it->Set_Render(true);
		}

		m_bRender = false;
	}
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	__super::Add_Col_DebugCom();
#endif
}

HRESULT CBigStatue::Render()
{
	return S_OK;
}

HRESULT CBigStatue::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(150.f);
	ColliderDesc.vCenter = _float3(-10.f, 1.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = _float(100.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBigStatue::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CBigStatue::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_BigStatue::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;
	m_PartObjects[CBigStatue::PART_BODY] = pBodyObj;


	/* Part_Mini_Statue	Part_Medal_BigStatue		Part_Dial*/
	CPartObject* pMini= { nullptr };
	CMini_BigStatue::PART_INTERACTPROPS_DESC MiniDesc = {};
	MiniDesc.pParentsTransform = m_pTransformCom;
	MiniDesc.pState = &m_eState;	
	
	CPartObject* pPart= { nullptr };
	CMedal_BigStatue::MEDAL_BIGSTATUE_DESC MedalDesc = {};
	MedalDesc.pState = &m_eState;
	
	switch (m_eType)
	{
	case BIGSTATUE_UNICON:
		MiniDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_180_pushstatue01a_Mini_Anim");
		MedalDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm73_102_unicornmedal01a");



		break;
	case BIGSTATUE_WOMEN:
		MiniDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_182_womanstatue01a_Mini_Anim");
		MedalDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm73_139_virginmedal01a");
		break;

	case BIGSTATUE_LION:
		MiniDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_183_lionstatue01a_Mini_Anim");
		MedalDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm73_145_virginmedal02a");


		break;
	}


	/* Part_Mini_Statue*/
	pMini = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Mini_BigStatue"), &MiniDesc));
	if (nullptr == pMini)
		return E_FAIL;
	m_PartObjects[CBigStatue::PART_MINI_STATUE] = pMini;


	MedalDesc.pParentsTransform =m_pTransformCom;
	MedalDesc.pParentWorldMatrix = static_cast<CMedal_BigStatue*>(pMini)->Get_WorldMatrix_Ptr();

	/* Part_Medal_BigStatue*/
	pPart = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Medal_BigStatue"), &MedalDesc));
	if (nullptr == pPart)
		return E_FAIL;
	m_PartObjects[CBigStatue::PART_MEDAL] = pPart;

	/* Part_Medal_Medal*/

	

	return S_OK;
}

HRESULT CBigStatue::Initialize_PartObjects()
{
	if (m_PartObjects[PART_BODY] != nullptr)
	{
		CModel* pBodyModel = { dynamic_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Body_Model"))) };

		CMini_BigStatue* pMiniStatue = dynamic_cast<CMini_BigStatue*>(m_PartObjects[PART_MINI_STATUE]);
		_float4x4* pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("ItemSet")) };
		pMiniStatue->Set_Socket(pCombinedMatrix);


		CModel* pMiniModel = { dynamic_cast<CModel*>(m_PartObjects[PART_MINI_STATUE]->Get_Component(TEXT("Com_Body_Model"))) };
		CMedal_BigStatue* pMedalStatue = dynamic_cast<CMedal_BigStatue*>(m_PartObjects[PART_MEDAL]);
		pCombinedMatrix = { const_cast<_float4x4*>(pMiniModel->Get_CombinedMatrix("ItemSet01")) };
		pMedalStatue->Set_Socket(pCombinedMatrix);

	}

	return S_OK;
}

HRESULT CBigStatue::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

void CBigStatue::Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;

}

_float4 CBigStatue::Get_Object_Pos()
{
	return _float4();
}

CBigStatue* CBigStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBigStatue* pInstance = new CBigStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBigStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CBigStatue::Clone(void* pArg)
{
	CBigStatue* pInstance = new CBigStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBigStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBigStatue::Free()
{
	__super::Free();

}
