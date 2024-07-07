#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

#define ZOMBIE_DOOR_RUB_TOTAL_INTERPOLATE_TO_WINDOW_TIME		0.5f
#define ZOMBIE_RUB_DOOR_RECOGNITION_RANGE						2.f
BEGIN(Client)

class CRub_Door_Zombie : public CTask_Node
{
public:
	enum class ANIM_STATE { _START, _LOOP, _FINISH, _END };
	enum class ANIM_TYPE { _A, _B, _END };

private:
	CRub_Door_Zombie();
	CRub_Door_Zombie(const CRub_Door_Zombie& rhs);
	virtual ~CRub_Door_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation(_float fTimeDelta);

public:
	inline void						SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

protected:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	PLAYING_INDEX					m_eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };
	const wstring					m_strAnimLayerTag = { TEXT("Gimmick_Door") };

	ANIM_TYPE						m_eAnimType = { ANIM_TYPE::_END };
	ANIM_STATE						m_eAnimState = { ANIM_STATE::_END };

	_float							m_fAccLinearInterpolateTime = { 0.f };
	_float4x4						m_InterpolateDeltaMatrix = {};

public:
	static CRub_Door_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END