#include "Easing.h"

CEasing::CEasing()
{
}

HRESULT CEasing::Initialize()
{
	return S_OK;
}

_float CEasing::Get_Ease(EASING_TYPE eEase, _float fStartValue, _float fEndValue, _float fRatio)
{
	return Find_Ease(eEase, fStartValue, fEndValue, fRatio);
}

_float3 CEasing::Get_Ease(EASING_TYPE eEase, _float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return Find_Ease(eEase, fStartValue, fEndValue, fRatio);
}

_float CEasing::EaseInQuad(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	return fEndValue * fRatio * fRatio + fStartValue;
}

_float CEasing::EaseOutQuad(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	return -fEndValue * fRatio * (fRatio - 2) + fStartValue;
}

_float CEasing::EaseInOutQuad(_float fStartValue, _float fEndValue, _float fRatio)
{
	fRatio /= .5f;
	fEndValue -= fStartValue;
	if (fRatio < 1) return fEndValue * 0.5f * fRatio * fRatio + fStartValue;
	fRatio--;
	return -fEndValue * 0.5f * (fRatio * (fRatio - 2) - 1) + fStartValue;
}

_float CEasing::EaseInCubic(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	return fEndValue * fRatio * fRatio * fRatio + fStartValue;
}

_float CEasing::EaseOutCubic(_float fStartValue, _float fEndValue, _float fRatio)
{
	fRatio--;
	fEndValue -= fStartValue;
	return fEndValue * (fRatio * fRatio * fRatio + 1) + fStartValue;
}

_float CEasing::EaseInOutCubic(_float fStartValue, _float fEndValue, _float fRatio)
{
	fRatio /= .5f;
	fEndValue -= fStartValue;
	if (fRatio < 1) return fEndValue * 0.5f * fRatio * fRatio * fRatio + fStartValue;
	fRatio -= 2;
	return fEndValue * 0.5f * (fRatio * fRatio * fRatio + 2) + fStartValue;
}

_float CEasing::EaseInQuart(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	return fEndValue * fRatio * fRatio * fRatio * fRatio + fStartValue;
}

_float CEasing::EaseOutQuart(_float fStartValue, _float fEndValue, _float fRatio)
{
	fRatio--;
	fEndValue -= fStartValue;
	return -fEndValue * (fRatio * fRatio * fRatio * fRatio - 1) + fStartValue;
}

_float CEasing::EaseInOutQuart(_float fStartValue, _float fEndValue, _float fRatio)
{
	fRatio /= .5f;
	fEndValue -= fStartValue;
	if (fRatio < 1) return fEndValue * 0.5f * fRatio * fRatio * fRatio * fRatio + fStartValue;
	fRatio -= 2;
	return -fEndValue * 0.5f * (fRatio * fRatio * fRatio * fRatio - 2) + fStartValue;
}

_float CEasing::EaseInQuint(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	return fEndValue * fRatio * fRatio * fRatio * fRatio * fRatio + fStartValue;
}

_float CEasing::EaseOutQuint(_float fStartValue, _float fEndValue, _float fRatio)
{
	fRatio--;
	fEndValue -= fStartValue;
	return fEndValue * (fRatio * fRatio * fRatio * fRatio * fRatio + 1) + fStartValue;
}

_float CEasing::EaseInOutQuint(_float fStartValue, _float fEndValue, _float fRatio)
{
	fRatio /= .5f;
	fEndValue -= fStartValue;
	if (fRatio < 1) return fEndValue * 0.5f * fRatio * fRatio * fRatio * fRatio * fRatio + fStartValue;
	fRatio -= 2;
	return fEndValue * 0.5f * (fRatio * fRatio * fRatio * fRatio * fRatio + 2) + fStartValue;
}

_float CEasing::EaseInSine(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	return -fEndValue * cosf(fRatio * (XM_PI * 0.5f)) + fEndValue + fStartValue;
}

_float CEasing::EaseOutSine(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	return fEndValue * sinf(fRatio * (XM_PI * 0.5f)) + fStartValue;
}

_float CEasing::EaseInOutSine(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	return -fEndValue * 0.5f * (cosf(XM_PI * fRatio) - 1) + fStartValue;
}

_float CEasing::EaseInExpo(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	return fEndValue * pow(2, 10 * (fRatio - 1)) + fStartValue;
}

_float CEasing::EaseOutExpo(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	return fEndValue * (-pow(2, -10 * fRatio) + 1) + fStartValue;
}

