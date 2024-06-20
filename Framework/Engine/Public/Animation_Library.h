#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation_Library final : public CBase
{
private:
	CAnimation_Library();
	virtual ~CAnimation_Library() = default;

public:
	HRESULT											Initialize();

public:
	HRESULT											Add_Prototype_Animation(const wstring& strAnimLayerTag, const wstring& strPrototypeTag);
	
	const vector<class CAnimation*>&				Get_Animation_PrototypeLayer(const wstring& strAnimLayerTag);
	class CAnimation*								Get_Animation_Prototype(const wstring& strAnimLayerTag, _uint iIndex);
	_uint											Get_NumAnim_Prototypes(const wstring& strAnimLayerTag);

	list<wstring>									Get_AnimationLayer_Tags();

private:
	vector<class CAnimation*>&						Find_AnimLayer(const wstring& strAnimLayerTag);

private:
	map<wstring, vector<class CAnimation*>>			m_AnimLayers;

public:
	static CAnimation_Library* Create();
	virtual void Free() override;
};

END