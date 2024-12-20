﻿#include "stdafx.h"
#include "..\Public\Player.h"

#include "FSM.h"
#include "Player_State_Move.h"
#include "Player_State_Hold.h"
#include "Player_State_SubHold.h"
#include "Player_State_Bite.h"

#include "Body_Player.h"
#include "Head_Player.h"
#include "Hair_Player.h"

#include "Knife.h"
#include "Weapon.h"
#include "Throwing_Weapon.h"
#include "Throwing_Weapon_Pin.h"
#include "FlashLight.h"

#include "CustomCollider.h"

#include "Character_Controller.h"
#include "Camera_Free.h"
#include "Camera_Event.h"
#include "Effect_Header_Player.h"

#include "Tab_Window.h"
#include "Selector_UI.h"
#include "Bone.h"
#include "Muzzle_Smoke.h"

#include "MovingShelf.h"
#include "Muzzle_ShockWave.h"
#include "Hit_Props.h"
#include "Decal_BulletHole.h"
#include "HG_Cartridge.h"
#include "SG_Cartridge.h"
#include "Mesh.h"
#include "Zombie.h"

#include "Call_Center.h"
#include "MissionBar_UI.h"

#define MODEL_SCALE 0.01f
#define SHOTGUN_BULLET_COUNT 7
#define SHOTGUN_SPREAD_AMOUNT 0.14f
#define HIT_PROPS_EFFECT_TYPE_COUNT 6
#define BULLET_HOLE_COUNT SHOTGUN_BULLET_COUNT*5.f
#define HG_CARTRIDGE_COUNT 10
#define SG_CARTRIDGE_COUNT 10

const wstring CPlayer::strAnimSetMoveName[ANIMSET_MOVE_END] = { TEXT("FINE"), TEXT("MOVE_HG"), TEXT("MOVE_STG"), TEXT("FINE_LIGHT"), TEXT("CAUTION"), TEXT("CAUTION_LIGHT"), TEXT("DNAGER"), TEXT("DANGER_LIGHT") };
const wstring CPlayer::strAnimSetHoldName[ANIMSET_HOLD_END] = { TEXT("HOLD_HG"), TEXT("HOLG_STG"), TEXT("HOLD_MLE"), TEXT("HOLD_SUP") };
const wstring CPlayer::strAnimSetEtcName[ANIMSET_ETC_END] = { TEXT("COMMON"), TEXT("BITE") };

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Initialize_PartModels()))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	///	For.CutScene
	CCall_Center::Get_Instance()->Add_Caller(this, CCall_Center::CALLER::_PL00);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_Scaled(MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);
	m_pController = m_pGameInstance->Create_Controller(_float4(0.f, 0.3f, 20.f, 1.f), &m_iIndex_CCT, this, 1.f, 0.445f, m_pTransformCom,
		m_pBodyModel->GetBoneVector(), "None");

	//For Camera.
	m_pTransformCom_Camera = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom_Camera)
		return E_FAIL;
	m_pTransformCom_Camera->SetRotationPerSec(0.75f);

	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 3.1f);
	m_pTransformCom_Camera->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 3.1f);
	//For Camera.
	Load_CameraPosition();
	if (FAILED(Ready_Camera()))
		return E_FAIL;

	CModel* pModel = Get_Body_Model();


	m_pL_Ball_Combined = pModel->Get_CombinedMatrix("l_leg_ball");
	m_pR_Ball_Combined = pModel->Get_CombinedMatrix("r_leg_ball");
	m_pRoot_Combined = pModel->Get_CombinedMatrix("root");

	if (nullptr == m_pL_Ball_Combined ||
		nullptr == m_pL_Ball_Combined ||
		nullptr == m_pL_Ball_Combined)
		return E_FAIL;

	m_pGameInstance->Add_Object_Sound(m_pTransformCom, 6);	// 일단 3개

	m_pGameInstance->SetPlayer(this);

	Ready_Effect();

	fill_n(m_SetProps, SETPROPS_NONE, -1);

	m_MuzzleSmoke_Delay = 50;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(0.f, 0.3f, 20.f, 1.f));
	m_pTransformCom_Camera->Set_State(CTransform::STATE_POSITION, _float4(0.f, 0.3f, 20.f, 1.f));
	m_pTransformCom->Look_At_ForLandObject(XMVectorSet(0.001f, 0.1f, 0.001f, 1.f));
	ResetCamera();

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	Priority_Tick_PartObjects(fTimeDelta);

#pragma region 예은 스파이 나중에 FSM으로 옮길지도
	m_fTimeTEST += fTimeDelta;


	if (DOWN == m_pGameInstance->Get_KeyState(VK_LBUTTON) && m_fTimeTEST > 0.5f)
	{
		m_fTimeTEST = 0.f;
		m_bInteract = true;
	}
	else
		m_bInteract = false;

#pragma endregion 

}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->IsPaused())
	{
		fTimeDelta = 0.f;
	}
#pragma region 예은ColTest - 컬링 방식에 따라 달라질 겁니당
	if (m_iCurCol != m_iPreCol|| m_iDir != m_iPreDir)
	{
		m_iPreCol = m_iCurCol;
		m_iPreDir = m_iDir;
		m_bChange = true;
	}
	else
		m_bChange = false;


#pragma endregion 예은ColTest

#pragma region 이동과 카메라
	if (nullptr != m_pController && false == m_isManualMove)
	{
		auto CameraPos = m_pController->GetPosition_Float4();
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, CameraPos);
		CameraPos.y += CONTROLLER_GROUND_GAP;
		m_pTransformCom_Camera->Set_State(CTransform::STATE_POSITION, CameraPos);


		FMOD_3D_ATTRIBUTES			FmodDesc;
		FmodDesc.forward.x = XMVectorGetX(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
		FmodDesc.forward.y = XMVectorGetY(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
		FmodDesc.forward.z = XMVectorGetZ(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));

		FmodDesc.up.x = XMVectorGetX(m_pTransformCom->Get_State_Vector(CTransform::STATE_UP));
		FmodDesc.up.y = XMVectorGetY(m_pTransformCom->Get_State_Vector(CTransform::STATE_UP));
		FmodDesc.up.z = XMVectorGetZ(m_pTransformCom->Get_State_Vector(CTransform::STATE_UP));

		FmodDesc.position.x = XMVectorGetX(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
		FmodDesc.position.y = XMVectorGetY(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
		FmodDesc.position.z = XMVectorGetZ(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));

		FmodDesc.velocity = FMOD_VECTOR{ 0.f, 0.f, 0.f };

		m_pGameInstance->Update_Listener(m_pTransformCom, _float3(0.f, 0.f, 0.f));
	}

	_vector			vMovedDirection = { XMLoadFloat3(&m_vRootTranslation) };
	if (DOWN == m_pGameInstance->Get_KeyState('B'))
	{
		_vector			vCurrentPostion = { m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) };
		vMovedDirection = XMVectorSetW(vCurrentPostion * -1.f, 0.f);
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('H'))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('K'))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}

	if (m_pController)
	{
		if (PRESSING == m_pGameInstance->Get_KeyState('W'))
		{
			m_bMove_Forward = true;
			m_bMove = true;

			if (m_bLerp_Move == false)
			{
				m_bLerp_Move = true;
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}
		}
		else
		{
			if (m_bMove_Forward && m_bCollision_Lerp == false)
			{
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}

			m_bMove_Forward = false;
		}

		if (PRESSING == m_pGameInstance->Get_KeyState('S'))
		{
			m_bMove_Backward = true;
			m_bMove = true;

			if (m_bLerp_Move == false)
			{
				m_bLerp_Move = true;
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}
		}
		else
		{
			if (m_bMove_Backward && m_bCollision_Lerp == false)
			{
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}

			m_bMove_Backward = false;
		}

		if (PRESSING == m_pGameInstance->Get_KeyState('D'))
		{
			m_bMove_Right = true;
			m_bMove = true;

			if (m_bLerp_Move == false)
			{
				m_bLerp_Move = true;
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}
		}
		else
		{
			if (m_bMove_Right && m_bCollision_Lerp == false)
			{
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}

			m_bMove_Right = false;
		}

		if (PRESSING == m_pGameInstance->Get_KeyState('A'))
		{
			m_bMove_Left = true;
			m_bMove = true;

			if (m_bLerp_Move == false)
			{
				m_bLerp_Move = true;
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}
		}
		else
		{
			if (m_bMove_Left && m_bCollision_Lerp == false)
			{
				m_bLerp = true;
				m_fLerpTime = 0.f;
			}

			m_bMove_Left = false;
		}

		_float4			vResultMoveDirFloat4 = {};
		XMStoreFloat4(&vResultMoveDirFloat4, vMovedDirection);

		if (false == m_isManualMove)
		{
			m_pController->Move(vResultMoveDirFloat4, fTimeDelta);
		}

		else
		{
			_vector				vCurrentPosition = { m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) };
			_vector				vResultPosition = { vCurrentPosition + vMovedDirection };

			_vector				vResultCamPos = { vResultPosition };
			vResultCamPos = XMVectorSetY(vResultCamPos, XMVectorGetY(vResultCamPos) + CONTROLLER_GROUND_GAP);
			m_pTransformCom_Camera->Set_State(CTransform::STATE_POSITION, vResultCamPos);

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vResultPosition);
		}
		XMStoreFloat3(&m_vRootTranslation, XMVectorZero());
	}

	if (m_bMove_Backward == false && m_bMove_Forward == false && m_bMove_Left == false && m_bMove_Right == false)
	{
		m_bLerp_Move = false;
		m_bMove = false;

		m_eMoveDir = MD_DEFAULT;
	}

	//For Camera.
	SetMoveDir();

#pragma region Camera

	if (DOWN == m_pGameInstance->Get_KeyState(VK_BACK))
	{
		m_isCamTurn = false;
	}

	if (m_pCamera && false == m_isCamTurn)
	{
		Calc_Camera_LookAt_Point(fTimeDelta);
	}

	RayCasting_Camera();

	if (m_bRecoil)
	{
		Apply_Recoil(fTimeDelta);
	}

	if (m_eState == HOLD && PRESSING == m_pGameInstance->Get_KeyState(VK_RBUTTON))
	{
		if (m_bAim == false)
		{
			m_bAim = true;
			m_fLerpTime = 0.f;
			m_bLerp = true;
		}
	}
	else
	{
		if (m_bAim)
		{
			m_bLerp = true;
			m_fLerpTime = 0.f;
		}

		m_bAim = false;
	}

#pragma endregion

#pragma endregion

#pragma region 현진 추가

	if (m_isPlay) {

		Update_Direction();
		Update_FSM();
		m_pFSMCom->Update(fTimeDelta);

		Update_KeyInput_Reload();
		Update_LightCondition();
		Update_Equip();
		Update_FootStep_Sound();
	}

#pragma endregion

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	Tick_PartObjects(fTimeDelta);

#pragma region Effect
	Tick_Effect(fTimeDelta);
#pragma endregion

	m_pGameInstance->Set_Player_Collider(m_iCurCol);

	m_pGameInstance->Set_Player_Dir(m_iDir);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (m_pGameInstance->IsPaused())
	{
		fTimeDelta = 0.f;
	}

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	if (m_isPlay) {
		Late_Tick_PartObjects(fTimeDelta);
	}

	Turn_Spine_Default(fTimeDelta);
	Turn_Spine_Light(fTimeDelta);		// Light 상태에서 상체전체를 카메라를 보도록 돌림
	Turn_Spine_Hold(fTimeDelta);		// Hold 상태에서 척추를 상하로만 돌림

	/*if (m_pController)
		m_pController->Update_Collider();*/

#pragma region 예은 추가
	Col_Section();
#pragma endregion 

#pragma region 나옹 추가
	Player_First_Behavior();

	Player_Mission_Timer(fTimeDelta);

	/*if (DOWN == m_pGameInstance->Get_KeyState('Z'))
	{
		Set_Tutorial_Start(UI_TUTORIAL_TYPE::TUTORIAL_AIM);
	}*/
#pragma endregion 

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif

#pragma region Camera Reset
	if (UP == m_pGameInstance->Get_KeyState('C'))
	{
		ResetCamera();
	}

	Late_Tick_Effect(fTimeDelta);
#pragma endregion

#pragma region Decal
	Calc_Decal_Map();
#pragma endregion
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

void CPlayer::Start()
{
	CGameObject* pTabWindow = m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_TabWindow"), 0);
	m_pTabWindow = dynamic_cast<CTab_Window*>(pTabWindow);
	if (nullptr == m_pTabWindow) {
		MSG_BOX(TEXT("CPlayer::Start"));
		assert(0);
	}

	Safe_AddRef(m_pTabWindow);

	list<CGameObject*>* pSelecter_UI = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

	for (auto& iter : *pSelecter_UI)
	{
		CSelector_UI* pSelect = dynamic_cast<CSelector_UI*>(iter);

		if (nullptr != pSelect)
		{
			/* 부모를 불러서 Selector를 구별한다 */
			if (false == pSelect->Get_IsChild())
			{
				m_SelectorVec.push_back(pSelect);
			}
		}
	}
}

void CPlayer::Priority_Tick_PartObjects(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
	{
		if (pPartObject)
			pPartObject->Priority_Tick(fTimeDelta);
	}
}

void CPlayer::Tick_PartObjects(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
	{
		if (pPartObject)
			pPartObject->Tick(fTimeDelta);
	}

	for (auto& pWeapon : m_Weapons) {
		if (nullptr != pWeapon)
			pWeapon->Tick(fTimeDelta);
	}
}

