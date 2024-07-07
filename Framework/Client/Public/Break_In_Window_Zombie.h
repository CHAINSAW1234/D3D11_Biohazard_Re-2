#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

#define ZOMBIE_BREAK_IN_TOTAL_INTERPOLATE_TO_WINDOW_TIME			0.5f

BEGIN(Client)

class CBreak_In_Window_Zombie : public CTask_Node
{
private:
	CBreak_In_Window_Zombie();
	CBreak_In_Window_Zombie(const CBreak_In_Window_Zombie& rhs);
	virtual ~CBreak_In_Window_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation(_float fTimeDelta);
	void							Set_TargetInterpolate_Matrix();

public:
	inline void						SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

protected:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	PLAYING_INDEX					m_eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };
	const wstring					m_strAnimLayerTag = { TEXT("Gimmick_Window") };
	_int							m_iAnimIndex = { -1 };

	_bool							m_isNeedInterpolation = { false };
	_float							m_fAccLinearInterpolateTime = { 0.f };
	_float4x4						m_DeltaInterpolateMatrix = {};

public:
	static CBreak_In_Window_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END