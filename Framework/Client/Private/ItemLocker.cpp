#include "stdafx.h"
#include "ItemLocker.h"
#include"Player.h"

#include"Body_Statue.h"

CItemLocker::CItemLocker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CItemLocker::CItemLocker(const CItemLocker& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CItemLocker::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItemLocker::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CItemLocker::Tick(_float fTimeDelta)
{
	__super::Tick_Col();
	if (!m_bVisible)
		return;

	if (m_bActivity)
	{
		// 인벤띄우기 후에 인벤이 닫기를 누르면 m_bActive=false;를 켜주면 됨.
		// 그리고 m_eState=Close로 할 것인데 이는 냅두면 예은이 처리함 false만 잘 해주세요
		return;
	}

	if (m_bCol[INTER_COL_NORMAL][COL_STEP1])
	{
		//UI띄우고
		if (*m_pPlayerInteract)
			Active();
	}

	__super::Tick(fTimeDelta);
	
}

void CItemLocker::Late_Tick(_float fTimeDelta)
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
	if (Activate_Col(Get_Collider_World_Pos(_float4(-50.f, 1.f, -50.f, 1.f))))
		if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0))
			Check_Col_Player(INTER_COL_NORMAL, COL_STEP1);
#ifdef _DEBUG
	__super::Add_Col_DebugCom();
#endif
}

HRESULT CItemLocker::Render()
{
	return S_OK;
}

HRESULT CItemLocker::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(120.f);
	ColliderDesc.vCenter = _float3(-50.f, 1.f, -50.f);

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

HRESULT CItemLocker::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CItemLocker::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_Statue::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pSoundCueSign = &m_bSoundCueSign;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CItemLocker::PART_BODY] = pBodyObj;



	return S_OK;
}

HRESULT CItemLocker::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CItemLocker::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

void CItemLocker::Active()
{
	*m_pPlayerInteract = false;
	m_eState = STATE_OPEN;
	m_bActivity = true;

}

_float4 CItemLocker::Get_Object_Pos()
{
	return _float4();
}

CItemLocker* CItemLocker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItemLocker* pInstance = new CItemLocker(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CItemLocker"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CItemLocker::Clone(void* pArg)
{
	CItemLocker* pInstance = new CItemLocker(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CItemLocker"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemLocker::Free()
{
	__super::Free();

}
