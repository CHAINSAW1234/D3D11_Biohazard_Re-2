#include "stdafx.h"
#include "Editor.h"

CEditor::CEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGUIObject{ pDevice, pContext }
{
}

HRESULT CEditor::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Tools()))
		return E_FAIL;

	return S_OK;
}

void CEditor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CEditor::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEditor::Reset()
{
	__super::Reset();

	return S_OK;
}

HRESULT CEditor::Add_Components()
{
	return S_OK;
}

HRESULT CEditor::Add_Tools()
{
	return S_OK;
}

HRESULT CEditor::Add_Tool(CTool** ppTool, _uint iToolType, const string& strToolTag, void* pArg)
{
	map<string, CTool*>::iterator		iter = { m_Tools.find(strToolTag) };
	if (iter != m_Tools.end())
	{
		MSG_BOX(TEXT("Tool Tag 중복"));
		return S_OK;
	}

	if (nullptr == ppTool)
		return E_FAIL;

	else if (CTool::TOOL_TYPE::TRANSFORMATION == (CTool::TOOL_TYPE)iToolType)
	{
		*ppTool = CTool_Transformation::Create(m_pDevice, m_pContext, pArg);

		if (nullptr == *ppTool)
			return E_FAIL;
	}

	else if (CTool::TOOL_TYPE::MODEL_SELECTOR == (CTool::TOOL_TYPE)iToolType)
	{
		*ppTool = CModel_Selector::Create(m_pDevice, m_pContext, pArg);

		if (nullptr == *ppTool)
			return E_FAIL;
	}

	else if (CTool::TOOL_TYPE::COLLIDER == (CTool::TOOL_TYPE)iToolType)
	{
		*ppTool = CTool_Collider::Create(m_pDevice, m_pContext, pArg);

		if (nullptr == *ppTool)
			return E_FAIL;
	}

	else if (CTool::TOOL_TYPE::ANIM_LIST == static_cast<CTool::TOOL_TYPE>(iToolType))
	{
		*ppTool = CTool_AnimList::Create(m_pDevice, m_pContext, pArg);

		if (nullptr == *ppTool)
			return E_FAIL;
	}

	else if (CTool::TOOL_TYPE::ANIM_PLAYER == static_cast<CTool::TOOL_TYPE>(iToolType))
	{
		*ppTool = CTool_AnimPlayer::Create(m_pDevice, m_pContext, pArg);

		if (nullptr == *ppTool)
			return E_FAIL;
	}

	else if (CTool::TOOL_TYPE::PART_OBJECT == static_cast<CTool::TOOL_TYPE>(iToolType))
	{
		*ppTool = CTool_PartObject::Create(m_pDevice, m_pContext, pArg);

		if (nullptr == *ppTool)
			return E_FAIL;
	}

	else if (CTool::TOOL_TYPE::BONE_LAYER == static_cast<CTool::TOOL_TYPE>(iToolType))
	{
		*ppTool = CTool_BoneLayer::Create(m_pDevice, m_pContext, pArg);

		if (nullptr == *ppTool)
			return E_FAIL;
	}

	else if (CTool::TOOL_TYPE::INVERSE_KINEMATIC == static_cast<CTool::TOOL_TYPE>(iToolType))
	{
		*ppTool = CTool_IK::Create(m_pDevice, m_pContext, pArg);

		if (nullptr == *ppTool)
			return E_FAIL;
	}

	else if (CTool::TOOL_TYPE::EVENT_INSERTER == static_cast<CTool::TOOL_TYPE>(iToolType))
	{
		*ppTool = CTool_EventInserter::Create(m_pDevice, m_pContext, pArg);

		if (nullptr == *ppTool)
			return E_FAIL;
	}

	else if (CTool::TOOL_TYPE::MESH_CONTROLLER == static_cast<CTool::TOOL_TYPE>(iToolType))
	{
		*ppTool = CTool_MeshController::Create(m_pDevice, m_pContext, pArg);

		if (nullptr == *ppTool)
			return E_FAIL;
	}

	else
	{
		return E_FAIL;
	}

	m_Tools.emplace(strToolTag, *ppTool);
	Safe_AddRef(*ppTool);

	return S_OK;
}

CGUIObject::COMPONENT_INFO CEditor::Find_ComponentInfo(const wstring& strComponentTag, _uint iComponentType)
{
	auto iter = m_ComponentInfos.find(strComponentTag);

	//  텍스처 태그의 컴포넌트가 등록이 안되있다면 nullptr반환
	if (iter == m_ComponentInfos.end())
		return COMPONENT_INFO{};

	//  만약 사이즈보다 텍스처 타입이 같거나 크다면 nullptr 반환 (인덱스 초과접근)
	if (iter->second.size() - 1 < iComponentType)
		return COMPONENT_INFO{};

	return iter->second[iComponentType];
}

void CEditor::Free()
{
	__super::Free();
}
