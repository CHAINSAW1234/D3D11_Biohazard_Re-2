#pragma once

#include "Tool_Defines.h"
#include "Tool_Selector.h"

BEGIN(Tool)

class CModel_Selector final :  public CTool_Selector
{
public:
	typedef struct tagModelSelectorDesc
	{
		list<CModel*>		Models;
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
	void						Select_Model();
	void						Select_Bone();

public:
	CModel*						Get_Model(const string& strModelTag) { return m_Models[strModelTag]; }
	map<string, CModel*>		Get_Models() { return m_Models; }
	CModel*						Get_CurrentSelectedModel();
	string						Get_CurrentSelectedModelTag() { return m_strSelectedModelTag; }
	string						Get_CurrentSelectedBoneTag() { return m_strSelectedBoneTag; }
	map<string, _float4x4>		Get_BoneCombinedMatrices();
	_float4x4*					Get_Selected_BoneCombinedMatrix_Ptr();

private:
	void						Set_RootBone();

private:
	_uint						m_iNumModels = { 0 };
	map<string, CModel*>		m_Models;
	string						m_strSelectedModelTag = { "" };
	string						m_strSelectedBoneTag = { "" };

	_bool						m_isShowBoneTags = { false };

public:
	static CModel_Selector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END