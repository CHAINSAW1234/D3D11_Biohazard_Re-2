#include "stdafx.h"
#include "Model_Selector.h"

CModel_Selector::CModel_Selector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CTool_Selector{ pDevice, pContext }
{
}

HRESULT CModel_Selector::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_strCollasingTag = "Tool_ModelSelector";

	return S_OK;
}

void CModel_Selector::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	static ImVec2 WinSize;
	WinSize = ImGui::GetWindowSize();	

	if (ImGui::CollapsingHeader(m_strCollasingTag.c_str()))
	{
		Select_Model();
		Select_Bone();		
		Select_RootBone();
	}	
}

HRESULT CModel_Selector::Add_Components()
{
	if (LEVEL_TOOL == m_pGameInstance->Get_CurrentLevel())
	{
		CModel* pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_LeonBody"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Leon_Body"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_LeonFace"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Leon_Face"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_LeonHair"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Leon_Hair"] = pModel;
	}	

	return S_OK;
}

void CModel_Selector::Select_Model()
{
	static string		strSelectTag = { "Select Model" };

	_uint				iNumModels = { static_cast<_uint>(m_Models.size()) };
	string*				ModelTags = { new string[iNumModels] };

	_uint				iIndex = { 0 };
	for (auto& Pair : m_Models)
	{
		ModelTags[iIndex++] = Pair.first;
	}

	if (ImGui::BeginCombo("Model", strSelectTag.c_str()))
	{
		//	모델 컴포넌트 선택하는 창
		for (_uint i = 0; i < iNumModels; ++i)
		{
			_bool		isSelected = { ModelTags[i] == strSelectTag };
			if (ImGui::Selectable(ModelTags[i].c_str(), isSelected))
			{
				strSelectTag = ModelTags[i];
				m_strSelectedModelTag = strSelectTag;
			}
			if (true == isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	Safe_Delete_Array(ModelTags);
}

void CModel_Selector::Select_Bone()
{
	static string		strSelectTag = { "Select Bone" };	

	map<string, CModel*>::iterator		iter = { m_Models.find(m_strSelectedModelTag) };
	if (iter == m_Models.end())
		return;

	if (ImGui::RadioButton("Show BoneTags ## CModel_Selector"), m_isShowBoneTags)
	{
		m_isShowBoneTags = !m_isShowBoneTags;
	}

	_uint				iNumBones = { static_cast<_uint>(m_Models.size()) };
	string*				BoneTags = { new string[iNumBones] };
	_uint				iIndex = { 0 };

	vector<string>		vecBoneTags = { m_Models[m_strSelectedModelTag]->Get_BoneNames() };
	for (auto& strBoneTag : vecBoneTags)
	{
		BoneTags[iIndex++] = strBoneTag;
	}

	if (ImGui::BeginCombo("Bone", strSelectTag.c_str()))
	{
		//	모델 컴포넌트 선택하는 창
		for (_uint i = 0; i < iNumBones; ++i)
		{
			_bool		isSelected = { BoneTags[i] == strSelectTag };
			if (ImGui::Selectable(BoneTags[i].c_str(), isSelected))
			{
				strSelectTag = BoneTags[i];
				m_strSelectedBoneTag = strSelectTag;
			}

			if (true == isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	Safe_Delete_Array(BoneTags);
}

CModel* CModel_Selector::Get_CurrentSelectedModel()
{
	map<string, CModel*>::iterator		iter = { m_Models.find(m_strSelectedModelTag) };
	if (iter == m_Models.end())
		return nullptr;

	return m_Models[m_strSelectedModelTag];
}

map<string, _float4x4> CModel_Selector::Get_BoneCombinedMatrices()
{
	map<string, _float4x4>			BoneCombinedMatrices;

	CModel*					pModel = { Get_Model(m_strSelectedModelTag) };
	if (nullptr == pModel)
		return BoneCombinedMatrices;

	vector<string>			BoneNames = { pModel->Get_BoneNames() };
	for (auto& strBoneTag : BoneNames)
	{
		_float4x4			BoneCombinedMatrix = { *pModel->Get_CombinedMatrix(strBoneTag) };
		BoneCombinedMatrices[strBoneTag] = BoneCombinedMatrix;
	}

	return BoneCombinedMatrices;
}

_float4x4* CModel_Selector::Get_Selected_BoneCombinedMatrix_Ptr()
{
	map<string, CModel*>::iterator		iter = { m_Models.find(m_strSelectedBoneTag) };

	if (iter == m_Models.end())
		return nullptr;

	return const_cast<_float4x4*>(iter->second->Get_CombinedMatrix(m_strSelectedBoneTag));
}

void CModel_Selector::Set_RootBone()
{
	map<string, CModel*>::iterator		iter = { m_Models.find(m_strSelectedModelTag) };
	if (iter == m_Models.end())
		return;

	m_Models[m_strSelectedModelTag]->Set_RootBone(m_strSelectedBoneTag);

}

CModel_Selector* CModel_Selector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CModel_Selector* pInatnace = new CModel_Selector(pDevice, pContext);

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
