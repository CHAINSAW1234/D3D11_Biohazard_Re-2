#include "stdafx.h"
#include "Tool_PartObject.h"

#include "AnimTestPartObject.h"
#include "AnimTestObject.h"

CTool_PartObject::CTool_PartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool{ pDevice, pContext }
{
}

HRESULT CTool_PartObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	TOOL_PARTOBJECT_DESC* pDesc = { static_cast<TOOL_PARTOBJECT_DESC*>(pArg) };
	m_pTestObject = pDesc->pTestObject;
	m_pCurrentPartTag = pDesc->pCurrentPartTag;
	m_pCurrentModelTag = pDesc->pCurrentModelTag;

	if (nullptr == m_pCurrentPartTag)
		return E_FAIL;

	Safe_AddRef(m_pTestObject);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strCollasingTag = "Tool_PartObject";

	return S_OK;
}

void CTool_PartObject::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	static _bool isChanged = { false };

	ImGui::Begin("Part Object Tool");

	Show_Default();
	On_Off_Buttons();

	Input_PartObjectTag();
	Create_Release_PartObject();
	Show_PartObject_Tags();

	Show_LinkBone();

	ImGui::End();
}

void CTool_PartObject::Input_PartObjectTag()
{
	static _char			szPartObjectTag[MAX_PATH] = {};
	ImGui::InputText("Part Object Tag: ", szPartObjectTag, static_cast<size_t>(sizeof(szPartObjectTag)));

	wstring					strPartObjectTag = { Convert_String_Wstring(szPartObjectTag) };
	m_strInputPartObjectTag = strPartObjectTag;
}

void CTool_PartObject::Create_Release_PartObject()
{
	if (ImGui::Button("Create PartObject ##CTool_PartObject::Create_Release_PartObject()"))
	{
		Add_PartObject();
	}

	if (ImGui::Button("Release PartObject ##CTool_PartObject::Create_Release_PartObject()"))
	{
		Release_PartObject(*m_pCurrentPartTag);
	}
}

void CTool_PartObject::Add_PartObject()
{
	if (m_strInputPartObjectTag == TEXT(""))
		return;

	if (true == Check_PartObjectExist(m_strInputPartObjectTag))
		return;

	CAnimTestObject* pTestObject = { dynamic_cast<CAnimTestObject*>(m_pTestObject) };
	if (nullptr == pTestObject)
		return;

	if (FAILED(pTestObject->Add_PartObject(m_strInputPartObjectTag)))
		return;

	CAnimTestPartObject* pTestPartObject = { pTestObject->Get_PartObject(m_strInputPartObjectTag) };
	if (nullptr == pTestPartObject)
		return;

	m_PartObjects.emplace(m_strInputPartObjectTag, pTestPartObject);
	Safe_AddRef(m_PartObjects[m_strInputPartObjectTag]);
}

void CTool_PartObject::Release_PartObject(const wstring& strPartTag)
{
	if (false == Check_PartObjectExist(strPartTag))
		return;

	CAnimTestObject* pTestObject = { dynamic_cast<CAnimTestObject*>(m_pTestObject) };
	if (nullptr == pTestObject)
		return;

	if (FAILED(pTestObject->Erase_PartObject(strPartTag)))
		return;

	Safe_Release(m_PartObjects[strPartTag]);
	m_PartObjects[strPartTag] = nullptr;

	m_PartObjects.erase(strPartTag);
}

void CTool_PartObject::Show_Default()
{
	ImGui::SeparatorText("Information");

	string		strPartTag = { Convert_Wstring_String(*m_pCurrentPartTag) };
	ImGui::Text("Current Selected PartObject : ");		ImGui::SameLine();
	ImGui::Text(strPartTag.c_str());

	ImGui::SeparatorText("");
}

void CTool_PartObject::On_Off_Buttons()
{
	if (ImGui::Button("UnLink Bone ##CTool_PartObject::On_Off_Buttons()"))
	{
		UnLink_Bone_All(*m_pCurrentPartTag);
	}

	CAnimTestObject* pTestObject = { dynamic_cast<CAnimTestObject*>(m_pTestObject) };
	string					strRootPartTag = { "" };
	if (nullptr != pTestObject)
	{
		strRootPartTag = Convert_Wstring_String(pTestObject->Get_RootActivePartTag());
	}

	ImGui::Text("Current Root Active Part : "); ImGui::SameLine();
	ImGui::Text(strRootPartTag.c_str());

	if (ImGui::Button("Set_Root_ActivePart "))
	{
		pTestObject->Set_RootActivePart(*m_pCurrentPartTag);
	}
}

