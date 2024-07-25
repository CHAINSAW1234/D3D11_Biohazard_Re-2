#include "stdafx.h"

#include "Item_Mesh_Viewer.h"
#include "Camera_Free.h"
#include "Light.h"
#include "Tab_Window.h"

constexpr _float	DIST_CAM_FAR_LIMIIT = 10.f;
constexpr _float	DIST_CAM_NEAR_LIMIT = 1.f;

constexpr _float	POPUP_HIDE_TIME_LIMIT = 0.4f;

constexpr _float	POPUP_HIDE_START_RADIAN = 0.f;
constexpr _float	POPUP_HIDE_END_RADIAN = 360.f;

constexpr _float	POPUP_HIDE_START_DIST = 10.f;
constexpr _float	PICKUP_HIDE_TIME_LIMIT = 0.2f;

constexpr _float	PICK_UP_IDLE_X_DIST = -0.2f;

constexpr _float	ZERO = 0.f;

//constexpr _float	POPUP_HIDE_END_DIST = 0.4f;

CItem_Mesh_Viewer::CItem_Mesh_Viewer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CItem_Mesh_Viewer::CItem_Mesh_Viewer(const CItem_Mesh_Viewer& rhs)
	: CGameObject{ rhs }
{

}

HRESULT CItem_Mesh_Viewer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem_Mesh_Viewer::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	GAMEOBJECT_DESC* pMapStructureDesc = (GAMEOBJECT_DESC*)pArg;

	m_pTransformCom->Set_WorldMatrix(pMapStructureDesc->worldMatrix);

	Load_ItemModelTags();

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eViewer_State = POP_UP;

	CGameObject* pCamera = m_pGameInstance->Find_Layer(g_Level, g_strCameraTag)->front();

	m_pCameraFree = dynamic_cast<CCamera_Free*>(pCamera);

	m_fDistCamZ = POPUP_HIDE_START_DIST;

	Safe_AddRef(m_pCameraFree);

	return S_OK;
}

void CItem_Mesh_Viewer::Start()
{
	if (nullptr == m_vecModelCom[HandGun] || nullptr == m_vecModelCom[ShotGun])
		return;

	m_vecModelCom[HandGun]->Hide_Mesh("LOD_1_Group_1_Sub_1__wp0100_VP70Custom_Mat_mesh0002", true);
	m_vecModelCom[HandGun]->Hide_Mesh("LOD_1_Group_2_Sub_1__wp0000_PowerUp_Mat_mesh0003", true);
	m_vecModelCom[HandGun]->Hide_Mesh("LOD_1_Group_6_Sub_1__wp0000_PowerUp_Mat_mesh0004", true);

	m_vecModelCom[ShotGun]->Hide_Mesh("LOD_1_Group_3_Sub_1__wp1100_mt_mesh0004", true);
	m_vecModelCom[ShotGun]->Hide_Mesh("LOD_1_Group_4_Sub_1__wp1100_mt_mesh0005", true);

	//m_LampTags[0] = "sm77002portablesafe01a00md_1_Group_0_Sub_2__sm77_002_Po_efc432d";
	//m_LampTags[1] = "sm77002portablesafe01a00md_1_Group_0_Sub_3__sm77_002_Po_5926c14";
	//m_LampTags[2] = "sm77002portablesafe01a00md_1_Group_0_Sub_4__sm77_002_Po_02a7153";
	//m_LampTags[3] = "sm77002portablesafe01a00md_1_Group_0_Sub_5__sm77_002_Po_ba99383";
	//m_LampTags[4] = "sm77002portablesafe01a00md_1_Group_0_Sub_6__sm77_002_Po_16b51e4";
	//m_LampTags[5] = "sm77002portablesafe01a00md_1_Group_0_Sub_7__sm77_002_Po_f2b4932";
	//m_LampTags[6] = "sm77002portablesafe01a00md_1_Group_0_Sub_8__sm77_002_Po_c90719f";
	//m_LampTags[7] = "sm77002portablesafe01a00md_1_Group_0_Sub_9__sm77_002_Po_320ce11";

	m_LampTags[2] = "sm77002portablesafe01a00md_1_Group_0_Sub_2__sm77_002_Po_efc432d";
	m_LampTags[1] = "sm77002portablesafe01a00md_1_Group_0_Sub_3__sm77_002_Po_5926c14";
	m_LampTags[5] = "sm77002portablesafe01a00md_1_Group_0_Sub_4__sm77_002_Po_02a7153";
	m_LampTags[0] = "sm77002portablesafe01a00md_1_Group_0_Sub_5__sm77_002_Po_ba99383";
	m_LampTags[6] = "sm77002portablesafe01a00md_1_Group_0_Sub_6__sm77_002_Po_16b51e4";
	m_LampTags[3] = "sm77002portablesafe01a00md_1_Group_0_Sub_7__sm77_002_Po_f2b4932";
	m_LampTags[7] = "sm77002portablesafe01a00md_1_Group_0_Sub_8__sm77_002_Po_c90719f";
	m_LampTags[4] = "sm77002portablesafe01a00md_1_Group_0_Sub_9__sm77_002_Po_320ce11";
	//3, 2, 6, 1, 7, 4, 8, 5

	m_ButtonTags[7] = "sm77002portablesafe01a00md_1_Group_1_Sub_1__sm77_002_Po_b6f4fe2";
	m_ButtonTags[6] = "sm77002portablesafe01a00md_1_Group_2_Sub_1__sm77_002_Po_5f4e97d";
	m_ButtonTags[5] = "sm77002portablesafe01a00md_1_Group_3_Sub_1__sm77_002_Po_85f6203";
	m_ButtonTags[4] = "sm77002portablesafe01a00md_1_Group_4_Sub_1__sm77_002_Po_f030480";
	m_ButtonTags[3] = "sm77002portablesafe01a00md_1_Group_5_Sub_1__sm77_002_Po_53e75b6";
	m_ButtonTags[2] = "sm77002portablesafe01a00md_1_Group_6_Sub_1__sm77_002_Po_0577076";
	m_ButtonTags[1] = "sm77002portablesafe01a00md_1_Group_7_Sub_1__sm77_002_Po_b9c21fc";
	m_ButtonTags[0] = "sm77002portablesafe01a00md_1_Group_8_Sub_1__sm77_002_Po_0b164c0";

	m_ButtonBoneTags[7] = "button_p001";
	m_ButtonBoneTags[6] = "button_p002";
	m_ButtonBoneTags[5] = "button_p003";
	m_ButtonBoneTags[4] = "button_p004";
	m_ButtonBoneTags[3] = "button_p005";
	m_ButtonBoneTags[2] = "button_p006";
	m_ButtonBoneTags[1] = "button_p007";
	m_ButtonBoneTags[0] = "button_p008";

	m_vecModelCom[portablesafe]->Set_RootBone("RootNode");
	m_vecModelCom[portablesafe]->Add_Bone_Layer_All_Bone(TEXT("Default"));
	m_vecModelCom[portablesafe]->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);
	m_vecModelCom[portablesafe]->Active_RootMotion_Rotation(false);
	m_vecModelCom[portablesafe]->Change_Animation(0, TEXT("Default"), 1);
}	

