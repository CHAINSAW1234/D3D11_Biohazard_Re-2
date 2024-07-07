#include "stdafx.h"
#include "Door.h"
#include "Player.h"
#include "Bone.h"
#include "PxCollider.h"

#include"Body_Door.h"
#include"CustomCollider.h"
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
	//m_bLock = m_tagPropDesc.tagDoor.bLock;
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

void CDoor::Start()
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
		}
	}
}

void CDoor::Tick(_float fTimeDelta)
{
	__super::Tick_Col();

	if (!m_bVisible)
		return;
#ifdef _DEBUG
#ifdef UI_POS
	Get_Object_Pos();
#endif
#endif

	m_eType == CDoor::DOOR_ONE ? OneDoor_Tick(fTimeDelta) : DoubleDoor_Tick(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CDoor::Late_Tick(_float fTimeDelta)
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
	m_eType == CDoor::DOOR_ONE ? OneDoor_Late_Tick(fTimeDelta) : DoubleDoor_Late_Tick(fTimeDelta);


#ifdef _DEBUG
	Add_Col_DebugCom();
#endif
}

HRESULT CDoor::Render()
{


	return S_OK;
}

HRESULT CDoor::Add_Components()
{
	if (m_eType == DOOR_DOUBLE)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};
		ColliderDesc.fRadius = _float(180.f);
		ColliderDesc.vCenter = _float3(-30.f, 1.f, 0.f);

		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
			return E_FAIL;

		ColliderDesc.fRadius = _float(150.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Normal_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
			return E_FAIL;

		ColliderDesc.fRadius = _float(120.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Normal_Step2"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP2], &ColliderDesc)))
			return E_FAIL;



		ColliderDesc.fRadius = _float(180.f);
		ColliderDesc.vCenter = _float3(-150.f, 1.f, 0.f);
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Double_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_DOUBLE][COL_STEP0], &ColliderDesc)))
			return E_FAIL;
		ColliderDesc.fRadius = _float(150.f);
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Double_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_DOUBLE][COL_STEP1], &ColliderDesc)))
			return E_FAIL;
		ColliderDesc.fRadius = _float(120.f);
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Double_Step2"), (CComponent**)&m_pColliderCom[INTER_COL_DOUBLE][COL_STEP2], &ColliderDesc)))
			return E_FAIL;
	}
	else
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

		ColliderDesc.fRadius = _float(150.f);
		ColliderDesc.vCenter = _float3(-60.f, 1.f, 0.f);
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
			return E_FAIL;

		ColliderDesc.fRadius = _float(110.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
			return E_FAIL;

		ColliderDesc.fRadius = _float(90.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Step2"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP2], &ColliderDesc)))
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
	
	/*PART_HANDLE*/


	return S_OK;
}

HRESULT CDoor::Initialize_PartObjects()
{

	return S_OK;
}

void CDoor::DoubleDoor_Tick(_float fTimeDelta)
{
	if (m_bActivity)
		m_fTime += fTimeDelta;

	if (m_fTime > 2.f)
	{
		m_fTime = 0.f;
		m_iHP = 5;
		if (!m_bCol[INTER_COL_NORMAL][COL_STEP1] && !m_bCol[INTER_COL_DOUBLE][COL_STEP1])
		{
			m_bActivity = false;
			m_eDoubleState = DOUBLEDOOR_STATIC;
		}
	}

	if (!m_bActivity && (m_bCol[INTER_COL_NORMAL][COL_STEP1] || m_bCol[INTER_COL_DOUBLE][COL_STEP1]))
	{
		if (*m_pPlayerInteract|| m_bCol[INTER_COL_NORMAL][COL_STEP2]|| m_bCol[INTER_COL_DOUBLE][COL_STEP2])
			DoubleDoor_Active();

		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;


		m_bCol[INTER_COL_DOUBLE][COL_STEP1] = false;
		m_bCol[INTER_COL_DOUBLE][COL_STEP2] = false;
	}

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
		case CDoor::L_DOUBLEDOOR_OPEN:
		{
			m_eDoubleState_Prev = m_eDoubleState;
			break;
		}
		case CDoor::R_DOUBLEDOOR_OPEN:
		{
			m_eDoubleState_Prev = m_eDoubleState;

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
		case CDoor::L_DOUBLEDOOR_OPEN:
		{
			m_eDoubleState_Prev = m_eDoubleState;
			break;
		}
		case CDoor::R_DOUBLEDOOR_OPEN:
		{
			m_eDoubleState_Prev = m_eDoubleState;

			break;
		}
		}
	}
	if (Activate_Col(Get_Collider_World_Pos(_float4(-90.f, 1.f, 0.f, 1.f)))|| Activate_Col(Get_Collider_World_Pos(_float4(-30.f, 1.f, 0.f, 1.f)))|| Activate_Col(Get_Collider_World_Pos(_float4(-150.f, 1.f, 0.f, 1.f))))
	{
		if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0)) // 인지?
		{
			if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP1)) // 상호작용?
			{
				if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP2) && !m_bActivity)
					m_bOnce = true;
			}
			else
				m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
		}
		else
		{
			m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
			m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
		}

		if (Check_Col_Player(INTER_COL_DOUBLE, COL_STEP0)) // 인지?
		{
			if (Check_Col_Player(INTER_COL_DOUBLE, COL_STEP1)) // 상호작용?
			{
				if (Check_Col_Player(INTER_COL_DOUBLE, COL_STEP2) && !m_bActivity)
					m_bOnce = true;
			}
			else
				m_bCol[INTER_COL_DOUBLE][COL_STEP2] = false;
		}
		else
		{
			m_bCol[INTER_COL_DOUBLE][COL_STEP1] = false;
			m_bCol[INTER_COL_DOUBLE][COL_STEP2] = false;
		}
	}
	else
	{
		m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
		m_bCol[INTER_COL_DOUBLE][COL_STEP0] = false;
		m_bCol[INTER_COL_DOUBLE][COL_STEP1] = false;
		m_bCol[INTER_COL_DOUBLE][COL_STEP2] = false;
	}

	if (!m_bBlock && m_bOnce && (m_bCol[INTER_COL_NORMAL][COL_STEP2] || m_bCol[INTER_COL_DOUBLE][COL_STEP2]))
	{
		m_bOnce = false;
		if (m_bLock)
			m_pPlayer->Set_Door_Setting(CPlayer::DOOR_BEHAVE_LOCK, Get_PlayerLook_Degree());
		else
			m_pPlayer->Set_Door_Setting(CPlayer::DOOR_BEHAVE_OPEN);

	}



}