void CPlayer::Late_Tick_PartObjects(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
	{
		if (pPartObject)
			pPartObject->Late_Tick(fTimeDelta);
	}

	for (auto& pWeapon : m_Weapons) {
		if (nullptr != pWeapon)
			pWeapon->Late_Tick(fTimeDelta);
	}
}
#pragma region 예은 추가
void CPlayer::Col_Section()
{
	list<CGameObject*>* pCollider = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Collider"));
	if (pCollider == nullptr)
		return;
	for (auto& iter : *pCollider)
	{
		if (m_pColliderCom->Intersect(static_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider")))))
		{
			CCustomCollider* pColCom = static_cast<CCustomCollider*>(iter);

			m_iCurCol = pColCom->Get_Col();
			m_iDir = pColCom->Get_Dir();
			m_iRegion = pColCom->Get_Region();
			m_iFloor = pColCom->Get_Floor();
			m_bRegion[m_iRegion] = true;
		}
	}
}

_float4x4 CPlayer::Get_Shelf_WorldMatrix()
{
	if (m_pShelf == nullptr)
		return _float4x4();
	return static_cast<CMovingShelf*>(m_pShelf)->Get_WorldMatrix();
}

void CPlayer::Set_Shelf_State(_int eState)
{
	static_cast<CMovingShelf*>(m_pShelf)->Set_Anim_State(eState);
}

_int CPlayer::Get_Shelf_Type()
{
	if (m_pShelf == nullptr)
		return CMovingShelf::SHELF_TYPE_END;
	return static_cast<CMovingShelf*>(m_pShelf)->Get_Shelf_Type();
}


#pragma endregion

#pragma region 나옹 추가
_bool* CPlayer::Col_Event_UI(CCustomCollider* pCustom)
{
	m_isNYResult = false;

	if (m_pColliderCom->Intersect(static_cast<CCollider*>(pCustom->Get_Component(TEXT("Com_Collider")))))
		m_isNYResult = true;
	else
		m_isNYResult = false;

	return &m_isNYResult;
}

void CPlayer::Player_First_Behavior()
{
	/* 1. 무기 첫 착용 시 "조준" 안내 */
	if (false == m_isPlayer_FirstBehavior[(_int)UI_TUTORIAL_TYPE::TUTORIAL_AIM])
	{
		if (EQUIP::NONE != m_eEquip)
		{
			Set_Tutorial_Start(UI_TUTORIAL_TYPE::TUTORIAL_AIM);

			if (UI_TUTORIAL_TYPE::TUTORIAL_AIM == m_eTutial_Type)
				m_isPlayer_FirstBehavior[(_int)UI_TUTORIAL_TYPE::TUTORIAL_AIM] = true;
		}
	}
}

CGameObject* CPlayer::Create_Selector_UI()
{
	for (auto& iter : m_SelectorVec)
	{
		if (false == *iter->Get_Using())
		{
			*iter->Get_Using() = true;

			return iter;
		}
	}

	return nullptr;
}

void CPlayer::Player_Mission_Timer(_float fTimeDelta)
{
	if (true == m_isFlod_EntranceDoor)
	{
		m_fMissionTimer += fTimeDelta;

		if (m_fMissionTimer >= 1.5f && false == m_MissionCollection[MISSION_TYPE::EXPLORING_SURROUNDING_MISSION])
		{
			MissionClear_Font(TEXT("주변 환경 탐색하기"), static_cast<_ubyte>(MISSION_TYPE::EXPLORING_SURROUNDING_MISSION));
		}
	}
}

void CPlayer::MissionClear_Font(wstring _missionText, _ubyte _missionType)
{
	list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

	for (auto& iter : *pUIList)
	{
		CMissionBar_UI* pMission = dynamic_cast<CMissionBar_UI*>(iter);

		if (nullptr != pMission)
		{
			if (static_cast<_ubyte>(CMissionBar_UI::MISSION_UI_TYPE::FONT_MISSION) == pMission->Get_MissionType())
			{
				if (!pMission->Get_vecTextBoxes()->empty())
				{
					CTextBox* pFont = pMission->Get_vecTextBoxes()->back();

					m_MissionCollection[_missionType] = true;

					*pMission->Get_MissionBar()->Get_FontStart_Ptr() = false;

					pFont->Set_Text(_missionText);

					m_isMissionClear = true;
				}
			}
		}
	}
}

#pragma endregion

#pragma region 현진 추가
CBody_Player* CPlayer::Get_Body()
{
	return static_cast<CBody_Player*>(m_PartObjects[PART_BODY]);
}

CModel* CPlayer::Get_Body_Model()
{
	return static_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(g_strModelTag));
}

void CPlayer::Set_Gravity(_bool isGravity)
{
	m_pController->SetGravity(isGravity);
}

void CPlayer::Set_Position(_float4 vPos)
{
	vPos.y += CONTROLLER_GROUND_GAP;
	m_pController->SetPosition(vPos);
}

void CPlayer::Set_Position(_fvector vPos)
{
	_vector vvPos = vPos + XMVectorSet(0.f, CONTROLLER_GROUND_GAP, 0.f, 0.f);
	m_pController->SetPosition(vvPos);
}

void CPlayer::Set_Render(_bool boolean)
{
	m_bRender = boolean;

	for (auto& pPartObject : m_PartObjects) {
		if (pPartObject)
			pPartObject->Set_Render(boolean);
	}

	if (m_bRender) {
		for (auto& pWeapons : m_Weapons) {
			if (pWeapons->Get_RenderLocation() != CWeapon::RENDERLOCATION::NONE)
				pWeapons->Set_Render(boolean);
		}
	}
	else {
		for (auto& pWeapons : m_Weapons) {
			pWeapons->Set_Render(boolean);
		}
	}

	Set_Spotlight(Get_Spotlight());
}

void CPlayer::Set_Knife_Active(_bool isActive)
{
	//static_cast<CKnife*>(m_PartObjects[PART_KNIFE])->Set_Active(isActive);

	//if (isActive && m_bRender) {
	//	m_PartObjects[PART_KNIFE]->Set_Render(true);
	//}
}

void CPlayer::Set_Play(_bool isPlay)
{
	m_isPlay = isPlay;
	if (!m_isPlay) {
		Stop_UpperBody();
		Change_State(HOLD);
		Change_State(MOVE);
	}
}

void CPlayer::Change_Body_Animation_Move(_uint iPlayingIndex, _uint iAnimIndex)
{
	Get_Body_Model()->Change_Animation(iPlayingIndex, Get_AnimSetMoveName(m_eAnimSet_Move), iAnimIndex);
}

void CPlayer::Change_Body_Animation_Hold(_uint iPlayingIndex, _uint iAnimIndex)
{
	Get_Body_Model()->Change_Animation(iPlayingIndex, Get_AnimSetHoldName(m_eAnimSet_Hold), iAnimIndex);
}

CModel* CPlayer::Get_Weapon_Model()
{
	if (nullptr == m_pWeapon)
		return nullptr;

	return static_cast<CModel*>(m_pWeapon->Get_Component(g_strModelTag));
}

_float3* CPlayer::Get_Body_RootDir()
{
	return static_cast<CBody_Player*>(m_PartObjects[0])->Get_RootTranslation();
}

void CPlayer::Set_Spotlight(_bool isSpotlight)
{
	m_isSpotlight = isSpotlight;

	m_PartObjects[PART_LIGHT]->Set_Render(m_isSpotlight);

	Update_AnimSet();
}

void CPlayer::Requst_Change_Equip(EQUIP eEquip)
{
	m_isRequestChangeEquip = true;
	m_eTargetEquip = eEquip;
}

void CPlayer::Set_Equip(EQUIP* eEquip)
{
	m_eEquip = *eEquip;
	*eEquip = NONE;
	CWeapon::RENDERLOCATION eRenderLocation = { CWeapon::MOVE };

	if (nullptr != m_pWeapon) {
		eRenderLocation = m_pWeapon->Get_RenderLocation();
		Change_WeaponLocation_To_Holster(m_pWeapon);
		Safe_Release(m_pWeapon);
	}

	if (m_eEquip == NONE) {
		m_pWeapon = nullptr;
	}
	else {
		if (m_Weapons[m_eEquip]->Get_RenderLocation() == CWeapon::HOLSTER) {
			if (m_SetProps[m_Weapons[m_eEquip]->Get_SetPropsLocation()] == m_eEquip) {
				m_SetProps[m_Weapons[m_eEquip]->Get_SetPropsLocation()] = -1;
			}
		}

		m_pWeapon = m_Weapons[m_eEquip];
		Safe_AddRef(m_pWeapon);
		m_pWeapon->Set_RenderLocation(eRenderLocation);

		if (m_pWeapon->Get_Equip() == STG) {
			Get_Body_Model()->Hide_Mesh("LOD_1_Group_100_Sub_1__pl0001_Sling_Mat_mesh0006", true);
		}


	}

	// 무기 위치를 변경하시오
	Update_AnimSet();
	NotifyObserver();
}

void CPlayer::Set_Equip_Gun(EQUIP* eEquip)
{
	m_eEquip_Gun = *eEquip;
	//Get_Body_Model()->Set_BoneLayer_PlayingInfo(3, TEXT("UpperBody"));
	//Get_Body_Model()->Reset_PreAnimation(3);
	if (m_eEquip_State == GUN) {
		Set_Equip(eEquip);
	}
}

void CPlayer::Set_Equip_Sub(EQUIP* eEquip)
{
	m_eEquip_Sub = *eEquip;
	//Get_Body_Model()->Set_BoneLayer_PlayingInfo(3, TEXT("Right_Arm"));
	//Get_Body_Model()->Reset_PreAnimation(3);
	if (m_eEquip_State == SUB) {
		Set_Equip(eEquip);
	}
}

void CPlayer::Set_Hp(_int iHp)
{
	m_iHp = iHp;
	if (m_iHp >= m_iMaxHp)
		m_iHp = m_iMaxHp;

	if (m_iHp <= 0) {
		;;		// 사망 처리?
	}

	Update_AnimSet();
	NotifyObserver();
}

void CPlayer::Change_State(STATE eState)
{
	m_pFSMCom->Change_State(eState);
	m_eState = eState;
}

void CPlayer::Change_Player_State_Bite(_int iAnimIndex, const wstring& strLayerTag, _float4x4 Interpolationmatrix, _float fTotalInterpolateTime)
{
	m_fCurrentInterpolateTime = 0.f;
	m_fTotalInterpolateTime = fTotalInterpolateTime;

	m_iBiteAnimIndex = iAnimIndex;
	m_strBiteLayerTag = strLayerTag;

	XMStoreFloat4x4(&m_vBiteInterpolateMatrix, Interpolationmatrix);

	Change_State(BITE);

	// bite zombie apply m

}

void CPlayer::Change_Equip_State(EQUIP_STATE eEquip_State)
{
	if (m_eEquip_State == eEquip_State)
		return;

	m_eEquip_State = eEquip_State;

	switch (m_eEquip_State) {
	case GUN:
		Requst_Change_Equip(m_eEquip_Gun);
		break;
	case SUB:
		Requst_Change_Equip(m_eEquip_Sub);
		break;
	}
}

void CPlayer::Change_WeaponLocation_To_Holster(CWeapon* pWeapon)
{
	if (m_SetProps[pWeapon->Get_SetPropsLocation()] != -1) {
		m_Weapons[m_SetProps[pWeapon->Get_SetPropsLocation()]]->Set_RenderLocation(CWeapon::NONE);
	}

	m_SetProps[pWeapon->Get_SetPropsLocation()] = pWeapon->Get_Equip();
	pWeapon->Set_RenderLocation(CWeapon::HOLSTER);

	if (pWeapon->Get_Equip() == STG) {
		Get_Body_Model()->Hide_Mesh("LOD_1_Group_100_Sub_1__pl0001_Sling_Mat_mesh0006", false);
	}
}

void CPlayer::Request_NextBiteAnimation(_int iAnimIndex)
{
	m_iBiteAnimIndex = iAnimIndex;
}

void CPlayer::Shot()
{
#pragma region RaySetting
	m_pGameInstance->Set_RayOrigin_Aim(m_pCamera->GetPosition());
	m_pGameInstance->Set_RayDir_Aim(m_pCamera->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK));
#pragma endregion

	RayCast_Shoot();

#pragma region Cartridge
	Initiate_Cartridge();
#pragma endregion

	m_bRecoil = true;

	m_fRecoil_Lerp_Time = 0.f;
	m_fRecoil_Lerp_Time_Omega = 0.f;

	switch (m_eEquip)
	{
	case EQUIP::HG: {
		auto Random_Real_X = m_pGameInstance->GetRandom_Real(0.075f, 0.1f);
		auto Random_Real_Y = m_pGameInstance->GetRandom_Real(0.15f, 0.2f);
		m_fRecoil_Rotate_Amount_X = Random_Real_X;
		m_fRecoil_Rotate_Amount_Y = Random_Real_Y;

		m_pMuzzle_Flash->Set_Render(true);
		m_pMuzzle_Flash->SetPosition(Get_MuzzlePosition());

		m_bMuzzleSmoke = true;
		m_vMuzzle_Smoke_Pos = Get_MuzzlePosition();
		m_MuzzleSmoke_Time = GetTickCount64();

		Change_Sound_3D(TEXT("Sound_Player_HG_Shot"), 3, 0);
		break;
	}
	case EQUIP::STG: {
		auto Random_Real_X = m_pGameInstance->GetRandom_Real(0.2f, 0.25f);
		auto Random_Real_Y = m_pGameInstance->GetRandom_Real(0.25f, 0.3f);
		m_fRecoil_Rotate_Amount_X = Random_Real_X;
		m_fRecoil_Rotate_Amount_Y = Random_Real_Y;

		m_pMuzzle_Flash_SG->Set_Render(true);
		m_pMuzzle_Flash_SG->SetPosition(Get_MuzzlePosition());

		m_iShockWaveIndex = m_pGameInstance->GetRandom_Int(0, 1);

		m_vecShockWave[m_iShockWaveIndex]->SetPosition(Get_MuzzlePosition_Forward());
		m_vecShockWave[m_iShockWaveIndex]->Set_Render(true);

		m_bMuzzleSmoke = true;
		m_vMuzzle_Smoke_Pos = Get_MuzzlePosition();
		m_MuzzleSmoke_Time = GetTickCount64();

		Change_Sound_3D(TEXT("Sound_Player_STG_Shot"), 2, 0);
		break;
	}
	}

	m_pTabWindow->UseItem(Get_Equip_As_ITEM_NUMBER(), 1);

	NotifyObserver();
	// 카메라 조작 코드 여기에 추가
	// 사운드등도 여기 넣어도 됨
}

