#include "stdafx.h"
#include "Model_Selector.h"
#include "ImGuizmo.h"

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

	for (auto& Pair: m_Models)
	{
		CModel*			pModel = { Pair.second };
		pModel->Add_Bone_Layer_All_Bone(TEXT("Default"));
		pModel->Set_OptimizationCulling(false);
	}

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

	Show_Bone_Transform();

	ImGui::End();
}

HRESULT CModel_Selector::Add_Components()
{
	if (LEVEL_TOOL == m_pGameInstance->Get_CurrentLevel())
	{
		CModel* pModel = { nullptr };

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////..........//////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////..PLAYER..//////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////..........//////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_HandGun"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Leon_HandGun"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_FlashLight"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Leon_FlashLight"] = pModel;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////..........//////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////..ZOMBIE..//////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////..........//////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		

		//	Load. Zombie
		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Body_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Body_Male"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Body_Female"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Body_Female"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Body_Male_Big"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Body_Male_Big"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Hat00_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Hat00"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Hat01_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Hat01"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face00_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Face00"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face01_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Face01"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face02_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Face02"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face03_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Face03"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face04_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Face04"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face05_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Face05"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face06_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Face06"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face07_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Face07"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face08_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Face08"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face09_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Face09"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face10_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Face10"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants00_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Pants00"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants00_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Pants00"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants00_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Pants00"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants01_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Pants01"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants02_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Pants02"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants03_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Pants03"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants04_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Pants04"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants05_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Pants05"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants06_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Pants06"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts00_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts00"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts01_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts01"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts02_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts02"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts03_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts03"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts04_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts04"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts05_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts05"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts06_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts06"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts06_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts06"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts08_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts08"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts09_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts09"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts11_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts11"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts12_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts12"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts70_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts70"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts72_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts72"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts73_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts73"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts81_Male"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["Zombie_Shirts81"] = pModel;

		

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_7800"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["PL7800"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_7850"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["PL7850"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_7870"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["PL7870"] = pModel;

		pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_7880"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["PL7880"] = pModel;


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////........///////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////..BOSS..///////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////........///////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

		/*pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Boss"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["BOSS"] = pModel;*/

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////..............////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////..ADDITIONAL..////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////..............////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

		/*pModel = { nullptr };

		pModel = { dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_TEST"))) };
		if (nullptr == pModel)
			return E_FAIL;

		m_Models["TEST"] = pModel;*/
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

void CModel_Selector::Show_Bone_Transform()
{
	if (nullptr == m_pTransformTarget)
		return;

	map<string, CModel*>::iterator		iter = { m_Models.find(*m_pCurrentModelTag) };
	if (iter == m_Models.end())
		return;

	CModel*			pModel = { iter->second };
	if (nullptr == pModel)
		return;	

	const _float4x4*		pBoneCombiendFloat4x4 = { pModel->Get_CombinedMatrix(*m_pCurrentBoneTag) };
	if (nullptr == pBoneCombiendFloat4x4)
		return;

	_matrix					BoneCombinedMatrix = { XMLoadFloat4x4(pBoneCombiendFloat4x4) };

	_matrix				WorldMatrix = { m_pTransformTarget->Get_WorldMatrix() };

	_matrix				BoneWorldMatrix = { BoneCombinedMatrix * WorldMatrix };
	_float4x4			BoneWorldFloat4x4;
	XMStoreFloat4x4(&BoneWorldFloat4x4, BoneWorldMatrix);

	static ImGuizmo::OPERATION		CurrentGizmoOperation(ImGuizmo::SCALE);
	static ImGuizmo::MODE			CurrentGizmoMode(ImGuizmo::WORLD);

	static _bool isUseSnap = { false };
	ImGui::Checkbox("Grab", &isUseSnap);
	ImGui::SameLine();

	_float3 vSnap = { 0.f, 0.f, 0.f };
	switch (CurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::InputFloat3("Translate Grab", &vSnap.x);
		break;
	case ImGuizmo::ROTATE:
		ImGui::InputFloat("Rotate Grab", &vSnap.x);
		break;
	case ImGuizmo::SCALE:
		ImGui::InputFloat("Scale Grab", &vSnap.x);
		break;
	}

	ImGuiIO& io = { ImGui::GetIO() };
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	_float4x4	ProjMatrix, ViewMatrix;

	ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
	ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);

	//	ImGuizmo::Manipulate(&ViewMatrix.m[0][0], &ProjMatrix.m[0][0], CurrentGizmoOperation, CurrentGizmoMode, &BoneWorldFloat4x4.m[0][0], NULL, isUseSnap ? &vSnap.x : NULL);
}

void CModel_Selector::Set_Transform(CTransform* pTransform)
{
	if (nullptr == pTransform)
		return;

	Safe_Release(m_pTransformTarget);
	m_pTransformTarget = nullptr;

	m_pTransformTarget = pTransform;
	Safe_AddRef(m_pTransformTarget);
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
	//	//	�� ������Ʈ �����ϴ� â
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
		//	�� ������Ʈ �����ϴ� â
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

	Safe_Release(m_pTransformTarget);
}

