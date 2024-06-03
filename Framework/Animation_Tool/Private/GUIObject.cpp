#include "stdafx.h"
#include "GUIObject.h"

CGUIObject::CGUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CGUIObject::Initialize(void* pArg)
{
	return S_OK;
}

void CGUIObject::Tick(_float fTimeDelta)
{
}

HRESULT CGUIObject::Render()
{
	return S_OK;
}

HRESULT CGUIObject::Add_Component(LEVEL eLevel, const wstring& strPrototypeTag, const wstring& strComponentTag, void* pArg)
{
	CComponent* pComponent = m_pGameInstance->Clone_Component(eLevel, strPrototypeTag);
	if (nullptr == pComponent)
		return E_FAIL;

	CGUIObject::COMPONENT_INFO ComponentInfo;
	ComponentInfo.pComponent = pComponent;
	ComponentInfo.strPrototypeTag = strPrototypeTag;

	auto	iter = m_ComponentInfos.find(strComponentTag);
	//  이미 태그가 있었을 경우
	if (iter != m_ComponentInfos.end())
	{		
		iter->second.push_back(ComponentInfo);
	}
	//  해당 태그의 첫 등록 경우
	else
	{
		vector<COMPONENT_INFO> Components;
		Components.clear();
		Components.push_back(ComponentInfo);

		m_ComponentInfos.emplace(strComponentTag, Components);
	}

	return S_OK;
}

HRESULT CGUIObject::Update_Tools(_float fTimeDelta)
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	for (auto& Tool : m_Tools)
	{
		ImVec2 vWinSize = ImGui::GetWindowSize();		

		if (ImGui::BeginTabBar(Tool.first.c_str(), tab_bar_flags))
		{
			if (ImGui::BeginTabItem(Tool.first.c_str()))
			{
				Set_ChildSize(Tool.first, Tool.second);

				ImGui::BeginChild(Tool.first.c_str(), ImVec2(
					vWinSize.x, Tool.second->Get_ChildHeight()),
					true, ImGuiWindowFlags_AlwaysVerticalScrollbar
				);

				if (nullptr != Tool.second)
					Tool.second->Tick(fTimeDelta);

				ImGui::EndChild();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
	return S_OK;
}

void CGUIObject::Set_ChildSize(const string& strToolTag, CTool* pTool)
{
	ImGui::Text("ChildHeight");
	ImGui::SameLine();
	_float fChildHegiht = pTool->Get_ChildHeight();
	ImGui::DragFloat(string("##" + strToolTag + "ChildHeight").c_str(), &fChildHegiht);

	pTool->Set_ChildHeight(fChildHegiht);
}

void CGUIObject::Hoverd_Check()
{
	if (ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered())
		m_isHovered = true;

	else
		m_isHovered = false;

	if (true == m_isHovered)
		ImGui::Text("IsHoverd");

	else
		ImGui::Text("IsntHoverd");

	ImGui::NewLine();
}

HRESULT CGUIObject::Reset()
{
	for (auto& Pair : m_ComponentInfos)
	{
		for (auto& pComponent : Pair.second)
		{
			Safe_Release(pComponent.pComponent);
		}

		Pair.second.clear();
	}

	m_ComponentInfos.clear();

	for (auto& Pair : m_Tools)
	{
		Safe_Release(Pair.second);
	}

	m_Tools.clear();

	if (FAILED(Initialize(nullptr)))
		return E_FAIL;

	/*if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Tools()))
		return E_FAIL;*/

	return S_OK;
}

void CGUIObject::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	for (auto& Pair : m_ComponentInfos)
	{
		for (auto& pComponent : Pair.second)
		{
			Safe_Release(pComponent.pComponent);
		}

		Pair.second.clear();
	}

	m_ComponentInfos.clear();

	for (auto& Pair : m_Tools)
	{
		Safe_Release(Pair.second);
	}

	m_Tools.clear();
}