void CItem_Mesh_Viewer::Tick(_float fTimeDelta)
{
	if(PICKUPITEM == m_eOperType || SECON_PICKUPITEM == m_eOperType)
		m_pTransformCom->Set_Scaled(m_fCurSize, m_fCurSize, m_fCurSize);

	_vector vFrontCamPos = (XMVector4Normalize(m_pCameraFree->GetLookDir_Vector()) * m_fDistCamZ) + m_pCameraFree->Get_Position_Vector();
	
	_vector vCamRight = m_pCameraFree->Get_Transform()->Get_State_Vector(CTransform::STATE_RIGHT);
	vCamRight *= m_fDistCamX;
	vFrontCamPos += vCamRight;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFrontCamPos);

	switch (m_eViewer_State)
	{
	case Client::POP_UP: {
		PopUp_Operation(fTimeDelta);
		break;
	}	

	case Client::UI_IDLE: {
		Idle_Operation(fTimeDelta);
		break;
	}

	case Client::HIDE: {
		Hide_Operation(fTimeDelta);
		break;
	}

	default:
		break;
	}
}

void CItem_Mesh_Viewer::Late_Tick(_float fTimeDelta)
{
	if (portablesafe == m_eItem_Number)
	{
		_float3				vDirection = { };
		if (FAILED(m_vecModelCom[m_eItem_Number]->Play_Animations(m_pTransformCom, fTimeDelta, &vDirection)))
		{
			int a = 0;
		}
	}

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_EXAMINE, this);
}

