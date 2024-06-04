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

	TOOL_PARTOBJECT_DESC*		pDesc = { static_cast<TOOL_PARTOBJECT_DESC*>(pArg) };
	m_pTestObject = pDesc->pTestObject;

	if (nullptr == m_pTestObject)
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

	Update_PartObjects();

	if (ImGui::CollapsingHeader(m_strCollasingTag.c_str()))
	{
		On_Off_Buttons();
	}
}

void CTool_PartObject::On_Off_Buttons()
{
	if (ImGui::RadioButton("Show Part Tags ##PartObject", m_isShowPartObjectTags))
	{
		m_isShowPartObjectTags = !m_isShowPartObjectTags;
	}
}

void CTool_PartObject::Update_PartObjects()
{
	if (nullptr == m_pTestObject)
		return;

	CAnimTestObject*		pTestObject = { dynamic_cast<CAnimTestObject*>(m_pTestObject) };
	if (nullptr == pTestObject)
		return;

	for (auto& Pair : m_PartObjects)
	{
		Safe_Release(Pair.second);
		Pair.second = nullptr;
	}

	m_PartObjects = pTestObject->Get_PartObjects();
	for (auto& Pair : m_PartObjects)
	{
		Safe_AddRef(Pair.second);
	}
}

void CTool_PartObject::Show_PartObject_Tags()
{
	if (false == m_isShowPartObjectTags)
		return;

	ImGui::NewLine();
	ImGui::Text("=============== PartObject Tags =================");

	for (auto& Pair : m_PartObjects)
	{
		string		strPartObjectTag = { Convert_Wstring_String(Pair.first) };
		ImGui::Text(string(string("Bone : ") + strPartObjectTag + string("## Show_CurrentSelectedInfos")).c_str());
	}

	ImGui::Text("=================================================");
}

void CTool_PartObject::Link_Bone(const wstring& strSrcPartTag, const wstring& strDstPartTag, const string& strSrcBoneTag, const string& strDstBoneTag)
{
	map<wstring, CAnimTestPartObject*>::iterator		iterSrc = { m_PartObjects.find(strSrcPartTag) };
	if (iterSrc == m_PartObjects.end())
		return;

	map<wstring, CAnimTestPartObject*>::iterator		iterDst = { m_PartObjects.find(strDstPartTag) };
	if (iterDst == m_PartObjects.end())
		return;

	CModel*				pSrcModel = { dynamic_cast<CModel*>(iterSrc->second->Get_Component(TEXT("Com_Model"))) };
	CModel*				pDstModel = { dynamic_cast<CModel*>(iterDst->second->Get_Component(TEXT("Com_Model"))) };

	if (nullptr == pSrcModel || nullptr == pDstModel)
		return;

	_float4x4*			pDstCombiendMatrix = { const_cast<_float4x4*>(pDstModel->Get_CombinedMatrix(strDstBoneTag)) };

	pSrcModel->Set_Surbodinate(strSrcBoneTag, true);
	pSrcModel->Set_Parent_CombinedMatrix_Ptr(strSrcBoneTag, pDstCombiendMatrix);
}

void CTool_PartObject::UnLink_Bone(const wstring& strPartTag, const string& strBoneTag)
{
	map<wstring, CAnimTestPartObject*>::iterator		iter = { m_PartObjects.find(strPartTag) };
	if (iter == m_PartObjects.end())
		return;

	CModel*			pModel = { dynamic_cast<CModel*>(iter->second->Get_Component(TEXT("Com_Model"))) };

	if (nullptr == pModel)
		return;

	pModel->Set_Surbodinate(strBoneTag, false);
}

vector<string> CTool_PartObject::Get_CurrentPartObject_BoneTags()
{
	map<wstring, CAnimTestPartObject*>::iterator		iter = { m_PartObjects.find(m_strSelectPartObjectTag) };

	if (iter == m_PartObjects.end())
		return vector<string>();

	CModel*			pModel = { dynamic_cast<CModel*>(m_PartObjects[m_strSelectPartObjectTag]->Get_Component(TEXT("Com_Model"))) };
	if (nullptr == pModel)
		return;

	vector<string>	BoneTags = { pModel->Get_BoneNames() };
	return BoneTags;
}

CTool_PartObject* CTool_PartObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTool_PartObject*		pInatnace = { new CTool_PartObject(pDevice, pContext) };

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

	for (auto& Pair : m_PartObjects)
	{
		Safe_Release(Pair.second);
		Pair.second = nullptr;
	}
	m_PartObjects.clear();
}
