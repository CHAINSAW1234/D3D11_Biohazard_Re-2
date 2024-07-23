#include "stdafx.h"
#include "BigStatue.h"
#include"Player.h"

#include "Body_BigStatue.h"
#include"Mini_BigStatue.h"
#include"Medal_BigStatue.h"
#include"Dial_BigStatue.h"
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
	{
		m_iPassWord[0] = 1; //여자
		m_iPassWord[1] = 2;//활
		m_iPassWord[2] = 0;//뱀
		m_eType = BIGSTATUE_WOMAN;
	}
	else if(m_tagPropDesc.strObjectPrototype.find(TEXT("183")) != wstring::npos)
	{
		m_iPassWord[0] = 4;
		m_iPassWord[1] = 2;
		m_iPassWord[2] = 5;
		m_eType = BIGSTATUE_LION;
	}
	else
	{
		m_iPassWord[0] = 3;
		m_iPassWord[1] = 3;
		m_iPassWord[2] = 2;
		m_eType = BIGSTATUE_UNICON;
	}

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

	if (m_fDelayLockTime > 0.f)
	{
		m_fDelayLockTime -= fTimeDelta;
		Camera_Active(PART_MEDAL, _float3(0.15f, -0.1f, -0.5f), CInteractProps::INTERACT_GIMMICK_TYPE::LOCK_GIMMICK);
	}
	if (m_fDelayLockTime < 0.f)
	{
		m_fDelayLockTime = 0.f;
		m_bCameraReset = true;
	}

	_bool bCam = false;
	if (m_eLockState == CBigStatue::LIVE_LOCK)
	{
		if (DOWN == m_pGameInstance->Get_KeyState('D'))
			m_eKeyInput = KEY_D;
		else if (DOWN == m_pGameInstance->Get_KeyState('A'))
			m_eKeyInput = KEY_A;
		else if (DOWN == m_pGameInstance->Get_KeyState('W'))
			m_eKeyInput = KEY_W;
		else if (DOWN == m_pGameInstance->Get_KeyState('S'))
			m_eKeyInput = KEY_S;
		if (DOWN == m_pGameInstance->Get_KeyState(VK_RBUTTON))
		{
			if (m_eLockState != CBigStatue::CLEAR_LOCK)
				m_eLockState = CBigStatue::STATIC_LOCK;
			bCam = true;
		}

	}
	else
		m_eKeyInput = KEY_NOTHING;

	if (m_bCamera && (bCam || static_cast<CDial_BigStatue*>(m_PartObjects[PART_DIAL])->Get_Clear() || m_bCameraReset))
	{
		if (!bCam&&!m_bCameraReset)
		{
			if (m_eLockState == CBigStatue::CLEAR_LOCK && m_fDelayLockTime == 0.f)
				m_fDelayLockTime = 5.f;
		}
		else if (bCam|| m_bCameraReset)
		{
			m_bCameraReset = false;
			Reset_Camera();
			m_bCamera = false;
		}
	}
	
	if (m_bCol[INTER_COL_NORMAL][COL_STEP0] /*&& !m_bActivity*/|| m_bAutoOpen)
	{
		if (*m_pPlayerInteract|| m_bAutoOpen)
			Active();
	}

	if (m_bCamera && m_eLockState != CLEAR_LOCK)
		Camera_Active(PART_DIAL, _float3(0.15f, -0.1f, -0.5f), CInteractProps::INTERACT_GIMMICK_TYPE::LOCK_GIMMICK);

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

	if (Activate_Col(Get_Collider_World_Pos(_float4(50.f, 1.f, 50.f, 1.f))) || Activate_Col(Get_Collider_World_Pos(_float4(-50.f, 1.f, 50.f, 1.f))))
	{
		if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0))
			Check_Col_Player(INTER_COL_NORMAL, COL_STEP1);
		else
			m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
	}
	else
	{
		m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
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

void CBigStatue::Do_Interact_Props()
{
	//메달 숨기기

}

HRESULT CBigStatue::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};
	ColliderDesc.fRadius = _float(150.f);
	switch (m_eType)
	{
	case CBigStatue::BIGSTATUE_LION:
	case CBigStatue::BIGSTATUE_UNICON:

		ColliderDesc.vCenter = _float3(-10.f, 1.f, 50.f);
		break;
	case CBigStatue::BIGSTATUE_WOMAN:
		ColliderDesc.vCenter = _float3(-10.f, 1.f, -50.f);
		break;
	}

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
	CMini_BigStatue::BODY_MINI_STATUE_DESC MiniDesc = {};
	MiniDesc.pParentsTransform = m_pTransformCom;
	MiniDesc.isMedalAnim = &m_isMedalAnim;
	MiniDesc.pState = &m_eState;	
	MiniDesc.eParts_Type = static_cast<_ubyte>(CMini_BigStatue::PARTS_TYPE::MINI_BODY);

	CPartObject* pMiniPart = { nullptr };
	CMini_BigStatue::BODY_MINI_STATUE_DESC MiniParts_Desc = {};
	MiniParts_Desc.pParentsTransform = m_pTransformCom;
	MiniParts_Desc.isMedalAnim = &m_isMedalAnim;
	MiniParts_Desc.pState = &m_eState;
	MiniParts_Desc.eParts_Type = static_cast<_ubyte>(CMini_BigStatue::PARTS_TYPE::MINI_PARTS);

	CPartObject* pPart= { nullptr };
	CMedal_BigStatue::MEDAL_BIGSTATUE_DESC MedalDesc = {};
	MedalDesc.pParentsTransform = m_pTransformCom;
	MedalDesc.pState = &m_eState;
	
	CPartObject* pDial= { nullptr };
	CDial_BigStatue::DIAL_BIGSTATUE_DESC DialDesc = {};
	DialDesc.pState = &m_eState;
	DialDesc.pLockState = &m_eLockState;
	DialDesc.pKeyInput = &m_eKeyInput;
	
	MedalDesc.isMedalAnim = &m_isMedalAnim;

	switch (m_eType)
	{
	case BIGSTATUE_UNICON:
		MiniParts_Desc.eMiniType = MiniDesc.eMiniType = BIGSTATUE_UNICON;
		MedalDesc.eMedelType = CMedal_BigStatue::MEDAL_TYPE::MEDAL_UNICORN;

		MiniDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_180_pushstatue01a_Mini_Anim");
		MiniParts_Desc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_180_pushstatue01a_Mini_Parts_Anim");
		MedalDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm73_102_unicornmedal01a");
		DialDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_176_hieroglyphicdiallock01a_Anim");
		break;

	case BIGSTATUE_WOMAN :
		MiniParts_Desc.eMiniType = MiniDesc.eMiniType = BIGSTATUE_WOMAN;
		MedalDesc.eMedelType = CMedal_BigStatue::MEDAL_TYPE::MEDAL_WOMAN;

		MiniDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_182_womanstatue01a_Mini_Anim");
		// MiniParts_Desc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_182_womanstatue01a_Mini_Part_Anim");
		MedalDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm73_139_virginmedal01a");
		DialDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_177_hieroglyphicdiallock01b_Anim");
		break;

	case BIGSTATUE_LION:
		MiniParts_Desc.eMiniType = MiniDesc.eMiniType = BIGSTATUE_LION;
		MedalDesc.eMedelType = CMedal_BigStatue::MEDAL_TYPE::MEDAL_LION;

		MiniDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_183_lionstatue01a_Mini_Anim");
		MiniParts_Desc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_183_lionstatue01a_Mini_Parts_Anim");
		MedalDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm73_145_virginmedal02a");
		DialDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_176_hieroglyphicdiallock01a_Anim");
		break;
	}

	/* 1. Part_Mini_Statue*/
	{
		pMini = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Mini_BigStatue"), &MiniDesc));
		if (nullptr == pMini)
			return E_FAIL;
		m_PartObjects[CBigStatue::PART_MINI_STATUE] = pMini;
	}
	
	/* 2. Mini Parts*/
	if(BIGSTATUE_WOMAN != m_eType)
	{
		MiniParts_Desc.vParts_WorldMatrix = static_cast<CMini_BigStatue*>(m_PartObjects[CBigStatue::PART_MINI_STATUE])->Get_WorldMatrix_Ptr();

		pMiniPart = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Mini_BigStatue"), &MiniParts_Desc));
		if (nullptr == pMiniPart)
			return E_FAIL;
		m_PartObjects[CBigStatue::PART_MINI_PARTS_STATUE] = pMiniPart;
	}

	/* 3. Part_Dial*/
	{
		DialDesc.pParentsTransform = m_pTransformCom;
		DialDesc.pParentWorldMatrix = static_cast<CDial_BigStatue*>(pMini)->Get_WorldMatrix_Ptr();
		DialDesc.pPassword = (_int*)m_iPassWord;
		DialDesc.eBigStatueType = m_eLockState;

		pDial = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Dial_BigStatue"), &DialDesc));
		if (nullptr == pDial)
			return E_FAIL;
		m_PartObjects[CBigStatue::PART_DIAL] = pDial;
	}
	
	/* 4. Part_Medal_BigStatue*/
	{
		MedalDesc.pParentWorldMatrix = static_cast<CMini_BigStatue*>(m_PartObjects[CBigStatue::PART_MINI_STATUE])->Get_WorldMatrix_Ptr();
		
		pPart = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Medal_BigStatue"), &MedalDesc));
		if (nullptr == pPart)
			return E_FAIL;
		m_PartObjects[CBigStatue::PART_MEDAL] = pPart;
	}

	return S_OK;
}

