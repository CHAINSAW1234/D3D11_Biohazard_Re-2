#pragma once

#include "Base.h"

BEGIN(Engine)

#define MAX_TIME_DELTA		1.f / 5.f

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	HRESULT Initialize();
	_float Compute_TimeDelta();

private:
	LARGE_INTEGER			m_CurTime;
	LARGE_INTEGER			m_OldTime;
	LARGE_INTEGER			m_OriginTime;
	LARGE_INTEGER			m_CpuTick;
	_float					m_fTimeDelta;
	_float					m_fMaxTimeDelta;

public:
	static CTimer* Create();
	virtual void Free() override;
};

END