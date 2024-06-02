#include "stdafx.h"
#include "Model_Selector.h"

CModel_Selector::CModel_Selector()
{
}

HRESULT CModel_Selector::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

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

	/*if (ImGui::BeginCombo("Model", m_ModelTags[iSelect].c_str()))
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
	}*/
}

HRESULT CModel_Selector::Add_Componets()
{
	if (LEVEL_TOOL == m_pGameInstance->Get_CurrentLevel())
	{
		CModel* pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_LeonBody"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Leon_Body"] = pModel;
	}
	

	return S_OK;
}

map<string, _float4x4> CModel_Selector::Get_BoneCombinedMatrices()
{
	map<string, _float4x4>			BoneCombinedMatrices;

	CModel* pModel = { Get_Model(m_strSelectedModelTag) };
	if (nullptr == pModel)
		return BoneCombinedMatrices;

	vector<string>		BoneNames = { pModel->Get_BoneNames() };
	for (auto& strBoneTag : BoneNames)
	{
		_float4x4			BoneCombinedMatrix = { *pModel->Get_CombinedMatrix(strBoneTag) };
		BoneCombinedMatrices[strBoneTag] = BoneCombinedMatrix;
	}

	return BoneCombinedMatrices;
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

	for (auto& Pair : m_Models)
	{
		Safe_Release(Pair.second);
		Pair.second = nullptr;
	}
	m_Models.clear();
}
