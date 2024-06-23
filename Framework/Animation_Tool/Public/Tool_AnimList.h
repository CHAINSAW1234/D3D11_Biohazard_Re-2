#pragma once

#include "Tool_Defines.h"
#include "Tool_Selector.h"

BEGIN(Engine)
class CAnimation;
END

BEGIN(Tool)

class CTool_AnimList : public CTool_Selector
{
public:
	//	키값으로 모델 태그 , 밸류로 map<string, CAnimation*> 구조로 변경하기
	typedef map<string, CAnimation*>			Animations;
	typedef map<string, Animations>				AnimationLayers;

public:
	typedef struct tagAnimListDesc
	{
		const string*							pCurrentModelTag = { nullptr };
		string*									pCurrentAnimationTag = { nullptr };
		wstring*								pCurrentAnimLayerTag = { nullptr };
	}ANIMLIST_DESC;

private:
	CTool_AnimList(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_AnimList() = default;

public:
	virtual HRESULT								Initialize(void* pArg) override;
	virtual void								Tick(_float fTimeDelta) override;

public:
	void										Set_CurrentModel(CModel* pModel);

private:	/* For.ShowList */
	void										Show_Default();
	void										Show_LayerTags();
	void										Show_AnimationTags();

public:
	CAnimation*									Get_CurrentAnimation();

private:
	//	모델 태그 => 애님 레이어 태그 => 애니메이션 태그 => Animation

	CModel*										m_pCurrentModel = { nullptr };

	const string*								m_pCurrentModelTag = { nullptr };
	wstring*									m_pCurrentAnimLayerTag = { nullptr };
	string*										m_pCurrentAnimTag = { nullptr };

public:
	static CTool_AnimList* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free();
};

END