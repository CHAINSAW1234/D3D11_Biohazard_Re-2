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

	BONELAYER_DESC*			pDesc = { static_cast<BONELAYER_DESC*>(pArg) };
	m_pCurrentBoneLayerTag = pDesc->pCurrentBoneLayerTag;
	m_pCurrentModelTag = pDesc->pCurrentModelTag;

	if (nullptr == m_pCurrentBoneLayerTag || nullptr == m_pCurrentModelTag)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strCollasingTag = "Tool_BoneLayer";

	return S_OK;
}

void CTool_BoneLayer::Tick(_float fTimeDelta)
{
	if (ImGui::CollapsingHeader(m_strCollasingTag.c_str()))
	{
		Input_LayerTag();

		Show_BoneLayers();
	}
}

void CTool_BoneLayer::Input_LayerTag()
{
	static _char			szLayerTag[MAX_PATH] = {};
	ImGui::InputText("PartTag : ", szLayerTag, static_cast<size_t>(sizeof(szLayerTag)));

	wstring					strLayerTag = { Convert_String_Wstring(szLayerTag) };
	m_strInputLayerTag = strLayerTag;
}

void CTool_BoneLayer::Show_BoneLayers()
{
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

void CTool_BoneLayer::Set_CurrentModel(CModel* pModel)
{
	Safe_Release(m_pCurrentModel);
	m_pCurrentModel = nullptr;

	m_pCurrentModel = pModel;
	Safe_AddRef(m_pCurrentModel);
}

void CTool_BoneLayer::Create_AnimLayer_AllBone(CModel* pModel)
{
	Add_AnimLayer_AllBone(pModel);
}

void CTool_BoneLayer::Create_AnimLayer_ChildBones(CModel* pModel, const string& strTopParentBoneTag)
{
	list<_uint>			BoneIndices;
	pModel->Get_Child_BoneIndices(strTopParentBoneTag, BoneIndices);

	Add_AnimLayer(pModel, BoneIndices);
}

void CTool_BoneLayer::Create_AnimLayer_Indices(CModel* pModel, list<_uint> BoneIndices)
{
	Add_AnimLayer(pModel, BoneIndices);
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
	CTool_BoneLayer*		pInatnace = { new CTool_BoneLayer(pDevice, pContext) };

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