_float CEasing::EaseInOutExpo(_float fStartValue, _float fEndValue, _float fRatio)
{
	fRatio /= .5f;
	fEndValue -= fStartValue;
	if (fRatio < 1) return fEndValue * 0.5f * pow(2, 10 * (fRatio - 1)) + fStartValue;
	fRatio--;
	return fEndValue * 0.5f * (-pow(2, -10 * fRatio) + 2) + fStartValue;
}

_float CEasing::EaseInCirc(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	return -fEndValue * (sqrt(1 - fRatio * fRatio) - 1) + fStartValue;
}

_float CEasing::EaseOutCirc(_float fStartValue, _float fEndValue, _float fRatio)
{
	fRatio--;
	fEndValue -= fStartValue;
	return fEndValue * sqrt(1 - fRatio * fRatio) + fStartValue;
}

_float CEasing::EaseInOutCirc(_float fStartValue, _float fEndValue, _float fRatio)
{
	fRatio /= .5f;
	fEndValue -= fStartValue;
	if (fRatio < 1) return -fEndValue * 0.5f * (sqrt(1 - fRatio * fRatio) - 1) + fStartValue;
	fRatio -= 2;
	return fEndValue * 0.5f * (sqrt(1 - fRatio * fRatio) + 1) + fStartValue;
}

_float CEasing::Linear(_float fStartValue, _float fEndValue, _float fRatio)
{
	_vector vStart = { fStartValue, fStartValue, fStartValue };
	_vector vEnd = { fEndValue, fEndValue, fEndValue };
	return XMVectorLerp(vStart, vEnd, fRatio).m128_f32[0];
}

_float CEasing::EaseInBounce(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	float d = 1.f;
	return fEndValue - EaseOutBounce(0, fEndValue, d - fRatio) + fStartValue;
}

_float CEasing::EaseOutBounce(_float fStartValue, _float fEndValue, _float fRatio)
{
	fRatio /= 1.f;
	fEndValue -= fStartValue;
	if (fRatio < (1 / 2.75f))
	{
		return fEndValue * (7.5625f * fRatio * fRatio) + fStartValue;
	}
	else if (fRatio < (2 / 2.75f))
	{
		fRatio -= (1.5f / 2.75f);
		return fEndValue * (7.5625f * (fRatio)*fRatio + .75f) + fStartValue;
	}
	else if (fRatio < (2.5 / 2.75))
	{
		fRatio -= (2.25f / 2.75f);
		return fEndValue * (7.5625f * (fRatio)*fRatio + .9375f) + fStartValue;
	}
	else
	{
		fRatio -= (2.625f / 2.75f);
		return fEndValue * (7.5625f * (fRatio)*fRatio + .984375f) + fStartValue;
	}
}

_float CEasing::EaseInOutBounce(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	float d = 1.f;
	if (fRatio < d * 0.5f) return EaseInBounce(0, fEndValue, fRatio * 2) * 0.5f + fStartValue;
	else return EaseOutBounce(0, fEndValue, fRatio * 2 - d) * 0.5f + fEndValue * 0.5f + fStartValue;
}

_float CEasing::EaseInBack(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;
	fRatio /= 1;
	float s = 1.70158f;
	return fEndValue * (fRatio)*fRatio * ((s + 1) * fRatio - s) + fStartValue;
}

_float CEasing::EaseOutBack(_float fStartValue, _float fEndValue, _float fRatio)
{
	float s = 1.70158f;
	fEndValue -= fStartValue;
	fRatio = (fRatio)-1;
	return fEndValue * ((fRatio)*fRatio * ((s + 1) * fRatio + s) + 1) + fStartValue;
}

_float CEasing::EaseInOutBack(_float fStartValue, _float fEndValue, _float fRatio)
{
	float s = 1.70158f;
	fEndValue -= fStartValue;
	fRatio /= .5f;
	if ((fRatio) < 1)
	{
		s *= (1.525f);
		return fEndValue * 0.5f * (fRatio * fRatio * (((s)+1) * fRatio - s)) + fStartValue;
	}
	fRatio -= 2;
	s *= (1.525f);
	return fEndValue * 0.5f * ((fRatio)*fRatio * (((s)+1) * fRatio + s) + 2) + fStartValue;
}

_float CEasing::EaseInElastic(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;

	float d = 1.f;
	float p = d * .3f;
	float s;
	float a = 0;

	if (fRatio == 0) return fStartValue;

	if ((fRatio /= d) == 1) return fStartValue + fEndValue;

	if (a == 0.f || a < abs(fEndValue))
	{
		a = fEndValue;
		s = p / 4;
	}
	else
	{
		s = p / (2 * XM_PI) * asinf(fEndValue / a);
	}

	return -(a * pow(2, 10 * (fRatio -= 1)) * sinf((fRatio * d - s) * (2 * XM_PI) / p)) + fStartValue;
}

