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
    _float3 Get_Ease(EASING_TYPE eEase, _float3 fStartValue, _float3 fEndValue, _float fRatio);

private:
    _float Find_Ease(EASING_TYPE eEase, _float fStartValue, _float fEndValue, _float fRatio);
    _float3 Find_Ease(EASING_TYPE eEase, _float3 fStartValue, _float3 fEndValue, _float fRatio);

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


private:
    _float3 EaseInQuad(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseOutQuad(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInOutQuad(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInCubic(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseOutCubic(_float3 fStartValue, _float3 fEndValue, _float fRatio);

    _float3 EaseInOutCubic(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInQuart(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseOutQuart(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInOutQuart(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInQuint(_float3 fStartValue, _float3 fEndValue, _float fRatio);

    _float3 EaseOutQuint(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInOutQuint(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInSine(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseOutSine(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInOutSine(_float3 fStartValue, _float3 fEndValue, _float fRatio);

    _float3 EaseInExpo(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseOutExpo(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInOutExpo(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInCirc(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseOutCirc(_float3 fStartValue, _float3 fEndValue, _float fRatio);

    _float3 EaseInOutCirc(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 Linear(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInBounce(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseOutBounce(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInOutBounce(_float3 fStartValue, _float3 fEndValue, _float fRatio);

    _float3 EaseInBack(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseOutBack(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInOutBack(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseInElastic(_float3 fStartValue, _float3 fEndValue, _float fRatio);
    _float3 EaseOutElastic(_float3 fStartValue, _float3 fEndValue, _float fRatio);

    _float3 EaseInOutElastic(_float3 fStartValue, _float3 fEndValue, _float fRatio);





public:
    static	CEasing* Create();
	virtual void Free() override;
};

END