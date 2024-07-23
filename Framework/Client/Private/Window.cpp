#include "stdafx.h"
#include "Window.h"
#include"Player.h"
#include"CustomCollider.h"
#include"Body_Window.h"
#include"Pannel_Window.h"
#include"Camera_Gimmick.h"

CWindow::CWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CWindow::CWindow(const CWindow& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CWindow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWindow::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	m_iNeedItem = woodbarricade01a;

	if (FAILED(m_pGameInstance->Add_Object_Sound(m_pTransformCom, 2)))
		return E_FAIL;
	m_pGameInstance->Set_Volume_3D(m_pTransformCom,0,0.1f);
	m_pGameInstance->Set_Volume_3D(m_pTransformCom, 1, 0.1f);
	m_pGameInstance->Set_Distance_3D(m_pTransformCom, 0, 1.f, 10.f);
	m_pGameInstance->Set_Distance_3D(m_pTransformCom, 1, 1.f, 10.f);
	return S_OK;
}

void CWindow::Start()
{
	__super::Start();
	list<CGameObject*>* pCollider = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Jombie_Collider"));
	if (pCollider == nullptr)
		return;
	for (auto& iter : *pCollider)
	{
		if (m_pColliderCom[INTER_COL_NORMAL][COL_STEP0]->Intersect(static_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider")))))
		{
			// 내 인덱스 넣어주기
			_int* iNum = static_cast<CCustomCollider*>(iter)->Node_InteractProps();
			*iNum = m_tagPropDesc.iIndex;
			
			//	 ++ 참조
			m_pMyCustomCollider = static_cast<CCustomCollider*>(iter);
			Safe_AddRef(m_pMyCustomCollider);
			break;
		}
	}

#ifdef _DEBUG
	if (nullptr == m_pMyCustomCollider)
	{
		MSG_BOX(TEXT("Called : void CWindow::Start() Custom Collider 못 찾 음"));
	}
#endif
}

void CWindow::Tick(_float fTimeDelta)
{
	__super::Tick_Col();
	
	if (!m_bVisible)
		return;

	if (m_pPlayer == nullptr)
		return;

	if (m_fDelayLockTime > 0.f)
		m_fDelayLockTime -= fTimeDelta;
	if (m_fDelayLockTime < 0.f)
	{
		m_fDelayLockTime = 0.f;
		m_isCamera_Reset = false;
		Reset_Camera();
		m_bCamera = false;
	}

	_bool bCam = { false };
	if (m_bCamera&& DOWN == m_pGameInstance->Get_KeyState(VK_ESCAPE))
	{
		bCam = true;
	}

	if (m_isCamera_Reset)
		bCam = true;
	if (m_bCamera && (bCam))
	{
		if (bCam&&m_bBarrigate)
		{
			if (m_fDelayLockTime == 0.f)
				m_fDelayLockTime = 2.f;
			 
		}
		else if (bCam )
		{
			Reset_Camera();
			m_bCamera = false;
		}
	}
	
	if (m_iHP[PART_BODY] <= 0)
		m_eState = WINDOW_BREAK;

	if (BARRIGATE_STATIC == m_eBarrigateState && m_iHP[PART_PANNEL] <= 0)
		m_eBarrigateState = BARRIGATE_BREAK;

	if (BARRIGATE_BREAK == m_eBarrigateState)
	{
		m_bBarrigateInstallable = true;
		m_bBarrigate = false;
	}

	if (m_bBarrigate)
		m_fBarrigateOldTime += fTimeDelta;
	else
		m_fBarrigateOldTime = 0.f;

	if (BARRIGATE_NEW == m_eBarrigateState && m_fBarrigateOldTime > 1000000.f && m_iHP[PART_PANNEL] != 0)
		m_eBarrigateState = BARRIGATE_STATIC;

	if (true == m_bBarrigateInstallable &&( m_bCol[INTER_COL_NORMAL][COL_STEP1] || m_bCol[INTER_COL_DOUBLE][COL_STEP1]))
		if ((*m_pPlayerInteract))
			Active();

	if (m_bCamera)
		Camera_Active(PART_BODY, _float3(0.1f, 0.15f, -1.f), INTERACT_GIMMICK_TYPE::KEY_GIMMICK, _float4(0.f, 1.5f, 0.f, 1.f));

	__super::Tick(fTimeDelta);
}

void CWindow::Late_Tick(_float fTimeDelta)
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
	if (Activate_Col(Get_Collider_World_Pos(_float4(-10.f, 1.f, 0.f, 1.f))))
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

HRESULT CWindow::Render()
{
	return S_OK;
}

HRESULT CWindow::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(150.f);
	ColliderDesc.vCenter = _float3(-10.f, 1.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = _float(100.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWindow::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CWindow::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_Window::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pSoundCueSign = &m_bSoundCueSign;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CWindow::PART_BODY] = pBodyObj;

	/*PART_PANNEL*/

	CPartObject* pPannel = { nullptr };
	CBody_Window::PART_INTERACTPROPS_DESC PannelDesc = {};
	PannelDesc.pSoundCueSign = &m_bSoundCueSign;
	PannelDesc.pParentsTransform = m_pTransformCom;
	PannelDesc.pState = &m_eBarrigateState;
	pPannel = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PannelWindow"), &PannelDesc));
	if (nullptr == pPannel)
		return E_FAIL;

	m_PartObjects[CWindow::PART_PANNEL] = pPannel;







	return S_OK;
}

HRESULT CWindow::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CWindow::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

void CWindow::Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;
	if(false == m_pGameInstance->IsPaused())
		m_pPlayer->Interact_Props(this);

	m_pGameInstance->Active_Camera(g_Level, m_pCameraGimmick);

	//tabwindow의 힘을 빌려야 할 차례입니다
	m_bCamera = true;
	//m_bBarrigate = true;
	//m_bBarrigateInstallable = false;
	//m_eBarrigateState = BARRIGATE_NEW;
	//m_iHP[PART_PANNEL] = 5;
}

_float4 CWindow::Get_Object_Pos()
{
	if (!m_bBarrigateInstallable)
		return _float4(0.f, 0.f, 0.f, 1.f); // 
	return m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION)+_float4(0.f,50.f,0.f,0.f);
}

void CWindow::Do_Interact_Props()
{
	Set_Barrigate();
	m_isCamera_Reset = true;
}

CWindow* CWindow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWindow* pInstance = new CWindow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWindow"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CWindow::Clone(void* pArg)
{
	CWindow* pInstance = new CWindow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CWindow"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWindow::Free()
{
	__super::Free();

	Safe_Release(m_pMyCustomCollider);
}
