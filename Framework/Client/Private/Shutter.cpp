#include "stdafx.h"
#include "Shutter.h"
#include"Player.h"

#include"Body_Shutter.h"
CShutter::CShutter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CShutter::CShutter(const CShutter& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CShutter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShutter::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (m_tagPropDesc.strGamePrototypeName.find("60_033") != string::npos)
		m_eType = SHUTTER_033;
	else if (m_tagPropDesc.strGamePrototypeName.find("60_034") != string::npos)
		m_eType = SHUTTER_034;
	else
		m_eType = SHUTTER_NORMAL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CShutter::Tick(_float fTimeDelta)
{
	__super::Tick_Col();

	if (!m_bVisible)
		return;
	
	__super::Tick(fTimeDelta);

}

void CShutter::Late_Tick(_float fTimeDelta)
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

HRESULT CShutter::Render()
{
	return S_OK;
}

HRESULT CShutter::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(100.f);
	ColliderDesc.vCenter = _float3(-10.f, 1.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CShutter::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CShutter::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_Shutter::BODY_SHUTTER_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.eShutter_Type = &m_eType;
	BodyDesc.eNormalState = &m_eNormalState;
	BodyDesc.e033State = &m_e033State;
	BodyDesc.e034State = &m_e034State;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CShutter::PART_BODY] = pBodyObj;

	/*PART_LEVER*/
	m_PartObjects[CShutter::PART_LEVER] = nullptr;

	return S_OK;
}

HRESULT CShutter::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CShutter::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

_float4 CShutter::Get_Object_Pos()
{

	return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos();

}

void CShutter::Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;
}

CShutter* CShutter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShutter* pInstance = new CShutter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CShutter"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CShutter::Clone(void* pArg)
{
	CShutter* pInstance = new CShutter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CShutter"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShutter::Free()
{
	__super::Free();

}