void CPlayer::Throw_Sub()
{
	m_pTabWindow->UseItem(Get_Equip_As_ITEM_NUMBER(), 1);

	CThrowing_Weapon::THROWING_WEAPON_DESC pDesc;
	pDesc.worldMatrix = m_pWeapon->Get_WorldMatrix();
	pDesc.eEquip = m_eEquip;
	pDesc.pParentsTransform = m_pTransformCom;

	if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Throwing_Weapon"), TEXT("Prototype_GameObject_Throwing_Weapon"), &pDesc)))
		return;

	CThrowing_Weapon_Pin::THROWING_WEAPON_PIN_DESC pDesc2;
	pDesc2.worldMatrix = m_pWeapon->Get_WorldMatrix();
	pDesc2.eEquip = m_eEquip;
	pDesc2.pParentsTransform = m_pTransformCom;

	if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Throwing_Weapon"), TEXT("Prototype_GameObject_Throwing_Weapon_Pin"), &pDesc2)))
		return;

	NotifyObserver();
}

void CPlayer::Reload()
{
	m_isReload = true;
	Get_Body_Model()->Set_Loop(3, false);
	Change_Body_Animation_Hold(3, HOLD_RELOAD);
	Get_Body_Model()->Set_TrackPosition(3, 0.f);
	Get_Body_Model()->Set_BlendWeight(3, 10.f, 20.f);
	//Get_Body_Model()->Set_BlendWeight(4, 0.f, 0.2f);
	if (nullptr != m_pWeapon) {
		Get_Weapon_Model()->Change_Animation(0, TEXT("Default"), 2);
		Get_Weapon_Model()->Set_TrackPosition(0, 0.f);
	}

	if (Get_Equip() == STG && m_isSpotlight) {
		m_isReloadedSpotLight = true;
		Set_Spotlight(false);
	}


	switch (m_eEquip) {
	case HG:
		Change_Sound_3D(TEXT("Sound_Player_HG_Reload"), 0, 2);
		break;
	case STG:
		Change_Sound_3D(TEXT("Sound_Player_STG_Reload"), 0, 2);
		break;
	}


}

void CPlayer::Stop_UpperBody()
{
	Get_Body_Model()->Set_TrackPosition(3, 0.f);
	Get_Body_Model()->Set_TrackPosition(4, 0.f);

	Get_Body_Model()->Set_Loop(3, true);
	Get_Body_Model()->Set_Loop(4, true);

	Get_Body_Model()->Set_BlendWeight(3, 0.f, 5.f);
	Get_Body_Model()->Set_BlendWeight(4, 0.f, 5.f);

	m_isRequestChangeEquip = false;
	m_eTargetEquip = NONE;
}

void CPlayer::Change_Sound_3D(const wstring& strSoundTag, _int iRandCnt, _uint iIndex)
{

	wstring strRandStoundTag;
	if (iRandCnt != 0) {
		_int iRand = rand() % iRandCnt;
		strRandStoundTag = strSoundTag + TEXT("_") + to_wstring(iRand) + TEXT(".mp3");

	}
	else {
		strRandStoundTag = strSoundTag + TEXT(".mp3");
	}


	m_pGameInstance->Change_Sound_3D(m_pTransformCom, strRandStoundTag, iIndex);
}

_bool CPlayer::IsShotAble()
{
	if (m_eEquip == NONE)
		return false;

	if (m_pTabWindow->Get_Search_Item_Quantity(Get_Equip_As_ITEM_NUMBER()) == 0)
		return false;

	return true;
}

_bool CPlayer::IsReloadAble()
{
	if (m_eEquip == NONE)
		return false;

	if (m_pTabWindow->Get_Search_Item_Quantity(Get_Equip_As_ITEM_NUMBER()) == Get_MaxBullet())
		return false;

	switch (Get_Equip_As_ITEM_NUMBER()) {
	case HandGun:
		if (m_pTabWindow->Get_Search_Item_Quantity(handgun_bullet01a) == 0)
			return false;
		break;
	case ShotGun:
		if (m_pTabWindow->Get_Search_Item_Quantity(shotgun_bullet01a) == 0)
			return false;
		break;
	}

	return true;
}

ITEM_NUMBER CPlayer::Get_Equip_As_ITEM_NUMBER()
{
	switch (m_eEquip) {
	case HG:
		return HandGun;
		break;
	case STG:
		return ShotGun;
		break;
	case GRENADE:
		return Grenade;
		break;
	case FLASHBANG:
		return Flash_Bomb;
		break;
	case NONE:
		break;
	}

	return ITEM_NUMBER_END;
}

_float CPlayer::Get_CamDegree()
{
	if (nullptr == m_pCamera || nullptr == m_pTransformCom)
		return 0;

	_float4 vCamLook = m_pCamera->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK);
	vCamLook.y = 0;
	vCamLook = XMVector3Normalize(vCamLook);

	_float4 vPlayerLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
	vPlayerLook.y = 0;
	vPlayerLook = XMVector3Normalize(vPlayerLook);

	return Cal_Degree_From_Directions_Between_Min180_To_180(vPlayerLook, vCamLook);
}

_float4 CPlayer::Get_MuzzlePosition()
{
	if (nullptr == m_pWeapon) {
		return _float4(0.f, 0.f, 0.f, 1.f);
	}

	return m_pWeapon->Get_MuzzlePosition();
}

_float4 CPlayer::Get_CartridgePosition()
{
	if (nullptr == m_pWeapon) {
		return _float4(0.f, 0.f, 0.f, 1.f);
	}

	return m_pWeapon->Get_CartridgePosition();
}

_float4 CPlayer::Get_CartridgeDir()
{
	if (nullptr == m_pWeapon) {
		return _float4(0.f, 0.f, 0.f, 1.f);
	}

	return m_pWeapon->Get_CartridgeDir();
}

_float4 CPlayer::Get_MuzzlePosition_Forward()
{
	if (nullptr == m_pWeapon) {
		return _float4(0.f, 0.f, 0.f, 1.f);
	}

	return m_pWeapon->Get_MuzzlePosition_Forward();
}

_int CPlayer::Get_MaxBullet()
{
	if (m_eEquip == NONE)
		return 0;

	return Get_Weapon()->Get_MaxBullet();
}

void CPlayer::Update_InterplationMatrix(_float fTimeDelta)
{
	if (m_fCurrentInterpolateTime >= m_fTotalInterpolateTime)
		return;

	m_fCurrentInterpolateTime += fTimeDelta;
	if (m_fCurrentInterpolateTime > m_fTotalInterpolateTime)
	{
		fTimeDelta += m_fTotalInterpolateTime - m_fCurrentInterpolateTime;
		m_fCurrentInterpolateTime = m_fTotalInterpolateTime;
	}

	_float				fRatio = { fTimeDelta / m_fTotalInterpolateTime };

	_vector				vScale, vTranslation, vQuaternion;

	XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, XMLoadFloat4x4(&m_vBiteInterpolateMatrix));

	_vector				vCurrentTranslation = { XMVectorSetW(vTranslation * fRatio, 0.f) };
	_vector				vCurrentQuaternion = { XMQuaternionSlerp(XMQuaternionIdentity(), XMQuaternionNormalize(vQuaternion), fRatio) };

	//	_vector				vCurrentQuaternionInv = { XMQuaternionInverse(XMQuaternionNormalize(vCurrentQuaternion)) };
	//	_matrix				vCurrentRotationInverse = { XMMatrixRotationQuaternion(vCurrentQuaternionInv) };
	//	vCurrentTranslation = XMVector3TransformNormal(vCurrentTranslation, vCurrentRotationInverse);

	_matrix            AIWorldMatrix = { m_pTransformCom->Get_WorldMatrix() };
	_matrix            CurrentRotationMatrix = { XMMatrixRotationQuaternion(XMQuaternionNormalize(vCurrentQuaternion)) };


	_vector				vWorldScale, vWorldQuaternion, vWorldTranslation;
	XMMatrixDecompose(&vWorldScale, &vWorldQuaternion, &vWorldTranslation, AIWorldMatrix);

	_vector				vResultQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vWorldQuaternion), XMQuaternionNormalize(vCurrentQuaternion)) };
	_vector				vResultTranslation = { XMVectorSetW(vWorldTranslation + vCurrentTranslation, 1.f) };

	_matrix				AplliedMatrix = { XMMatrixAffineTransformation(vWorldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };
	m_pTransformCom->Set_WorldMatrix(AplliedMatrix);

}

void CPlayer::Update_FSM()
{

	switch (m_eState) {
	case MOVE:
		if (m_pGameInstance->Get_KeyState(VK_RBUTTON) == PRESSING) {
			if (nullptr != m_pWeapon && NONE != m_eEquip_Gun &&
				Get_Body_Model()->Is_Loop_PlayingInfo(3))
				Change_State(HOLD);
		}
		//if (m_pGameInstance->Get_KeyState(VK_SPACE) == PRESSING) {
		//	if (NONE != m_eEquip_Sub &&
		//		Get_Body_Model()->Is_Loop_PlayingInfo(3))
		//		Change_State(SUBHOLD);
		//}
		break;
	case HOLD:
		if (m_pGameInstance->Get_KeyState(VK_RBUTTON) != PRESSING) {
			Change_State(MOVE);
		}
		//if (m_pGameInstance->Get_KeyState(VK_SPACE) == PRESSING) {
		//	if (NONE != m_eEquip_Sub)
		//		Change_State(SUBHOLD);
		//}

		break;
	case SUBHOLD:
		if (m_pGameInstance->Get_KeyState(VK_SPACE) != PRESSING &&
			Get_Body_Model()->Is_Loop_PlayingInfo(0) &&
			Get_Body_Model()->Is_Loop_PlayingInfo(3)) {
			Change_State(MOVE);
		}
		break;
	case BITE:
		break;
	}
}

void CPlayer::Update_KeyInput_Reload()
{

	if (m_eState == BITE)
		return;

	if (Get_Body_Model()->Is_Loop_PlayingInfo(3)) {
		if (m_pGameInstance->Get_KeyState('R') == DOWN
			&& IsReloadAble()) {
			Reload();
		}
	}
	else if (Get_Body_Model()->isFinished(3) &&
		Get_Body_Model()->Get_AnimIndex_PlayingInfo(3) == HOLD_RELOAD) {
		Get_Body_Model()->Set_BlendWeight(3, 0.f, 6.f);

		if (Get_Body_Model()->Get_BlendWeight(3) <= 0.01f) {
			Get_Body_Model()->Set_Loop(3, true);

			// 총알 개수 업데이트
			m_isReload = false;

			ITEM_NUMBER eItem = { ITEM_NUMBER_END };
			_int iNumBullet = { 0 };
			switch (Get_Equip_As_ITEM_NUMBER()) {
			case HandGun:
				eItem = handgun_bullet01a;
				iNumBullet = m_pTabWindow->Get_Search_Item_Quantity(eItem);
				break;
			case ShotGun:
				eItem = shotgun_bullet01a;
				iNumBullet = m_pTabWindow->Get_Search_Item_Quantity(eItem);
				break;
			}

			_int iNumLoadedBullet = { 0 };
			iNumLoadedBullet = m_pTabWindow->Get_Search_Item_Quantity(Get_Equip_As_ITEM_NUMBER());
			if (iNumBullet >= Get_Weapon()->Get_MaxBullet() - iNumLoadedBullet) {
				iNumBullet = Get_Weapon()->Get_MaxBullet() - iNumLoadedBullet;
			}

			m_pTabWindow->UseItem(Get_Equip_As_ITEM_NUMBER(), -iNumBullet);
			m_pTabWindow->UseItem(eItem, iNumBullet);

			if (m_isReloadedSpotLight) {
				m_isReloadedSpotLight = false;
				Set_Spotlight(true);
			}

			NotifyObserver();

		}
	}
}

