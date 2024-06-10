#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

BEGIN(Tool)

class CTool_BoneLayer final : public CTool
{
public:
	typedef struct tagBoneLayerDesc
	{
		const string*				pCurrentBoneTag = { nullptr };
		const string*				pCurrentModelTag = { nullptr };
		wstring*					pCurrentBoneLayerTag = { nullptr };
	}BONELAYER_DESC;

private:
	CTool_BoneLayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_BoneLayer() = default;

public:
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;

private:
	void							Input_LayerTag();
	void							Input_Start_End_BoneIndex();
	void							Input_TopParent_BoneIndex();
	void							Create_BoneLayer();

public:
	void							Show_BoneLayers();

public:
	void							Set_Current_Model(CModel* pModel);

private:
	void							Show_Default();

private:
	void							Release_BoneLayer(const wstring& strBoneLayerTag);	
	void							Release_BoneLayer_CurrentBoneLayer();

	list<wstring>					Get_BoneLayerTags(CModel* pModel);

private:
	void							Add_AnimLayer_AllBone(CModel* pModel);
	void							Add_AnimLayer_ChildBones(CModel* pModel);
	void							Add_AnimLayer_Range(CModel* pModel);

private:
	//	레이어 생성할 시 레이어 태그로 사용될 태그 입력란
	wstring							m_strInputLayerTag = { TEXT("") };

	wstring*						m_pCurrentBoneLayerTag = { nullptr };
	const string*					m_pCurrentBoneTag = { nullptr };
	const string*					m_pCurrentModelTag = { nullptr };

	_int							m_iTopParentBoneIndex = { 0 };
	_int							m_iStartBoneIndex = { 0 };
	_int							m_iEndBoneIndex = { 0 };

	CModel*							m_pCurrentModel = { nullptr };

public:
	static CTool_BoneLayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END