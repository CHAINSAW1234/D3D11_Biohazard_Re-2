#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

BEGIN(Tool)

class CTool_BoneLayer final : public CTool
{
public:
	typedef struct tagBoneLayerDesc
	{
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

public:
	void							Show_BoneLayers();

public:
	void							Set_CurrentModel(CModel* pModel);

private:
	void							Show_Default();

public:
	void							Create_AnimLayer_AllBone(CModel* pModel);
	void							Create_AnimLayer_ChildBones(CModel* pModel, const string& strTopParentBoneTag);
	void							Create_AnimLayer_Indices(CModel* pModel, list<_uint> BoneIndices);

	list<wstring>					Get_BoneLayerTags(CModel* pModel);

private:
	void							Add_AnimLayer(CModel* pModel, list<_uint> BoneIndices);
	void							Add_AnimLayer_AllBone(CModel* pModel);

private:
	//	레이어 생성할 시 레이어 태그로 사용될 태그 입력란
	wstring							m_strInputLayerTag = { TEXT("") };

	wstring*						m_pCurrentBoneLayerTag = { nullptr };
	const string*					m_pCurrentModelTag = { nullptr };

	CModel*							m_pCurrentModel = { nullptr };

public:
	static CTool_BoneLayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END