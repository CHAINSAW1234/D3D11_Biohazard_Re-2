#pragma once

#include "Tool_Defines.h"
#include "Tool_Selector.h"

BEGIN(Tool)

class CModel_Selector final : public CTool_Selector
{
public:
	typedef struct tagModelSelectorDesc
	{
		string* pCurrentModelTag = { nullptr };
		string* pCurrentBoneTag = { nullptr };
		string* pCurrentRootBoneTag = { nullptr };
	}MODELSELECTOR_DESC;

private:
	CModel_Selector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CModel_Selector() = default;

public:
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;

private:
	HRESULT						Add_Components();

private:
	void						On_Off_Buttons();

public:		/* For.Acces */
	CModel* Get_Model(const string& strModelTag);
	string						Get_Model_Tag(CModel* pModel);
	map<string, CModel*>		Get_Models() { return m_Models; }
	map<string, CModel*>* Get_Models_Ptr() { return &m_Models; }
	CModel* Get_CurrentSelectedModel();
	map<string, _float4x4>		Get_BoneCombinedMatrices();
	_float4x4* Get_Selected_BoneCombinedMatrix_Ptr();

public:		/* For.SHowList */
	void						Show_Default();
	void						Show_BoneTags();
	void						Show_ModelTags();

private:	/* For.Bone */
	void						Select_Bone();
	void						Set_RootBone();

private:	/* For.Model */
	void						Select_Model();

private:
	_bool						Check_BoneExist_CurrentModel(const string& strBoneTag);
	_bool						Check_BoneExist(const string& strModelTag, const string& strBoneTag);
	_bool						Check_ModelExist(const string& strModelTag);

	string						Find_RootBoneTag();

private:
	_uint						m_iNumModels = { 0 };
	map<string, CModel*>		m_Models;

	string* m_pCurrentModelTag = { nullptr };
	string* m_pCurrentBoneTag = { nullptr };
	string* m_pCurrentRootBoneTag = { nullptr };

	_bool						m_isShowModelTags = { false };
	_bool						m_isShowBoneTags = { false };

public:
	static CModel_Selector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END