void CPlayer::Update_LightCondition()
{
	if (m_eState == BITE)
		return;

	//Get_Body_Model()->Set_Loop(4, false);
	if (Get_Body_Model()->Get_CurrentAnimIndex(4) == LIGHT_ON_OFF) {
		if (Get_Body_Model()->isFinished(4)) {
			Set_Spotlight(!m_isSpotlight);
			//Get_Body_Model()->Set_TrackPosition(4, 0.f, true);
			Get_Body_Model()->Set_BlendWeight(4, 0, 6.f);
			Get_Body_Model()->Set_Loop(4, true);
			Change_Body_Animation_Move(4, ANIM_IDLE);
		}
	}

	// 이후 실제 라이트와 비교해서 처리하셈
	if (m_pGameInstance->Get_KeyState('E') == DOWN) {
		if (Get_Body_Model()->Is_Loop_PlayingInfo(3) &&
			Get_Body_Model()->Is_Loop_PlayingInfo(4)) {
			//Get_Body_Model()->Set_TrackPosition(4, 0.f, true);
			Change_Body_Animation_Move(4, LIGHT_ON_OFF);
			Get_Body_Model()->Set_Loop(4, false);
			Get_Body_Model()->Set_BlendWeight(4, 10.f, 6.f);

		}
	}
}

void CPlayer::Update_Equip()
{
	if (m_eState == BITE)
		return;

	if (m_isRequestChangeEquip) {
		if (Get_Body_Model()->Is_Loop_PlayingInfo(3)) {
			Get_Body_Model()->Set_Loop(3, false);

			Get_Body_Model()->Set_BlendWeight(3, 10.f, 20.f);

			if (nullptr != m_pWeapon) {
				m_pWeapon->Set_RenderLocation(CWeapon::MOVE);
				Change_Body_Animation_Hold(3, MOVETOHOLSTER);

				switch (m_eEquip) {
				case HG:
					Change_Sound_3D(TEXT("Sound_Player_HG_HolsterToMove"), 0, 1);
					break;
				case STG:
					Change_Sound_3D(TEXT("Sound_Player_STG_HolsterToMove"), 0, 1);
					break;
				}

			}
			else {
				if (m_eState == SUBHOLD) {
					Set_Equip_Sub(&m_eTargetEquip);
				}
				else {
					Set_Equip_Gun(&m_eTargetEquip);
				}
				Change_Body_Animation_Hold(3, HOLSTERTOMOVE);


			}
		}
		else if (Get_Body_Model()->isFinished(3)) {
			if (Get_Body_Model()->Get_BlendWeight(3) <= 0.1f) {
				m_isRequestChangeEquip = false;
				Get_Body_Model()->Set_Loop(3, true);
			}
			else if (Get_Body_Model()->Get_AnimIndex_PlayingInfo(3) == HOLSTERTOMOVE) {
				Get_Body_Model()->Set_BlendWeight(3, 0.f, 5.f);
			}
			else if (Get_Body_Model()->Get_AnimIndex_PlayingInfo(3) == MOVETOHOLSTER) {
				if (m_eState == SUBHOLD) {
					Set_Equip_Sub(&m_eTargetEquip);
				}
				else {
					Set_Equip_Gun(&m_eTargetEquip);
				}

				switch (m_eEquip) {
				case HG:
					Change_Sound_3D(TEXT("Sound_Player_HG_Holster_Fin"), 0, 1);
					break;
				case STG:
					Change_Sound_3D(TEXT("Sound_Player_STG_Holster_Fin"), 0, 1);
					break;
				}

				if (NONE == m_eEquip) {
					Get_Body_Model()->Set_BlendWeight(3, 0.f, 5.f);
				}
				else {
					Change_Body_Animation_Hold(3, HOLSTERTOMOVE);
				}
			}
		}
	}
	else {
		//무기 스왑 1~4번 검색하고 
		for (_uint i = 49; i < 53; i++)
		{
			if (m_pGameInstance->Get_KeyState(i) == DOWN)
			{
				m_pTabWindow->Hotkey_PopUp();

				if (ITEM_NUMBER_END != m_pTabWindow->Get_Item_On_HotKey(i - 49))
				{
					ITEM_NUMBER eHotkeyNum = m_pTabWindow->Get_Item_On_HotKey(i - 49);
					switch (eHotkeyNum)
					{
					case Client::HandGun: {
						Requst_Change_Equip(HG);
						break;
					}

					case Client::ShotGun: {
						Requst_Change_Equip(STG);
						break;
					}

					case Client::Flash_Bomb: {
						Requst_Change_Equip(FLASHBANG);
						break;
					}

					case Client::Grenade: {
						Requst_Change_Equip(GRENADE);
						break;
					}

					default:
						break;
					}
				}
			}
		}
	}
}

void CPlayer::Update_AnimSet()
{
#pragma region Move
	if (m_isSpotlight) {
		if (m_iHp >= 4) {
			m_eAnimSet_Move = FINE_LIGHT;
		}
		else if (m_iHp >= 2) {
			m_eAnimSet_Move = CAUTION_LIGHT;
		}
		else {
			m_eAnimSet_Move = DANGER_LIGHT;
		}
	}
	else {
		if (m_iHp >= 4) {
			switch (m_eEquip) {
			case NONE:
				m_eAnimSet_Move = FINE;
				break;
			case HG:
				m_eAnimSet_Move = MOVE_HG;
				break;
			case STG:
				m_eAnimSet_Move = MOVE_STG;
				break;
			}

		}
		else if (m_iHp >= 2) {
			m_eAnimSet_Move = CAUTION;
		}
		else {
			m_eAnimSet_Move = DANGER;
		}
	}
#pragma endregion

#pragma region Hold
	switch (m_eEquip) {
	case HG:
		m_eAnimSet_Hold = HOLD_HG;
		break;
	case STG:
		m_eAnimSet_Hold = HOLD_STG;
		break;
	case GRENADE:
	case FLASHBANG:
		m_eAnimSet_Hold = HOLD_SUP;
		break;
	case NONE:
		break;
	}
#pragma endregion

	switch (m_eState) {
	case MOVE:
		Change_Body_Animation_Move(0, Get_Body_Model()->Get_CurrentAnimIndex(0));
		Change_Body_Animation_Move(1, Get_Body_Model()->Get_CurrentAnimIndex(1));
		//Get_Body_Model()->Reset_PreAnim_CurrentAnim(0);
		Get_Body_Model()->Reset_PreAnim_CurrentAnim(1);
		break;
	case HOLD:
		Change_Body_Animation_Hold(0, Get_Body_Model()->Get_CurrentAnimIndex(0));
		Change_Body_Animation_Hold(1, Get_Body_Model()->Get_CurrentAnimIndex(1));
		break;
	case SUBHOLD:
		Change_Body_Animation_Hold(0, Get_Body_Model()->Get_CurrentAnimIndex(0));
		Change_Body_Animation_Hold(1, Get_Body_Model()->Get_CurrentAnimIndex(1));
		break;
	}

}

void CPlayer::Update_Direction()
{
	DWORD dwDirection = 0;

	if (m_pGameInstance->Get_KeyState('W') == PRESSING &&
		m_pGameInstance->Get_KeyState('S') == PRESSING)
		;
	else if (m_pGameInstance->Get_KeyState('W') == PRESSING)
		dwDirection |= DIRECTION_FRONT;
	else if (m_pGameInstance->Get_KeyState('S') == PRESSING)
		dwDirection |= DIRECTION_BACK;

	if (m_pGameInstance->Get_KeyState('A') == PRESSING &&
		m_pGameInstance->Get_KeyState('D') == PRESSING)
		;
	else if (m_pGameInstance->Get_KeyState('A') == PRESSING)
		dwDirection |= DIRECTION_LEFT;
	else if (m_pGameInstance->Get_KeyState('D') == PRESSING)
		dwDirection |= DIRECTION_RIGHT;

	m_dwDirection = dwDirection;
}

void CPlayer::Update_FootStep_Sound()
{
	if (m_eState == BITE || !m_isFootStep)
		return;

	_float4				vL_Ball_Position_Local_Float3 = { *(_float4*)(&m_pL_Ball_Combined->m[CTransform::STATE_POSITION][0]) };
	_float4				vR_Ball_Position_Local_Float3 = { *(_float4*)(&m_pR_Ball_Combined->m[CTransform::STATE_POSITION][0]) };
	_float4				vRoot_Position_Local_Float3 = { *(_float4*)(&m_pRoot_Combined->m[CTransform::STATE_POSITION][0]) };

	_float				fL_Ball_Height = { vL_Ball_Position_Local_Float3.y };
	_float				fR_Ball_Height = { vR_Ball_Position_Local_Float3.y };
	_float				fRootHeight = { vRoot_Position_Local_Float3.y };

	_float				fDistanceYToLBall = { fL_Ball_Height - fRootHeight };
	_float				fDistanceYToRBall = { fR_Ball_Height - fRootHeight };

	_float				fRange = { 12.f };

	static _bool m_isUp_L_Leg = false;
	static _bool m_isUp_R_Leg = false;

	static _float fPrevDistanceYToLBall = {};
	static _float fPrevDistanceYToRBall = {};

	if (fRange < fDistanceYToLBall &&
		fRange >= fPrevDistanceYToLBall)
	{
		m_isUp_L_Leg = true;
	}

	if (fRange < fDistanceYToRBall &&
		fRange >= fPrevDistanceYToRBall)
	{
		m_isUp_R_Leg = true;
	}

	if (fRange > fDistanceYToLBall && true == m_isUp_L_Leg)
	{
		Change_Sound_3D(TEXT("Sound_Player_FootStep_Stone"), 15, 4);
		m_isUp_L_Leg = false;
	}

	if (fRange > fDistanceYToRBall && true == m_isUp_R_Leg)
	{
		Change_Sound_3D(TEXT("Sound_Player_FootStep_Stone"), 15, 4);
		m_isUp_R_Leg = false;
	}

	fPrevDistanceYToLBall = fDistanceYToLBall;
	fPrevDistanceYToRBall = fDistanceYToRBall;
}

void CPlayer::Turn_Spine_Default(_float fTimeDelta)
{
	CModel* pModel = { Get_Body_Model() };
	if (nullptr != pModel)
	{
		_vector				vMyLook = { m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK) };
		_float4				vHeadWorldLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
		_float				fHeadMagnitude = sqrt(vHeadWorldLook.x * vHeadWorldLook.x + vHeadWorldLook.z * vHeadWorldLook.z);

		_matrix				CamWorldMatrix = { m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW) };
		_float4				vCamLook = { CamWorldMatrix.r[CTransform::STATE_LOOK] };
		_float				fCamMagnitude = sqrt(vCamLook.x * vCamLook.x + vCamLook.z * vCamLook.z);

		_float				fDot = { vHeadWorldLook.x * vCamLook.x + vHeadWorldLook.z * vCamLook.z };
		_float				fAngle = acos(fDot / (fHeadMagnitude * fCamMagnitude));
		_vector				vRotateAxis = { m_pTransformCom->Get_State_Vector(CTransform::STATE_UP) };

		_float4				vHeadToCam = CamWorldMatrix.r[CTransform::STATE_POSITION] - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
		_float4				vCross = XMVector3Cross(vMyLook, vHeadToCam);

		if (fAngle < XMConvertToRadians(100.f)) {
			fAngle = 0.f;
		}
		else {
			fAngle = XMConvertToRadians(45.f);
		}

		if (vCross.y > 0) {
			fAngle *= -1;
		}

		static _float fCurAngle = 0.f;
		if (!m_isTurnSpineDefault) {
			if (fabs(fCurAngle) > 0.0001) {
				fCurAngle += fTimeDelta * -fCurAngle * 2;
			}
			else {
				fCurAngle = 0.f;
				return;
			}
		}
		else {
			fCurAngle += fTimeDelta * (fAngle - fCurAngle) * 2;
		}

		list<_uint>			ChildJointIndices;
		pModel->Get_Child_ZointIndices("spine_0", "head", ChildJointIndices);

		ChildJointIndices.pop_back();

		_uint				iNumChildJoint = { static_cast<_uint>(ChildJointIndices.size()) };
		_float				fDevidedAngle = { fCurAngle / iNumChildJoint };
		vector<string>		BoneNames = { pModel->Get_BoneNames() };

		vRotateAxis = XMVector3TransformNormal(vRotateAxis, m_pTransformCom->Get_WorldMatrix_Inverse());

		_vector				vNewQuaternion = { XMQuaternionRotationAxis(vRotateAxis, fDevidedAngle) };

		vNewQuaternion = XMVectorSetX(vNewQuaternion, 0.f);
		vNewQuaternion = XMQuaternionNormalize(vNewQuaternion);

		_matrix				RotationMatrix = { XMMatrixRotationQuaternion(vNewQuaternion) };

		for (auto& iJointIndex : ChildJointIndices)
		{
			pModel->Add_Additional_Transformation_World(BoneNames[iJointIndex], RotationMatrix);
		}
	}
}

