#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

BEGIN(Tool)

class CTool_MeshController final : public CTool
{
public:
	typedef struct tagMeshControllerDesc
	{
	}MESHCONTROLLER_DESC;

private:
	CTool_MeshController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_MeshController() = default;

public:
	virtual HRESULT								Initialize(void* pArg) override;
	virtual void								Tick(_float fTimeDelta) override;

private:
	void										Input_Text();

	void										Show_MeshTags_HideMesh();
	void										Show_Mesh_Group();

public:
	HRESULT										Set_Models(map<string, CModel*> Models);
	HRESULT										Add_Model(string strModelTag, CModel* pModel);

private:
	CModel*										Find_Model(string strModelTag);
	string										Find_ModelTag(CModel* pModel);

private:
	void										Hide_Mesh(string strModelTag, string strMeshTag);
	void										Show_Mesh(string strModelTag, string strMeshTag);

	void										Hide_Mesh_All(string strModelTag);
	void										Show_Mesh_All(string strModelTag);

	void										Hide_Mesh_Group(string strModelTag, wstring strGroupTag);
	void										Show_Mesh_Group(string strModelTag, wstring strGroupTag);
	//	그룹태그, 메쉬의 그룹내에서의 인덱스		=> 그룹내에서 하나만 활성화하고 전부 숨기는 기능
	void										Active_Mesh_InGroup(wstring strGroupTag, _uint iGroupedIndex);

private:
	void										Create_Group(wstring strGroupTag, vector<string> MeshTags);
	void										Add_Group(wstring strGroupTag, string strMeshTag);
	void										Erase_Group(wstring strGroupTag);
	void										Erase_Group_Element(wstring strGroupTag, string strMeshTag);
	void										Erase_Group_Element(wstring strGroupTag, _uint iMeshIndex);

private:
	map<string, CModel*>						m_Models;

	wstring										m_strCurrentInputGroupTag = { TEXT("") };

	//	레이어 태그, vector< 메쉬 태그들 >		=> 그룹 내에서 0번 켜 1번 꺼 이런식으로하나씩 키기? 
	map<string, vector<string>>					m_MeshLayer;

public:
	static CTool_MeshController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END