_float CEasing::EaseOutElastic(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;

	float d = 1.f;
	float p = d * .3f;
	float s;
	float a = 0;

	if (fRatio == 0) return fStartValue;

	if ((fRatio /= d) == 1) return fStartValue + fEndValue;

	if (a == 0.f || a < abs(fEndValue))
	{
		a = fEndValue;
		s = p * 0.25f;
	}
	else
	{
		s = p / (2 * XM_PI) * asinf(fEndValue / a);
	}

	return (a * pow(2, -10 * fRatio) * sinf((fRatio * d - s) * (2 * XM_PI) / p) + fEndValue + fStartValue);
}

_float CEasing::EaseInOutElastic(_float fStartValue, _float fEndValue, _float fRatio)
{
	fEndValue -= fStartValue;

	_float d = 1.f;
	_float p = d * .3f;
	_float s;
	_float a = 0;

	if (fRatio == 0) return fStartValue;

	if ((fRatio /= d * 0.5f) == 2) return fStartValue + fEndValue;

	if (a == 0.f || a < abs(fEndValue))
	{
		a = fEndValue;
		s = p / 4;
	}
	else
	{
		s = p / (2 * XM_PI) * asinf(fEndValue / a);
	}

	if (fRatio < 1) return -0.5f * (a * pow(2, 10 * (fRatio -= 1)) * sinf((fRatio * d - s) * (2 * XM_PI) / p)) + fStartValue;
	return a * pow(2, -10 * (fRatio -= 1)) * sinf((fRatio * d - s) * (2 * XM_PI) / p) * 0.5f + fEndValue + fStartValue;
}




_float3 CEasing::EaseInQuad(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	fEndValue.x -= fStartValue.x;
	fEndValue.y -= fStartValue.y;
	fEndValue.z -= fStartValue.z;

	return fEndValue * fRatio * fRatio + fStartValue;
}

_float3 CEasing::EaseOutQuad(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	fEndValue.x -= fStartValue.x;
	fEndValue.y -= fStartValue.y;
	fEndValue.z -= fStartValue.z;

	return -fEndValue * fRatio * (fRatio - 2) + fStartValue;
}