void CDoor::DoubleDoor_Active()
{

	*m_pPlayerInteract = false;
	m_bActivity = true;

	if (m_bLock)
		return;

	_float fScala = Radian_To_Player();

	if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
	{
		if (m_bCol[INTER_COL_NORMAL][COL_STEP1] && m_bCol[INTER_COL_DOUBLE][COL_STEP1])
			m_eDoubleState = L_DOUBLEDOOR_OPEN;
		else if (m_bCol[INTER_COL_NORMAL][COL_STEP1])
			m_eDoubleState = LSIDE_DOUBLEDOOR_OPEN_L;
		else
			m_eDoubleState = RSIDE_DOUBLEDOOR_OPEN_R;

	}
	else
	{
		if (m_bCol[INTER_COL_NORMAL][COL_STEP1] && m_bCol[INTER_COL_DOUBLE][COL_STEP1])
			m_eDoubleState = R_DOUBLEDOOR_OPEN;
		else if (m_bCol[INTER_COL_NORMAL][COL_STEP1])
			m_eDoubleState = LSIDE_DOUBLEDOOR_OPEN_R;
		else
			m_eDoubleState = RSIDE_DOUBLEDOOR_OPEN_L;
	}
	

}

_float4 CDoor::Get_Object_Pos()
{
	if (m_fTime > 0.3f)
		return _float4();
	if (m_eType == DOOR_DOUBLE)
	{
		_float fScala = Radian_To_Player();
		if ((!m_bCol[INTER_COL_NORMAL][COL_STEP0] && !m_bCol[INTER_COL_DOUBLE][COL_STEP0]) 
			|| (m_bCol[INTER_COL_NORMAL][COL_STEP0] && m_bCol[INTER_COL_DOUBLE][COL_STEP0]))
		{
			_float4 vPosR; 
			_float4 vPosL;
			if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
			{
				vPosR = static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(0); // Lside_L
				vPosL = static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(2); // Rside_L
			}
			else
			{
				vPosR = static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(1); //Lside_R
				vPosL = static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(3); //Rside_R
			}
			return XMVectorSetW( (vPosR+vPosL)/2.f,1.f);
		}
		else if (m_bCol[INTER_COL_NORMAL][COL_STEP0])
		{
			if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
				return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(0);
			else
				return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(1);
		}
		else
			if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
				return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(2);
			else
				return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(3);

	}
	else
	{
		_float fScala = Radian_To_Player();
		if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
			return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(0);
		else
			return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(1);
	}

		
	return _float4();
}

