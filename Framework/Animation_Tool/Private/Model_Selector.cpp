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

	static string		strSelectTag = { "Select Model" };

	_uint			iNumModels = { static_cast<_uint>(m_Models.size()) };
	string*			ModelTags = { new string[iNumModels] };

	_uint			iIndex = { 0 };
	for (auto& Pair : m_Models)
	{
		ModelTags[iIndex++] = Pair.first;
	}

	if (ImGui::CollapsingHeader(m_strCollasingTag.c_str()))
	{
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

			//	애니메이션 레이어 만들기 ( 텍스트 입력 및 레이어 생성 )
			Create_AnimLayer();

			//	뼈 보이기 버튼 ( 폰트 )
			if (ImGui::Button("Show_BoneTag ## ModelSelector"))
			{
				m_isShowBoneTags = !m_isShowBoneTags;
			}

			Safe_Delete_Array(ModelTags);
			ImGui::EndCombo();
		}
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

void CModel_Selector::Create_AnimLayer()
{
	static _char			szLayerTag[MAX_PATH] = {};
	ImGui::InputText("AnimLayerTag : ## ModelSelctor", szLayerTag, static_cast<size_t>(sizeof(szLayerTag)));

	if (ImGui::Button("Add_PartObject"))
	{
		_tchar		szTemp[MAX_PATH] = { L"" };
		MultiByteToWideChar(CP_ACP, 0, szLayerTag, (_uint)strlen(szLayerTag), szTemp, MAX_PATH);

		_int		iStartBoneIndex = { 0 };
		_int		iEndBoneIndex = { 0 };
		ImGui::InputInt("BoneStartIndex : ## ModelSelector_AnimLayer", &iStartBoneIndex);
		ImGui::InputInt("BoneEndIndex : ## ModelSelector_AnimLayer", &iEndBoneIndex);		

		//	스타트 부터 엔드 인덱스까지
		if (ImGui::Button("Add_Layer_BoneIndices ## ModelSelector"))
		{
			list<_uint>			BoneIndices;
			for (_uint i = static_cast<_uint>(iStartBoneIndex); i <= static_cast<_uint>(iEndBoneIndex); ++i)
			{
				BoneIndices.push_back(i);
			}

			Add_AnimLayer(szTemp, BoneIndices);
		}

		_int		iTopParentIndex = { 0 };
		ImGui::InputInt("BoneTopParentIndex : ## ModelSelector_AnimLayer", &iTopParentIndex);

		//	특정 인덱스의 자식뼈 전부
		if (ImGui::Button("Add_Layer_ChildBones ## ModelSelector"))
		{
			map<string, CModel*>::iterator		iter = { m_Models.find(m_strSelectedModelTag) };
			if (iter != m_Models.end())
			{
				list<_uint>		ChildBoneIndices;
				m_Models[m_strSelectedModelTag]->Get_Child_BoneIndices(m_strCurrentSelectBoneTag, ChildBoneIndices);

				Add_AnimLayer(szTemp, ChildBoneIndices);
			}
		}

		//	모든 인덱스
		if (ImGui::Button("Add_Layer_AllBone ## ModelSelector"))
		{
			Add_AnimLayer_AllBone(szTemp);
		}
	}

		

	if (true == m_isShowBoneTags)
	{
		Show_BoneTags();
	}


}

void CModel_Selector::Add_AnimLayer(const wstring& strAnimLayerTag, list<_uint> BoneIndices)
{
	map<string, CModel*>::iterator		iter = { m_Models.find(m_strSelectedModelTag) };
	if (iter == m_Models.end())
		return;

	m_Models[m_strSelectedModelTag]->Add_Bone_Layer(strAnimLayerTag, BoneIndices);
}

void CModel_Selector::Add_AnimLayer_AllBone(const wstring& strAnimLayerTag)
{
	map<string, CModel*>::iterator		iter = { m_Models.find(m_strSelectedModelTag) };
	if (iter == m_Models.end())
		return;

	m_Models[m_strSelectedModelTag]->Add_Bone_Layer_All_Bone(strAnimLayerTag);
}

void CModel_Selector::Set_RootBone()
{
	map<string, CModel*>::iterator		iter = { m_Models.find(m_strSelectedModelTag) };
	if (iter == m_Models.end())
		return;


	m_Models[m_strSelectedModelTag]->Set_RootBone(m_strCurrentSelectBoneTag);
}

void CModel_Selector::Show_BoneTags()
{
	map<string, CModel*>::iterator		iter = { m_Models.find(m_strSelectedModelTag) };
	if (iter == m_Models.end())
		return;

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "BoneTags");

	ImGui::BeginChild("Scrolling ## ShowBoneTags() ");

	vector<string>			BoneTags = { m_Models[m_strSelectedModelTag]->Get_BoneNames() };
	for (auto& strBoneTag : BoneTags)
	{
		if (ImGui::Button(string(string("Select") + strBoneTag).c_str()))
		{
			m_strCurrentSelectBoneTag = strBoneTag;
		}

		ImGui::Text(strBoneTag.c_str());

	}
	ImGui::EndChild();	
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
