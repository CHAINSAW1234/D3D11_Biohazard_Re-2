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
		string*										pCurrentAnimTag = { nullptr };
		string*										pCurrentModelTag = { nullptr };
		/*wstring* pCurrentBoneLayerTag = {nullptr}; */
		wstring*									pCurrentPartTag = { nullptr };
		CGameObject*								pTestObject = { nullptr };
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
	void											Show_LinkBone();
	
private:
	void											Link_Bone_Auto(const wstring& strSrcPartTag, const wstring& strDstPartTag);
	void											Link_Bone_Manual(const wstring& strSrcPartTag, const wstring& strDstPartTag, const string& strSrcBoneTag, const string& strDstBoneTag);
	void											UnLink_Bone_All(const wstring& strPartTag);
	void											UnLink_Bone(const wstring& strPartTag, const string& strBoneTag);

	void											Update_ModelTag();

public:
	void											Set_CurrentAnimation(CAnimation* pAnimation);
	HRESULT											Set_Models(map<string, CModel*>* pModels);

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
	map<string, CModel*>*							m_pModels = { nullptr };				//	모델 셀렉터에서 미리 지워버리면 문제 발생하므로 참조시 어떻게 풀어나갈지...
	
	wstring*										m_pCurrentPartTag = { nullptr };
	string*											m_pCurrentModelTag = { nullptr };

public:
	static CTool_PartObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END