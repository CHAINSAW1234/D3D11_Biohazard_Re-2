#include "stdafx.h"

#include "ContextMenu.h"
#include "Context_Highlighter.h"
#include "LayOut_UI.h"

constexpr _float CONTEXT_HIGHLIGHTER_POSZ = 0.51f;

CContextMenu::CContextMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice , pContext }
{
}

CContextMenu::CContextMenu(const CContextMenu& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CContextMenu::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CContextMenu::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(Init_Context_Highlighter()))
			return E_FAIL;

		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;

		if (FAILED(Find_LayOut()))
			return E_FAIL;
	}

	m_eContext_State = UI_IDLE;

	Set_Value_Color(&m_vColor[0]);

	return S_OK;
}

void CContextMenu::Tick(_float fTimeDelta)
{
#pragma region 나영
	if (nullptr == m_pLayOut)
	{
		Find_LayOut();

		if (nullptr == m_pLayOut)
			MSG_BOX(TEXT("CContextMenu() : Inventory가 사용할 수 있는 Layout이 없습니다. "));
	}

#pragma endregion

	if (true == m_bDead)
	{
		HoveringFor_LayOut((_int)CLayOut_UI::MENU_HOVER_TYPE::END_MENU);

		for (auto& iter : m_vecChildUI)
			iter->Set_Dead(true);

		m_pContext_Highlighter->Set_Dead(true);

		return;
	}

	switch (m_eContext_State)
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

void CContextMenu::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
	{
		for (auto& iter : m_vecChildUI)
			iter->Set_Dead(true);

		return;
	}

	__super::Late_Tick(fTimeDelta);

	m_vPrePos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
}

HRESULT CContextMenu::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CContextMenu::PopUp_Operation(_float fTimeDelta)
{

}

void CContextMenu::Idle_Operation(_float fTimeDelta)
{
	_bool IsNoOneHover = true;

	CContextMenu* pHoveredMenu = nullptr;

	m_eContextEvent = EVENT_IDLE;

	for (_uint i = 0; i < m_iContextMenuCount; i++)
	{
		if (true == static_cast<CContextMenu*>(m_vecChildUI[i])->IsMouseHover())
		{
			IsNoOneHover = false;
			pHoveredMenu = static_cast<CContextMenu*>(m_vecChildUI[i]);
			m_iHoverMenu_Type = i;
		}
	}

	m_pContext_Highlighter->Set_Dead(IsNoOneHover);

	if (false == IsNoOneHover)
	{
		_float4 HoveredPos = dynamic_cast<CTransform*>(pHoveredMenu->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION);
		HoveredPos.z = CONTEXT_HIGHLIGHTER_POSZ;
		m_pContext_HighlighterTransform->Set_State(CTransform::STATE_POSITION, HoveredPos);

		if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON))
		{
			Set_EventbyTexture(pHoveredMenu->Get_ChildTextureNum(0));
		}

#pragma region 나옹
		if (m_iPrev_HoverMenu_Type != m_iHoverMenu_Type)
		{
			HoveringFor_LayOut(m_iHoverMenu_Type);
			m_iPrev_HoverMenu_Type = m_iHoverMenu_Type;
		}
#pragma endregion

	}
}

void CContextMenu::Hide_Operation(_float fTimeDelta)
{
	for (auto& iter : m_vecChildUI)
	{
		iter->Set_Dead(true);
	}
}