void CPlayer::Turn_Spine_Hold(_float fTimeDelta)
{
	CModel* pModel = { Get_Body_Model() };
	if (nullptr != pModel)
	{
		_vector				vMyLook = { m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK) };
		_float4				vHeadWorldLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
		_float				fHeadMagnitude = sqrt(vHeadWorldLook.x * vHeadWorldLook.x + vHeadWorldLook.y * vHeadWorldLook.y + vHeadWorldLook.z * vHeadWorldLook.z);
		_float				fHeadWorldAngle = acosf(vHeadWorldLook.y / fHeadMagnitude);

		_matrix				CamWorldMatrix = { m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW) };
		_float4				vCamLook = { CamWorldMatrix.r[CTransform::STATE_LOOK] };
		_float				fCamMagnitude = sqrt(vCamLook.x * vCamLook.x + vCamLook.y * vCamLook.y + vCamLook.z * vCamLook.z);
		_float				fCamAngle = acosf(vCamLook.y / fCamMagnitude);

		_vector				vRotateAxis = { m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT) };
		_float				fAngle = fabs(fHeadWorldAngle - fCamAngle);

		if (vHeadWorldLook.y < vCamLook.y) {
			fAngle *= -1;
		}

		static _float fCurAngle = 0.f;
		if (!m_isTurnSpineHold) {
			if (fabs(fCurAngle) > 0.0001) {
				fCurAngle += fTimeDelta * -fCurAngle * 10;
			}
			else {
				fCurAngle = 0.f;
				return;
			}
		}
		else {
			fCurAngle += fTimeDelta * (fAngle - fCurAngle) * 10;
		}

		list<_uint>			ChildJointIndices;
		pModel->Get_Child_ZointIndices("spine_0", "r_arm_clavicle", ChildJointIndices);

		ChildJointIndices.pop_back();

		_uint				iNumChildJoint = { static_cast<_uint>(ChildJointIndices.size()) };
		_float				fDevidedAngle = { fCurAngle / iNumChildJoint };
		vector<string>		BoneNames = { pModel->Get_BoneNames() };

		vRotateAxis = XMVector3TransformNormal(vRotateAxis, m_pTransformCom->Get_WorldMatrix_Inverse());

		_vector				vNewQuaternion = { XMQuaternionRotationAxis(vRotateAxis, fDevidedAngle) };

		vNewQuaternion = XMVectorSetY(vNewQuaternion, 0.f);
		vNewQuaternion = XMVectorSetZ(vNewQuaternion, 0.f);
		//vNewQuaternion *= -1;
		vNewQuaternion = XMQuaternionNormalize(vNewQuaternion);

		_matrix				RotationMatrix = { XMMatrixRotationQuaternion(vNewQuaternion) };

		for (auto& iJointIndex : ChildJointIndices)
		{
			pModel->Add_Additional_Transformation_World(BoneNames[iJointIndex], RotationMatrix);
		}
	}
}

void CPlayer::Turn_Spine_Light(_float fTimeDelta)
{
	CModel* pModel = { Get_Body_Model() };
	if (nullptr != pModel)
	{
		_vector				vMyLook = { m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK) };
		_float4				vHeadWorldLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
		_float				fHeadMagnitude = sqrt(vHeadWorldLook.x * vHeadWorldLook.x + vHeadWorldLook.y * vHeadWorldLook.y + vHeadWorldLook.z * vHeadWorldLook.z);
		_float				fHeadWorldAngle = acosf(vHeadWorldLook.y / fHeadMagnitude);

		_matrix				CamWorldMatrix = { m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW) };
		_float4				vCamLook = { CamWorldMatrix.r[CTransform::STATE_LOOK] };
		_float				fCamMagnitude = sqrt(vCamLook.x * vCamLook.x + vCamLook.y * vCamLook.y + vCamLook.z * vCamLook.z);
		_float				fCamAngle = acosf(vCamLook.y / fCamMagnitude);

		_vector				vRotateAxis = { m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT) };
		_float				fAngle = fabs(fHeadWorldAngle - fCamAngle);

		if (vHeadWorldLook.y < vCamLook.y) {
			fAngle *= -1;
		}

		static _float fCurAngle = 0.f;
		if (!m_isTurnSpineLight) {
			if (fabs(fCurAngle) > 0.0001) {
				fCurAngle += fTimeDelta * -fCurAngle * 10;
			}
			else {
				fCurAngle = 0.f;
				return;
			}
		}
		else {
			fCurAngle += fTimeDelta * (fAngle - fCurAngle) * 10;
		}

		list<_uint>			ChildJointIndices;
		pModel->Get_Child_ZointIndices("l_arm_clavicle", "l_arm_wrist", ChildJointIndices);


		_uint				iNumChildJoint = { static_cast<_uint>(ChildJointIndices.size()) };
		_float				fDevidedAngle = { fCurAngle / iNumChildJoint };
		vector<string>		BoneNames = { pModel->Get_BoneNames() };

		vRotateAxis = XMVector3TransformNormal(vRotateAxis, m_pTransformCom->Get_WorldMatrix_Inverse());

		_vector				vNewQuaternion = { XMQuaternionRotationAxis(vRotateAxis, fDevidedAngle) };

		vNewQuaternion = XMVectorSetY(vNewQuaternion, 0.f);
		vNewQuaternion = XMVectorSetZ(vNewQuaternion, 0.f);
		//vNewQuaternion *= -1;
		vNewQuaternion = XMQuaternionNormalize(vNewQuaternion);
		_matrix				RotationMatrix = { XMMatrixRotationQuaternion(vNewQuaternion) };

		for (auto& iJointIndex : ChildJointIndices)
		{
			pModel->Add_Additional_Transformation_World(BoneNames[iJointIndex], RotationMatrix);
		}
	}
}

void CPlayer::SetMoveDir()
{
	//F
	if (m_bMove_Forward == true && m_bMove_Backward == false && m_bMove_Right == false && m_bMove_Left == false)
	{
		m_eMoveDir = MD_F;
	}

	//B
	if (m_bMove_Forward == false && m_bMove_Backward == true && m_bMove_Right == false && m_bMove_Left == false)
	{
		m_eMoveDir = MD_B;
	}

	//R
	if (m_bMove_Forward == false && m_bMove_Backward == false && m_bMove_Right == true && m_bMove_Left == false)
	{
		m_eMoveDir = MD_R;
	}

	//L
	if (m_bMove_Forward == false && m_bMove_Backward == false && m_bMove_Right == false && m_bMove_Left == true)
	{
		m_eMoveDir = MD_L;
	}

	//FR
	if (m_bMove_Forward == true && m_bMove_Backward == false && m_bMove_Right == true && m_bMove_Left == false)
	{
		m_eMoveDir = MD_FR;
	}

	//FL
	if (m_bMove_Forward == true && m_bMove_Backward == false && m_bMove_Right == false && m_bMove_Left == true)
	{
		m_eMoveDir = MD_FL;
	}

	//BR
	if (m_bMove_Forward == false && m_bMove_Backward == true && m_bMove_Right == true && m_bMove_Left == false)
	{
		m_eMoveDir = MD_BR;
	}

	//BL
	if (m_bMove_Forward == false && m_bMove_Backward == true && m_bMove_Right == false && m_bMove_Left == true)
	{
		m_eMoveDir = MD_BL;
	}

	if (m_eMoveDir != m_ePrevMoveDir)
	{
		m_fLerpTime = 0.f;
		m_bLerp_Move = true;
		m_bLerp = true;
	}

	m_ePrevMoveDir = m_eMoveDir;
}

void CPlayer::ResetCamera()
{
	auto Char_RotMat = m_pTransformCom->Get_RotationMatrix_Pure();
	m_pTransformCom_Camera->Set_RotationMatrix_Pure(Char_RotMat);

	m_fRight_Dist_Look = m_fRight_Dist_Look_Default;
	m_fUp_Dist_Look = m_fUp_Dist_Look_Default;
	m_fLook_Dist_Look = m_fLook_Dist_Look_Default;

	m_fLerpAmount_Right = m_fRight_Dist_Pos;
	m_fLerpAmount_Up = m_fUp_Dist_Pos;
	m_fLerpAmount_Look = m_fLook_Dist_Pos;

	Calc_Camera_LookAt_Point(1 / 60.f);
}

void CPlayer::Apply_Recoil(_float fTimeDelta)
{
	//m_fRecoil_Lerp_Time += fTimeDelta*10.f;
	m_fRecoil_Lerp_Time_Omega += fTimeDelta * 20.f;
	if (m_fRecoil_Lerp_Time_Omega >= PxPiDivTwo)
	{
		m_fRecoil_Lerp_Time_Omega = PxPiDivTwo;
	}
	m_fRecoil_Lerp_Time = sin(m_fRecoil_Lerp_Time_Omega);

	m_fRecoil_Rotate_Amount_X_Current = Lerp(0, m_fRecoil_Rotate_Amount_X, m_fRecoil_Lerp_Time);
	m_fRecoil_Rotate_Amount_Y_Current = Lerp(0, m_fRecoil_Rotate_Amount_Y, m_fRecoil_Lerp_Time);

	if ((m_fRecoil_Rotate_Amount_X_Current >= m_fRecoil_Rotate_Amount_X) || (m_fRecoil_Rotate_Amount_Y_Current >= m_fRecoil_Rotate_Amount_Y))
	{
		m_bRecoil = false;
		m_fRecoil_Rotate_Amount_X = 0.f;
		m_fRecoil_Rotate_Amount_X_Current = 0.f;
		m_fRecoil_Rotate_Amount_Y = 0.f;
		m_fRecoil_Rotate_Amount_Y_Current = 0.f;
	}

	m_pTransformCom_Camera->Turn(m_pTransformCom->Get_State_Vector(CTransform::STATE_UP), -fTimeDelta * 10.f * m_fRecoil_Rotate_Amount_X);
	m_pTransformCom_Camera->Turn(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT), -fTimeDelta * 10.f * m_fRecoil_Rotate_Amount_Y);
}

void CPlayer::Set_ManualMove(_bool isManualMove)
{
	m_isManualMove = isManualMove;

	if (false == m_isManualMove)
	{
		_float4				vPosition = { m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION) };
		vPosition.y += CONTROLLER_GROUND_GAP;
		m_pController->SetPosition(vPosition);
	}
}

void CPlayer::Move_Manual(_fmatrix WorldMatrix)
{
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	_float4				vPosition = { m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION) };
	vPosition.y += CONTROLLER_GROUND_GAP;
	m_pController->SetPosition(vPosition);
}

void CPlayer::Set_Muzzle_Smoke()
{
	if (m_eEquip == HG)
	{
		if (m_bMuzzleSmoke)
		{
			if (m_MuzzleSmoke_Delay + m_MuzzleSmoke_Time < GetTickCount64())
			{
				m_pMuzzle_Smoke->Set_Render(true);
				m_pMuzzle_Smoke->SetPosition(m_vMuzzle_Smoke_Pos);
				m_bMuzzleSmoke = false;
			}
		}
	}
}

void CPlayer::PickUp_Item(CGameObject* pPickedUp_Item)
{
	m_pGameInstance->Set_IsPaused(true);

	m_pTabWindow->PickUp_Item(pPickedUp_Item);

	m_isCamTurn = true;
}

void CPlayer::Interact_Props(CGameObject* pPickedUp_Item)
{
	m_pGameInstance->Set_IsPaused(true);

	m_pTabWindow->Interact_Props(pPickedUp_Item);

	m_isCamTurn = true;
}

void CPlayer::Set_Weapon_Accessories(ITEM_NUMBER eCallItemType, _uint iAccessories)
{
	switch (eCallItemType)
	{
	case Client::HandGun:
		switch (iAccessories)
		{
		case 0:
			m_Weapons[HG]->Set_Weapon_Accessory("wp0000vp70_1_Group_1_Sub_1__wp0100_VP70Custom_Mat_mesh0002", false);
			break;

		case 1:
			m_Weapons[HG]->Set_Weapon_Accessory("wp0000vp70_1_Group_2_Sub_1__wp0000_PowerUp_Mat_mesh0003", false);
			break;

		case 2:
			m_Weapons[HG]->Set_Weapon_Accessory("wp0000vp70_1_Group_6_Sub_1__wp0000_PowerUp_Mat_mesh0004", false);
			break;

		default:
			break;
		}

		break;


	case Client::ShotGun:
		switch (iAccessories)
		{
		case 0:
			m_Weapons[STG]->Set_Weapon_Accessory("wp1000shotgun_1_Group_3_Sub_1__wp1000_mt_mesh0004", false);
			break;

		case 1:
			m_Weapons[STG]->Set_Weapon_Accessory("wp1000shotgun_1_Group_4_Sub_1__wp1100_mt_mesh0005", false);
			break;

		default:
			break;
		}

		break;

	default:
		break;
	}

}