void CItem_Mesh_Viewer::Idle_Operation(_float fTimeDelta)
{
	//Add_Additional_Transfortion_World
	switch (m_eOperType)
	{
	case Client::CItem_Mesh_Viewer::EXAMIN: {
		if (DOWN == m_pGameInstance->Get_KeyState(VK_SPACE)) {
			m_eOperType = EXAMIN_PUZZLE;
			m_pTransformCom->Look_At(m_pGameInstance->Get_Camera_Pos_Vector());
			//m_pTransformCom->Rotation(m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT), 1.57f);
			m_pTransformCom->Turn(m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT), 0.25f);
			m_fDistCamZ = 0.18f;
			m_iSelected_Button = 0;
			Set_Answer();
			break;
		}

		if (true == m_pGameInstance->Check_Wheel_Down())
		{
			m_fDistCamZ -= 0.01f;
		}
		else if (true == m_pGameInstance->Check_Wheel_Up())
		{
			m_fDistCamZ += 0.01f;
		}

		static		_float2			vSpeed = { 0.f, 0.f };
		if (PRESSING == m_pGameInstance->Get_KeyState(VK_LBUTTON))
		{
			_long	MouseMove = { 0 };
			if (MouseMove = m_pGameInstance->Get_MouseDeltaPos().x)
			{
				vSpeed.x += fTimeDelta * MouseMove * 0.01f;
			}
			if (MouseMove = m_pGameInstance->Get_MouseDeltaPos().y)
			{
				vSpeed.y += fTimeDelta * MouseMove * 0.01f;
			}
		}
		if (vSpeed.x > 0.f)
		{
			vSpeed.x -= fTimeDelta * 0.1f;
			if (vSpeed.x < 0.f)
				vSpeed.x = 0.f;
		}
		else
		{
			vSpeed.x += fTimeDelta * 0.1f;
			if (vSpeed.x > 0.f)
				vSpeed.x = 0.f;
		}
		if (vSpeed.y > 0.f)
		{
			vSpeed.y -= fTimeDelta * 0.1f;
			if (vSpeed.y < 0.f)
				vSpeed.y = 0.f;
		}
		else
		{
			vSpeed.y += fTimeDelta * 0.1f;
			if (vSpeed.y > 0.f)
				vSpeed.y = 0.f;
		}

		//_vector MyUp = m_pTransformCom->Get_State_Vector(CTransform::STATE_UP);
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), vSpeed.x * -1.f);
		//m_pTransformCom->Turn(MyUp, vSpeed.x * -1.f);
		m_pTransformCom->Turn(m_pGameInstance->Get_Camera_Transform()->Get_State_Vector(CTransform::STATE_RIGHT), vSpeed.y * -1.f);

		break;
	}

	case Client::CItem_Mesh_Viewer::PICKUPITEM: {
		if (false == m_bStop)
		{
			m_fPopupHide_CurTime += fTimeDelta;

			if (m_fPopupHide_CurTime > POPUP_HIDE_TIME_LIMIT)
			{
				m_bStop = true;
				m_fPopupHide_CurTime = 0.f;
				//m_fDistCamZ = m_fPopupHide_EndDist;
				return;
			}

			if (1.f > m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT)
			{
				m_fCurSize = m_pGameInstance->Get_Ease(Ease_OutQuint, m_fStartSize, m_fEndSize,
					m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);

				m_fDistCamX = m_pGameInstance->Get_Ease(Ease_OutQuint, ZERO, PICK_UP_IDLE_X_DIST,
					m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);
			}
		}

		break;
	}

	case Client::CItem_Mesh_Viewer::SECON_PICKUPITEM: {
		break;
	}

	case Client::CItem_Mesh_Viewer::EXAMIN_PUZZLE: {
		switch (m_ePZ_Progress)
		{
		case Client::CItem_Mesh_Viewer::PUZZLE_IDLE: {
			if (DOWN == m_pGameInstance->Get_KeyState(VK_RBUTTON)) {
				m_eOperType = EXAMIN;
				m_isFirstPress = false;
				PS_Button_Resset();
				break;
			}

			if (-1 == Check_Puzzle_Success() &&  0 < m_iPuzzle_Progress)
			{
				m_ePZ_Progress = PUZZLE_FAILED;
				break;
			}

			if (1 == Check_Puzzle_Success() && 0 < m_iPuzzle_Progress)
			{
				m_ePZ_Progress = PUZZLE_SUCCEEC;
				break;
			}

			for (_uint i = 0; i < 8; i++)
			{
				if (PRESSED == m_eButtonStates[i])
					m_vecModelCom[m_eItem_Number]->Add_Additional_Transformation_World(m_ButtonBoneTags[i], XMMatrixTranslation(0.f, -0.3f, 0.f));
			}

			if (DOWN == m_pGameInstance->Get_KeyState(VK_SPACE)) {
				m_eButtonStates[m_iSelected_Button] = PRESSED;
				m_iInputAnswer[m_iPuzzle_Progress] = m_iSelected_Button;
				m_iPuzzle_Progress++;
				Puszzle_button_Sound(m_iSelected_Button);


				break;
			}

			if (DOWN == m_pGameInstance->Get_KeyState('D')) {
				if (m_iSelected_Button + 1 < 8)
					m_iSelected_Button += 1;
			}
			if (DOWN == m_pGameInstance->Get_KeyState('A')) {
				if (m_iSelected_Button - 1 > -1)
					m_iSelected_Button -= 1;
			}
			if (DOWN == m_pGameInstance->Get_KeyState('W')) {
				if (m_iSelected_Button - 2 > -1)
					m_iSelected_Button -= 2;
			}
			if (DOWN == m_pGameInstance->Get_KeyState('S')) {
				if (m_iSelected_Button + 2 < 8)
					m_iSelected_Button += 2;
			}
			break;
		}
			
		case Client::CItem_Mesh_Viewer::PUZZLE_SUCCEEC: {
			static _uint iSuccec_Equence = 0;
			if (iSuccec_Equence == 0)
				m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Portuble_Puzzle_Clear.mp3"), CH4_2D ,0.5f);

			if (DOWN == m_pGameInstance->Get_KeyState(VK_RBUTTON)) {
				m_ePZ_Progress = PUZZLE_FAILED;
				iSuccec_Equence = 0;
				break;
			}

			m_fPopupHide_CurTime += fTimeDelta;

			if (iSuccec_Equence < 7)
			{
				if (m_fPopupHide_CurTime > 0.2f)
				{
					if (iSuccec_Equence % 2 == 0 && iSuccec_Equence < 6)
					{
						for (_uint i = 0; i < 8; i++)
						{
							if (i % 2 == 0)
								m_eLampStates[m_iInputAnswer[i]] = SUCCEED;
							else
								m_eLampStates[m_iInputAnswer[i]] = LAMP_IDLE;
						}
						iSuccec_Equence++;
						m_fPopupHide_CurTime = 0.f;
						break;
					}

					else if (iSuccec_Equence % 2 != 0 && iSuccec_Equence < 6)
					{
						for (_uint i = 0; i < 8; i++)
						{
							if (i % 2 != 0)
								m_eLampStates[m_iInputAnswer[i]] = SUCCEED;
							else
								m_eLampStates[m_iInputAnswer[i]] = LAMP_IDLE;
						}
						iSuccec_Equence++;
						m_fPopupHide_CurTime = 0.f;
						break;
					}

					else if (iSuccec_Equence == 6)
					{
						for (_uint i = 0; i < 8; i++)
						{
							m_eLampStates[i] = SUCCEED;
						}

						_bool isPlay = true;
						m_pGameInstance->IsPlaying_2D(CH4_2D, &isPlay);
						if (false == isPlay)
						{
							iSuccec_Equence++;
							m_fPopupHide_CurTime = 0.f;
						}
						break;
					}
				}
			}

			if (iSuccec_Equence == 7)
			{
				if (1.f > m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT)
				{
					m_fDistCamZ = 0.4f;
					m_pTransformCom->Look_At(m_pCameraFree->Get_Position_Vector() - XMVectorSet(-0.5f, 0.5f, 0.f, 0.f));
				}
				else
				{
					m_fPopupHide_CurTime = 0.f;
					m_fPreRadian = 0.f;
					iSuccec_Equence++;
				}
			}

			if (iSuccec_Equence == 8)
			{
				m_vecModelCom[m_eItem_Number]->Change_Animation(0, TEXT("Default"), 0);
				m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_Map_sm77_portable_safe2_9.mp3"), CH4_2D, 0.5f);
				iSuccec_Equence++;
			}

			if (iSuccec_Equence == 9)
			{
				if (1.f > m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT)
				{
					m_fDistCamZ = m_pGameInstance->Get_Ease(Ease_OutQuint, 0.4f, POPUP_HIDE_START_DIST,
						m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);
				}
				else
				{
					m_eItem_Number = blankkey01a;
					Set_ScaleByItemNum(m_eItem_Number);
					m_fPopupHide_CurTime = 0.f;
					iSuccec_Equence++;
				}
			}

			if (iSuccec_Equence == 10)
			{
				if (1.f > m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT)
				{
					m_fDistCamZ = m_pGameInstance->Get_Ease(Ease_OutQuint, POPUP_HIDE_START_DIST, 0.4f,
						m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);
					m_pTransformCom->Look_At(m_pCameraFree->Get_Position_Vector() - XMVectorSet(-0.5f, 0.5f, 0.f, 0.f));
				}
				else
				{
					//m_eItem_Number = blankkey01a;
					m_fPopupHide_CurTime = 0.f;
					m_fDistCamX = PICK_UP_IDLE_X_DIST;
					m_eOperType = SECON_PICKUPITEM;
					CGameObject* pT = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_TabWindow"))->front();
					static_cast<CTab_Window*>(pT)->Set_PortabTo_Blank();
					iSuccec_Equence++;
				}
			}

			break;
		}
			
		case Client::CItem_Mesh_Viewer::PUZZLE_FAILED: {
			m_fPopupHide_CurTime += fTimeDelta;
			if (m_fPopupHide_CurTime > 0.1f)
			{
				PS_Button_Resset();
				m_eButtonStates[m_iSelected_Button] = PRESSED;
				m_eLampStates[m_iSelected_Button] = SUCCEED;
				Set_Answer();
				m_iPuzzle_Progress = 1;
				m_fPopupHide_CurTime = 0;
				m_ePZ_Progress = PUZZLE_IDLE;
				m_iInputAnswer[0] = m_iSelected_Button;
				break;
			}

			for (_uint i = 0; i < m_iPuzzle_Progress; i++)
			{
				m_eLampStates[m_iInputAnswer[i]] = FAILED;
			}

			break;
		}
			
		default:
			break;
		}

		break;}

	default:
		break;
	}

	//m_vecModelCom[portablesafe]->Add_Additional_Transformation_World("button_p001", XMMatrixTranslation(0.f, 0.f, 1.f));
	//m_vecModelCom[portablesafe]->Hide_Mesh("button_p001", true);
}