void CContextMenu::Set_Operation(ITEM_TYPE eItemType, _bool bActive, _float3 fAppearPos, _float3 fArrivalPos)
{
	m_bDead = false;
	m_fAppearPos = fAppearPos;
	m_fArrivalPos = fArrivalPos;
	m_eContext_State = UI_IDLE;

	Set_Position(XMLoadFloat3(&fAppearPos));

	switch (eItemType)
	{
	case Client::EQUIPABLE: { //bActive가 장착 여부임
		m_iContextMenuCount = 4;
		
		m_eContextType = eItemType;

		if (false == bActive)
		{
			static_cast<CContextMenu*>(m_vecChildUI[0])->Set_ChildTextureNum(0, 0);
			static_cast<CContextMenu*>(m_vecChildUI[0])->Set_MyChild_Text(0, 0, TEXT("장착"));
		}

		else
		{
			static_cast<CContextMenu*>(m_vecChildUI[0])->Set_ChildTextureNum(0, 1);
			static_cast<CContextMenu*>(m_vecChildUI[0])->Set_MyChild_Text(0, 0, TEXT("장착 해제"));
		}

		static_cast<CContextMenu*>(m_vecChildUI[1])->Set_ChildTextureNum(0, 3);
		static_cast<CContextMenu*>(m_vecChildUI[1])->Set_MyChild_Text(0, 0, TEXT("검사"));

		static_cast<CContextMenu*>(m_vecChildUI[2])->Set_ChildTextureNum(0, 2);
		static_cast<CContextMenu*>(m_vecChildUI[2])->Set_MyChild_Text(0, 0, TEXT("조합"));

		static_cast<CContextMenu*>(m_vecChildUI[3])->Set_ChildTextureNum(0, 5);
		static_cast<CContextMenu*>(m_vecChildUI[3])->Set_MyChild_Text(0, 0, TEXT("단축키"));

		break;
	}
		
	case Client::CONSUMABLE_EQUIPABLE: { //bActive가 장착 여부임
		m_iContextMenuCount = 4;

		m_eContextType = eItemType;

		if (false == bActive)
		{
			static_cast<CContextMenu*>(m_vecChildUI[0])->Set_ChildTextureNum(0, 9);
			static_cast<CContextMenu*>(m_vecChildUI[0])->Set_MyChild_Text(0, 0, TEXT("장착"));
		}

		else
		{
			static_cast<CContextMenu*>(m_vecChildUI[0])->Set_ChildTextureNum(0, 10);
			static_cast<CContextMenu*>(m_vecChildUI[0])->Set_MyChild_Text(0, 0, TEXT("장착 해제"));
		}

		static_cast<CContextMenu*>(m_vecChildUI[1])->Set_ChildTextureNum(0, 3);
		static_cast<CContextMenu*>(m_vecChildUI[1])->Set_MyChild_Text(0, 0, TEXT("검사"));

		static_cast<CContextMenu*>(m_vecChildUI[2])->Set_ChildTextureNum(0, 2);
		static_cast<CContextMenu*>(m_vecChildUI[2])->Set_MyChild_Text(0, 0, TEXT("조합"));

		static_cast<CContextMenu*>(m_vecChildUI[3])->Set_ChildTextureNum(0, 5);
		static_cast<CContextMenu*>(m_vecChildUI[3])->Set_MyChild_Text(0, 0, TEXT("단축키"));

		break;
	}
		
	case Client::USEABLE: { //bActive가 사용 가능 여부임
		m_iContextMenuCount = 4;

		m_eContextType = eItemType;

		static_cast<CContextMenu*>(m_vecChildUI[0])->Set_ChildTextureNum(0, 4);
		static_cast<CContextMenu*>(m_vecChildUI[0])->Set_MyChild_Text(0, 0, TEXT("사용"));

		if (false == bActive)
		{
			//static_cast<CContextMenu*>(m_vecChildUI[0])->Child_Frame_Change_ValueColor(0, 0);
		}

		else
		{
			//static_cast<CContextMenu*>(m_vecChildUI[0])->Child_Frame_Change_ValueColor(0, 1);
		}

		static_cast<CContextMenu*>(m_vecChildUI[1])->Set_ChildTextureNum(0, 3);
		static_cast<CContextMenu*>(m_vecChildUI[1])->Set_MyChild_Text(0, 0, TEXT("검사"));

		static_cast<CContextMenu*>(m_vecChildUI[2])->Set_ChildTextureNum(0, 2);
		static_cast<CContextMenu*>(m_vecChildUI[2])->Set_MyChild_Text(0, 0, TEXT("조합"));

		static_cast<CContextMenu*>(m_vecChildUI[3])->Set_ChildTextureNum(0, 8);
		static_cast<CContextMenu*>(m_vecChildUI[3])->Set_MyChild_Text(0, 0, TEXT("폐기"));
		break;
	}
		
	case Client::CONSUMABLE: { // 액티브 관계없음
		m_iContextMenuCount = 3;

		m_eContextType = eItemType;

		static_cast<CContextMenu*>(m_vecChildUI[0])->Set_ChildTextureNum(0, 3);
		static_cast<CContextMenu*>(m_vecChildUI[0])->Set_MyChild_Text(0, 0, TEXT("검사"));


		static_cast<CContextMenu*>(m_vecChildUI[1])->Set_ChildTextureNum(0, 2);
		static_cast<CContextMenu*>(m_vecChildUI[1])->Set_MyChild_Text(0, 0, TEXT("조합"));


		static_cast<CContextMenu*>(m_vecChildUI[2])->Set_ChildTextureNum(0, 8);
		static_cast<CContextMenu*>(m_vecChildUI[2])->Set_MyChild_Text(0, 0, TEXT("폐기"));


		break;
	}
		
	case Client::QUEST: {
		m_iContextMenuCount = 1;

		m_eContextType = eItemType;

		static_cast<CContextMenu*>(m_vecChildUI[0])->Set_ChildTextureNum(0, 3);
		static_cast<CContextMenu*>(m_vecChildUI[0])->Set_MyChild_Text(0, 0, TEXT("검사"));

		break;
	}

	case Client::COMBINABLE_PICKED_UP: {
		m_iContextMenuCount = 2;

		m_eContextType = eItemType;

		static_cast<CContextMenu*>(m_vecChildUI[0])->Set_ChildTextureNum(0, 2);
		static_cast<CContextMenu*>(m_vecChildUI[0])->Set_MyChild_Text(0, 0, TEXT("조합"));

		static_cast<CContextMenu*>(m_vecChildUI[1])->Set_ChildTextureNum(0, 7);
		static_cast<CContextMenu*>(m_vecChildUI[1])->Set_MyChild_Text(0, 0, TEXT("바꾸기"));

		break;
	}

	case Client::UNCOMBINABLE_PICKED_UP: {
		m_iContextMenuCount = 1;

		m_eContextType = eItemType;

		static_cast<CContextMenu*>(m_vecChildUI[0])->Set_ChildTextureNum(0, 7);
		static_cast<CContextMenu*>(m_vecChildUI[0])->Set_MyChild_Text(0, 0, TEXT("바꾸기"));
		break;
	}
		
	default:
		break;
	}


	for (_uint i = 0; i < m_iContextMenuCount; i++)
	{
		m_vecChildUI[i]->Set_Dead(false);
	}
}

