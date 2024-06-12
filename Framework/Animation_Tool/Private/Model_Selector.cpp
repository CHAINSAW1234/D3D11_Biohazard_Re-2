#include "stdafx.h"
#include "Model_Selector.h"

CModel_Selector::CModel_Selector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CTool_Selector{ pDevice, pContext }
{
}

HRESULT CModel_Selector::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	MODELSELECTOR_DESC* pDesc = { static_cast<MODELSELECTOR_DESC*>(pArg) };
	m_pCurrentBoneTag = pDesc->pCurrentBoneTag;
	m_pCurrentRootBoneTag = pDesc->pCurrentRootBoneTag;
	m_pCurrentModelTag = pDesc->pCurrentModelTag;

	if (nullptr == m_pCurrentBoneTag || nullptr == m_pCurrentModelTag || nullptr == m_pCurrentRootBoneTag)
		return E_FAIL;

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

	ImGui::Begin("Model_Bone_Selector");

	Show_Default();
	Show_ModelTags();
	Show_BoneTags();

	ImGui::End();
}

HRESULT CModel_Selector::Add_Components()
{
	if (LEVEL_TOOL == m_pGameInstance->Get_CurrentLevel())
	{
		CModel* pModel = { nullptr };

		//	Load. Leon
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

		//	Load. Zombie
		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_ZombieBody"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Body"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_ZombieFace"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Face"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_ZombieHat"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Hat"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_ZombieShirts"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_ZombiePants"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Pants"] = pModel;
	}

	return S_OK;
}

void CModel_Selector::On_Off_Buttons()
{
}

CModel* CModel_Selector::Get_Model(const string& strModelTag)
{
	map<string, CModel*>::iterator		iter = { m_Models.find(strModelTag) };
	if (iter == m_Models.end())
		return nullptr;

	return iter->second;
}

string CModel_Selector::Get_Model_Tag(CModel* pModel)
{
	string		strModelTag = { "" };
	if (nullptr == pModel)
		return strModelTag;

	for (auto& Pair : m_Models)
	{
		if (pModel == Pair.second)
		{
			strModelTag = { Pair.first };
		}
	}

	return strModelTag;
}

CModel* CModel_Selector::Get_CurrentSelectedModel()
{
	map<string, CModel*>::iterator		iter = { m_Models.find(*m_pCurrentModelTag) };
	if (iter == m_Models.end())
		return nullptr;

	return iter->second;
}

map<string, _float4x4> CModel_Selector::Get_BoneCombinedMatrices()
{
	map<string, _float4x4>			BoneCombinedMatrices;

	CModel* pModel = { Get_Model(*m_pCurrentModelTag) };
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
	map<string, CModel*>::iterator		iter = { m_Models.find(*m_pCurrentBoneTag) };

	if (iter == m_Models.end())
		return nullptr;

	return const_cast<_float4x4*>(iter->second->Get_CombinedMatrix(*m_pCurrentBoneTag));
}

void CModel_Selector::Show_Default()
{
	ImGui::SeparatorText("Information");

	ImGui::Text("Current Selected Model : ");		ImGui::SameLine();
	ImGui::Text(m_pCurrentModelTag->c_str());

	ImGui::Text("Current Selected Bone : ");		ImGui::SameLine();
	ImGui::Text(m_pCurrentBoneTag->c_str());		ImGui::SameLine();
	if (ImGui::Button("Set Root Bone ##CModel_Selector::Show_Default()"))
	{
		Set_RootBone();
		*m_pCurrentRootBoneTag = *m_pCurrentBoneTag;
	}

	ImGui::Text("Root Bone : ");					ImGui::SameLine();
	ImGui::Text(Find_RootBoneTag().c_str());

	ImGui::SeparatorText("");
}

void CModel_Selector::Show_BoneTags()
{
	if (false == Check_ModelExist(*m_pCurrentModelTag))
		return;

	static _float2		vSize = { 200.f, 100.f };
	if (ImGui::CollapsingHeader("Show Bones ##CModel_Selector::Show_BoneTags()"))
	{
		vector<string>			BoneTags = { m_Models[*m_pCurrentModelTag]->Get_BoneNames() };

		ImGui::SliderFloat2("ListBoxSize ##CModel_Selector::Show_BoneTags()", (_float*)&vSize, 100.f, 400.f);
		if (ImGui::BeginListBox("Bone Tag ##CModel_Selector::Show_BoneTags()", *(ImVec2*)&vSize))
		{
			_uint				iBoneIndex = { 0 };
			for (auto& strBoneTag : BoneTags)
			{
				string			strBoneIndex = { to_string(iBoneIndex++) };
				string			strCombinedText = { strBoneIndex + string(" : ") + strBoneTag };

				if (ImGui::Selectable(strCombinedText.c_str()))
				{
					*m_pCurrentBoneTag = strBoneTag;
				}
			}

			ImGui::EndListBox();
		}
	}
}

