#include "stdafx.h"
#include "Map_Manager.h"
#include "Player_Map_UI.h"
#include "Tab_Window.h"
#include "Static_Map_UI.h"

#define ALPHA_ZERO				_float4(0.f, 0.f, 0.f, 0.f)
#define BLENDING                0.7f
#define BLENDING_SPEED          5.f /* ���� ���� �ٲ� �� ���� �ӵ� ���� �� */
#define TARGET_SPEED            50.f

CMap_Manager::CMap_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice, pContext }
{
}

CMap_Manager::CMap_Manager(const CMap_Manager& rhs)
	:/* m_pPropManager{ CProp_Manager::Get_Instance() },*/
	CCustomize_UI{ rhs }
{
}

HRESULT CMap_Manager::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMap_Manager::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;

		CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

		if (m_eMap_Location > LOCATION_MAP_VISIT::LOCATION_MAP_VISIT_END)
		{
			m_eMap_Location = LOCATION_MAP_VISIT::LOCATION_MAP_VISIT_END;
		}

		m_eMap_Location = CustomUIDesc->eMapUI_Type;

		m_wstrFile = CustomUIDesc->wstrFileName;

		Find_MapStateType();
	}

	/* Font Default */
	if (!m_vecTextBoxes.empty())
	{
		m_vOriginTextColor = m_vecTextBoxes.back()->Get_FontColor();

		for (auto& iter : m_vecTextBoxes)
		{
			iter->Set_FontColor(ALPHA_ZERO);
		}
	}

	Find_Player();
	
	if (nullptr == m_pGetMap_Item && MAP_UI_TYPE::BACKGROUND_MAP != m_eMapComponent_Type)
	{
		CGameObject* pBackGround = Find_BackGround();

		if (nullptr != pBackGround)
		{
			CStatic_Map_UI* pGetMap = static_cast<CStatic_Map_UI*>(pBackGround);

			m_pGetMap_Item = pGetMap->Get_Map_Ptr();
		}
	}

	/* Tool */
	_float4 pos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

	pos.z = 0.01f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);

	m_fOrigin_Blending = m_vColor[0].fBlender_Value;

	m_vColor[0].vColor.w = 0.f;

	m_isRender = false;

	
	return S_OK;
}

void CMap_Manager::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Exception_Handle();
	
	Rendering(fTimeDelta);

	Transform_Control(fTimeDelta);

	Mouse_Pos(fTimeDelta);
}

void CMap_Manager::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMap_Manager::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMap_Manager::Change_Tool()
{
	return S_OK;
}

