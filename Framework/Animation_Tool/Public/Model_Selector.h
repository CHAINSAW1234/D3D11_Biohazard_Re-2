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
	CModel_Selector();
	virtual ~CModel_Selector() = default;

public:
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;

private:
	HRESULT						Add_Componets();

public:
	CModel*						Get_Model(const string& strModelTag) { return m_Models[strModelTag]; }
	map<string, _float4x4>		Get_BoneCombinedMatrices();

private:
	_uint						m_iNumModels = { 0 };
	map<string, CModel*>		m_Models;

	string						m_strSelectedModelTag = { "" };

public:
	static CModel_Selector* Create(void* pArg);
	virtual void Free() override;
};

END