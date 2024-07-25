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

	m_bCanPush[ROW_2][COL_2] = true;
	m_bCanPush[ROW_2][COL_1] = true;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Initialize_PartObjects()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Object_Sound(m_pTransformCom, 3)))
		return E_FAIL;



	m_iNeedItem = blankkey01a;
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
				static_cast<CCabinet*>(iter)->Set_Lock();
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



	if (m_eMachine_Key_State == CReaderMachine::READERMACHINE_KEY_LIVE)
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
		if (DOWN == m_pGameInstance->Get_KeyState(VK_ESCAPE))
		{
			m_eMachine_Key_State = CReaderMachine::READERMACHINE_KEY_STATIC;
			m_iPush[0] = -1;
			m_iPush[1] = -1;
			m_iPush[2] = -1;
			bCam = true;
		}
	}
	else
		m_eKeyInput = KEY_NOTHING;

	if (m_bCamera && (bCam|| m_bCameraReset))
	{
		Reset_Camera();
		m_bCamera = false;
		m_bCameraReset = false;
	}

	if (m_bDoOpen)
	{
		if (Open_Cabinet())
		{
			Change_Sound(TEXT("sound_Map_sm42_key_device2_1.mp3"),0);
			m_iSelectPushNum = 0;
			m_bDoOpen = false;
			m_bCameraReset = true;
			m_eMachine_Key_State = CReaderMachine::READERMACHINE_KEY_STATIC;

		}
		else
		{
			Change_Sound(TEXT("sound_Map_sm42_key_device2_11.mp3"), 0);
			m_iSelectPushNum = 0;
			m_bDoOpen = false;
		}
	}

	if (m_bCol[INTER_COL_NORMAL][COL_STEP1]&&!m_bCamera)
	{
		if (*m_pPlayerInteract && false == m_pGameInstance->IsPaused())
			Active();
	}


	if (m_bCamera)
		Camera_Active(PART_READER, _float3(-0.5f, -1.f, -0.5f));

	__super::Tick(fTimeDelta);

}

void CReaderMachine::Camera_Active(CReaderMachine::READERMACHINE_PART ePart, _float3 vRatio)
{
	CPart_InteractProps* pPartLock = { nullptr };
	pPartLock = static_cast<CPart_InteractProps*>(m_PartObjects[ePart]);
	m_pCameraGimmick->SetPosition(XMVectorSetW(pPartLock->Get_Pos_vector() + pPartLock->Get_World_Look_Dir() * _vector { vRatio.x, vRatio.y, vRatio.z }, 1.f));
	m_pCameraGimmick->LookAt(pPartLock->Get_Pos());
}

_bool CReaderMachine::Open_Cabinet()
{
	_int iNum = m_iPush[0] * 100 + m_iPush[1] * 10 + m_iPush[2];
	auto& iter = m_Cabinets.find(to_string(iNum));
	if (iter != m_Cabinets.end())
	{
		m_iPush[0] = -1;
		m_iPush[1] = -1;
		m_iPush[2] = -1;
		m_eMachine_Key_State = READERMACHINE_KEY_CORRECT;
		iter->second->Set_Electric_Open();
		return true;

	}
	else
	{
		m_iPush[0] = -1;
		m_iPush[1] = -1;
		m_iPush[2] = -1;
		m_eMachine_Key_State = READERMACHINE_KEY_WRONG;

		return false;
	}
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

	if (Activate_Col(Get_Collider_World_Pos(_float4(0.f, 0.f, 0.f,1.f))) )
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
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;

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
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
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
	memcpy_s(Key_Desc.pCanPush, sizeof(_bool) * 5 * 3, m_bCanPush, sizeof(_bool) * 5 * 3);
	Key_Desc.pPush = (_int*)m_iPush;
	Key_Desc.pSelectCol = &m_iSelectCol;
	Key_Desc.pSelectRow = &m_iSelectRow;
	Key_Desc.pDoOpen = &m_bDoOpen;
	Key_Desc.pSoundCueSign = &m_bSoundCueSign;
	Key_Desc.pKeyNum = &m_iSelectPushNum;
	Key_Desc.pHideKey = (_bool*)m_bKey;
	memcpy_s(Key_Desc.iKeyPad,sizeof(_int)*5*3, m_iKeyPad, sizeof(_int) * 5 * 3);
	Key_Desc.strModelComponentName =TEXT("Prototype_Component_Model_sm42_020_keystrokedevice01a_Anim");
	pKeyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Key_Reader"), &Key_Desc));
	if (nullptr == pKeyObj)
		return E_FAIL;
	m_PartObjects[CReaderMachine::PART_READER] = pKeyObj;

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_ReaderMachine::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pSoundCueSign = &m_bSoundCueSign;
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

void CReaderMachine::Do_Interact_Props()
{

	if(!m_bKey[0])
		m_bKey[0] = true;
	else
		m_bKey[1] = true;


}

void CReaderMachine::Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;
	m_eMachine_Key_State = READERMACHINE_KEY_LIVE;

	m_pGameInstance->Active_Camera(g_Level, m_pCameraGimmick);

	m_bCamera = true;

	if (!m_bKey[0] || !m_bKey[1])
			m_pPlayer->Interact_Props(this);


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