void CItem_Mesh_Viewer::Hide_Operation(_float fTimeDelta)
{
	m_fPopupHide_CurTime += fTimeDelta;

	if (m_fPopupHide_CurTime > POPUP_HIDE_TIME_LIMIT)
	{
		m_eViewer_State = STATE_END;
		m_fPopupHide_CurTime = 0.f;
		m_fDistCamZ = POPUP_HIDE_START_DIST;
		m_fDistCamX = ZERO;
		m_bDead = true;
		m_eItem_Number = ITEM_NUMBER_END;
		m_matMoveCenter = XMMatrixIdentity();
		m_fPickupHide_StartDist = DIST_CAM_FAR_LIMIIT;
		return;
	}

	switch (m_eOperType)
	{
	case Client::CItem_Mesh_Viewer::EXAMIN: {
		if (1.f > m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT)
		{
			m_fDistCamZ = m_pGameInstance->Get_Ease(Ease_OutQuint, m_fPopupHide_EndDist, POPUP_HIDE_START_DIST,
				m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);
		}
		break;
	}
		
	case Client::CItem_Mesh_Viewer::SECON_PICKUPITEM:
	case Client::CItem_Mesh_Viewer::PICKUPITEM: {
		if (1.f > m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT)
		{
			m_fDistCamZ = m_pGameInstance->Get_Ease(Ease_OutQuint, m_fPopupHide_EndDist, POPUP_HIDE_START_DIST,
				m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);

			m_fDistCamX = m_pGameInstance->Get_Ease(Ease_OutQuint, PICK_UP_IDLE_X_DIST, ZERO,
				m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);

			m_fCurSize = m_pGameInstance->Get_Ease(Ease_OutQuint, m_fEndSize, m_fStartSize,
				m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);
		}
		break;
	}
		
	default:
		break;
	}
}

_int CItem_Mesh_Viewer::Check_Puzzle_Success()
{
	_int isPuzzleSuccess = 0;

	for (_uint i = 0; i < m_iPuzzle_Progress; i++)
	{
		if (m_iSetByNumAnswer[i] != m_iInputAnswer[i])
		{
			isPuzzleSuccess = -1;
			m_eLampStates[m_iInputAnswer[i]] = FAILED;
		}
		else if (m_iSetByNumAnswer[i] == m_iInputAnswer[i])
		{
			m_eLampStates[m_iInputAnswer[i]] = SUCCEED;
		}		
	}

	if (m_iPuzzle_Progress == 8 && isPuzzleSuccess == 0)
		isPuzzleSuccess = 1;

	return isPuzzleSuccess;
}

void CItem_Mesh_Viewer::PS_Button_Resset()
{
	for (_uint i = 0; i < 8; i++)
	{
		m_eButtonStates[i] = BUTTON_IDLE;
		m_eLampStates[i] = LAMP_IDLE;
		m_iSetByNumAnswer[i] = 0;
		m_iInputAnswer[i] = -1;
	}
	m_ePZ_Progress = PUZZLE_IDLE;
}

