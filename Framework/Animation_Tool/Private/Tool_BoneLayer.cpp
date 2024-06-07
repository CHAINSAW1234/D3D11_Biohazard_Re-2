#include "stdafx.h"
#include "Tool_BoneLayer.h"

CTool_BoneLayer::CTool_BoneLayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool{ pDevice, pContext }
{
}

HRESULT CTool_BoneLayer::Initialize(void* pArg)
{
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
	}
}

void CTool_BoneLayer::Input_LayerTag()
{
	static _char			szLayerTag[MAX_PATH] = {};
	ImGui::InputText("PartTag : ", szLayerTag, static_cast<size_t>(sizeof(szLayerTag)));

	wstring			strLayerTag = { Convert_String_Wstring(szLayerTag) };
	m_strAnimLayerTag = strLayerTag;
}

void CTool_BoneLayer::Create_AnimLayer_AllBone(CModel* pModel)
{
	Add_AnimLayer_AllBone(pModel);
}

void CTool_BoneLayer::Create_AnimLayer_BoneChilds(CModel* pModel, const string& strTopParentBoneTag)
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

	pModel->Add_Bone_Layer(m_strAnimLayerTag, BoneIndices);
}

void CTool_BoneLayer::Add_AnimLayer_AllBone(CModel* pModel)
{
	if (nullptr == pModel)
		return;

	pModel->Add_Bone_Layer_All_Bone(m_strAnimLayerTag);
}

CTool_BoneLayer* CTool_BoneLayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	return nullptr;
}

void CTool_BoneLayer::Free()
{
}
