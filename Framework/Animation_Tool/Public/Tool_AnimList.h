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
	typedef struct tagAnimListDesc
	{
		const string*			pCurrentModelTag = { nullptr };
		string*					pCurrentAnimationTag = { nullptr };
	}ANIMLIST_DESC;

private:
	CTool_AnimList(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_AnimList() = default;

public:
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;

public:
	HRESULT						Add_Animations(map<string, map<string, CAnimation*>> ModelAnimations);

private:	/* For.SHowList */
	void						Show_Default();
	void						Show_AnimationTags();

public:
	CAnimation*					Get_Animation(const string& strAnimTag);
	map<string, map<string, CAnimation*>>*			Get_ModelAnimations_Ptr() { return &m_ModelAnimations; }
	CAnimation*					Get_CurrentAnimation();
	void						Add_Animation_CurrentModel(CAnimation* pAnimation);

private:
	_bool						Check_ModelExist(const string& strModelTag);
	_bool						Check_AnimExtist(const string& strAnimTag);

private:
	//	키값으로 모델 태그 , 밸류로 map<string, CAnimation*> 구조로 변경하기
	typedef map<string, CAnimation*> Animations;
	map<string, Animations>		m_ModelAnimations;

	const string*				m_pCurrentModelTag = { nullptr };
	string*						m_pCurrentAnimTag = { nullptr };

public:
	static CTool_AnimList* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free();
};

END