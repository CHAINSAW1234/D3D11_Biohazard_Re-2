#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation_Layer final : public CBase
{
private:
	CAnimation_Layer();
	virtual ~CAnimation_Layer() = default;

public:
	HRESULT										Initialize();

public:
	HRESULT										Add_Animation(class CAnimation* pAnimation);

public:
	inline class CAnimation*					Get_Animation(_uint iAnimIndex) {
		CAnimation*			pAnimation = { nullptr };
		_uint				iNumAnims = { Get_NumAnims() };
		if (iAnimIndex < iNumAnims)
		{
			pAnimation = m_Animations[iAnimIndex];
		}

		return pAnimation;
	}
	inline const vector<class CAnimation*>&		Get_Animations() { return m_Animations; }
	inline _uint								Get_NumAnims() { return static_cast<_uint>(m_Animations.size()); }

private:
	vector<class CAnimation*>					m_Animations;

public:
	static CAnimation_Layer* Create();
	virtual void Free() override;
};

END