void CContextMenu::Set_EventbyTexture(_uint iTextureNum)
{
	switch (iTextureNum)
	{
	case 0: {
		m_eContextEvent = EQUIP_ITEM;
		break;
	}

	case 1: {
		m_eContextEvent = UNEQUIP_ITEM;
		break;
	}

	case 2: {
		m_eContextEvent = COMBINED_ITEM;
		break;
	}

	case 3: {
		m_eContextEvent = EXAMINE_ITEM;
		break;
	}

	case 4: {
		m_eContextEvent = USE_ITEM;
		break;
	}

	case 5: {
		m_eContextEvent = HOTKEY_ASSIGNED_ITEM;
		break;
	}

	case 6: {
		break;
	}

	case 7: {
		m_eContextEvent = SWITCH_ITEM;
		break;
	}

	case 8: {
		m_eContextEvent = DISCARD_ITEM;
		break;
	}

	case 9: {
		m_eContextEvent = UNEQUIP_ITEM;
		break;
	}

	case 10: {
		m_eContextEvent = UNEQUIP_ITEM;
		break;
	}

	case 11: {
		break;
	}

	case 12: {
		break;
	}

	case 13: {
		break;
	}

	case 14: {
		break;
	}

	default:
		break;
	}
}

HRESULT CContextMenu::Find_LayOut()
{
	list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

	for (auto& iter : *pUIList)
	{
		CLayOut_UI* pLayOut = dynamic_cast<CLayOut_UI*>(iter);

		if (nullptr != pLayOut)
		{
			if (true == pLayOut->Get_IsCheckTyping_Type())
			{
				m_pLayOut = pLayOut;

				Safe_AddRef<CLayOut_UI*>(m_pLayOut);

				break;
			}
		}
	}

	return S_OK;
}

void CContextMenu::HoveringFor_LayOut(_int _hoverType)
{
	if (nullptr == m_pLayOut)
		return;

	if((_int)CLayOut_UI::MENU_HOVER_TYPE::END_MENU == _hoverType)
	{
		m_pLayOut->Set_Typing_LayOut(false, _hoverType);
	}

	else
	{
		m_pLayOut->Set_Typing_LayOut(true, _hoverType);
	}
}

HRESULT CContextMenu::Init_Context_Highlighter()
{
	ifstream inputFileStream;
	wstring selectedFilePath;


	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Context_Highlighter.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Context_Highlighter"),
		(CGameObject**)&m_pContext_Highlighter, m_pDevice, m_pContext);

	if (nullptr == m_pContext_Highlighter)
		return E_FAIL;

	m_pContext_Highlighter->Set_Dead(true);
	m_pContext_HighlighterTransform = dynamic_cast<CTransform*>(m_pContext_Highlighter->Get_Component(g_strTransformTag));


	Safe_AddRef(m_pContext_Highlighter);
	Safe_AddRef(m_pContext_HighlighterTransform);


	return S_OK;
}

CContextMenu* CContextMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CContextMenu* pInstance = new CContextMenu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CContextMenu"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CContextMenu::Clone(void* pArg)
{
	CContextMenu* pInstance = new CContextMenu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CContextMenu"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CContextMenu::Free()
{
	__super::Free();

	Safe_Release(m_pContext_Highlighter);
	Safe_Release(m_pContext_HighlighterTransform);

	if(nullptr != m_pLayOut)
	{
		Safe_Release<CLayOut_UI*>(m_pLayOut);
		m_pLayOut = nullptr;
	}

}
