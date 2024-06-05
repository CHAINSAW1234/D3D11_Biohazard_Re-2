#include "stdafx.h"
#include "Tool_BoneLayer.h"

CTool_BoneLayer::CTool_BoneLayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool{ pDevice, pContext }
{
}

HRESULT CTool_BoneLayer::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	BONELAYER_DESC* pDesc = { static_cast<BONELAYER_DESC*>(pArg) };
	m_pCurrentBoneLayerTag = pDesc->pCurrentBoneLayerTag;
	m_pCurrentModelTag = pDesc->pCurrentModelTag;
	m_pCurrentBoneTag = pDesc->pCurrentBoneTag;

	if (nullptr == m_pCurrentBoneLayerTag || nullptr == m_pCurrentModelTag || nullptr == m_pCurrentBoneTag)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strCollasingTag = "Tool_BoneLayer";

	return S_OK;
}

void CTool_BoneLayer::Tick(_float fTimeDelta)
{

	ImGui::Begin("Bone Layer");

	Show_Default();

	Input_LayerTag();
	Input_Start_End_BoneIndex();

	Create_BoneLayer();
	Show_BoneLayers();

	ImGui::End();
}

void CTool_BoneLayer::Input_LayerTag()
{
	static _char			szLayerTag[MAX_PATH] = {};
	ImGui::InputText("BoneLayerTag : ", szLayerTag, static_cast<size_t>(sizeof(szLayerTag)));

	wstring					strLayerTag = { Convert_String_Wstring(szLayerTag) };
	m_strInputLayerTag = strLayerTag;
}

void CTool_BoneLayer::Input_Start_End_BoneIndex()
{
	if (nullptr == m_pCurrentModel)
		return;

	_uint		iNumBones = { static_cast<_uint>(m_pCurrentModel->Get_BoneNames().size()) };

	ImGui::InputInt("Start Bone Index ##CTool_BoneLayer::Input_Start_End_BoneIndex()", &m_iStartBoneIndex);
	ImGui::InputInt("End Bone Index ##CTool_BoneLayer::Input_Start_End_BoneIndex()", &m_iEndBoneIndex);

	if (m_iStartBoneIndex >= static_cast<_int>(iNumBones))
		m_iStartBoneIndex = static_cast<_int>(iNumBones) - 1;

	if (m_iEndBoneIndex >= static_cast<_int>(iNumBones))
		m_iEndBoneIndex = static_cast<_int>(iNumBones) - 1;

	if (0 > m_iStartBoneIndex)
		m_iStartBoneIndex = 0;

	if (0 > m_iEndBoneIndex)
		m_iEndBoneIndex = 0;
}

void CTool_BoneLayer::Create_BoneLayer()
{
	if (ImGui::Button("Create Bone Layer : Range Bones ##CTool_BoneLayer::Create_BoneLayer()"))
	{
		list<_uint>			BoneIndices;
		for (_int i = m_iStartBoneIndex; i <= m_iEndBoneIndex; ++i)
		{
			BoneIndices.push_back(static_cast<_uint>(i));
		}

		Create_BoneLayer_Indices(BoneIndices);
	}

	if (ImGui::Button("Create Bone Layer : Child Bones ##CTool_BoneLayer::Create_BoneLayer()"))
	{
		Create_BoneLayer_ChildBones();
	}

	if (ImGui::Button("Create Bone Layer : All Bone ##CTool_BoneLayer::Create_BoneLayer()"))
	{
		Create_BoneLayer_AllBone();
	}
}

void CTool_BoneLayer::Show_BoneLayers()
{
	if (nullptr == m_pCurrentModel)
		return;

	if (ImGui::CollapsingHeader("Show Bone Layers ##CTool_BoneLayer::Show_BoneLayers()"))
	{
		list<wstring>			BoneLayerTags = { m_pCurrentModel->Get_BoneLayer_Tags() };

		if (ImGui::BeginListBox("Bone Layer Tags ##CTool_BoneLayer::Show_BoneLayers()"))
		{
			for (auto& wstrBoneLayerTag : BoneLayerTags)
			{
				string		strBoneLayerTag = { Convert_Wstring_String(wstrBoneLayerTag) };
				if (ImGui::Selectable(strBoneLayerTag.c_str()))
				{
					*m_pCurrentBoneLayerTag = wstrBoneLayerTag;
				}
			}

			ImGui::EndListBox();
		}
	}
}

void CTool_BoneLayer::Set_Current_Model(CModel* pModel)
{
	Safe_Release(m_pCurrentModel);
	m_pCurrentModel = nullptr;

	m_pCurrentModel = pModel;
	Safe_AddRef(m_pCurrentModel);
}

void CTool_BoneLayer::Show_Default()
{
	ImGui::SeparatorText("Information");

	string		strBoneLayerTag = { Convert_Wstring_String(*m_pCurrentBoneLayerTag) };
	ImGui::Text("Current Selected BoneLayer : ");		ImGui::SameLine();
	ImGui::Text(strBoneLayerTag.c_str());

	ImGui::SeparatorText("");
}

void CTool_BoneLayer::Create_BoneLayer_AllBone()
{
	Add_AnimLayer_AllBone(m_pCurrentModel);
}

void CTool_BoneLayer::Create_BoneLayer_ChildBones()
{
	list<_uint>			BoneIndices;
	m_pCurrentModel->Get_Child_BoneIndices(*m_pCurrentBoneTag, BoneIndices);

	Add_AnimLayer(m_pCurrentModel, BoneIndices);
}

void CTool_BoneLayer::Create_BoneLayer_Indices(list<_uint> BoneIndices)
{
	Add_AnimLayer(m_pCurrentModel, BoneIndices);
}

list<wstring> CTool_BoneLayer::Get_BoneLayerTags(CModel* pModel)
{
	if (nullptr == pModel)
		return list<wstring>();

	return pModel->Get_BoneLayer_Tags();
}

void CTool_BoneLayer::Add_AnimLayer(CModel* pModel, list<_uint> BoneIndices)
{
	if (nullptr == pModel)
		return;

	pModel->Add_Bone_Layer(m_strInputLayerTag, BoneIndices);
}

void CTool_BoneLayer::Add_AnimLayer_AllBone(CModel* pModel)
{
	if (nullptr == pModel)
		return;

	pModel->Add_Bone_Layer_All_Bone(m_strInputLayerTag);
}

CTool_BoneLayer* CTool_BoneLayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTool_BoneLayer* pInatnace = { new CTool_BoneLayer(pDevice, pContext) };

	if (FAILED(pInatnace->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTool_BoneLayer"));

		Safe_Release(pInatnace);
	}

	return pInatnace;
}

void CTool_BoneLayer::Free()
{
	__super::Free();

	Safe_Release(m_pCurrentModel);
}