void CItem_Mesh_Viewer::Puszzle_button_Sound(_int iButton)
{
	switch (iButton)
	{
	case 0:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_Map_sm77_portable_safe2_1.mp3"), 0.5f);
		break;
	case 1:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_Map_sm77_portable_safe2_2.mp3"), 0.5f);
		break;
	case 2:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_Map_sm77_portable_safe2_3.mp3"), 0.5f);
		break;
	case 3:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_Map_sm77_portable_safe2_4.mp3"), 0.5f);
		break;
	case 4:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_Map_sm77_portable_safe2_5.mp3"), 0.5f);
		break;
	case 5:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_Map_sm77_portable_safe2_6.mp3"), 0.5f);
		break;
	case 6:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_Map_sm77_portable_safe2_7.mp3"), 0.5f);
		break;
	case 7:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_Map_sm77_portable_safe2_8.mp3"), 0.5f);
		break;
	default:
		break;
	}
}

void CItem_Mesh_Viewer::Set_Operation(UI_OPERRATION eOperation, ITEM_NUMBER eCallItemType, _uint iOperateType)
{
	switch (eOperation)
	{
	case Client::POP_UP: {
		m_bDead = false;
		m_eItem_Number = eCallItemType;
		m_eViewer_State = eOperation;
		m_fDistCamZ = DIST_CAM_FAR_LIMIIT;
		m_fPopupHide_CurTime = 0.f;
		Set_ScaleByItemNum(eCallItemType);
		m_eOperType = static_cast<OPERATION_TYPE>(iOperateType);

		_float4 fCenter = m_vecModelCom[m_eItem_Number]->GetCenterPoint();
		_matrix TempMat = XMMatrixTranslation(-fCenter.x, -fCenter.y * 0.5f, fCenter.z);
		m_matMoveCenter *= TempMat;
		
		m_fPreRadian = 0.f;

		PS_Button_Resset();
		break;
	}

	case Client::UI_IDLE: {
		if (PICKUPITEM == iOperateType)
		{
			m_bDead = false;
			m_eViewer_State = eOperation;
			m_fPopupHide_CurTime = 0.f;
			m_bStop = false;
			m_fPickupIdle_StartDist = m_fDistCamZ;
		}
		if (SECON_PICKUPITEM == iOperateType)
		{
			m_bDead = false;
			m_eItem_Number = eCallItemType;
			m_eViewer_State = eOperation;
			
			Set_ScaleByItemNum(eCallItemType);

			m_fPopupHide_CurTime = 0.f;
			m_fDistCamZ = m_fPopupHide_EndDist;
			m_fDistCamX = PICK_UP_IDLE_X_DIST;
			m_fCurSize = m_fEndSize;

			m_eOperType = static_cast<OPERATION_TYPE>(iOperateType);
			_float4 fCenter = m_vecModelCom[m_eItem_Number]->GetCenterPoint();
			_matrix TempMat = XMMatrixTranslation(-fCenter.x, -fCenter.y * 0.5f, -fCenter.z);
			m_matMoveCenter *= TempMat;
			_vector vFrontCamPos = (XMVector4Normalize(m_pCameraFree->GetLookDir_Vector()) * m_fDistCamZ) + m_pCameraFree->Get_Position_Vector();
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFrontCamPos);

			//m_pTransformCom->Look_At(m_pCameraFree->Get_Position_Vector());
		}
		
		break;
	}

	case Client::HIDE: {
		m_bDead = false;
		m_eViewer_State = eOperation;
		m_fPopupHide_CurTime = 0.f;
		if (PICKUPITEM == iOperateType)
		{
			m_bStop = false;
			m_fPickupHide_StartDist = m_fDistCamZ;
		}
		break;
	}
		
	default:
		break;
	}
}

void CItem_Mesh_Viewer::Set_Answer()
{
	_uint iFindNumPos = 0;

	for (_uint i = 0; i < 8; i++)
	{
		if (m_iSelected_Button == m_iAnswer[i])
		{
			iFindNumPos = i;
		}
	}

	for (_uint i = 0; i < 8; i++)
	{
		m_iSetByNumAnswer[i] = m_iAnswer[iFindNumPos];

		if (iFindNumPos + 1 == 8)
			iFindNumPos = 0;
		else
			iFindNumPos++;
	}
}

