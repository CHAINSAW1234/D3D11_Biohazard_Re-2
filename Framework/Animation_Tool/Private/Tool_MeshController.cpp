#include "stdafx.h"
#include "Tool_MeshController.h"

CTool_MeshController::CTool_MeshController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTool{ pDevice, pContext }
{
}

HRESULT CTool_MeshController::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	MESHCONTROLLER_DESC*		pDesc = { static_cast<MESHCONTROLLER_DESC*>(pArg) };

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strCollasingTag = "Tool_MeshController";

    return S_OK;
}

void CTool_MeshController::Tick(_float fTimeDelta)
{
	ImGui::Begin("Mesh_Controller");

	Show_MeshTags_HideMesh();

	ImGui::End();
}

void CTool_MeshController::Input_Text()
{
}

enum MODE{ MODE_HIDE, MODE_SHOW, MODE_END };
void CTool_MeshController::Show_MeshTags_HideMesh()
{
	for (auto& Pair : m_Models)
	{
		CModel*					pModel = { Pair.second };
		vector<string>			MeshTags = { pModel->Get_MeshTags() };
		
		string					strModelTag = { Find_ModelTag(pModel) };

		if (ImGui::CollapsingHeader(strModelTag.c_str()))
		{

			if (ImGui::Button(string(string("Hide All ##CTool_MeshController::Show_MeshTags_HideMesh()") + strModelTag).c_str()))
			{
				Hide_Mesh_All(strModelTag);
			}		ImGui::SameLine();

			if (ImGui::Button(string(string("Show All ##CTool_MeshController::Show_MeshTags_HideMesh()") + strModelTag).c_str()))
			{
				Show_Mesh_All(strModelTag);
			}
			/*
			_uint						iMode = { 0 };
			static MODE					eMode = { MODE_END };

			static _bool				isHideMode = { false };
			static _bool				isShowMode = { false };

			isHideMode = { eMode == MODE_HIDE };
			isShowMode = { eMode == MODE_SHOW };
			
			if(ImGui::RadioButton(string(string("Hide ##CTool_MeshController::Show_MeshTags_HideMesh()") + strModelTag).c_str(), &isHideMode))
			{
				eMode = MODE_HIDE;
			}	ImGui::SameLine();
			
			if (ImGui::RadioButton(string(string("Show ##CTool_MeshController::Show_MeshTags_HideMesh()") + strModelTag).c_str(), &isShowMode))
			{
				eMode = MODE_SHOW;
			}*/

			static _float2				vSize = { 200.f, 100.f };
			
			ImGui::SliderFloat2(string(string("ListBoxSize ##CTool_MeshController::Show_MeshTags_HideMesh()") + strModelTag).c_str(), (_float*)&vSize, 100.f, 400.f);
			
			if (ImGui::BeginListBox(string(string("Mesh Tags ##CTool_MeshController::Show_MeshTags_HideMesh()") + strModelTag).c_str(), *(ImVec2*)&vSize))
			{
				for (auto& strMeshTag : MeshTags)
				{
					if (ImGui::Selectable(strMeshTag.c_str()))
					{
						_bool			isHideMesh = { Find_Model(strModelTag)->Is_Hide_Mesh(strMeshTag) };

						if(true == isHideMesh)
							Show_Mesh(strModelTag, strMeshTag);

						else
							Hide_Mesh(strModelTag, strMeshTag);
					}
				}

				ImGui::EndListBox();
			}
		}

	}
}

void CTool_MeshController::Show_Mesh_Group()
{
}

HRESULT CTool_MeshController::Set_Models(map<string, CModel*> Models)
{
	for (auto& Pair : m_Models)
	{
		Safe_Release(Pair.second);
		Pair.second = nullptr;
	}
	m_Models.clear();

	m_Models = Models;
	for (auto& Pair : m_Models)
	{
		Safe_AddRef(Pair.second);
	}

	return S_OK;
}

HRESULT CTool_MeshController::Add_Model(string strModelTag, CModel* pModel)
{
	if (nullptr == pModel)
		return E_FAIL;

	CModel*			pFindedModel = { Find_Model(strModelTag) };
	if (nullptr != pFindedModel)
		return E_FAIL;

	m_Models.emplace(strModelTag, pModel);
	Safe_AddRef(pModel);

	return S_OK;
}

CModel* CTool_MeshController::Find_Model(string strModelTag)
{
	map<string, CModel*>::iterator			iter = { m_Models.find(strModelTag) };
	if (iter == m_Models.end())
		return nullptr;

	return iter->second;
}

string CTool_MeshController::Find_ModelTag(CModel* pModel)
{
	string			strModelTag = { "" };
	for (auto& Pair : m_Models)
	{
		if (Pair.second == pModel)
		{
			strModelTag = Pair.first;
			break;
		}
	}

	return strModelTag;
}

void CTool_MeshController::Hide_Mesh(string strModelTag, string strMeshTag)
{
	CModel*			pModel = { Find_Model(strModelTag) };
	if (nullptr == pModel)
		return;

	pModel->Hide_Mesh(strMeshTag, true);
}

void CTool_MeshController::Show_Mesh(string strModelTag, string strMeshTag)
{
	CModel* pModel = { Find_Model(strModelTag) };
	if (nullptr == pModel)
		return;

	pModel->Hide_Mesh(strMeshTag, false);
}

void CTool_MeshController::Hide_Mesh_All(string strModelTag)
{
	CModel*			pModel = { Find_Model(strModelTag) };
	if (nullptr == pModel)
		return;

	_uint			iNumMesh = { pModel->Get_NumMeshes() };
	for (_uint i = 0; i < iNumMesh; ++i)
	{
		pModel->Hide_Mesh(i, true);
	}
}

void CTool_MeshController::Show_Mesh_All(string strModelTag)
{
	CModel* pModel = { Find_Model(strModelTag) };
	if (nullptr == pModel)
		return;

	_uint			iNumMesh = { pModel->Get_NumMeshes() };
	for (_uint i = 0; i < iNumMesh; ++i)
	{
		pModel->Hide_Mesh(i, false);
	}
}

CTool_MeshController* CTool_MeshController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTool_MeshController*			pInatnace = { new CTool_MeshController(pDevice, pContext) };

	if (FAILED(pInatnace->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTool_MeshController"));

		Safe_Release(pInatnace);
	}

	return pInatnace;
}

void CTool_MeshController::Free()
{
	__super::Free();

	for (auto& Pair : m_Models)
	{
		Safe_Release(Pair.second);
		Pair.second = nullptr;
	}
	m_Models.clear();
}