void CTool_PartObject::Show_LinkBone()
{
	if (false == Check_PartObjectExist(*m_pCurrentPartTag))
		return;

	static _float2		vSize = { 200.f, 100.f };
	ImGui::SeparatorText("Link To Target PartObject");
	ImGui::SliderFloat2("ListBoxSize ##CTool_PartObject::Button_LinkBone()", (_float*)&vSize, 100.f, 400.f);

	if (ImGui::BeginListBox("Target PartObjects ##CTool_PartObject::Button_LinkBone()", *(ImVec2*)&vSize))
	{
		for (auto& Pair : m_PartObjects)
		{
			wstring         wstrPartTag = { Pair.first };
			if (wstrPartTag == *m_pCurrentPartTag)
				continue;

			string			strPartTag = { Convert_Wstring_String(wstrPartTag) };
			if (ImGui::Selectable(strPartTag.c_str()))
			{
				Link_Bone_Auto(*m_pCurrentPartTag, wstrPartTag);
			}
		}

		ImGui::EndListBox();
	}
	ImGui::SeparatorText("##");
}

_bool CTool_PartObject::Check_PartObjectExist(const wstring& strPartTag)
{
	map<wstring, CAnimTestPartObject*>::iterator		iter = { m_PartObjects.find(strPartTag) };

	return iter != m_PartObjects.end();
}

void CTool_PartObject::Show_PartObject_Tags()
{
	static _float2		vSize = { 200.f, 100.f };

	if (ImGui::CollapsingHeader("Show PartObjects ##CTool_PartObject::Show_PartObject_Tags()"))
	{
		ImGui::SliderFloat2("ListBoxSize ##CTool_PartObject::Show_PartObject_Tags()", (_float*)&vSize, 100.f, 400.f);
		if (ImGui::BeginListBox("##CTool_PartObject::Show_PartObject_Tags()"))
		{
			for (auto& Pair : m_PartObjects)
			{
				wstring         wstrPartTag = { Pair.first };
				string			strPartTag = { Convert_Wstring_String(wstrPartTag) };
				if (ImGui::Selectable(strPartTag.c_str()))
				{
					*m_pCurrentPartTag = wstrPartTag;

					if (false == Check_PartObjectExist(*m_pCurrentPartTag))
						continue;

					CModel* pSrcModel = { m_PartObjects[*m_pCurrentPartTag]->Get_CurrentModelComponent() };
					string			strModelTag = { *m_pCurrentModelTag };

					for (auto& Pair : *m_pModels)
					{
						CModel* pDstModel = { Pair.second };
						if (pSrcModel == pDstModel)
						{
							*m_pCurrentModelTag = Pair.first;
							break;
						}
					}
				}
			}

			ImGui::EndListBox();
		}
	}
}

void CTool_PartObject::Link_Bone_Auto(const wstring& strSrcPartTag, const wstring& strDstPartTag)
{
	if (false == Check_PartObjectExist(strSrcPartTag) ||
		false == Check_PartObjectExist(strDstPartTag))
		return;

	CModel* pSrcModel = { dynamic_cast<CModel*>(m_PartObjects[strSrcPartTag]->Get_CurrentModelComponent()) };
	CModel* pDstModel = { dynamic_cast<CModel*>(m_PartObjects[strDstPartTag]->Get_CurrentModelComponent()) };

	if (pSrcModel == pDstModel)
		return;

	if (nullptr == pSrcModel ||
		nullptr == pDstModel)
		return;

	vector<string>			SrcBoneTags = { pSrcModel->Get_BoneNames() };
	vector<string>			DstBoneTags = { pDstModel->Get_BoneNames() };

	sort(SrcBoneTags.begin(), SrcBoneTags.end());
	sort(DstBoneTags.begin(), DstBoneTags.end());

	vector<string>			IntersectionBoneTags;
	IntersectionBoneTags.resize(min(SrcBoneTags.size(), DstBoneTags.size()));

	vector<string>::iterator			iter = { set_intersection(SrcBoneTags.begin(), SrcBoneTags.end(), DstBoneTags.begin(), DstBoneTags.end(), IntersectionBoneTags.begin()) };

	IntersectionBoneTags.resize(iter - IntersectionBoneTags.begin());

	for (auto& strIntersectBoneTag : IntersectionBoneTags)
	{
		_float4x4* pDstCombiendMatrix = { const_cast<_float4x4*>(pDstModel->Get_CombinedMatrix(strIntersectBoneTag)) };
		if (nullptr == pDstCombiendMatrix)
			continue;

		pSrcModel->Set_Surbodinate(strIntersectBoneTag, true);
		pSrcModel->Set_Parent_CombinedMatrix_Ptr(strIntersectBoneTag, pDstCombiendMatrix);
	}
}

