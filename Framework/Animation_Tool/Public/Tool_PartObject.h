#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

BEGIN(Engine)
class CGameObject;
class CPartObject;
END

BEGIN(Tool)

class CTool_PartObject final : public CTool
{
public:
	typedef struct tagToolPartObjectDesc
	{
		CGameObject*				pTestObject = { nullptr };
	}TOOL_PARTOBJECT_DESC;

private:
	CTool_PartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_PartObject() = default;

public:
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;

private:
	void							On_Off_Buttons();

private:
	void							Update_PartObjects();

	void							Show_PartObject_Tags();
	
private:
	void							Link_Bone(const wstring& strSrcPartTag, const wstring& strDstPartTag, const string& strSrcBoneTag, const string& strDstBoneTag);
	void							UnLink_Bone(const wstring& strPartTag, const string& strBoneTag);

public:
	vector<string>					Get_CurrentPartObject_BoneTags();

private:
	CGameObject*									m_pTestObject = { nullptr };
	map<wstring, class CAnimTestPartObject*>		m_PartObjects;

	_bool											m_isShowPartObjectTags = { false };
	wstring											m_strSelectPartObjectTag = { L"" };

public:
	static CTool_PartObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END