void CPlayer::RayCast_Shoot()
{
	_float4 vBlockPoint;
	_float4 vBlockNormal;
	_bool	bHit_Props = false;

	_bool isHit = false; // 사운드 재생용 Hit 성공 체크

	if (m_eEquip == STG)
	{
		auto vCamPos = m_pCamera->GetPosition();
		auto vCamLook = m_pCamera->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK);

		_bool bDynamic = false;

		if (m_pGameInstance->RayCast_Effect(m_pCamera->GetPosition(), m_pCamera->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK), &vBlockPoint, &vBlockNormal, false, &bDynamic))
		{
			auto iIndex = m_pGameInstance->GetRandom_Int(0, HIT_PROPS_EFFECT_TYPE_COUNT - 1);
			m_vecHit_Props_HG[iIndex]->Set_Render(true);
			m_vecHit_Props_HG[iIndex]->SetPosition(vBlockPoint);

			if (bDynamic == false)
			{
				m_vecDecal_BulletHole[m_iDecal_Index]->Set_Render(true);
				m_vecDecal_BulletHole[m_iDecal_Index]->SetPosition(vBlockPoint);
				m_vecDecal_BulletHole[m_iDecal_Index]->LookAt(vBlockNormal);
				++m_iDecal_Index;

				if (m_iDecal_Index >= m_vecDecal_BulletHole.size())
				{
					m_iDecal_Index = 0;
				}
			}
		}
		else
		{
			isHit = m_pGameInstance->RayCast_Shoot(m_pCamera->GetPosition(), m_pCamera->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK), &vBlockPoint, &vBlockNormal, true, true, &bHit_Props);
		}

		for (size_t i = 0; i < SHOTGUN_BULLET_COUNT; ++i)
		{
			auto vDelta_Random = _float4(m_pGameInstance->GetRandom_Real(-SHOTGUN_SPREAD_AMOUNT, SHOTGUN_SPREAD_AMOUNT), m_pGameInstance->GetRandom_Real(-SHOTGUN_SPREAD_AMOUNT, SHOTGUN_SPREAD_AMOUNT), m_pGameInstance->GetRandom_Real(-SHOTGUN_SPREAD_AMOUNT, SHOTGUN_SPREAD_AMOUNT), 0.f);
			auto NewCamLook = vCamLook + vDelta_Random;
			NewCamLook = Float4_Normalize(NewCamLook);

			_bool bDynamic = false;
			if (m_pGameInstance->RayCast_Effect(vCamPos, NewCamLook, &vBlockPoint, &vBlockNormal, true, &bDynamic))
			{
				m_vecHit_Dynamic[i] = bDynamic;
				continue;
			}

			m_pGameInstance->RayCast_Shoot(vCamPos, NewCamLook, &vBlockPoint, &vBlockNormal, true, false, &bHit_Props);
		}

		auto BlockPoints = m_pGameInstance->GetBlockPoints_Props();
		auto BlockNormals = m_pGameInstance->GetBlockNormals_Props();

		for (size_t i = 0; i < BlockPoints->size(); ++i)
		{
			m_vecHit_Props_SG[i]->Set_Render(true);
			m_vecHit_Props_SG[i]->SetPosition((*BlockPoints)[i]);

			if (m_vecHit_Dynamic[i] == false)
			{
				m_vecDecal_BulletHole[m_iDecal_Index]->Set_Render(true);
				m_vecDecal_BulletHole[m_iDecal_Index]->SetPosition((*BlockPoints)[i]);
				m_vecDecal_BulletHole[m_iDecal_Index]->LookAt((*BlockNormals)[i]);
				++m_iDecal_Index;

				if (m_iDecal_Index >= m_vecDecal_BulletHole.size())
				{
					m_iDecal_Index = 0;
				}
			}
		}

		BlockPoints->clear();
		BlockNormals->clear();
	}
	else
	{
		_bool bDynamic = false;

		if (m_pGameInstance->RayCast_Effect(m_pCamera->GetPosition(), m_pCamera->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK), &vBlockPoint, &vBlockNormal, false, &bDynamic))
		{
			auto iIndex = m_pGameInstance->GetRandom_Int(0, HIT_PROPS_EFFECT_TYPE_COUNT - 1);
			m_vecHit_Props_HG[iIndex]->Set_Render(true);
			m_vecHit_Props_HG[iIndex]->SetPosition(vBlockPoint);

			if (bDynamic == false)
			{
				m_vecDecal_BulletHole[m_iDecal_Index]->Set_Render(true);
				m_vecDecal_BulletHole[m_iDecal_Index]->SetPosition(vBlockPoint);
				m_vecDecal_BulletHole[m_iDecal_Index]->LookAt(vBlockNormal);
				++m_iDecal_Index;

				if (m_iDecal_Index >= m_vecDecal_BulletHole.size())
				{
					m_iDecal_Index = 0;
				}
			}

			return;
		}

		isHit = m_pGameInstance->RayCast_Shoot(m_pCamera->GetPosition(), m_pCamera->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK), &vBlockPoint, &vBlockNormal, false, true, &bHit_Props);

	}
	static _int iMissCnt = 0;

	if (CCall_Center::Get_Instance()->Is_Focus_Player()) {		// 조건 들어갈 예정 : 형준형의 변수 들어감
		if (!isHit &&
			(rand() % max((5 - iMissCnt), 1) == 0)
			) {
			wstring strSoundTag;
			_int iRandCnt;
			switch (m_iHp) {
			case 5:
				strSoundTag = TEXT("Fine");
				iRandCnt = 6;
				break;
			case 4:
			case 3:
				strSoundTag = TEXT("Caution");
				iRandCnt = 5;
				break;
			case 2:
			case 1:
				strSoundTag = TEXT("Danger");
				iRandCnt = 6;
				break;
			}

			strSoundTag = TEXT("Sound_Player_Abuse_") + strSoundTag;
			Change_Sound_3D(strSoundTag, iRandCnt, 3);

			iMissCnt = 0;
		}
		else {
			++iMissCnt;
		}
	}

}

#pragma endregion

void CPlayer::Calc_YPosition_Camera()
{
	_float4 YDelta = Sub_Float4(m_pController->GetPosition_Float4(), m_vPrev_Position);
	YDelta.z = 0.f;
	YDelta.x = 0.f;
	m_vCameraPosition = Add_Float4_Coord(m_vCameraPosition, YDelta);
	m_vCamera_LookAt_Point = Add_Float4_Coord(m_vCamera_LookAt_Point, YDelta);
}

void CPlayer::Calc_Camera_Transform(_float fTimeDelta)
{
	_vector vLook = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_LOOK);
	_vector vUp = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_UP);
	_vector vRight = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT);
	_vector vLookAtPoint;
	_vector vPos = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_POSITION);

	auto Pos = m_pCamera->Get_Position_Float4();

	switch (m_eMoveDir)
	{
	case MD_F:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.85f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 1.15f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 1.15f);
		}
		break;
	case MD_B:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.4f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.4f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.9f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.9f);
		}
		break;
	case MD_R:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.5f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 0.85f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.5f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 0.85f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos);
		}
		break;
	case MD_L:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.5f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 1.15f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 1.15f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.5f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 1.15f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 1.15f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos);
		}
		break;
	case MD_FR:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 0.85f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.85f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 1.15f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 0.85f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 1.15f);
		}
		break;
	case MD_FL:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 1.15f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 1.15f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.85f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 1.15f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 1.15f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 1.15f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 1.15f);
		}
		break;
	case MD_BR:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.4f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 0.85f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.4f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 0.85f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.85f);
		}
		break;
	case MD_BL:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.4f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 1.15f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 1.15f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.4f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.85f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos * 1.15f, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos * 1.15f) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.85f);
		}
		break;
	case MD_DEFAULT:
		if (m_bAim)
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos * 0.5f, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos * 0.5f);
		}
		else
		{
			m_fLerpAmount_Look = Lerp(m_fLerpAmount_Look, m_fLook_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Right = Lerp(m_fLerpAmount_Right, m_fRight_Dist_Pos, m_fLerpTime);
			m_fLerpAmount_Up = Lerp(m_fLerpAmount_Up, m_fUp_Dist_Pos, m_fLerpTime);

			m_vCameraPosition = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Pos) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Pos) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Pos);
		}
		break;
	}

	if (m_bCollision_Lerp == false)
	{
		if (m_bLerp)
		{
			_vector vOrigin = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fLerpAmount_Right) + XMVectorScale(XMVector4Normalize(vUp), m_fLerpAmount_Up) + XMVectorScale(XMVector4Normalize(vLook), m_fLerpAmount_Look);
			_vector NewPos = XMVectorLerp(vOrigin, m_vCameraPosition, m_fLerpTime);
			m_pCamera->SetPosition(NewPos);
		}
		else
		{
			m_pCamera->SetPosition(m_vCameraPosition);
		}
	}
	else
	{
		if (m_bLerp)
		{
			_vector NewPos = XMVectorLerp(m_pCamera->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION), m_vCameraPosition, m_fLerpTime);
			m_pCamera->SetPosition(NewPos);
		}
		else
		{
			m_pCamera->SetPosition(m_vCameraPosition);
		}
	}

	vLookAtPoint = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Look) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Look) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Look);
	XMStoreFloat4(&m_vCamera_LookAt_Point, vLookAtPoint);
	m_pCamera->LookAt(m_vCamera_LookAt_Point);

	m_vLookPoint_To_Position_Dir = Sub_Float4(m_vCamera_LookAt_Point, m_vCameraPosition);

	_vector vCamPos = XMLoadFloat4(&m_vCameraPosition);
	_vector vLookAtPos = XMLoadFloat4(&m_vCamera_LookAt_Point);
	_vector vDelta = XMVector4Normalize(vLookAtPos - vCamPos);
	XMStoreFloat4(&m_vRayDir, vDelta);

	_vector vCharPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	vCharPos = XMVectorSetY(vCharPos, XMVectorGetY(vCharPos) + 1.f);
	_vector vCameraPos = XMLoadFloat4(&m_vCameraPosition);

	m_fRayDist = XMVectorGetX(XMVector3Length(vCharPos - vCameraPos));
}

void CPlayer::Calc_Camera_LookAt_Point(_float fTimeDelta)
{
	POINT		ptDeltaPos = { m_pGameInstance->Get_MouseDeltaPos() };
	_float		fMouseSensor = { 0.1f };

	if (abs(ptDeltaPos.x) > 2.5f || abs(ptDeltaPos.y) > 2.5f)
	{
		m_bRotate_Delay = true;
		m_bRotate_Delay_Start = false;
		m_fRotate_Delay_Amount = 1.f;

		m_fPrev_Rotate_Amount_X = ptDeltaPos.x * 0.1f;
		m_fPrev_Rotate_Amount_Y = ptDeltaPos.y * 0.1f;
	}

	if (m_bRotate_Delay && (abs(ptDeltaPos.x) <= 2.5f && abs(ptDeltaPos.y) <= 2.5f))
	{
		m_bRotate_Delay_Start = true;
		m_fRotate_Delay_Amount = 1.f;
		m_bRotate_Delay = false;
	}

	if (m_bRotate_Delay_Start == true)
	{
		m_fRotate_Delay_Amount -= (fTimeDelta * 2.5f);

		if (m_fRotate_Delay_Amount <= 0.f)
		{
			m_bRotate_Delay_Start = false;
			m_fRotate_Delay_Amount = 0.f;
		}
	}

	if (m_bRotate_Delay_Start)
	{
		m_fRotate_Amount_X = (fTimeDelta * m_fRotate_Delay_Amount) * (_float)m_fPrev_Rotate_Amount_X * fMouseSensor;
		m_fRotate_Amount_Y = (fTimeDelta * m_fRotate_Delay_Amount) * (_float)m_fPrev_Rotate_Amount_Y * fMouseSensor;
	}
	else
	{
		m_fRotate_Amount_X = fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor;
		m_fRotate_Amount_Y = fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor;
	}

	if (m_bCollision_Lerp == false)
	{
		if (m_bAim == false)
			m_fLerpTime += fTimeDelta * 0.7f;
		else
			m_fLerpTime += fTimeDelta;
	}
	else
	{
		m_fLerpTime += fTimeDelta * 1.5f;
	}

	if (m_fLerpTime >= 1.f)
	{
		m_bLerp = false;
		m_fLerpTime = 1.f;

		if (m_bCollision_Lerp)
			m_bCollision_Lerp = false;
	}

	if (m_bTurnAround == false)
	{
		if (m_bAim == false)
		{
			//Parallel Rotate
			{
				m_pTransformCom_Camera->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRotate_Amount_X);
			}

			//Vertical Rotate
			{
				m_pTransformCom_Camera->Turn(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT), m_fRotate_Amount_Y);

				auto Camera_Look = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_LOOK);
				auto Camera_Parallel_Look = XMVectorSetY(Camera_Look, 0.f);

				if (abs(GetAngleBetweenVectors_Radians(Camera_Look, Camera_Parallel_Look)) > 1.f)
				{
					m_pTransformCom_Camera->Turn(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT), -m_fRotate_Amount_Y);
				}
			}
		}
		else
		{
			//Parallel Rotate
			if ((_long)0 != ptDeltaPos.x)
			{
				m_pTransformCom_Camera->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRotate_Amount_X);
			}

			//Vertical Rotate
			if ((_long)0 != ptDeltaPos.y)
			{
				m_pTransformCom_Camera->Turn(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT), m_fRotate_Amount_Y);

				auto Camera_Look = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_LOOK);
				auto Camera_Parallel_Look = XMVectorSetY(Camera_Look, 0.f);
				if (abs(GetAngleBetweenVectors_Radians(Camera_Look, Camera_Parallel_Look)) > 1.f)
				{
					m_pTransformCom_Camera->Turn(m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT), -m_fRotate_Amount_Y);
				}
			}
		}
	}
	else
	{
		m_fTurnAround_Time += fTimeDelta;

		if (m_fTurnAround_Time >= 1.f)
		{
			m_fTurnAround_Time = 1.f;
			m_bTurnAround = false;
		}

		auto CamPos = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_POSITION);
		auto Look = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_LOOK);
		XMStoreFloat4(&Look, XMVectorSlerp(XMVector3Normalize(XMLoadFloat4(&Look)), XMLoadFloat4(&m_vTurnAround_Look_Vector), m_fTurnAround_Time));
		CamPos = Add_Float4_Coord(CamPos, Look);

		m_pTransformCom_Camera->Look_At(CamPos);
	}

	Calc_Camera_Transform(fTimeDelta);

	if (DOWN == m_pGameInstance->Get_KeyState(VK_TAB))
		m_bMouseCursorClip = !m_bMouseCursorClip;

	if (true == m_bMouseCursorClip)
	{
		POINT ptPos = {};

		GetCursorPos(&ptPos);
		ScreenToClient(g_hWnd, &ptPos);

		RECT rc = {};
		GetClientRect(g_hWnd, &rc);

		ptPos.x = _long(_float(rc.right - rc.left) * 0.5f);
		ptPos.y = _long(_float(rc.bottom - rc.top) * 0.5f);

		m_pGameInstance->Set_MouseCurPos(ptPos);

		ClientToScreen(g_hWnd, &ptPos);
		SetCursorPos(ptPos.x, ptPos.y);
	}

	return;
}