void CTool_PartObject::Link_Bone_Manual(const wstring& strSrcPartTag, const wstring& strDstPartTag, const string& strSrcBoneTag, const string& strDstBoneTag)
{
	if (false == Check_PartObjectExist(strSrcPartTag) ||
		false == Check_PartObjectExist(strDstPartTag))
		return;
	CModel* pSrcModel = { dynamic_cast<CModel*>(m_PartObjects[strSrcPartTag]->Get_CurrentModelComponent()) };
	CModel* pDstModel = { dynamic_cast<CModel*>(m_PartObjects[strDstPartTag]->Get_CurrentModelComponent()) };

	if (nullptr == pSrcModel ||
		nullptr == pDstModel)
		return;

	if (pSrcModel == pDstModel)
		return;

	_float4x4* pDstCombiendMatrix = { const_cast<_float4x4*>(pDstModel->Get_CombinedMatrix(strDstBoneTag)) };

	pSrcModel->Set_Surbodinate(strSrcBoneTag, true);
	pSrcModel->Set_Parent_CombinedMatrix_Ptr(strSrcBoneTag, pDstCombiendMatrix);
}

void CTool_PartObject::UnLink_Bone_All(const wstring& strPartTag)
{
	if (false == Check_PartObjectExist(strPartTag))
		return;

	CModel* pModel = { dynamic_cast<CModel*>(m_PartObjects[strPartTag]->Get_CurrentModelComponent()) };
	if (nullptr == pModel)
		return;

	vector<string>		BoneTags = { pModel->Get_BoneNames() };
	for (auto& strBoneTag : BoneTags)
	{
		pModel->Set_Surbodinate(strBoneTag, false);
	}
}

void CTool_PartObject::UnLink_Bone(const wstring& strPartTag, const string& strBoneTag)
{
	if (false == Check_PartObjectExist(strPartTag))
		return;

	CModel* pModel = { dynamic_cast<CModel*>(m_PartObjects[strPartTag]->Get_CurrentModelComponent()) };
	//	CModel* pModel = { dynamic_cast<CModel*>(m_PartObjects[strPartTag]->Get_Component(TEXT("Com_Model"))) };
	if (nullptr == pModel)
		return;

	pModel->Set_Surbodinate(strBoneTag, false);
}

void CTool_PartObject::Set_CurrentAnimation(CAnimation* pAnimation)
{
	Safe_Release(m_pCurrentAnimation);
	m_pCurrentAnimation = nullptr;

	m_pCurrentAnimation = pAnimation;
	Safe_AddRef(m_pCurrentAnimation);
}

HRESULT CTool_PartObject::Set_Models(map<string, CModel*>* pModels)
{
	if (nullptr == pModels)
		return E_FAIL;

	m_pModels = pModels;
	for (auto& Pair : *m_pModels)
	{
		if (nullptr == Pair.second)
			return E_FAIL;
	}

	return S_OK;
}

vector<string> CTool_PartObject::Get_CurrentPartObject_BoneTags()
{
	map<wstring, CAnimTestPartObject*>::iterator		iter = { m_PartObjects.find(*m_pCurrentPartTag) };

	if (iter == m_PartObjects.end())
		return vector<string>();

	CModel* pModel = { dynamic_cast<CModel*>(m_PartObjects[*m_pCurrentPartTag]->Get_Component(TEXT("Com_Model"))) };
	if (nullptr == pModel)
		return vector<string>();

	vector<string>	BoneTags = { pModel->Get_BoneNames() };
	return BoneTags;
}

CAnimTestPartObject* CTool_PartObject::Get_CurrentPartObject()
{
	if (false == Check_PartObjectExist(*m_pCurrentPartTag))
		return nullptr;

	return m_PartObjects[*m_pCurrentPartTag];
}

CTool_PartObject* CTool_PartObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTool_PartObject* pInatnace = { new CTool_PartObject(pDevice, pContext) };

	if (FAILED(pInatnace->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTool_PartObject"));

		Safe_Release(pInatnace);
	}

	return pInatnace;
}

void CTool_PartObject::Free()
{
	__super::Free();

	Safe_Release(m_pTestObject);
	Safe_Release(m_pCurrentAnimation);

	for (auto& Pair : m_PartObjects)
	{
		Safe_Release(Pair.second);
		Pair.second = nullptr;
	}
	m_PartObjects.clear();
}