void CMap_Manager::Mouse_Pos(_float fTimeDelta)
{
	if (!(m_eFloorType == *m_pMapPlayer->Get_ViewFloor_Type() || MAP_FLOOR_TYPE::FLOOR_FREE == m_eFloorType))
		return;
	
	if (true == m_isMouse_Control)
	{
		if (PRESSING == m_pGameInstance->Get_KeyState(VK_LBUTTON))
		{
			POINT		ptDeltaPos = m_pGameInstance->Get_MouseDeltaPos();

			/* �ӵ� ���� �����ֱ� */
			if ((_long)0 != ptDeltaPos.x)
			{
				_float4 pos = {};

				pos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

				if (MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
					pos.x -= (fTimeDelta * (_float)ptDeltaPos.x * TARGET_SPEED);
				else
					pos.x += fTimeDelta * (_float)ptDeltaPos.x * m_fMouseSensor * 3.f;

				m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);

				if (MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
					m_vPlayer_MovePos.x += fTimeDelta * (_float)ptDeltaPos.x * m_fMouseSensor * 3.f;

			}

			if ((_long)0 != ptDeltaPos.y)
			{
				_float4 pos = {};

				pos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

				if (MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
					pos.y += (fTimeDelta * (_float)ptDeltaPos.y * TARGET_SPEED);
				else
					pos.y -= fTimeDelta * (_float)ptDeltaPos.y * m_fMouseSensor * 3.f;

				m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);

				if (MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
					m_vPlayer_MovePos.y -= fTimeDelta * (_float)ptDeltaPos.y * m_fMouseSensor * 3.f;
			}
		}

		if (MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
		{
			if(false == m_IsChild)
			{
				if (true == m_pTab_Window->Get_MinMapRender())
				{
					if (DOWN == m_pGameInstance->Get_KeyState(VK_LBUTTON))
					{
						if (false == m_isOneSound)
						{
							m_isOneSound = true;

							m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_keyDownMiniMap.mp3"), CH_TICK_2D, 0.1f);
						}
					}

					if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON))
					{
						m_isOneSound = false;
					}
				}
			}
		}
	}	
}


CGameObject* CMap_Manager::Find_MapPlayer()
{
	list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

	if (nullptr != pUIList)
	{
		for (auto& iter : *pUIList)
		{
			CPlayer_Map_UI* pPlayer_UI = dynamic_cast<CPlayer_Map_UI*>(iter);

			if (nullptr != pPlayer_UI)
			{
				return pPlayer_UI;
			}
		}
	}

	return nullptr;
}

void CMap_Manager::Exception_Handle()
{
	/* Player ã�� */
	if (nullptr == m_pMapPlayer)
	{
		CGameObject* pMapPlayer = Find_MapPlayer();
		m_pMapPlayer = static_cast<CPlayer_Map_UI*>(pMapPlayer);
		m_pMapPlayer_Transform = static_cast<CTransform*>(m_pMapPlayer->Get_Component(g_strTransformTag));

		if (nullptr == m_pMapPlayer_Transform)
			MSG_BOX(TEXT("CMap_Manager() : Map_Manager�� Target�� InMap ���� Player�� ã�� �� �����ϴ�."));
	}

	if (nullptr == m_pPlayer)
	{
		Find_Player();

		if (nullptr == m_pPlayer)
			MSG_BOX(TEXT("CMap_Manager() : Player�� ã�� �� �����ϴ�."));
	}

	/* Tab Window ã�� */
	if (nullptr == m_pTab_Window)
	{
		CGameObject* pTabWindow = m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_TabWindow"), 0);

		m_pTab_Window = static_cast<CTab_Window*>(pTabWindow);

		if (nullptr == m_pTab_Window)
			MSG_BOX(TEXT("CMap_Manager() : Tab Window�� ã�� �� �����ϴ�."));
	}

	if (nullptr == m_pGetMap_Item && MAP_UI_TYPE::BACKGROUND_MAP != m_eMapComponent_Type)
	{
		CGameObject* pBackGround = Find_BackGround();

		if (nullptr != pBackGround)
		{
			CStatic_Map_UI* pGetMap = static_cast<CStatic_Map_UI*>(pBackGround);

			m_pGetMap_Item = pGetMap->Get_Map_Ptr();
		}

		if (nullptr == m_pGetMap_Item)
			MSG_BOX(TEXT("CMap_Manager() : Get Map ������ ã�� �� �����ϴ�."));
	}

	{
		Transform_Adjustment();

		if (false == m_isTransfrom_Setting)
			MSG_BOX(TEXT("CMap_UI���� ���𰡰� Transform Setting�� �������� �ʾҽ��ϴ�."));
	}

}
void CMap_Manager::Transform_Adjustment()
{
	/* Main */
	if (MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type || MAP_UI_TYPE::DOOR_MAP == m_eMapComponent_Type
		|| MAP_UI_TYPE::WINDOW_MAP == m_eMapComponent_Type || MAP_UI_TYPE::FONT_MAP == m_eMapComponent_Type
		|| MAP_UI_TYPE::ITEM_MAP == m_eMapComponent_Type || MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
	{
		_float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

		if (MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
			vMainPos.z = 0.08f;
		else
			vMainPos.z = 0.2f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);

		m_isTransfrom_Setting = true;
	}

	else if (MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
	{
		_float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

		if (false == m_IsChild)
			vMainPos.z = 0.1f;

		else if (true == m_IsChild)
			vMainPos.z = 0.2f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);

		m_isTransfrom_Setting = true;
	}

	else if (MAP_UI_TYPE::MASK_MAP == m_eMapComponent_Type || MAP_UI_TYPE::NAMELINE_MAP == m_eMapComponent_Type ||
		MAP_UI_TYPE::SEARCH_TYPE_MAP == m_eMapComponent_Type || MAP_UI_TYPE::FLOOR_TYPE_MAP == m_eMapComponent_Type)
	{
		_float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

		vMainPos.z = 0.0f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);

		m_isTransfrom_Setting = true;
	}

	/* Mask */
	else if (MAP_UI_TYPE::BACKGROUND_MAP == m_eMapComponent_Type)
	{
		_float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

		vMainPos.z = 0.9f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);

		m_isTransfrom_Setting = true;
	}

	else if (MAP_UI_TYPE::FONT_MAP == m_eMapComponent_Type)
	{
		/* Font */
		if (!m_vecTextBoxes.empty())
		{
			for (auto& iter : m_vecTextBoxes)
			{
				CTransform* pFontTrans = static_cast<CTransform*>(iter->Get_Component(g_strTransformTag));
				_float4 vFontTrans = pFontTrans->Get_State_Float4(CTransform::STATE_POSITION);
				vFontTrans.z = 0.1f;
				pFontTrans->Set_State(CTransform::STATE_POSITION, vFontTrans);
			}
		}

		/* Texture */
		_float4 pTextureTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

		pTextureTrans.z = 0.1f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTextureTrans);

		m_isTransfrom_Setting = true;
	}

	else if (MAP_UI_TYPE::TARGET_NOTIFY == m_eMapComponent_Type)
	{
		m_isTransfrom_Setting = true;
	}

	else if (MAP_UI_TYPE::ANNOUNCEMENT_MAP == m_eMapComponent_Type)
	{
		_float4 pTextureTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

		pTextureTrans.z = 0.0f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTextureTrans);

		m_isTransfrom_Setting = true;
	}
}