HRESULT CPlayer::Ready_Camera()
{
	if (m_pCamera == nullptr)
		m_pCamera = dynamic_cast<CCamera_Free*>(*(*m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, g_strCameraTag)).begin());

	if (m_pCamera == nullptr)
		return E_FAIL;

	m_pCamera->Active_Camera(true);

	m_pCamera->SetPlayer(this);

	_vector vLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);
	_vector vUp = m_pTransformCom->Get_State_Vector(CTransform::STATE_UP);
	_vector vRight = m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT);
	_vector vLookAtPoint;
	_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	vLookAtPoint = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fRight_Dist_Look) + XMVectorScale(XMVector4Normalize(vUp), m_fUp_Dist_Look) + XMVectorScale(XMVector4Normalize(vLook), m_fLook_Dist_Look);
	XMStoreFloat4(&m_vCamera_LookAt_Point, vLookAtPoint);

	m_pCamera->SetPlayer(this);

	m_pCamera->SetPosition(m_vCameraPosition);
	m_pCamera->LookAt(m_vCamera_LookAt_Point);

	m_vLookPoint_To_Position_Dir = Sub_Float4(m_vCamera_LookAt_Point, m_vCameraPosition);

	_vector vCharPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	vCharPos = XMVectorSetY(vCharPos, XMVectorGetY(vCharPos) + 1.f);
	_vector vCameraPos = XMLoadFloat4(&m_vCameraPosition);

	m_fRayDist = XMVectorGetX(XMVector4Length(vCharPos - vCameraPos));

	m_vOrigin_LookAt_Point = m_vCamera_LookAt_Point;

	m_fLerpAmount_Look = m_fLook_Dist_Pos;
	m_fLerpAmount_Right = m_fRight_Dist_Pos;
	m_fLerpAmount_Up = m_fUp_Dist_Pos;

	m_pCamera->Bind_PipeLine();

	return S_OK;
}

void CPlayer::Load_CameraPosition()
{
	string filePath = "../Camera_Position/Camera_Position";

	//File Import
	ifstream File(filePath, std::ios::binary | std::ios::in);

	File.read((char*)&m_vCameraPosition, sizeof(_float4));
	File.read((char*)&m_fRight_Dist_Look, sizeof(_float));
	File.read((char*)&m_fUp_Dist_Look, sizeof(_float));
	File.read((char*)&m_fLook_Dist_Look, sizeof(_float));

	m_fRight_Dist_Look_Default = m_fRight_Dist_Look;
	m_fUp_Dist_Look_Default = m_fUp_Dist_Look;
	m_fLook_Dist_Look_Default = m_fLook_Dist_Look;

	m_fUp_Dist_Look -= CONTROLLER_GROUND_GAP;
	m_fUp_Dist_Look_Default -= CONTROLLER_GROUND_GAP;

	m_fLook_Dist_Pos = m_vCameraPosition.z;
	m_fRight_Dist_Pos = m_vCameraPosition.x;
	m_fUp_Dist_Pos = m_vCameraPosition.y - CONTROLLER_GROUND_GAP;

	File.close();
}

void CPlayer::RayCasting_Camera()
{
	_vector vLook = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_LOOK);
	_vector vUp = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_UP);
	_vector vRight = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_RIGHT);
	//_vector vLookAtPoint;
	_vector vPos = m_pTransformCom_Camera->Get_State_Vector(CTransform::STATE_POSITION);
	_float4 CamPos = vPos + XMVectorScale(XMVector4Normalize(vRight), m_fLerpAmount_Right) + XMVectorScale(XMVector4Normalize(vUp), m_fLerpAmount_Up) + XMVectorScale(XMVector4Normalize(vLook), m_fLerpAmount_Look);
	//_float4 CamPos = m_pCamera->Get_Position_Float4();

	_float4 BlockPoint;
	_float4 TempPos;
	//TempPos.x = CamPos.x + (m_vRayDir.x * m_fRayDist*1.5f);
	//TempPos.y = CamPos.y + (m_vRayDir.y * m_fRayDist*1.5f);
	//TempPos.z = CamPos.z + (m_vRayDir.z * m_fRayDist*1.5f);
	//TempPos.w = 1.f;

	TempPos = CamPos + (m_vRayDir * m_fRayDist);
	TempPos.w = 1.f;

	_float4 TempCamPos = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_POSITION);
	TempCamPos.y = m_pCamera->Get_Position_Float4().y;
	_float4 vTempDir = Float4_Normalize(m_pCamera->Get_Position_Float4() - TempCamPos);

	m_vRayDir.x *= -1.f;
	m_vRayDir.y *= -1.f;
	m_vRayDir.z *= -1.f;

	if (m_pGameInstance->SphereCast(TempPos, m_vRayDir, &BlockPoint, m_fRayDist))
	{
		m_fLerpTime = 0.f;
		m_bCollision_Lerp = true;
		m_bLerp = true;
		m_bMove_Forward = false;
		m_bMove_Backward = false;
		m_bLerp_Move = false;
		m_bMove = false;

		m_vRayDir.x *= -1.f;
		m_vRayDir.y *= -1.f;
		m_vRayDir.z *= -1.f;

		auto Position = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
		Position.y = m_vCameraPosition.y;
		_float4 DeltaDir = Float4_Normalize(Position - BlockPoint);
		BlockPoint.x += DeltaDir.x * 0.3f;
		BlockPoint.y += DeltaDir.y * 0.3f;
		BlockPoint.z += DeltaDir.z * 0.3f;

		auto vCharacterPos = m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_POSITION) - BlockPoint;

		m_pCamera->SetPosition(BlockPoint);
	}
	else
	{
		m_vRayDir.x *= -1.f;
		m_vRayDir.y *= -1.f;
		m_vRayDir.z *= -1.f;
	}
}

void CPlayer::Ready_Effect()
{
	m_pMuzzle_Flash = CMuzzle_Flash::Create(m_pDevice, m_pContext);
	m_pMuzzle_Flash->SetSize(0.3f, 0.3f);

	m_pMuzzle_Flash_SG = CMuzzle_Flash_SG::Create(m_pDevice, m_pContext);
	m_pMuzzle_Flash_SG->SetSize(0.6f, 0.6f);

	m_pMuzzle_Smoke = CMuzzle_Smoke::Create(m_pDevice, m_pContext);
	m_pMuzzle_Smoke->Initialize(nullptr);
	m_pMuzzle_Smoke->SetSize(0.2f, 0.2f);

	for (size_t i = 0; i < 2; ++i)
	{
		auto pShockWave = CMuzzle_ShockWave::Create(m_pDevice, m_pContext);
		pShockWave->SetType((_uint)i);
		pShockWave->Initialize(nullptr);
		pShockWave->SetSize(0.3f, 0.3f);
		m_vecShockWave.push_back(pShockWave);
	}

	m_vecHit_Props_HG.clear();
	for (size_t i = 0; i < HIT_PROPS_EFFECT_TYPE_COUNT; ++i)
	{
		auto pHit_Props = CHit_Props::Create(m_pDevice, m_pContext);
		pHit_Props->SetType((_uint)i);
		pHit_Props->Initialize(nullptr);
		pHit_Props->SetSize(0.6f, 0.6f);
		m_vecHit_Props_HG.push_back(pHit_Props);
	}

	m_vecHit_Props_SG.clear();
	for (size_t i = 0; i < SHOTGUN_BULLET_COUNT; ++i)
	{
		auto pHit_Props = CHit_Props::Create(m_pDevice, m_pContext);
		pHit_Props->SetType(i % 6);
		pHit_Props->Initialize(nullptr);
		pHit_Props->SetSize(0.6f, 0.6f);
		m_vecHit_Props_SG.push_back(pHit_Props);
	}

	m_vecDecal_BulletHole.clear();
	for (size_t i = 0; i < BULLET_HOLE_COUNT; ++i)
	{
		auto pDecal = CDecal_BulletHole::Create(m_pDevice, m_pContext);
		pDecal->Initialize(nullptr);
		m_vecDecal_BulletHole.push_back(pDecal);
	}

	m_vecHit_Dynamic.clear();
	m_vecHit_Dynamic.resize(SHOTGUN_BULLET_COUNT);

	m_vecHG_Cartridges.clear();
	for (size_t i = 0; i < HG_CARTRIDGE_COUNT; ++i)
	{
		auto pCartridge = CHG_Cartridge::Create(m_pDevice, m_pContext);
		m_vecHG_Cartridges.push_back(pCartridge);
	}
	m_vecHG_Cartridges[0]->Start();

	m_vecSG_Cartridges.clear();
	for (size_t i = 0; i < SG_CARTRIDGE_COUNT; ++i)
	{
		auto pCartridge = CSG_Cartridge::Create(m_pDevice, m_pContext);
		m_vecSG_Cartridges.push_back(pCartridge);
	}
	m_vecSG_Cartridges[0]->Start();
}

void CPlayer::Release_Effect()
{
	Safe_Release(m_pMuzzle_Flash);
	Safe_Release(m_pMuzzle_Flash_SG);
	Safe_Release(m_pMuzzle_Smoke);

	for (size_t i = 0; i < m_vecHit_Props_HG.size(); ++i)
	{
		Safe_Release(m_vecHit_Props_HG[i]);
	}

	for (size_t i = 0; i < m_vecShockWave.size(); ++i)
	{
		Safe_Release(m_vecShockWave[i]);
	}

	for (size_t i = 0; i < m_vecHit_Props_SG.size(); ++i)
	{
		Safe_Release(m_vecHit_Props_SG[i]);
	}

	for (size_t i = 0; i < m_vecDecal_BulletHole.size(); ++i)
	{
		Safe_Release(m_vecDecal_BulletHole[i]);
	}

	for (size_t i = 0; i < m_vecHG_Cartridges.size(); ++i)
	{
		Safe_Release(m_vecHG_Cartridges[i]);
	}

	for (size_t i = 0; i < m_vecSG_Cartridges.size(); ++i)
	{
		Safe_Release(m_vecSG_Cartridges[i]);
	}
}

