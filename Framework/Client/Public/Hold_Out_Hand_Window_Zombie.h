#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

#define ZOMBIE_HOLD_WINDOW_RECOGNITION_RANGE						4.f
#define ZOMBIE_HOLD_TOTAL_INTERPOLATE_TO_WINDOW_TIME				0.5f

BEGIN(Client)

class CHold_Out_Hand_Window_Zombie : public CTask_Node
{
public:
	enum class ANIM_STATE { _START, _LOOP, _FINISH, _END };
	enum class ANIM_TYPE { _1, _2, _END };

private:
	CHold_Out_Hand_Window_Zombie();
	CHold_Out_Hand_Window_Zombie(const CHold_Out_Hand_Window_Zombie& rhs);
	virtual ~CHold_Out_Hand_Window_Zombie() = default;

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

	PLAYING_INDEX					m_eBasePlayingIndex = { PLAYING_INDEX::INDEX_0};
	const wstring					m_strAnimLayerTag = { TEXT("Gimmick_Window") };

	ANIM_STATE						m_eAnimState = { ANIM_STATE::_END };
	ANIM_TYPE						m_eAnimType = { ANIM_TYPE::_END };

	_float							m_fAccLinearInterpolateTime = { 0.f };
	_float4x4						m_DeltaInterpolateMatrix = {};

public:
	static CHold_Out_Hand_Window_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END