#include "stdafx.h"
#include "NewpoliceStatue.h"

#include"Body_NewpoliceStatue.h"
#include "Medal_NewpoliceStatue.h"

#include "Body_ItemProp.h"
#include "Camera_Gimmick.h"
#include "Player.h"
#include"Selector_UI.h"
CNewpoliceStatue::CNewpoliceStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CNewpoliceStatue::CNewpoliceStatue(const CNewpoliceStatue& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CNewpoliceStatue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNewpoliceStatue::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Initialize_PartObjects()))
		return E_FAIL;
	



	if (FAILED(m_pGameInstance->Add_Object_Sound(m_pTransformCom, 3)))
		return E_FAIL;

	m_pGameInstance->Set_Volume_3D(m_pTransformCom,0, 0.9f);

	if (m_pTransformCom->IsIdentityWorldMatrix())
	{
		m_bLocalized = true;
		m_vLocal_To_World_Pos = m_pModelCom->GetCenterPoint();
	}
	
	return S_OK;
}

void CNewpoliceStatue::Tick(_float fTimeDelta)
{
	__super::Tick_Col();
	if (!m_bVisible)
	{
		if (nullptr != m_pSelector)
		{
			m_pSelector = static_cast<CSelector_UI*>(m_pSelector->Destroy_Selector());

			m_pSelector = nullptr;
		}
		return;
	}


	_bool bCam = { false };
	if (DOWN == m_pGameInstance->Get_KeyState(VK_ESCAPE))
	{
		bCam = true;
	}
	/* 예시 코드*/
	if (m_isCamera_Reset)
		bCam = true;
	if (m_bCamera && (bCam))
	{
		Reset_Camera();
		m_bCamera = false;
	}
	



	if (m_bCol[INTER_COL_NORMAL][COL_STEP1] && !m_bActivity)
	{
		if (*m_pPlayerInteract && false == m_pGameInstance->IsPaused())
			Active();
	}
	

	if (m_bCamera)
		if ((m_fZoomOut > 0.f) && ((m_fZoomOut -= fTimeDelta) > 0.f))
			Camera_Active(PART_BODY, _float3(0.015f, 3.f, -5.55f), CInteractProps::INTERACT_GIMMICK_TYPE::KEY_GIMMICK);
		else
			Camera_Active(PART_PART3, _float3(0.015f, 0.15f, 0.85f), CInteractProps::INTERACT_GIMMICK_TYPE::KEY_GIMMICK);


	Animation_BaseOn_MedalType();

	__super::Tick(fTimeDelta);
}

void CNewpoliceStatue::Late_Tick(_float fTimeDelta)
{
	if (m_pPlayer == nullptr)
		return;

	if (!Visible())
	{

		if (nullptr != m_pSelector)
		{
			m_pSelector = static_cast<CSelector_UI*>(m_pSelector->Destroy_Selector());

			m_pSelector = nullptr;
		}
		return;
	}

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
	if (Activate_Col(Get_Collider_World_Pos(_float4(50.f, 1.f, 50.f, 1.f))) && m_iEXCode == 3)
	{
		if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0))
		{
			Check_Col_Player(INTER_COL_NORMAL, COL_STEP1);
			Opreate_Selector_UI(true, Get_Object_Pos());

		}
		else
		{
			m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
			if (nullptr != m_pSelector)
			{
				m_pSelector = static_cast<CSelector_UI*>(m_pSelector->Destroy_Selector());

				m_pSelector = nullptr;
			}
		}
	}
	else
	{
		m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;

		if (nullptr != m_pSelector)
		{
			m_pSelector = static_cast<CSelector_UI*>(m_pSelector->Destroy_Selector());

			m_pSelector = nullptr;
		}
	}
	__super::Late_Tick(fTimeDelta);


#ifdef _DEBUG
	__super::Add_Col_DebugCom();
#endif
}

HRESULT CNewpoliceStatue::Render()
{

	return S_OK;
}

