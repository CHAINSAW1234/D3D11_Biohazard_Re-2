#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"
#include "Zombie.h"

#define FOOT_UP_IGNORE_TIME				0.5f
#define FOOT_DOWN_IGNORE_TIME			0.5f

BEGIN(Client)

class CFoot_Sound_Zombie : public CTask_Node
{
private:
	CFoot_Sound_Zombie();
	CFoot_Sound_Zombie(const CFoot_Sound_Zombie& rhs);
	virtual ~CFoot_Sound_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

private:
	void							Update_Current_Bone_Positions_Local();

	_float4							Get_Current_L_Ball_Position_Local();
	_float4							Get_Current_R_Ball_Position_Local();
	_float4							Get_Current_Root_Position_Local();

private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	_float							m_fAccActiveTime = { 0.f };

private:
	const _float4x4*				m_pRoot_Combiend = { nullptr };
	const _float4x4*				m_pL_Ball_Combiend = { nullptr };
	const _float4x4*				m_pR_Ball_Combiend = { nullptr };

	_float							m_fAccIgnoreUpTime_L_Leg = { 0.f };
	_float							m_fAccIgnoreUpTime_R_Leg = { 0.f };
	_float							m_fAccIgnoreDownTime_L_Leg = { 0.f };
	_float							m_fAccIgnoreDownTime_R_Leg = { 0.f };

	_bool							m_isUp_L_Leg = { false };
	_bool							m_isUp_R_Leg = { false };

	_float4							m_vPre_L_Ball_Position_Local = {};
	_float4							m_vPre_R_Ball_Position_Local = {};

	_bool							m_isStart = { false };

public:
	static CFoot_Sound_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END