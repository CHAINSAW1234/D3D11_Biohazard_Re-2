#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

BEGIN(Tool)

class CTool_BoneLayer final : public CTool
{
private:
	CTool_BoneLayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_BoneLayer() = default;

public:
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;

private:
	void							Input_LayerTag();

public:
	void							Create_AnimLayer_AllBone(CModel* pModel);
	void							Create_AnimLayer_BoneChilds(CModel* pModel, const string& strTopParentBoneTag);
	void							Create_AnimLayer_Indices(CModel* pModel, list<_uint> BoneIndices);

	wstring							Get_CurrentLayerTag() { return m_strAnimLayerTag; }

	list<wstring>					Get_BoneLayerTags(CModel* pModel);

private:
	void							Add_AnimLayer(CModel* pModel, list<_uint> BoneIndices);
	void							Add_AnimLayer_AllBone(CModel* pModel);

private:
	wstring							m_strAnimLayerTag = { TEXT("") };

public:
	static CTool_BoneLayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END