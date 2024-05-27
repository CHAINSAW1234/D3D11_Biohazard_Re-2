#include "stdafx.h"
#include "Model_Selector.h"

CModel_Selector::CModel_Selector()
{
}

HRESULT CModel_Selector::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MODEL_SELECTOR_DESC* pDesc = (MODEL_SELECTOR_DESC*)pArg;

	m_iNumModels = (_uint)pDesc->pModelInfos->size();

	m_ModelInfos.resize(m_iNumModels);
	m_ModelTags = new string[m_iNumModels];

	const vector<MODEL_INFO>* pModelInfos = pDesc->pModelInfos;
	for (size_t i = 0; i < (size_t)m_iNumModels; ++i)
	{
		MODEL_INFO		ModelInfo;
		ModelInfo = (*pModelInfos)[i];

		if (nullptr == ModelInfo.pModel)
			return E_FAIL;

		m_ModelInfos[i] = ModelInfo;
		Safe_AddRef(ModelInfo.pModel);

		m_ModelTags[i] = pDesc->ModelTags[i];
	}

	return S_OK;
}

void CModel_Selector::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	static ImVec2 WinSize;
	WinSize = ImGui::GetWindowSize();

	static _int			iSelect = { 0 };
	static _uint		iSizeCurrentTextures = { 0 };
	static _uint		iCurrentImageIndex = { 0 };

	if (ImGui::BeginCombo("Model", m_ModelTags[iSelect].c_str()))
	{
		for (_uint i = 0; i < m_iNumModels; ++i)
		{
			bool is_selected = (m_ModelTags[iSelect] == m_ModelTags[i]);

			if (ImGui::Selectable(m_ModelTags[i].c_str(), is_selected))
			{
				iSelect = i;
				Change_CurrentModelInfo(m_ModelInfos[iSelect]);
			}

			if (true == is_selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void CModel_Selector::Change_CurrentModelInfo(MODEL_INFO ModelInfo)
{
	Safe_Release(m_CurrentInfo.pModel);

	m_CurrentInfo = ModelInfo;
	Safe_AddRef(m_CurrentInfo.pModel);
}

CModel_Selector* CModel_Selector::Create(void* pArg)
{
	CModel_Selector* pInatnace = new CModel_Selector();

	if (FAILED(pInatnace->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel_Selector"));

		Safe_Release(pInatnace);
	}

	return pInatnace;
}

void CModel_Selector::Free()
{
	__super::Free();

	for (auto& ModelInfo : m_ModelInfos)
	{
		Safe_Release(ModelInfo.pModel);
	}

	m_ModelInfos.clear();

	Safe_Delete_Array(m_ModelTags);

	Safe_Release(m_CurrentInfo.pModel);
}
