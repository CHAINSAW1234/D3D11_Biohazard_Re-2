#include "stdafx.h"
#include "ReaderMachine.h"
#include"Player.h"

#include"Body_ReaderMachine.h"
#include"Key_ReaderMachine.h"
#include"Cabinet.h"
#include"Camera_Gimmick.h"
CReaderMachine::CReaderMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CReaderMachine::CReaderMachine(const CReaderMachine& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CReaderMachine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CReaderMachine::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Initialize_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CReaderMachine::Start()
{
	__super::Start();
	list<CGameObject*>* pInteractObjList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_InteractObj"));
	for (auto iter : *pInteractObjList)
	{
		if (static_cast<CInteractProps*>(iter)->Get_DESC()->iPartObj == OBJ_CABINET)
		{
			if (static_cast<CCabinet*>(iter)->Get_Cabinet_Type() == CCabinet::TYPE_ELECTRIC)
			{
				string strTag = static_cast<CCabinet*>(iter)->Get_Electric_Tag();
				m_Cabinets.insert({ strTag, static_cast<CCabinet*>(iter) });
			}

		}
	}
	
}

void CReaderMachine::Tick(_float fTimeDelta)
{
	__super::Tick_Col();

	if (!m_bVisible)
		return;
	_bool bCam = false;

	if (m_eMachine_Key_State == CCabinet::LIVE_LOCK)
	{
		if (DOWN == m_pGameInstance->Get_KeyState('D'))
			m_eKeyInput = KEY_D;
		else if (DOWN == m_pGameInstance->Get_KeyState('A'))
			m_eKeyInput = KEY_A;
		else if (DOWN == m_pGameInstance->Get_KeyState('W'))
			m_eKeyInput = KEY_W;
		else if (DOWN == m_pGameInstance->Get_KeyState('S'))
			m_eKeyInput = KEY_S;
		else if (DOWN == m_pGameInstance->Get_KeyState(VK_SPACE))
			m_eKeyInput = KEY_SPACE;
		if (DOWN == m_pGameInstance->Get_KeyState(VK_RBUTTON))
		{
			m_eMachine_Key_State = CReaderMachine::READERMACHINE_KEY_STATIC;
			bCam = true;
		}

	}
	else
		m_eKeyInput = KEY_NOTHING;

	if (m_bCamera && (bCam))
	{
		Reset_Camera();
		m_bCamera = false;
	}
	if (m_bCamera)
		Camera_Active(PART_READER, _float3(20.5f, 40.5f, 20.5f));
	

	if (m_bCol[INTER_COL_NORMAL][COL_STEP1])
	{
		if (*m_pPlayerInteract)
			Active();
	}
	__super::Tick(fTimeDelta);

}
void CReaderMachine::Reset_Camera()
{
	m_pCameraGimmick->Active_Camera(false);
	m_pPlayer->ResetCamera();
}

void CReaderMachine::Camera_Active(CReaderMachine::READERMACHINE_PART ePart, _float3 vRatio)
{
	CPart_InteractProps* pPartLock = { nullptr };
	pPartLock = static_cast<CPart_InteractProps*>(m_PartObjects[ePart]);
	m_pCameraGimmick->SetPosition(XMVectorSetW(pPartLock->Get_Pos_vector() + pPartLock->Get_World_Look_Dir() * _vector { vRatio.x, vRatio.y, vRatio.z }, 1.f));
	m_pCameraGimmick->LookAt(pPartLock->Get_Pos());
}

void CReaderMachine::Late_Tick(_float fTimeDelta)
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

HRESULT CReaderMachine::Render()
{
	return S_OK;
}

HRESULT CReaderMachine::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(120.f);
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

HRESULT CReaderMachine::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CReaderMachine::PART_END);

	/*PART_READER*/
	CPartObject* pKeyObj = { nullptr };
	CKey_ReaderMachine::KEY_READER_DESC Key_Desc = {};
	Key_Desc.pParentsTransform = m_pTransformCom;
	Key_Desc.pState = &m_eState;
	Key_Desc.pKeyInput = &m_eKeyInput;
	Key_Desc.pKeyState = &m_eMachine_Key_State;
	Key_Desc.strModelComponentName =TEXT("Prototype_Component_Model_sm42_020_keystrokedevice01a_Anim");
	pKeyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Key_Reader"), &Key_Desc));
	if (nullptr == pKeyObj)
		return E_FAIL;
	m_PartObjects[CReaderMachine::PART_READER] = pKeyObj;

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_ReaderMachine::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CReaderMachine::PART_BODY] = pBodyObj;


	return S_OK;
}

HRESULT CReaderMachine::Initialize_PartObjects()
{

	CModel* pBodyModel = { dynamic_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Body_Model"))) };

	CKey_ReaderMachine* pKey = dynamic_cast<CKey_ReaderMachine*>(m_PartObjects[PART_READER]);
	_float4x4* pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("_01")) };
	pKey->Set_Socket(pCombinedMatrix);



	return S_OK;
}

HRESULT CReaderMachine::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

void CReaderMachine::Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;
	m_eMachine_Key_State = READERMACHINE_KEY_LIVE;

	m_pCameraGimmick->Active_Camera(true);
	//tabwindow�� ���� ������ �� �����Դϴ�
	m_bCamera = true;
}

_float4 CReaderMachine::Get_Object_Pos()
{
	return _float4();
}

CReaderMachine* CReaderMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CReaderMachine* pInstance = new CReaderMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CReaderMachine"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CReaderMachine::Clone(void* pArg)
{
	CReaderMachine* pInstance = new CReaderMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CReaderMachine"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CReaderMachine::Free()
{
	__super::Free();

}
