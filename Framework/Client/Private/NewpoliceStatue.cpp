#include "stdafx.h"
#include "NewpoliceStatue.h"

#include"Body_NewpoliceStatue.h"
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

	return S_OK;
}

void CNewpoliceStatue::Tick(_float fTimeDelta)
{
	__super::Tick_Col();
	if (!m_bVisible)
		return;
//#ifdef _DEBUG
//#ifdef UI_POS
//		Get_Object_Pos();
//#endif
//#endif

	if (m_pGameInstance->Get_KeyState('U') == DOWN)
		m_eState = POLICEHALLSTATUE_0;
	if (m_pGameInstance->Get_KeyState('I') == DOWN)
		m_eState = POLICEHALLSTATUE_1;

	if (m_pGameInstance->Get_KeyState('O') == DOWN)
		m_eState = POLICEHALLSTATUE_3;
	if (m_pGameInstance->Get_KeyState('P') == DOWN)
		m_eState = POLICEHALLSTATUE_2;
	__super::Tick(fTimeDelta);
}

void CNewpoliceStatue::Late_Tick(_float fTimeDelta)
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

HRESULT CNewpoliceStatue::Render()
{

	return S_OK;
}

HRESULT CNewpoliceStatue::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(120.f);
	ColliderDesc.vCenter = _float3(0.f, -50.f, 0.f);
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

HRESULT CNewpoliceStatue::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CNewpoliceStatue::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_NewpoliceStatue::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CNewpoliceStatue::PART_BODY] = pBodyObj;

	/*Part_Item*/
	m_PartObjects[CNewpoliceStatue::PART_PART] = nullptr;

	return S_OK;
}

HRESULT CNewpoliceStatue::Initialize_PartObjects()
{

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

_float4 CNewpoliceStatue::Get_Object_Pos()
{

	
	return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos();


	return _float4();
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