void CMap_Manager::Transform_Control(_float fTimeDelta)
{
	if (false == m_pTab_Window->Get_MinMapRender())
		return;

	if (MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
		return;

	if (*m_pMapPlayer->Get_ViewFloor_Type() == m_eFloorType || MAP_FLOOR_TYPE::FLOOR_FREE == m_eFloorType)
	{
		// m_isRender = true;

		if (m_fBlending <= m_fOrigin_Blending)
		{
			m_fBlending = m_fOrigin_Blending;
		}

		else
			m_fBlending -= fTimeDelta * BLENDING_SPEED;
	}
}

void CMap_Manager::Rendering(_float fTimeDelta)
{
	if (false == m_isStatic_Type)
	{
		if(nullptr != m_pGetMap_Item && false == *m_pGetMap_Item)
		{
			m_isRender = false;

			return;
		}
	}

	if(m_eMapComponent_Type != MAP_UI_TYPE::TARGET_NOTIFY)
	{
		Open_Map();
	}

	if (!m_vecTextBoxes.empty())
	{
		if (true == m_isRender)
		{
			for (auto& iter : m_vecTextBoxes)
				iter->Set_FontColor(m_vOriginTextColor);
		}

		else if (false == m_isRender)
		{
			for (auto& iter : m_vecTextBoxes)
				iter->Set_FontColor(ALPHA_ZERO);
		}
	}

	else if (m_eFloorType != *m_pMapPlayer->Get_ViewFloor_Type() && MAP_FLOOR_TYPE::FLOOR_FREE != m_eFloorType)
	{
		if (m_isPrevRender != m_pTab_Window->Get_MinMapRender())
		{
			m_fBlending = 1.f;

			m_isRender = false;
		}

		/* Floor Search ���� */
		if (true == m_isRender)
		{
			/* 1. ��ü ������ ��ġ ���� */
			if (false == m_isLastPosition)
			{
				m_vLastMatrix = m_pTransformCom->Get_WorldMatrix();

				m_isLastPosition = true;
			}

			if (m_fBlending >= 1.f)
			{
				m_fBlending = 1.f;

				m_isRender = false;

				if (true == m_isLastPosition)
				{
					m_isLastPosition = false;
					m_pTransformCom->Set_WorldMatrix(m_vLastMatrix);
				}
			}

			else
			{
				/* ������ �� ���� ��¦ �ö󰡰� ��*/
				_float4x4 m_fStoreRotation = m_pTransformCom->Get_RotationMatrix_Pure();
				_matrix RotationMatrix = XMMatrixIdentity();

				m_pTransformCom->Set_RotationMatrix_Pure(RotationMatrix);
				m_pTransformCom->Go_Up(5.f);
				m_pTransformCom->Set_RotationMatrix_Pure(m_fStoreRotation);

				/* 2. ��ü Blending�ؼ� �ڿ������� ������� */
				m_vCurrentColor.w = 0.f; 
				m_fBlending += fTimeDelta * BLENDING_SPEED;
			}
		}
	}
}

void CMap_Manager::Open_Map()
{
    if (nullptr != m_pTab_Window)
    {
        /* 1. Map Open */
        if (true == m_pTab_Window->Get_MinMapRender())
		{
			if(false == m_isFirstRender)
			{
				m_fBlending = 0.f;
			}

			m_isFirstRender = true;

			m_isRender = true;
		}

        /* 2. Map Close*/
        else
		{
			m_isFirstRender = false;

			m_isRender = false;
		}
    }
}

CGameObject* CMap_Manager::Find_BackGround()
{
	list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

	for (auto& iter : *pUIList)
	{
		CStatic_Map_UI* pMapMain = dynamic_cast<CStatic_Map_UI*>(iter);

		if (nullptr != pMapMain)
		{
			if (pMapMain->m_eMapComponent_Type == MAP_UI_TYPE::BACKGROUND_MAP)
			{
				return pMapMain;
			}
		}
	}

	return nullptr;
}


/* Type�� ���� ���� �Լ� */
void CMap_Manager::Find_MapStateType()
{
	/* 1 Floor */
	if (TEXT("UI_Map") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::MAIN_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_1;
	}

	else if (TEXT("UI_Map_Font") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::FONT_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_1;
	}

	else if (TEXT("UI_Map_Window") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::WINDOW_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_1;
	}

	else if (m_wstrFile == TEXT("UI_Map_Item"))
	{
		m_eMapComponent_Type = MAP_UI_TYPE::ITEM_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_1;
	}

	/* 2 Floor */
	else if (m_wstrFile == TEXT("UI_Map_Floor2"))
	{
		m_eMapComponent_Type = MAP_UI_TYPE::MAIN_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_2;
	}

	else if (m_wstrFile == TEXT("UI_Map_Font2"))
	{
		m_eMapComponent_Type = MAP_UI_TYPE::FONT_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_2;
	}

	/* ���� */
	else if (m_wstrFile == TEXT("UI_Map_Player"))
	{
		m_eMapComponent_Type = MAP_UI_TYPE::PLAYER_MAP;
	}

	else if (m_wstrFile == TEXT("Map_Line"))
	{
		m_eMapComponent_Type = MAP_UI_TYPE::NAMELINE_MAP;
	}

	else if (m_wstrFile == TEXT("Map_Search_Type"))
	{
		m_eMapComponent_Type = MAP_UI_TYPE::SEARCH_TYPE_MAP;
	}

	else if (m_wstrFile == TEXT("Map_Target"))
	{
		m_eMapComponent_Type = MAP_UI_TYPE::TARGET_MAP;
	}

	else if (m_wstrFile == TEXT("Map_BackGround"))
	{
		m_eMapComponent_Type = MAP_UI_TYPE::BACKGROUND_MAP;
	}

	else if (TEXT("Map_Mask") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::MASK_MAP;
	}

	else if (TEXT("Map_Mask_Font") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::FONT_MASK_MAP;
	}

	else if (TEXT("UI_Map_Floor_Type") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::FLOOR_TYPE_MAP;
	}

	else if (TEXT("UI_Map_Floor3") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::MAIN_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_3;

	}

	else if (TEXT("UI_Map_Item_Floor2") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::ITEM_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_2;

	}

	else if (TEXT("UI_Map_Door") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::DOOR_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_1;
	}

	else if (TEXT("UI_Map_Door_Floor2") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::DOOR_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_2;

	}

	else if (TEXT("UI_Map_Door_Floor3") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::DOOR_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_3;
	}

	else if (TEXT("Map_Font2") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::FONT_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_2;
	}

	else if (TEXT("Map_Font3") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::FONT_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_3;
	}

	else if (TEXT("UI_Map_Item_Floor3") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::ITEM_MAP;
		m_eFloorType = MAP_FLOOR_TYPE::FLOOR_3;
	}

	else if (TEXT("UI_Map_Target_Notify") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::TARGET_NOTIFY;
	}

	else if (TEXT("UI_Map_Announcement") == m_wstrFile)
	{
		m_eMapComponent_Type = MAP_UI_TYPE::ANNOUNCEMENT_MAP;
	}
}

void CMap_Manager::Free()
{
	__super::Free();
}
