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

	CAnimTestObject*		pTestObject = { dynamic_cast<CAnimTestObject*>(m_pTestObject) };
	if (nullptr == pTestObject)
		return;

	if (FAILED(pTestObject->Add_PartObject(m_strInputPartObjectTag)))
		return;
	
	CAnimTestPartObject*		pTestPartObject = { pTestObject->Get_PartObject(m_strInputPartObjectTag) };
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
}

_bool CTool_PartObject::Check_PartObjectExist(const wstring& strPartTag)
{
	map<wstring, CAnimTestPartObject*>::iterator		iter = { m_PartObjects.find(strPartTag) };
	
	return iter != m_PartObjects.end();
}

void CTool_PartObject::Show_PartObject_Tags()
{
	if (ImGui::CollapsingHeader("Show PartObjects ##CTool_PartObject::Show_PartObject_Tags()"))
	{
		if (ImGui::BeginListBox("##CTool_PartObject::Show_PartObject_Tags()"))
		{
			for (auto& Pair : m_PartObjects)
			{
				wstring         wstrPartTag = { Pair.first };
				string			strPartTag = { Convert_Wstring_String(wstrPartTag) };
				if (ImGui::Selectable(strPartTag.c_str()))
				{
					*m_pCurrentPartTag = wstrPartTag;
				}
			}

			ImGui::EndListBox();
		}
	}
}

void CTool_PartObject::Link_Bone(const wstring& strSrcPartTag, const wstring& strDstPartTag, const string& strSrcBoneTag, const string& strDstBoneTag)
{
	if (false == Check_PartObjectExist(strSrcPartTag) || 
		false == Check_PartObjectExist(strDstPartTag))
		return;

	CModel*			pSrcModel = { dynamic_cast<CModel*>(m_PartObjects[strSrcPartTag]->Get_Component(TEXT("Com_Model"))) };
	CModel*			pDstModel = { dynamic_cast<CModel*>(m_PartObjects[strDstPartTag]->Get_Component(TEXT("Com_Model"))) };

	if (nullptr == pSrcModel || 
		nullptr == pDstModel)
		return;

	_float4x4* pDstCombiendMatrix = { const_cast<_float4x4*>(pDstModel->Get_CombinedMatrix(strDstBoneTag)) };

	pSrcModel->Set_Surbodinate(strSrcBoneTag, true);
	pSrcModel->Set_Parent_CombinedMatrix_Ptr(strSrcBoneTag, pDstCombiendMatrix);
}

void CTool_PartObject::UnLink_Bone(const wstring& strPartTag, const string& strBoneTag)
{
	if (false == Check_PartObjectExist(strPartTag))
		return;

	CModel*			pModel = { dynamic_cast<CModel*>(m_PartObjects[strPartTag]->Get_Component(TEXT("Com_Model")))};
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
