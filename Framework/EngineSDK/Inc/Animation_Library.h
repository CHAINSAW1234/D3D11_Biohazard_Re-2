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
	//	파일 패스를 프로토타입 태그로 관리하기
	HRESULT											Add_Prototypes_Animation(const wstring& strAnimLayerTag, const string& strDirPath);

	HRESULT											Clone_Animation(const wstring& strAnimLayerTag, _uint iAnimIndex, class CAnimation** ppAnimation);
	
	const vector<class CAnimation*>&				Get_Animation_PrototypeLayer(const wstring& strAnimLayerTag);
	class CAnimation*								Get_Animation_Prototype(const wstring& strAnimLayerTag, _uint iIndex);
	_uint											Get_NumAnim_Prototypes(const wstring& strAnimLayerTag);

	list<wstring>									Get_AnimationLayer_Tags();

private:
	vector<class CAnimation*>&						Find_AnimLayer(const wstring& strAnimLayerTag);

	_bool											Is_Exist_AnimLayer(const wstring& strAnimLayerTag);

private:
	map<wstring, vector<class CAnimation*>>			m_AnimLayers;

	class CGameInstance*							m_pGameInstance = { nullptr };

public:
	static CAnimation_Library* Create();
	virtual void Free() override;
};

END