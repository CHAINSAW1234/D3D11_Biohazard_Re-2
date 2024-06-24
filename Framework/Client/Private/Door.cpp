#include "stdafx.h"
#include "Door.h"
#include "Player.h"
#include "Bone.h"
#include "PxCollider.h"

#include"Body_Door.h"
CDoor::CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CDoor::CDoor(const CDoor& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoor::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if ((m_tagPropDesc.strGamePrototypeName.find("007") != string::npos) || (m_tagPropDesc.strGamePrototypeName.find("038") != string::npos) || (m_tagPropDesc.strGamePrototypeName.find("113") != string::npos))
		m_eType = DOOR_DOUBLE;
	else
		m_eType = DOOR_ONE;
	if (m_eType == DOOR_DOUBLE)
	{
		if ((m_tagPropDesc.strGamePrototypeName.find("038") != string::npos))
			m_eDoubleDoorType = DOUBLE_DOOR_MODEL_TYPE::FRONT_DOOR;
		else
			m_eDoubleDoorType = DOUBLE_DOOR_MODEL_TYPE::NORMAL_DOOR;

	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;



	return S_OK;
}

void CDoor::Tick(_float fTimeDelta)
{
	__super::Check_Player();
	if (!m_bVisible)
	{
		m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());
		if (m_eType == DOOR_DOUBLE)
			m_pColDoubledoorCom->Tick(m_pTransformCom->Get_WorldMatrix());
		return;
	}

	if (m_pPlayer == nullptr)
		return;

	m_eType == CDoor::DOOR_ONE ? OneDoor_Tick(fTimeDelta) : DoubleDoor_Tick(fTimeDelta);

	__super::Tick(fTimeDelta);
	m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());
	if (m_eType == DOOR_DOUBLE)
		m_pColDoubledoorCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CDoor::Late_Tick(_float fTimeDelta)
{
	if (!Visible())
		return;

	if (m_bRender == false)
		return;
	__super::Late_Tick(fTimeDelta);
	m_eType == CDoor::DOOR_ONE ? OneDoor_Late_Tick(fTimeDelta) : DoubleDoor_Late_Tick(fTimeDelta);


#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[INTERACTPROPS_COL_SPHERE]);
	if(m_eType == DOOR_DOUBLE)
		m_pGameInstance->Add_DebugComponents(m_pColDoubledoorCom);
#endif
}

HRESULT CDoor::Render()
{


	return S_OK;
}

HRESULT CDoor::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(80.f);
	ColliderDesc.vCenter = _float3(-10.f, 1.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom[INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;
	if (m_eType == DOOR_DOUBLE)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc1{};

		ColliderDesc1.fRadius = _float(80.f);
		ColliderDesc1.vCenter = _float3(-180.f, 1.f, 0.f);
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Double"), (CComponent**)&m_pColDoubledoorCom, &ColliderDesc1)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDoor::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CDoor::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_Door::BODY_DOOR_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eType;
	BodyDesc.pDoubleDoorType = &m_eDoubleDoorType;
	BodyDesc.pDoubleDoorState = &m_eDoubleState;
	BodyDesc.pDoubleDoorState_Prev = &m_eDoubleState_Prev;
	BodyDesc.pOneDoorState = &m_eOneState;
	BodyDesc.pOneDoorState_Prev = &m_eOneState_Prev;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CDoor::PART_BODY] = pBodyObj;

	/*PART_LOCK*/
	m_PartObjects[CDoor::PART_LOCK] = nullptr;


	return S_OK;
}

HRESULT CDoor::Initialize_PartObjects()
{

	return S_OK;
}

void CDoor::DoubleDoor_Tick(_float fTimeDelta)
{

	if (m_bActive)
		m_fTime += fTimeDelta;

	if (m_fTime > 4.f)
	{
		m_fTime = 0.f;
		m_bActive = false;
		m_eDoubleState = DOUBLEDOOR_STATIC;
	}

	if ((m_bCol||m_bDoubleCol) && !m_bActive)
	{
		//UI띄우고
		if (*m_pPlayerInteract)
			DoubleDoor_Active();
		m_bCol = false;
		m_bDoubleCol = false;
	}

	m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pColDoubledoorCom->Tick(m_pTransformCom->Get_WorldMatrix());

}

