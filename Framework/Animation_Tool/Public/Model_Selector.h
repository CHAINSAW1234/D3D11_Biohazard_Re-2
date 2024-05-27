#pragma once

#include "Tool_Defines.h"
#include "Tool_Selector.h"

BEGIN(Tool)

class CModel_Selector final :  public CTool_Selector
{
public:
	typedef struct tagModelInfo
	{
		CModel*			pModel = { nullptr };
		wstring			strPrototypeTag = { TEXT("") };
	}MODEL_INFO;

	typedef struct tagModelSelectorDesc
	{
		const vector<MODEL_INFO>*		pModelInfos = { nullptr };
		vector<string>					ModelTags;
		vector<CModel::MODEL_TYPE>		Types;
	}MODEL_SELECTOR_DESC;


private:
	CModel_Selector();
	virtual ~CModel_Selector() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	const MODEL_INFO& Get_CurrentModelInfo() const {
		return m_CurrentInfo;
	}

private:
	void Change_CurrentModelInfo(MODEL_INFO ModelInfo);

private:
	_uint						m_iNumModels = { 0 };
	vector<MODEL_INFO>			m_ModelInfos;

	string*						m_ModelTags = { nullptr };

	MODEL_INFO					m_CurrentInfo = {};

public:
	static CModel_Selector* Create(void* pArg);
	virtual void Free() override;
};

END