void CDoor::OneDoor_Tick(_float fTimeDelta)
{

	if (m_bActivity)
		m_fTime += fTimeDelta;

	if (m_fTime > 2.f)
	{
		m_fTime = 0.f;
		m_iHP = 5;
		if (!m_bCol[INTER_COL_NORMAL][COL_STEP1])
		{
			m_bActivity = false;
			m_eOneState = ONEDOOR_STATIC;
		}
	}

	if (m_bCol[INTER_COL_NORMAL][COL_STEP1] && !m_bActivity)
	{
		//UI띄우고
		if (*m_pPlayerInteract || m_bCol[INTER_COL_NORMAL][COL_STEP2])
			OneDoor_Active();
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;

	}
	else
	{
		m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
	}
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


	if (Activate_Col(Get_Collider_World_Pos(_float4(-60.f, 1.f, 0.f, 1.f))))
	{
		if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0)) // 인지?
		{
			if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP1)) // 상호작용?
			{
				if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP2) && !m_bActivity)
					m_bOnce = true;
			}
			else
				m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
		}
		else
		{
			m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
			m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
		}
	}
	else
	{
		m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
	}
	if (!m_bBlock && m_bOnce&& m_bCol[INTER_COL_NORMAL][COL_STEP2])
	{
		m_bOnce = false;
		if (m_bLock)
			m_pPlayer->Set_Door_Setting(CPlayer::DOOR_BEHAVE_LOCK, Get_PlayerLook_Degree());
		else
			m_pPlayer->Set_Door_Setting(CPlayer::DOOR_BEHAVE_OPEN);
	}
	
}

_float CDoor::Radian_To_Player()
{
	_vector vLook = XMVector4Normalize(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
	_vector vDir = XMVector4Normalize(m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	_float fScala = XMVectorGetX(XMVector4Dot(vLook, vDir));
	if (fScala > 1.f)
		fScala = 1.f;
	else if (fScala < -1.f)
		fScala = -1.f;
	return fScala;
}

_float CDoor::Radian_To_Jombie(class CTransform* pTransform)
{	
	_vector vLook = XMVector4Normalize(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
	_vector vDir = XMVector4Normalize(pTransform->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	_float fScala = XMVectorGetX(XMVector4Dot(vLook, vDir));
	if (fScala > 1.f)
		fScala = 1.f;
	else if (fScala < -1.f)
		fScala = -1.f;
	return fScala;
}

void CDoor::OneDoor_Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;

	if (m_bLock)
		return;

	_float fScala = Radian_To_Player();

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
}
