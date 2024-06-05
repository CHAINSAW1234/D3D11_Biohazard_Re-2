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
		wstring*					pCurrentPartTag = { nullptr };
		CGameObject*				pTestObject = { nullptr };
	}TOOL_PARTOBJECT_DESC;

private:
	CTool_PartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_PartObject() = default;

public:
	virtual HRESULT									Initialize(void* pArg) override;
	virtual void									Tick(_float fTimeDelta) override;

private:
	void											Input_PartObjectTag();
	void											Create_Release_PartObject();

private:
	void											Show_Default();
	void											Show_PartObject_Tags();

private:
	void											Add_PartObject();
	void											Release_PartObject(const wstring& strPartTag);

private:
	void											On_Off_Buttons();

private:
	
private:
	void											Link_Bone(const wstring& strSrcPartTag, const wstring& strDstPartTag, const string& strSrcBoneTag, const string& strDstBoneTag);
	void											UnLink_Bone(const wstring& strPartTag, const string& strBoneTag);

public:
	void											Set_CurrentAnimation(CAnimation* pAnimation);

private:
	_bool											Check_PartObjectExist(const wstring& strPartTag);

public:
	vector<string>									Get_CurrentPartObject_BoneTags();
	class CAnimTestPartObject*						Get_CurrentPartObject();

private:
	CAnimation*										m_pCurrentAnimation = { nullptr };

	wstring											m_strInputPartObjectTag = { TEXT("") };

	CGameObject*									m_pTestObject = { nullptr };
	map<wstring, class CAnimTestPartObject*>		m_PartObjects;
	
	wstring*										m_pCurrentPartTag = { nullptr };

public:
	static CTool_PartObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END