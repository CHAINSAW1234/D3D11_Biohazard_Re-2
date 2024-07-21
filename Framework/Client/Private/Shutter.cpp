#include "stdafx.h"
#include "Shutter.h"
#include"Player.h"

#include"Body_Shutter.h"
#include "Call_Center.h"

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
	{
		m_eType = SHUTTER_033;
		CCall_Center::Get_Instance()->Add_Caller(this, CCall_Center::CALLER::_SM60_033);
	}
	else if (m_tagPropDesc.strGamePrototypeName.find("60_034") != string::npos)
	{
		m_eType = SHUTTER_034;
		CCall_Center::Get_Instance()->Add_Caller(this, CCall_Center::CALLER::_SM60_034);
	}
	else
		m_eType = SHUTTER_NORMAL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Object_Sound(m_pTransformCom, 1)))
		return E_FAIL;



	m_pGameInstance->Set_Volume_3D(m_pTransformCom, 0,0.8f);


	return S_OK;
}

void CShutter::Start()
{
	__super::Start();
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

	_vector vScaleVector;
	_vector vRotationQuat;
	_vector vTranslationVector;

	XMMatrixDecompose(&vScaleVector, &vRotationQuat, &vTranslationVector, m_pTransformCom->Get_WorldMatrix());

	_vector vRotationAngles = XMQuaternionRotationRollPitchYawFromVector(vRotationQuat);

	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.vRotation = vRotationQuat;
	ColliderDesc.bQuat = true;
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(200.f, 100.f, 500.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
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
	BodyDesc.pSoundCueSign = &m_bSoundCueSign;
	BodyDesc.e033State = &m_e033State;
	BodyDesc.e034State = &m_e034State;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	BodyDesc.pIsOutOfControll = &m_isOutOfControll;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CShutter::PART_BODY] = pBodyObj;


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
	return _float4();
}

void CShutter::Set_Shutter_Open_State()
{
	switch (m_eType)
	{
	case SHUTTER_NORMAL:
		m_eNormalState = SHUTTER_OPEN;
		break;
	case SHUTTER_033:
		m_e033State = SHUTTER_033_HALF_OPEN;
		break;
	case SHUTTER_034:
		m_e034State = SHUTTER_034_OPEN;
		break;
	}
}

CGameObject* CShutter::Get_PartObject(SHUTTER_PART ePartType)
{

	if (static_cast<_uint>(ePartType) >= SHUTTER_PART::PART_END)
		return nullptr;

	return m_PartObjects[static_cast<_uint>(ePartType)];
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