HRESULT CItem_Mesh_Viewer::Add_Components()
{
	/* For.Com_Shader */

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Body_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Anim_Shader"), (CComponent**)&m_pAnimShaderCom)))
		return E_FAIL;

	if (FAILED(Load_ItemsModel()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Mesh_Viewer::Render()
{
	if (ITEM_NUMBER_END == m_eItem_Number)
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	const LIGHT_DESC* pDesc = m_pGameInstance->Get_LightDesc(g_strDirectionalTag, 1);

	if (nullptr == pDesc)
		return E_FAIL;

	CShader* pShader = { nullptr };
	if (m_eItem_Number == portablesafe) {
		pShader = m_pAnimShaderCom;
	}
	else {
		pShader = m_pShaderCom;
	}
	_float4 vup = -m_pCameraFree->Get_Transform()->Get_State_Float4(CTransform::STATE_UP);

	if (FAILED(pShader->Bind_RawValue("g_vLightDir", &vup, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &pDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_PBRLerpTime", m_pGameInstance->Get_PBRLerpTime(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_PrevIrradianceTexture(pShader, "g_PrevIrradianceTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_CurIrradianceTexture(pShader, "g_CurIrradianceTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_PrevCubeMapTexture(pShader, "g_PrevEnvironmentTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_CurCubeMapTexture(pShader, "g_CurEnvironmentTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(pShader, TEXT("Target_LUT"), "g_SpecularLUTTexture")))
		return E_FAIL;

	list<_uint>			NonHideIndices = { m_vecModelCom[m_eItem_Number]->Get_NonHideMeshIndices() };
	for (auto& i : NonHideIndices)
	{
		if (FAILED(m_vecModelCom[m_eItem_Number]->Bind_ShaderResource_Texture(pShader, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_vecModelCom[m_eItem_Number]->Bind_ShaderResource_Texture(pShader, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_vecModelCom[m_eItem_Number]->Bind_ShaderResource_Texture(pShader, "g_AlphaTexture", static_cast<_uint>(i), aiTextureType_METALNESS)))
		{
			_bool isAlphaTexture = false;
			if (FAILED(pShader->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isAlphaTexture = true;
			if (FAILED(pShader->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
				return E_FAIL;
		}

		if (FAILED(m_vecModelCom[m_eItem_Number]->Bind_ShaderResource_Texture(pShader, "g_AOTexture", static_cast<_uint>(i), aiTextureType_SHININESS)))
		{
			_bool isAOTexture = false;
			if (FAILED(pShader->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isAOTexture = true;
			if (FAILED(pShader->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
				return E_FAIL;
		}

		if (FAILED(m_vecModelCom[m_eItem_Number]->Bind_ShaderResource_Texture(pShader, "g_EmissiveTexture", static_cast<_uint>(i), aiTextureType_EMISSIVE)))
		{
			_bool isEmissive = false;
			if (FAILED(pShader->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isEmissive = true;
			if (FAILED(pShader->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
				return E_FAIL;
		}


		if (m_eItem_Number == portablesafe) {
			if (FAILED(m_vecModelCom[m_eItem_Number]->Bind_BoneMatrices(pShader, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			_int iIndex = -1;
			_float4 vColor = { 1.f, 1.f, 1.f ,1.f };

			if (m_eOperType == EXAMIN_PUZZLE)
			{
				for (_int j = 0; j < 8; ++j) {
					if (m_vecModelCom[m_eItem_Number]->Get_MeshTags()[i] == m_LampTags[j]) {
						switch (m_eLampStates[j]) {
						case SUCCEED:
							vColor = _float4(0.f, 1.f, 0.f, 1.f);
							break;
						case FAILED:
							vColor = _float4(1.f, 0.f, 0.f, 1.f);
							break;
						}
					}
				}
				for (_int j = 0; j < 8; ++j) {
					if (m_vecModelCom[m_eItem_Number]->Get_MeshTags()[i] == m_ButtonTags[j])
					{
						if (j == m_iSelected_Button)
							vColor = _float4(2.f, 2.f, 2.f, 1.f);

						else if (m_eButtonStates[j] == PRESSED)
							vColor = _float4(0.4f, 0.4f, 0.4f, 1.f);
					}
				}
			}
			
			if (FAILED(pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
				return E_FAIL;



			if (FAILED(pShader->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_EXAMINE)))
				return E_FAIL;
		}
		else {
			if (FAILED(pShader->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_EXAMINE)))
				return E_FAIL;
		}

		m_vecModelCom[m_eItem_Number]->Render(static_cast<_uint>(i));
	}


	return S_OK;
}

void CItem_Mesh_Viewer::PopUp_Operation(_float fTimeDelta)
{
	m_fPopupHide_CurTime += fTimeDelta;

	if (m_fPopupHide_CurTime > POPUP_HIDE_TIME_LIMIT)
	{
		m_eViewer_State = UI_IDLE;
		m_fPopupHide_CurTime = 0.f;
		m_fDistCamZ = m_fPopupHide_EndDist;
		m_bStop = true;
		m_fPreRadian = 0.f;
		return;
	}

	if (1.f > m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT)
	{
		m_fDistCamZ = m_pGameInstance->Get_Ease(Ease_OutQuint, POPUP_HIDE_START_DIST, m_fPopupHide_EndDist,
			m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);

		_float fRadian = m_pGameInstance->Get_Ease(Ease_Linear, POPUP_HIDE_START_RADIAN, XMConvertToRadians(POPUP_HIDE_END_RADIAN),
			m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);

		_float fResultRadian = fRadian - m_fPreRadian;

		fResultRadian /= XMConvertToRadians(360.0f);

		//_vector MyUp = m_pTransformCom->Get_State_Vector(CTransform::STATE_UP);
		//m_pTransformCom->Turn(MyUp, fResultRadian);
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fResultRadian);
		m_fPreRadian = fRadian;
		// 1. �̹� Tick �� Radian  - ���� Radian
		// 2. CTransform�� ���� �ִ� m_fRotationPerSec�� ������ߵ�  
		//m_pTransformCom->Rotation(MyUp, fRadian);
		//_vector CamUp = m_pGameInstance->Get_Camera_Transform()->Get_State_Vector(CTransform::STATE_UP);
		//m_pTransformCom->Rotation(CamUp, fRadian);
		// //m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
	}
}

HRESULT CItem_Mesh_Viewer::Bind_ShaderResources()
{
	CShader* pShader = { nullptr };
	if (m_eItem_Number == portablesafe) {
		pShader = m_pAnimShaderCom;
	}
	else {
		pShader = m_pShaderCom;
	}

	if (nullptr == pShader)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(pShader, "g_WorldMatrix", m_matMoveCenter)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}



CItem_Mesh_Viewer* CItem_Mesh_Viewer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Mesh_Viewer* pInstance = new CItem_Mesh_Viewer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CItem_Mesh_Viewer"));

		Safe_Release(pInstance);
	}
	
	return pInstance;

}

CGameObject* CItem_Mesh_Viewer::Clone(void* pArg)
{
	CItem_Mesh_Viewer* pInstance = new CItem_Mesh_Viewer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CItem_Mesh_Viewer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Mesh_Viewer::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pAnimShaderCom);

	for (auto& iter : m_vecModelCom)
	{
		Safe_Release(iter);
	}

	Safe_Release(m_pCameraFree);
}


HRESULT CItem_Mesh_Viewer::Load_ItemsModel()
{
	wstring wstrModelComTag = TEXT("Com_Model");
	
	for (_uint i = 0; i < m_vecModelTag.size(); i++)
	{
		CModel* pItemModel = nullptr;

		wstring ModelTagTrashCan = wstrModelComTag + to_wstring(i);

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(g_Level, m_vecModelTag[i],
			ModelTagTrashCan, (CComponent**)&pItemModel)))
			return E_FAIL;

		m_vecModelCom.push_back(pItemModel);
	}

	return S_OK;
}

HRESULT CItem_Mesh_Viewer::Load_ItemModelTags()
{
	wstring filePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Item_Prototype.dat");

	HANDLE		hFile = CreateFile(filePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD	dwByte(0);

	_uint iObjectNum = { 0 };
	if (!ReadFile(hFile, &iObjectNum, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;

	for (_uint i = 0; iObjectNum > i; ++i)
	{

		PROTOTYPE_INFORM* Inform = new PROTOTYPE_INFORM;

		_uint dwLen = { 0 };

		_bool bAnim = { false };
		if (!ReadFile(hFile, &bAnim, sizeof(_uint), &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			CloseHandle(hFile);
			return E_FAIL;
		}
		Inform->bAnim = bAnim;


		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			CloseHandle(hFile);
			return E_FAIL;
		}
		char* strModelPath = new char[dwLen / sizeof(char) + 1];
		if (!ReadFile(hFile, strModelPath, dwLen, &dwByte, nullptr))
		{
			delete[] strModelPath;
			Safe_Delete(Inform);
			CloseHandle(hFile);

			return E_FAIL;
		}
		strModelPath[dwLen / sizeof(char)] = '\0';
		Inform->strModelPath = strModelPath;
		delete[] strModelPath;


		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			CloseHandle(hFile);
			return E_FAIL;
		}
		wchar_t* wstrModelPrototypeName = new wchar_t[dwLen / sizeof(wchar_t) + 1];
		if (!ReadFile(hFile, wstrModelPrototypeName, dwLen, &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			delete[] wstrModelPrototypeName;
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrModelPrototypeName[dwLen / sizeof(wchar_t)] = L'\0';
		Inform->wstrModelPrototypeName = wstrModelPrototypeName;
		delete[] wstrModelPrototypeName;

		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			Safe_Delete(Inform);
			return E_FAIL;
		}
		wchar_t* wstrGameObjectPrototypeName = new wchar_t[dwLen / sizeof(wchar_t) + 1];
		if (!ReadFile(hFile, wstrGameObjectPrototypeName, dwLen, &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			delete[] wstrGameObjectPrototypeName;
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrGameObjectPrototypeName[dwLen / sizeof(wchar_t)] = L'\0';
		Inform->wstrGameObjectPrototypeName = wstrGameObjectPrototypeName;
		delete[] wstrGameObjectPrototypeName;

		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			Safe_Delete(Inform);
			return E_FAIL;
		}
		char* strGameObjectPrototypeName = new char[dwLen / sizeof(char) + 1];
		if (!ReadFile(hFile, strGameObjectPrototypeName, dwLen, &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			delete[] strGameObjectPrototypeName;

			CloseHandle(hFile);
			return E_FAIL;
		}
		strGameObjectPrototypeName[dwLen / sizeof(char)] = '\0';
		Inform->strGameObjectPrototypeName = strGameObjectPrototypeName;
		delete[] strGameObjectPrototypeName;

		if (Inform->wstrGameObjectPrototypeName.find(TEXT("sm7")) != wstring::npos)
		{
			m_vecModelTag.push_back(Inform->wstrModelPrototypeName);//â��
		}

		Safe_Delete(Inform);

	}

	CloseHandle(hFile);

	m_vecModelTag.push_back(TEXT("Prototype_Component_Model_statuebookhand"));

	if (ITEM_NUMBER_END != m_vecModelTag.size())
		return E_FAIL;

	else
	{
		wstring magazin = m_vecModelTag[vp70powerup];
		wstring power = m_vecModelTag[vp70longmagazine];

		m_vecModelTag[vp70powerup] = power;
		m_vecModelTag[vp70longmagazine] = magazin;

		return S_OK;
	}
		
}

void CItem_Mesh_Viewer::Set_ScaleByItemNum(ITEM_NUMBER eCallItemType)
{
	switch (eCallItemType)
	{
	case Client::emergencyspray01a://���� �Ʒ���
		m_fPopupHide_EndDist = 0.5f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::greenherb01a://���� �Ʒ���
		m_fPopupHide_EndDist = 1.f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::redherb01a://���� �Ʒ���
		m_fPopupHide_EndDist = 0.5f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::blueherb01a://���� �Ʒ���
		m_fPopupHide_EndDist = 0.5f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::herbsgg01a:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::herbsgr01a:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::herbsgb01a:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::herbsggb01a:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::herbsggg01a:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::herbsgrb01a:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::herbsrb01a:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::handgun_bullet01a:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixTranslation(0.f, 0.f, -0.04f);
		//m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::shotgun_bullet01a:
		m_fPopupHide_EndDist = 0.6f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::woodbarricade01a:
		m_fPopupHide_EndDist = 2.f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::blastingfuse01a:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixTranslation(-0.05f, -0.01f, 0.f);
		break;
	case Client::_9vbattery01a:
		m_fPopupHide_EndDist = 0.2f;
		m_pTransformCom->Set_Scaled(1.5f, 1.5f, 1.5f);
		m_fCurSize = 1.5f;
		m_fStartSize = 1.5f;
		m_fEndSize = 1.f;
		m_matMoveCenter = XMMatrixTranslation(0.f, 0.02f, 0.f);
		break;
	case Client::gunpowder01a:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::gunpowder01b:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::strengtheningyellow01a:
		m_fPopupHide_EndDist = 0.3f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::vp70powerup:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::vp70longmagazine:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::shotgunpartsstock_00:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::shotgunpartsbarrel:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::unicornmedal01a:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::spadekey01a:
		m_fPopupHide_EndDist = 0.14f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::cardkeylv101a:
		m_fPopupHide_EndDist = 0.13f;
		m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::cardkeylv201a:
		m_fPopupHide_EndDist = 0.13f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::valvehandle01a:
		m_fPopupHide_EndDist = 0.21f;
		m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
		m_fCurSize = 0.01f;
		m_fStartSize = 0.01f;
		m_fEndSize = 0.007f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::kingscepter01a:
		m_fPopupHide_EndDist = 0.22f;
		m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
		m_fCurSize = 0.01f;
		m_fStartSize = 0.01f;
		m_fEndSize = 0.007f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::virginheart01a:
		m_fPopupHide_EndDist = 0.2f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::blankkey01a:
		m_fPopupHide_EndDist = 0.15f;
		m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
		m_fCurSize = 0.01f;
		m_fStartSize = 0.01f;
		m_fEndSize = 0.007f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::statuebook01a:
		m_fPopupHide_EndDist = 1.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::statuehand01a:
		m_fPopupHide_EndDist = 1.4f;
		m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
		m_fCurSize = 0.01f;
		m_fStartSize = 0.01f;
		m_fEndSize = 0.007f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::virginmedal01a:
		m_fPopupHide_EndDist = 0.2f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::diakey01a:
		m_fPopupHide_EndDist = 0.2f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::virginmedal02a:
		m_fPopupHide_EndDist = 0.2f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::chaincutter01a:
		m_fPopupHide_EndDist = 0.2f;
		m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
		m_fCurSize = 0.01f;
		m_fStartSize = 0.01f;
		m_fEndSize = 0.007f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::clairesbag01a:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::HandGun:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::ShotGun:
		m_fPopupHide_EndDist = 0.7f;
		m_pTransformCom->Set_Scaled(0.5f, 0.5f, 0.5f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixTranslation(0.f, 0.f, -0.5f);
		//m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::Flash_Bomb:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::Grenade:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::vp70stock:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		m_fCurSize = 1.f;
		m_fStartSize = 1.f;
		m_fEndSize = 0.7f;
		m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::portablesafe:
		m_fPopupHide_EndDist = 0.4f;
		m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
		m_fCurSize = 0.01f;
		m_fStartSize = 0.01f;
		m_fEndSize = 0.007f;
		m_matMoveCenter = XMMatrixTranslation(0.04f, 0.0f, 0.f);
		//m_matMoveCenter = XMMatrixIdentity();
		break;
	case Client::statuebookhand:
		m_fPopupHide_EndDist = 0.15f;
		m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
		m_fCurSize = 0.01f;
		m_fStartSize = 0.01f;
		m_fEndSize = 0.007f;
		m_matMoveCenter = XMMatrixIdentity();
		break;

	default:
		break;
	}		
	
	_vector vFrontCamPos = (XMVector4Normalize(m_pCameraFree->GetLookDir_Vector()) * m_fDistCamZ) + m_pCameraFree->Get_Position_Vector();

	_float3 vScaled = m_pTransformCom->Get_Scaled();
	m_pTransformCom->Set_WorldMatrix(m_pCameraFree->Get_Transform()->Get_WorldMatrix());
	m_pTransformCom->Set_Scaled(vScaled.x, vScaled.y, vScaled.z);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFrontCamPos);
	m_pTransformCom->Look_At(m_pCameraFree->Get_Position_Vector() - XMVectorSet(0.f, 10.f, 0.0f, 0.f));
}

void CItem_Mesh_Viewer::Set_Weapon_Accessories(ITEM_NUMBER eCallItemType, _uint iAccessories)
{
	switch (eCallItemType)
	{
	case Client::HandGun:
		switch (iAccessories)
		{
		case 0:
			m_vecModelCom[HandGun]->Hide_Mesh("LOD_1_Group_1_Sub_1__wp0100_VP70Custom_Mat_mesh0002", false);
			break;

		case 1:
			m_vecModelCom[HandGun]->Hide_Mesh("LOD_1_Group_2_Sub_1__wp0000_PowerUp_Mat_mesh0003", false);
			break;

		case 2:
			m_vecModelCom[HandGun]->Hide_Mesh("LOD_1_Group_6_Sub_1__wp0000_PowerUp_Mat_mesh0004", false);
			break;

		default:
			break;}

		break;


	case Client::ShotGun:
		switch (iAccessories)
		{
		case 0:
			m_vecModelCom[ShotGun]->Hide_Mesh("LOD_1_Group_3_Sub_1__wp1100_mt_mesh0004", false);
			break;

		case 1:
			m_vecModelCom[ShotGun]->Hide_Mesh("LOD_1_Group_4_Sub_1__wp1100_mt_mesh0005", false);
			break;

		default:
			break;}

		break;

	default:
		break;
	}
}