void CDoor::DoubleDoor_Late_Tick(_float fTimeDelta)
{
	if (m_eDoubleDoorType == DOUBLE_DOOR_MODEL_TYPE::FRONT_DOOR)
	{
		switch (m_eDoubleState)
		{
		case CDoor::LSIDE_DOUBLEDOOR_OPEN_L:
		{
			m_eDoubleState_Prev = m_eDoubleState;

			break;

		}
		case CDoor::LSIDE_DOUBLEDOOR_OPEN_R:
		{
			m_eDoubleState_Prev = m_eDoubleState;

			break;

		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_L:
		{
			m_eDoubleState_Prev = m_eDoubleState;

			break;

		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_R:
		{
			m_eDoubleState_Prev = m_eDoubleState;

			break;

		}
		case CDoor::DOUBLEDOOR_STATIC:
		{
			break;
		}
		}

	}
	else
	{
		switch (m_eDoubleState)
		{
		case CDoor::LSIDE_DOUBLEDOOR_OPEN_L:
		{
			m_eDoubleState_Prev = m_eDoubleState;
			break;
		}
		case CDoor::LSIDE_DOUBLEDOOR_OPEN_R:
		{
			m_eDoubleState_Prev = m_eDoubleState;

			break;
		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_L:
		{
			m_eDoubleState_Prev = m_eDoubleState;

			break;
		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_R:
		{
			m_eDoubleState_Prev = m_eDoubleState;

			break;
		}
		case CDoor::DOUBLEDOOR_STATIC:
		{
			break;
		}

		}
	}

	Check_Col_Sphere_Player(); // 여긴 m_bCol 을 true로만 바꿔주기 때문에 반드시 false를 해주는 부분이 있어야함

	CCollider* pPlayerCol = static_cast<CCollider*>(m_pPlayer->Get_Component(TEXT("Com_Collider")));
	if (pPlayerCol->Intersect(m_pColDoubledoorCom))
		m_bDoubleCol = true;
}
void CDoor::DoubleDoor_Active()
{

	*m_pPlayerInteract = false;
	m_bActive = true;
	_vector vLook = XMVector4Normalize(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
	_vector vDir = XMVector4Normalize(m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	_float fScala = XMVectorGetX(XMVector4Dot(vLook, vDir));
	if (fScala > 1.f)
		fScala = 1.f;
	else if (fScala < -1.f)
		fScala = -1.f;

	if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
	{
		if (m_bCol && m_bDoubleCol)
			m_eDoubleState = LSIDE_DOUBLEDOOR_OPEN;
		else if (m_bCol)
			m_eDoubleState = LSIDE_DOUBLEDOOR_OPEN_L;
		else
			m_eDoubleState = RSIDE_DOUBLEDOOR_OPEN_R;

	}
	else
	{
		if (m_bCol && m_bDoubleCol)
			m_eDoubleState = RSIDE_DOUBLEDOOR_OPEN;
		else if (m_bCol)
			m_eDoubleState = LSIDE_DOUBLEDOOR_OPEN_R;
		else
			m_eDoubleState = RSIDE_DOUBLEDOOR_OPEN_L;
	}
	

}

void CDoor::OneDoor_Tick(_float fTimeDelta)
{

	if (m_bActive)
		m_fTime += fTimeDelta;

	if (m_fTime > 4.f)
	{
		m_fTime = 0.f;
		m_bActive = false;
		m_eOneState = ONEDOOR_STATIC;
	}

	if (m_bCol && !m_bActive)
	{
		//UI띄우고
		if (*m_pPlayerInteract)
			OneDoor_Active();
		m_bCol = false;
	}
	m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());

}

void CDoor::OneDoor_Late_Tick(_float fTimeDelta)
{
	switch (m_eOneState)
	{
	case CDoor::ONEDOOR_OPEN_L:
	{
		m_eOneState_Prev = m_eOneState;
		break;
	}
	case CDoor::ONEDOOR_OPEN_R:
		m_eOneState_Prev = m_eOneState;

	case CDoor::ONEDOOR_STATIC:
		break;
	}
	Check_Col_Sphere_Player(); // 여긴 m_bCol 을 true로만 바꿔주기 때문에 반드시 false를 해주는 부분이 있어야함

}

void CDoor::OneDoor_Active()
{
	*m_pPlayerInteract = false;
	m_bActive = true;
	_vector vLook = XMVector4Normalize(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
	_vector vDir = XMVector4Normalize(m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	_float fScala = XMVectorGetX(XMVector4Dot(vLook, vDir));
	if (fScala > 1.f)
		fScala = 1.f;
	else if (fScala < -1.f)
		fScala = -1.f;

	if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
		m_eOneState = ONEDOOR_OPEN_L;
	else
		m_eOneState = ONEDOOR_OPEN_R;



}


CDoor* CDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDoor* pInstance = new CDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDoor"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CDoor::Clone(void* pArg)
{
	CDoor* pInstance = new CDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDoor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoor::Free()
{
	__super::Free();
	if(m_eType ==  DOOR_DOUBLE)
		Safe_Release(m_pColDoubledoorCom);
}
