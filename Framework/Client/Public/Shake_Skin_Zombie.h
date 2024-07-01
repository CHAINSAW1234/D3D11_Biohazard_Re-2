#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CShake_Skin_Zombie : public CTask_Node
{
private:
	CShake_Skin_Zombie();
	CShake_Skin_Zombie(const CShake_Skin_Zombie& rhs);
	virtual ~CShake_Skin_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }
private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };
	
private:
	const wstring&					m_strL_Leg_AnimLayerTag = { TEXT("Add_Leg_L") };
	const wstring&					m_strR_Leg_AnimLayerTag = { TEXT("Add_Leg_R") };
	const wstring&					m_strL_Arm_AnimLayerTag = { TEXT("Add_Arm_L") };
	const wstring&					m_strR_Arm_AnimLayerTag = { TEXT("Add_Arm_R") };
	const wstring&					m_strL_Shoulder_AnimLayerTag = { TEXT("Add_Shoulder_L") };
	const wstring&					m_strR_Shoulder_AnimLayerTag = { TEXT("Add_Shoulder_R") };

	//	const PLAYING_INDEX				m_eL_Leg_PlayingIndex = { PLAYING_INDEX::IN}
	

public:
	static CShake_Skin_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END