#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CEasing final : public CBase
{
private:
	CEasing();
	virtual ~CEasing() = default;

public:
    HRESULT	Initialize();

public:
    _float Get_Ease(EASING_TYPE eEase, _float fStartValue, _float fEndValue, _float fRatio);

private:
    _float Find_Ease(EASING_TYPE eEase, _float fStartValue, _float fEndValue, _float fRatio);

private:
    _float EaseInQuad(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseOutQuad(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInOutQuad(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInCubic(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseOutCubic(_float fStartValue, _float fEndValue, _float fRatio);

    _float EaseInOutCubic(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInQuart(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseOutQuart(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInOutQuart(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInQuint(_float fStartValue, _float fEndValue, _float fRatio);

    _float EaseOutQuint(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInOutQuint(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInSine(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseOutSine(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInOutSine(_float fStartValue, _float fEndValue, _float fRatio);

    _float EaseInExpo(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseOutExpo(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInOutExpo(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInCirc(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseOutCirc(_float fStartValue, _float fEndValue, _float fRatio);

    _float EaseInOutCirc(_float fStartValue, _float fEndValue, _float fRatio);
    _float Linear(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInBounce(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseOutBounce(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInOutBounce(_float fStartValue, _float fEndValue, _float fRatio);

    _float EaseInBack(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseOutBack(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInOutBack(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseInElastic(_float fStartValue, _float fEndValue, _float fRatio);
    _float EaseOutElastic(_float fStartValue, _float fEndValue, _float fRatio);

    _float EaseInOutElastic(_float fStartValue, _float fEndValue, _float fRatio);

public:
    static	CEasing* Create();
	virtual void Free() override;
};

END