void CPlayer::Tick_Effect(_float fTimeDelta)
{
	m_pMuzzle_Smoke->Tick(fTimeDelta);
	m_pMuzzle_Flash->Tick(fTimeDelta);

	if (m_eEquip == STG)
	{
		m_pMuzzle_Flash_SG->SetPosition(Get_MuzzlePosition());
		m_pMuzzle_Flash_SG->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecShockWave.size(); ++i)
	{
		m_vecShockWave[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecHit_Props_SG.size(); ++i)
	{
		m_vecHit_Props_SG[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecHit_Props_HG.size(); ++i)
	{
		m_vecHit_Props_HG[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecDecal_BulletHole.size(); ++i)
	{
		m_vecDecal_BulletHole[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecHG_Cartridges.size(); ++i)
	{
		m_vecHG_Cartridges[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecSG_Cartridges.size(); ++i)
	{
		m_vecSG_Cartridges[i]->Tick(fTimeDelta);
	}

	Set_Muzzle_Smoke();
}

void CPlayer::Late_Tick_Effect(_float fTimeDelta)
{
	m_pMuzzle_Smoke->Late_Tick(fTimeDelta);
	m_pMuzzle_Flash->Late_Tick(fTimeDelta);
	m_pMuzzle_Flash_SG->Late_Tick(fTimeDelta);

	for (size_t i = 0; i < m_vecShockWave.size(); ++i)
	{
		m_vecShockWave[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecHit_Props_SG.size(); ++i)
	{
		m_vecHit_Props_SG[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecHit_Props_HG.size(); ++i)
	{
		m_vecHit_Props_HG[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecDecal_BulletHole.size(); ++i)
	{
		m_vecDecal_BulletHole[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecHG_Cartridges.size(); ++i)
	{
		m_vecHG_Cartridges[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecSG_Cartridges.size(); ++i)
	{
		m_vecSG_Cartridges[i]->Late_Tick(fTimeDelta);
	}

}

void CPlayer::Initiate_Cartridge()
{
	if (m_eEquip == HG)
	{
		_float4 vDir = Get_CartridgeDir();
		_float4 vPos = Get_CartridgePosition();
		_float4 vLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
		m_vecHG_Cartridges[m_iHG_Cartridge_Index]->Initiate(vPos, vDir, Float4_Normalize(vLook));

		++m_iHG_Cartridge_Index;

		if (m_iHG_Cartridge_Index >= HG_CARTRIDGE_COUNT)
		{
			m_iHG_Cartridge_Index = 0;
		}
	}
	else
	{
		_float4 vDir = Get_CartridgeDir();
		_float4 vPos = Get_CartridgePosition();
		_float4 vLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
		m_vecSG_Cartridges[m_iSG_Cartridge_Index]->Initiate(vPos, vDir, Float4_Normalize(vLook));

		++m_iSG_Cartridge_Index;

		if (m_iSG_Cartridge_Index >= SG_CARTRIDGE_COUNT)
		{
			m_iSG_Cartridge_Index = 0;
		}
	}
}

void CPlayer::Calc_Decal_Map()
{
	if (m_bCalcDecalMap == false)
		return;

	_matrix WorldMat;

	switch (m_eBiteType)
	{
	case BITE_TYPE_FOR_EFFECT::STAND_FRONT:
	{
		WorldMat = XMLoadFloat4x4(m_pBodyModel->Get_CombinedMatrix("r_arm_clavicle")) * m_pTransformCom->Get_WorldMatrix();
		_vector vRight = WorldMat.r[0];
		_vector vUp = WorldMat.r[1];
		_vector vLook = WorldMat.r[2];
		vRight = XMVector3Normalize(vRight);
		vUp = XMVector3Normalize(vUp);
		vLook = XMVector3Normalize(vLook);
		WorldMat.r[0] = vRight;
		WorldMat.r[1] = vUp;
		WorldMat.r[2] = vLook;
		XMStoreFloat4x4(&m_DecalMatrix, WorldMat);

		Perform_Skinning();

		if (m_pHeadModel)
		{
			m_pHeadModel->SetDecalWorldMatrix_Player_Front(m_DecalMatrix);
			m_pHeadModel->Perform_Calc_DecalMap_Player();
		}
		break;
	}
	case BITE_TYPE_FOR_EFFECT::STAND_BACK:
	{
		WorldMat = XMLoadFloat4x4(m_pBodyModel->Get_CombinedMatrix("r_arm_humerus")) * m_pTransformCom->Get_WorldMatrix();
		_vector vRight = WorldMat.r[0];
		_vector vUp = WorldMat.r[1];
		_vector vLook = WorldMat.r[2];
		vRight = XMVector3Normalize(vRight);
		vUp = XMVector3Normalize(vUp);
		vLook = XMVector3Normalize(vLook);
		WorldMat.r[0] = vRight;
		WorldMat.r[1] = vUp;
		WorldMat.r[2] = vLook;
		XMStoreFloat4x4(&m_DecalMatrix, WorldMat);

		Perform_Skinning();

		if (m_pHeadModel)
		{
			m_pHeadModel->SetDecalWorldMatrix_Player_Back(m_DecalMatrix);
			m_pHeadModel->Perform_Calc_DecalMap_Player();
		}

		break;
	}
	}

	m_bCalcDecalMap = false;
}

void CPlayer::Perform_Skinning()
{
	//Face Model
	{
		if (nullptr != m_pHeadModel)
		{
			m_pHeadModel->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());

			list<_uint> NonHideIndex = m_pHeadModel->Get_NonHideMeshIndices();

			for (auto& i : NonHideIndex)
			{
				m_pHeadModel->Bind_Resource_Skinning(i);
				m_pGameInstance->Perform_Skinning((*m_pHeadModel->GetMeshes())[i]->GetNumVertices());
			}
		}
	}
}

HRESULT CPlayer::Add_Components()
{
	/* Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.vSize = _float3(0.8f, 1.2f, 0.8f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(Add_FSM_States()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_FSM_States()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_FSM"), (CComponent**)&m_pFSMCom)))
		return E_FAIL;

	m_pFSMCom->Add_State(MOVE, CPlayer_State_Move::Create(this));

	m_pFSMCom->Add_State(HOLD, CPlayer_State_Hold::Create(this));
	m_pFSMCom->Add_State(SUBHOLD, CPlayer_State_SubHold::Create(this));
	m_pFSMCom->Add_State(BITE, CPlayer_State_Bite::Create(this));

	m_pFSMCom->Change_State(MOVE);

	Change_Body_Animation_Move(0, CPlayer::ANIM_IDLE);
	Get_Body_Model()->Set_Loop(0, true);
	Get_Body_Model()->Set_BlendWeight(0, 1.f);
	Get_Body_Model()->Set_BlendWeight(1, 0.f);

	return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CPlayer::PART_END);

	/* For.Part_Body */
	CPartObject* pBodyObject = { nullptr };
	CBody_Player::BODY_DESC		BodyDesc{};

	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.pRootTranslation = &m_vRootTranslation;

	pBodyObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Body_Player"), &BodyDesc));
	if (nullptr == pBodyObject)
		return E_FAIL;

	m_PartObjects[CPlayer::PART_BODY] = pBodyObject;

	/* For.Part_Head */
	CPartObject* pHeadObject = { nullptr };
	CHead_Player::HEAD_DESC		HeadDesc{};

	HeadDesc.pParentsTransform = m_pTransformCom;
	HeadDesc.pState = &m_eState;

	pHeadObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Head_Player"), &HeadDesc));
	if (nullptr == pHeadObject)
		return E_FAIL;

	m_PartObjects[CPlayer::PART_HEAD] = pHeadObject;

	/* For.Part_Hair */
	CPartObject* pHairObject = { nullptr };
	CHair_Player::HAIR_DESC		HairDesc{};

	HairDesc.pParentsTransform = m_pTransformCom;
	HairDesc.pState = &m_eState;

	pHairObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Hair_Player"), &HairDesc));
	if (nullptr == pHairObject)
		return E_FAIL;

	m_PartObjects[CPlayer::PART_HAIR] = pHairObject;

	CPartObject* pFlashLightObject = { nullptr };
	CPartObject::PARTOBJECT_DESC		FlashLightDesc{};
	FlashLightDesc.pParentsTransform = m_pTransformCom;

	pFlashLightObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_FlashLight"), &FlashLightDesc));
	if (nullptr == pFlashLightObject)
		return E_FAIL;
	m_PartObjects[CPlayer::PART_LIGHT] = pFlashLightObject;


	/*CPartObject* pKnifeObject = { nullptr };
	CPartObject::PARTOBJECT_DESC		KnifeDesc{};
	KnifeDesc.pParentsTransform = m_pTransformCom;

	pKnifeObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Knife"), &KnifeDesc));
	if (nullptr == pKnifeObject)
		return E_FAIL;
	m_PartObjects[CPlayer::PART_KNIFE] = pKnifeObject;
	static_cast<CKnife*>(pKnifeObject)->Set_Socket_Ptr({ const_cast<_float4x4*>(Get_Body_Model()->Get_CombinedMatrix("setProp_D_00")) });*/


	m_Weapons.clear();
	m_Weapons.resize(CPlayer::NONE);

	///* For.Part_Weapon */
	CWeapon* pWeaponObject = { nullptr };
	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentsTransform = m_pTransformCom;

	WeaponDesc.eEquip = HG;
	WeaponDesc.eSetprops_Location = SETPROPS_A;
	WeaponDesc.pSocket[CWeapon::MOVE] = WeaponDesc.pSocket[CWeapon::MOVE_LIGHT] = WeaponDesc.pSocket[CWeapon::HOLD] = { const_cast<_float4x4*>(Get_Body_Model()->Get_CombinedMatrix("r_weapon")) };
	WeaponDesc.pSocket[CWeapon::HOLSTER] = { const_cast<_float4x4*>(Get_Body_Model()->Get_CombinedMatrix("setProp_A_00")) };

	_matrix			WeaponTransformMatrix = { XMMatrixRotationY(XMConvertToRadians(90.f)) };
	WeaponTransformMatrix *= XMMatrixRotationX(XMConvertToRadians(-90.f));
	WeaponDesc.fTransformationMatrices[CWeapon::MOVE] = WeaponDesc.fTransformationMatrices[CWeapon::MOVE_LIGHT]
		= WeaponDesc.fTransformationMatrices[CWeapon::HOLD] = WeaponTransformMatrix;

	WeaponTransformMatrix = { XMMatrixIdentity() };
	WeaponTransformMatrix *= XMMatrixRotationX(XMConvertToRadians(-100.f));
	WeaponTransformMatrix *= XMMatrixTranslation(0.f, 1.f, 6.f);
	WeaponDesc.fTransformationMatrices[CWeapon::HOLSTER] = WeaponTransformMatrix;
	pWeaponObject = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Weapon"), &WeaponDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;
	m_Weapons[CPlayer::HG] = pWeaponObject;

	WeaponDesc.eEquip = STG;
	WeaponDesc.eSetprops_Location = SETPROPS_E;
	WeaponDesc.pSocket[CWeapon::MOVE] = WeaponDesc.pSocket[CWeapon::MOVE_LIGHT] = WeaponDesc.pSocket[CWeapon::HOLD] = { const_cast<_float4x4*>(Get_Body_Model()->Get_CombinedMatrix("r_weapon")) };
	WeaponDesc.pSocket[CWeapon::HOLSTER] = { const_cast<_float4x4*>(Get_Body_Model()->Get_CombinedMatrix("setProp_E_00")) };

	WeaponTransformMatrix = { XMMatrixRotationY(XMConvertToRadians(90.f)) };
	WeaponTransformMatrix *= XMMatrixRotationX(XMConvertToRadians(-90.f));
	WeaponTransformMatrix *= XMMatrixRotationZ(XMConvertToRadians(-25.f));
	WeaponTransformMatrix *= XMMatrixTranslation(0.f, 0.f, -5.f);
	WeaponDesc.fTransformationMatrices[CWeapon::MOVE] = WeaponDesc.fTransformationMatrices[CWeapon::HOLD] = WeaponTransformMatrix;

	WeaponTransformMatrix = { XMMatrixIdentity() };
	WeaponTransformMatrix *= XMMatrixRotationZ(XMConvertToRadians(-90.f));
	WeaponTransformMatrix *= XMMatrixTranslation(0.f, 0.f, 30.f);
	WeaponDesc.fTransformationMatrices[CWeapon::MOVE_LIGHT] = WeaponTransformMatrix;

	WeaponTransformMatrix = XMMatrixIdentity();
	WeaponTransformMatrix *= XMMatrixRotationX(XMConvertToRadians(-95.f));
	WeaponTransformMatrix *= XMMatrixRotationY(XMConvertToRadians(100.f));
	WeaponTransformMatrix *= XMMatrixTranslation(0.f, 5.f, 2.f);
	WeaponDesc.fTransformationMatrices[CWeapon::HOLSTER] = WeaponTransformMatrix;

	pWeaponObject = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Weapon"), &WeaponDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;
	m_Weapons[CPlayer::STG] = pWeaponObject;

	WeaponDesc.eEquip = GRENADE;
	WeaponDesc.eSetprops_Location = SETPROPS_D;
	WeaponDesc.pSocket[CWeapon::MOVE] = WeaponDesc.pSocket[CWeapon::MOVE_LIGHT] = WeaponDesc.pSocket[CWeapon::HOLD] = { const_cast<_float4x4*>(Get_Body_Model()->Get_CombinedMatrix("r_weapon")) };
	WeaponDesc.pSocket[CWeapon::HOLSTER] = { const_cast<_float4x4*>(Get_Body_Model()->Get_CombinedMatrix("setProp_D_00")) };

	WeaponTransformMatrix = { XMMatrixIdentity() };
	WeaponTransformMatrix *= XMMatrixRotationX(XMConvertToRadians(-90.f));
	WeaponDesc.fTransformationMatrices[CWeapon::MOVE] = WeaponDesc.fTransformationMatrices[CWeapon::HOLD] =
		WeaponDesc.fTransformationMatrices[CWeapon::MOVE_LIGHT] = WeaponTransformMatrix;


	WeaponTransformMatrix = { XMMatrixIdentity() };
	WeaponTransformMatrix *= XMMatrixTranslation(4.f, 0.f, 0.f);
	WeaponDesc.fTransformationMatrices[CWeapon::HOLSTER] = WeaponTransformMatrix;

	pWeaponObject = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Weapon"), &WeaponDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;
	m_Weapons[CPlayer::GRENADE] = pWeaponObject;

	WeaponDesc.eEquip = FLASHBANG;
	WeaponDesc.pSocket[CWeapon::MOVE] = WeaponDesc.pSocket[CWeapon::MOVE_LIGHT] = WeaponDesc.pSocket[CWeapon::HOLD] = { const_cast<_float4x4*>(Get_Body_Model()->Get_CombinedMatrix("r_weapon")) };
	WeaponDesc.pSocket[CWeapon::HOLSTER] = { const_cast<_float4x4*>(Get_Body_Model()->Get_CombinedMatrix("setProp_D_00")) };

	WeaponTransformMatrix = { XMMatrixIdentity() };
	WeaponDesc.fTransformationMatrices[CWeapon::MOVE] = WeaponDesc.fTransformationMatrices[CWeapon::HOLD] = WeaponTransformMatrix;

	WeaponDesc.fTransformationMatrices[CWeapon::MOVE_LIGHT] = WeaponTransformMatrix;

	WeaponDesc.fTransformationMatrices[CWeapon::HOLSTER] = WeaponTransformMatrix;

	pWeaponObject = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Weapon"), &WeaponDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;
	m_Weapons[CPlayer::FLASHBANG] = pWeaponObject;


	return S_OK;
}

HRESULT CPlayer::Initialize_PartModels()
{
	CModel* pBodyModel = { dynamic_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Model"))) };
	CModel* pHeadModel = { dynamic_cast<CModel*>(m_PartObjects[PART_HEAD]->Get_Component(TEXT("Com_Model"))) };
	CModel* pHairModel = { dynamic_cast<CModel*>(m_PartObjects[PART_HAIR]->Get_Component(TEXT("Com_Model"))) };

	m_pBodyModel = pBodyModel;
	m_pHeadModel = pHeadModel;
	m_pHairModel = pHairModel;

	if (nullptr == pBodyModel ||
		nullptr == pHeadModel ||
		nullptr == pHairModel)
		return E_FAIL;

	pHeadModel->Link_Bone_Auto(pBodyModel);
	pHairModel->Link_Bone_Auto(pBodyModel);

#pragma region
	CFlashLight* pFlashLight = dynamic_cast<CFlashLight*>(m_PartObjects[PART_LIGHT]);
	_float4x4* pLeftWeaponCombinedMatrix = { const_cast<_float4x4*>(Get_Body_Model()->Get_CombinedMatrix("l_weapon")) };
	pFlashLight->Set_Socket_Ptr(pLeftWeaponCombinedMatrix);
#pragma endregion

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pFSMCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom_Camera);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);
	m_PartObjects.clear();

	for (auto& pWeapon : m_Weapons)
		Safe_Release(pWeapon);
	m_Weapons.clear();

	Safe_Release(m_pWeapon);
	Safe_Release(m_pTabWindow);
	Release_Effect();
}