_float3 CEasing::EaseInOutQuad(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInCubic(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseOutCubic(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInOutCubic(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInQuart(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseOutQuart(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInOutQuart(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInQuint(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseOutQuint(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInOutQuint(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInSine(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseOutSine(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInOutSine(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInExpo(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseOutExpo(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInOutExpo(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInCirc(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseOutCirc(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInOutCirc(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::Linear(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInBounce(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseOutBounce(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInOutBounce(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInBack(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseOutBack(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInOutBack(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInElastic(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseOutElastic(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float3 CEasing::EaseInOutElastic(_float3 fStartValue, _float3 fEndValue, _float fRatio)
{
	return _float3();
}

_float CEasing::Find_Ease(EASING_TYPE eEase, _float fStart, _float fEnd, _float fValue)
{
	switch (eEase)
	{
	case Ease_InQuad:
		return EaseInQuad(fStart, fEnd, fValue);
	case Ease_OutQuad:
		return EaseOutQuad(fStart, fEnd, fValue);
	case Ease_InOutQuad:
		return EaseInOutQuad(fStart, fEnd, fValue);
	case Ease_InCubic:
		return EaseInCubic(fStart, fEnd, fValue);
	case Ease_OutCubic:
		return EaseOutCubic(fStart, fEnd, fValue);
	case Ease_InOutCubic:
		return EaseInOutCubic(fStart, fEnd, fValue);
	case Ease_InQuart:
		return EaseInQuart(fStart, fEnd, fValue);
	case Ease_OutQuart:
		return EaseOutQuart(fStart, fEnd, fValue);
	case Ease_InOutQuart:
		return EaseInOutQuart(fStart, fEnd, fValue);
	case Ease_InQuint:
		return EaseInQuint(fStart, fEnd, fValue);
	case Ease_OutQuint:
		return EaseOutQuint(fStart, fEnd, fValue);
	case Ease_InOutQuint:
		return EaseInOutQuint(fStart, fEnd, fValue);
	case Ease_InSine:
		return EaseInSine(fStart, fEnd, fValue);
	case Ease_OutSine:
		return EaseOutSine(fStart, fEnd, fValue);
	case Ease_InOutSine:
		return EaseInOutSine(fStart, fEnd, fValue);
	case Ease_InExpo:
		return EaseInExpo(fStart, fEnd, fValue);
	case Ease_OutExpo:
		return EaseOutExpo(fStart, fEnd, fValue);
	case Ease_InOutExpo:
		return EaseInOutExpo(fStart, fEnd, fValue);
	case Ease_InCirc:
		return EaseInCirc(fStart, fEnd, fValue);
	case Ease_OutCirc:
		return EaseOutCirc(fStart, fEnd, fValue);
	case Ease_InOutCirc:
		return EaseInOutCirc(fStart, fEnd, fValue);
	case Ease_Linear:
		return Linear(fStart, fEnd, fValue);
	case Ease_InBounce:
		return EaseInBounce(fStart, fEnd, fValue);
	case Ease_OutBounce:
		return EaseOutBounce(fStart, fEnd, fValue);
	case Ease_InOutBounce:
		return EaseInOutBounce(fStart, fEnd, fValue);
	case Ease_InBack:
		return EaseInBack(fStart, fEnd, fValue);
	case Ease_OutBack:
		return EaseOutBack(fStart, fEnd, fValue);
	case Ease_InOutBack:
		return EaseInOutBack(fStart, fEnd, fValue);
	case Ease_InElastic:
		return EaseInElastic(fStart, fEnd, fValue);
	case Ease_OutElastic:
		return EaseOutElastic(fStart, fEnd, fValue);
	case Ease_InOutElastic:
		return EaseInOutElastic(fStart, fEnd, fValue);
	case Ease_END:
		break;
	default:
		break;
	}

	return 1000.f;
}

_float3 CEasing::Find_Ease(EASING_TYPE eEase, _float3 fStart, _float3 fEnd, _float fValue)
{
	switch (eEase)
	{
	case Ease_InQuad:
		return EaseInQuad(fStart, fEnd, fValue);
	case Ease_OutQuad:
		return EaseOutQuad(fStart, fEnd, fValue);
	case Ease_InOutQuad:
		return EaseInOutQuad(fStart, fEnd, fValue);
	case Ease_InCubic:
		return EaseInCubic(fStart, fEnd, fValue);
	case Ease_OutCubic:
		return EaseOutCubic(fStart, fEnd, fValue);
	case Ease_InOutCubic:
		return EaseInOutCubic(fStart, fEnd, fValue);
	case Ease_InQuart:
		return EaseInQuart(fStart, fEnd, fValue);
	case Ease_OutQuart:
		return EaseOutQuart(fStart, fEnd, fValue);
	case Ease_InOutQuart:
		return EaseInOutQuart(fStart, fEnd, fValue);
	case Ease_InQuint:
		return EaseInQuint(fStart, fEnd, fValue);
	case Ease_OutQuint:
		return EaseOutQuint(fStart, fEnd, fValue);
	case Ease_InOutQuint:
		return EaseInOutQuint(fStart, fEnd, fValue);
	case Ease_InSine:
		return EaseInSine(fStart, fEnd, fValue);
	case Ease_OutSine:
		return EaseOutSine(fStart, fEnd, fValue);
	case Ease_InOutSine:
		return EaseInOutSine(fStart, fEnd, fValue);
	case Ease_InExpo:
		return EaseInExpo(fStart, fEnd, fValue);
	case Ease_OutExpo:
		return EaseOutExpo(fStart, fEnd, fValue);
	case Ease_InOutExpo:
		return EaseInOutExpo(fStart, fEnd, fValue);
	case Ease_InCirc:
		return EaseInCirc(fStart, fEnd, fValue);
	case Ease_OutCirc:
		return EaseOutCirc(fStart, fEnd, fValue);
	case Ease_InOutCirc:
		return EaseInOutCirc(fStart, fEnd, fValue);
	case Ease_Linear:
		return Linear(fStart, fEnd, fValue);
	case Ease_InBounce:
		return EaseInBounce(fStart, fEnd, fValue);
	case Ease_OutBounce:
		return EaseOutBounce(fStart, fEnd, fValue);
	case Ease_InOutBounce:
		return EaseInOutBounce(fStart, fEnd, fValue);
	case Ease_InBack:
		return EaseInBack(fStart, fEnd, fValue);
	case Ease_OutBack:
		return EaseOutBack(fStart, fEnd, fValue);
	case Ease_InOutBack:
		return EaseInOutBack(fStart, fEnd, fValue);
	case Ease_InElastic:
		return EaseInElastic(fStart, fEnd, fValue);
	case Ease_OutElastic:
		return EaseOutElastic(fStart, fEnd, fValue);
	case Ease_InOutElastic:
		return EaseInOutElastic(fStart, fEnd, fValue);
	case Ease_END:
		break;
	default:
		break;
	}

	return _float3(1000.f, 1000.f, 1000.f);
}

CEasing* CEasing::Create()
{
	CEasing* pInstance = new CEasing();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CEasing"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEasing::Free()
{
}