void CModel_Selector::Show_ModelTags()
{
	list<string>			ModelTags;
	for (auto& Pair : m_Models)
		ModelTags.push_back(Pair.first);

	static _float2		vSize = { 200.f, 100.f };
	if (ImGui::CollapsingHeader("Show Models ##CModel_Selector::Show_ModelTags()"))
	{
		ImGui::SliderFloat2("ListBoxSize ##CModel_Selector::Show_ModelTags()", (_float*)&vSize, 100.f, 400.f);
		if (ImGui::BeginListBox("Model Tag ##CModel_Selector::Show_ModelTags()", *(ImVec2*)&vSize))
		{
			ImGui::NewLine();

			for (auto& strModelTag : ModelTags)
			{
				if (ImGui::Selectable(strModelTag.c_str()))
				{
					*m_pCurrentModelTag = strModelTag;
				}
			}

			ImGui::EndListBox();
		}
	}
}

void CModel_Selector::Select_Bone()
{
	//static string		strSelectTag = { "Select Bone" };

	//map<string, CModel*>::iterator		iter = { m_Models.find(m_strSelectedModelTag) };
	//if (iter == m_Models.end())
	//	return;

	//if (ImGui::RadioButton("Show BoneTags ## CModel_Selector"), m_isShowBoneTags)
	//{
	//	m_isShowBoneTags = !m_isShowBoneTags;
	//}

	//_uint				iNumBones = { static_cast<_uint>(m_Models.size()) };
	//string* BoneTags = { new string[iNumBones] };
	//_uint				iIndex = { 0 };

	//vector<string>		vecBoneTags = { m_Models[m_strSelectedModelTag]->Get_BoneNames() };
	//for (auto& strBoneTag : vecBoneTags)
	//{
	//	BoneTags[iIndex++] = strBoneTag;
	//}

	//if (ImGui::BeginCombo("Bone", strSelectTag.c_str()))
	//{
	//	//	모델 컴포넌트 선택하는 창
	//	for (_uint i = 0; i < iNumBones; ++i)
	//	{
	//		_bool		isSelected = { BoneTags[i] == strSelectTag };
	//		if (ImGui::Selectable(BoneTags[i].c_str(), isSelected))
	//		{
	//			strSelectTag = BoneTags[i];
	//			m_strSelectedBoneTag = strSelectTag;
	//		}

	//		if (true == isSelected)
	//		{
	//			ImGui::SetItemDefaultFocus();
	//		}
	//	}
	//	ImGui::EndCombo();
	//}

	//Safe_Delete_Array(BoneTags);
}

void CModel_Selector::Set_RootBone()
{
	if (false == Check_BoneExist_CurrentModel(*m_pCurrentBoneTag))
		return;

	m_Models[*m_pCurrentModelTag]->Set_RootBone(*m_pCurrentBoneTag);

}

void CModel_Selector::Select_Model()
{
	static string		strSelectTag = { "Select Model" };

	_uint				iNumModels = { static_cast<_uint>(m_Models.size()) };
	string* ModelTags = { new string[iNumModels] };

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
				*m_pCurrentModelTag = strSelectTag;
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

_bool CModel_Selector::Check_BoneExist_CurrentModel(const string& strBoneTag)
{
	return Check_BoneExist(*m_pCurrentModelTag, strBoneTag);
}

_bool CModel_Selector::Check_BoneExist(const string& strModelTag, const string& strBoneTag)
{
	if (false == Check_ModelExist(strModelTag))
		return false;

	vector<string>					BoneTags = { m_Models[strModelTag]->Get_BoneNames() };
	vector<string>::iterator		iter = { find(BoneTags.begin(), BoneTags.end(), strBoneTag) };

	return iter != BoneTags.end();
}

_bool CModel_Selector::Check_ModelExist(const string& strModelTag)
{
	map<string, CModel*>::iterator		iter = { m_Models.find(strModelTag) };

	return iter != m_Models.end();
}

string CModel_Selector::Find_RootBoneTag()
{
	if (false == Check_ModelExist(*m_pCurrentModelTag))
		return string();

	return m_Models[*m_pCurrentModelTag]->Find_RootBoneTag();
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