HRESULT CNewpoliceStatue::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(250.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 200.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = _float(200.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CNewpoliceStatue::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CNewpoliceStatue::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_NewpoliceStatue::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pSoundCueSign = &m_bSoundCueSign;

	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CNewpoliceStatue::PART_BODY] = pBodyObj;

	/* Item Medal */
	CMedal_NewpoliceStatue::BODY_MEDAL_POLICESTATUS		MedalDesc = {};
	MedalDesc.eMedalRenderType = m_eMedalRender;
	MedalDesc.pSoundCueSign = &m_bSoundCueSign;

	/* 1. Unicorn Medal */
	CPartObject* pMedal1 = { nullptr };
	MedalDesc.pSoundCueSign = &m_bSoundCueSign;

	MedalDesc.pParentsTransform = m_pTransformCom;
	MedalDesc.pState = &m_eState; 
	MedalDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm73_102_unicornmedal01a");
	MedalDesc.eMedelType = CMedal_NewpoliceStatue::MEDAL_TYPE::MEDAL_UNICORN;
	memcpy(MedalDesc.eMedalRenderType, m_eMedalRender, sizeof(MedalDesc.eMedalRenderType));

	pMedal1 = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Medal_NewpoliceStatue"), &MedalDesc));

	if (nullptr == pMedal1)
		return E_FAIL;

	else
	{
		m_PartObjects[CNewpoliceStatue::PART_PART1] = pMedal1;
	}


	/* 2 */
	CPartObject* pMedal2 = { nullptr };
	MedalDesc.pSoundCueSign = &m_bSoundCueSign;
	MedalDesc.pParentsTransform = m_pTransformCom;
	MedalDesc.pState = &m_eState;
	MedalDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm73_145_virginmedal02a");
	MedalDesc.eMedelType = CMedal_NewpoliceStatue::MEDAL_TYPE::MEDAL_LION;
	memcpy(MedalDesc.eMedalRenderType, m_eMedalRender, sizeof(MedalDesc.eMedalRenderType));

	pMedal2 = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Medal_NewpoliceStatue"), &MedalDesc));

	if (nullptr == pMedal2)
		return E_FAIL;

	else
	{
		m_PartObjects[CNewpoliceStatue::PART_PART2] = pMedal2;
	}
	
	/* 3 */
	CPartObject* pMedal3 = { nullptr };
	MedalDesc.pSoundCueSign = &m_bSoundCueSign;
	MedalDesc.pParentsTransform = m_pTransformCom;
	MedalDesc.pState = &m_eState; 
	MedalDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm73_139_virginmedal01a");
	MedalDesc.eMedelType = CMedal_NewpoliceStatue::MEDAL_TYPE::MEDAL_WOMAN;
	memcpy(MedalDesc.eMedalRenderType, m_eMedalRender, sizeof(MedalDesc.eMedalRenderType));

	pMedal3 = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Medal_NewpoliceStatue"), &MedalDesc));

	if (nullptr == pMedal3)
		return E_FAIL;

	else
		m_PartObjects[CNewpoliceStatue::PART_PART3] = pMedal3;

	return S_OK;
}

HRESULT CNewpoliceStatue::Initialize_PartObjects()
{
	if (m_PartObjects[PART_BODY] != nullptr)
	{
		CModel* pBodyModel = { dynamic_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Body_Model"))) };
		 
		CMedal_NewpoliceStatue* pItem1 = dynamic_cast<CMedal_NewpoliceStatue*>(m_PartObjects[PART_PART1]);
		_float4x4* pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("ItemSet1")) };
		pItem1->Set_Socket(pCombinedMatrix);

		CMedal_NewpoliceStatue* pItem2 = dynamic_cast<CMedal_NewpoliceStatue*>(m_PartObjects[PART_PART2]);
		pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("ItemSet2")) };
		pItem2->Set_Socket(pCombinedMatrix);

		CMedal_NewpoliceStatue* pItem3 = dynamic_cast<CMedal_NewpoliceStatue*>(m_PartObjects[PART_PART3]);
		pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("ItemSet3")) };
		pItem3->Set_Socket(pCombinedMatrix);
	}


	return S_OK;
}


HRESULT CNewpoliceStatue::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CNewpoliceStatue::Do_Interact_Props()
{
	m_isGiveMedal = true;

	m_eMedalRender[m_iEXCode] = true;
	m_iEXCode++;
	m_fZoomOut = 2.f;
}

_float4 CNewpoliceStatue::Get_Object_Pos()
{

	
	return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos();


	return _float4();
}

void CNewpoliceStatue::Animation_BaseOn_MedalType()
{
	if(true == m_isGiveMedal)
	{
		_int iMedalCnt = { 0 };

		m_isGiveMedal = false; 

		for (_uint i = 0; i < 3; i++)
		{
			if (true == m_eMedalRender[i])
				++iMedalCnt;
		}

		if (1 == iMedalCnt)
			m_eState = POLICEHALLSTATUE_1;

		else if (2 == iMedalCnt)
			m_eState = POLICEHALLSTATUE_2;

		else if (3 == iMedalCnt)
		{
			m_eState = POLICEHALLSTATUE_3;
		}

		else
			m_eState = POLICEHALLSTATUE_0;

		if (iMedalCnt != m_iPrevMedalCnt)
		{
			m_iPrevMedalCnt = iMedalCnt;

			/* Mission 달성*/
		}
	}
}

void CNewpoliceStatue::Active()
{

	if (m_iEXCode == 0)
		m_iNeedItem = virginmedal01a;
	else if (m_iEXCode == 1)
		m_iNeedItem = unicornmedal01a;
	else
		m_iNeedItem = virginmedal02a;

	if (m_iEXCode > 3)
		m_iEXCode = 3;
	


	m_pGameInstance->Active_Camera(g_Level, m_pCameraGimmick);

	m_bCamera = true;

	m_pPlayer->Interact_Props(this);
}

CNewpoliceStatue* CNewpoliceStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNewpoliceStatue* pInstance = new CNewpoliceStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNewpoliceStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;

}
CGameObject* CNewpoliceStatue::Clone(void* pArg)
{
	CNewpoliceStatue* pInstance = new CNewpoliceStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CNewpoliceStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNewpoliceStatue::Free()
{
	__super::Free();

}