HRESULT CBigStatue::Initialize_PartObjects()
{
	if (m_PartObjects[PART_BODY] != nullptr)
	{
		CModel* pBodyModel = { static_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Body_Model"))) };

		CMini_BigStatue* pMiniStatue = static_cast<CMini_BigStatue*>(m_PartObjects[PART_MINI_STATUE]);
		_float4x4* pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("ItemSet")) };
		pMiniStatue->Set_Socket(pCombinedMatrix);
		
		if (m_eType == BIGSTATUE_UNICON)
		{
			CMini_BigStatue* pMiniPartsStatue = dynamic_cast<CMini_BigStatue*>(m_PartObjects[PART_MINI_PARTS_STATUE]);
			_float4x4* pCombinedMatrixParts = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("ItemSet")) };
			pMiniPartsStatue->Set_Socket(pCombinedMatrixParts);
		}

		CModel* pMiniModel = { static_cast<CModel*>(m_PartObjects[PART_MINI_STATUE]->Get_Component(TEXT("Com_Body_Model"))) };
		CMedal_BigStatue* pMedalStatue = static_cast<CMedal_BigStatue*>(m_PartObjects[PART_MEDAL]);
		pCombinedMatrix = { const_cast<_float4x4*>(pMiniModel->Get_CombinedMatrix("ItemSet01")) };
		pMedalStatue->Set_Socket(pCombinedMatrix);

		CDial_BigStatue* pDial = static_cast<CDial_BigStatue*>(m_PartObjects[PART_DIAL]);
		pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("GimmickSet")) };
		pDial->Set_Socket(pCombinedMatrix);

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

	if (m_bAutoOpen)
	{
		//여기서 아이템 겟
		/*if (false == m_pGameInstance->IsPaused())
			m_pPlayer->Interact_Props(this);*/
		m_bAutoOpen = false;
		return;
	}

	if (m_eLockState != CLEAR_LOCK)
	{
		m_eLockState = LIVE_LOCK;

		m_pGameInstance->Active_Camera(g_Level, m_pCameraGimmick);

		m_bCamera = true;

	}
	else
	{
		/*if (false == m_pGameInstance->IsPaused())
			m_pPlayer->Interact_Props(this);*/
		//여기도 아이템 겟
	}
	

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
