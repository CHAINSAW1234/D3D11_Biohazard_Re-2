#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"
#include "Zombie.h"

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
	void							Add_Blend_Animation(COLLIDER_TYPE eIntersectCollider, DIRECTION eHitDirection);

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }
	
private:
	void							Update_BlendWeights();

private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };
	
private:
	static const wstring					m_strL_Leg_AnimLayerTag;
	static const wstring					m_strR_Leg_AnimLayerTag;
	static const wstring					m_strL_Arm_AnimLayerTag;
	static const wstring					m_strR_Arm_AnimLayerTag;
	static const wstring					m_strL_Shoulder_AnimLayerTag;
	static const wstring					m_strR_Shoulder_AnimLayerTag;
	static const wstring					m_strBody_AnimLayerTag;
	static const wstring					m_strHead_AnimLayerTag;

	static const wstring					m_strL_Leg_Twist_BoneLayerTag;
	static const wstring					m_strR_Leg_Twist_BoneLayerTag;
	static const wstring					m_strL_Arm_Twist_BoneLayerTag;
	static const wstring					m_strR_Arm_Twist_BoneLayerTag;
	static const wstring					m_strL_Shoulder_Twist_BoneLayerTag;
	static const wstring					m_strR_Shoulder_Twist_BoneLayerTag;
	static const wstring					m_str_Body_Twist_BoneLayerTag;
	static const wstring					m_str_Head_Twist_BoneLayerTag;

	static const PLAYING_INDEX				m_eL_Leg_PlayingIndex = { PLAYING_INDEX::INDEX_10 };
	static const PLAYING_INDEX				m_eR_Leg_PlayingIndex = { PLAYING_INDEX::INDEX_11 };
	static const PLAYING_INDEX				m_eL_Arm_PlayingIndex = { PLAYING_INDEX::INDEX_12 };
	static const PLAYING_INDEX				m_eR_Arm_PlayingIndex = { PLAYING_INDEX::INDEX_13 };
	static const PLAYING_INDEX				m_eL_Shoulder_PlayingIndex = { PLAYING_INDEX::INDEX_14 };
	static const PLAYING_INDEX				m_eR_Shoulder_PlayingIndex = { PLAYING_INDEX::INDEX_15 };
	static const PLAYING_INDEX				m_eBody_PlayingIndex = { PLAYING_INDEX::INDEX_16 };
	static const PLAYING_INDEX				m_eHead_PlayingIndex = { PLAYING_INDEX::INDEX_17 };

	unordered_set<PLAYING_INDEX>			m_ActivePlayingIndcies;

public:
	static CShake_Skin_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END