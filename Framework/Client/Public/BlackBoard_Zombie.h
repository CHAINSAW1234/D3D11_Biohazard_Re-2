#pragma once

#include "Client_Defines.h"
#include "BlackBoard.h"

BEGIN(Client)

class CBlackBoard_Zombie : public CBlackBoard
{
public:
	CBlackBoard_Zombie();
	CBlackBoard_Zombie(const CBlackBoard_Zombie& rhs);
	virtual ~CBlackBoard_Zombie() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);
	void							Initialize_BlackBoard(class CZombie* pAI);

public: // Setter
	void							SetPlayer(class CPlayer* pPlayer)
	{
		m_pPlayer = pPlayer;
	}
	class CZombie* GetAI()
	{
		return m_pAI;
	}

public: // Getter
	class CPlayer*					GetPlayer()
	{
		return m_pPlayer;
	}

	_int							Get_CurrentState() { return m_iCurrentState; }
	_int							Get_PreState() { return m_iPreState; }

	_uint							Get_Current_MotionType_Body();
	_uint							Get_Pre_MotionType_Body();

	CModel*							Get_PartModel(_uint iPartType);

public:		/* Utility */
	_bool							Compute_Direction_To_Player(_float3* pDirection);
	_bool							Compute_Direction_To_Player_Local(_float3* pDirection);

	CTransform*						Get_Transform(CGameObject* pObject);
	CTransform*						Get_Transform_AI();

public:		/* Anim Branch Check */
	_bool							Is_Start_Anim(_uint iPartID, _uint iAnimIndex);
	_bool							Is_Loop_Anim(_uint iPartID, _uint iAnimIndex);

	_bool							Is_Move_Anim(_uint iPartID, _uint iAnimIndex);
	_bool							Is_Turn_Anim(_uint iPartID, _uint iAnimIndex);
	
	
protected:
	class CPathFinder*				m_pPathFinder = { nullptr };
	class CPlayer*					m_pPlayer = { nullptr };
	class CZombie*					m_pAI = { nullptr };

	_int							m_iPreState = { -1 };			//	현재상태와 이전상태 => ( 최초에 -1 로 초기화 )
	_int							m_iCurrentState = { -1 };

public:
	static CBlackBoard_Zombie* Create();

public:
	virtual void Free() override;
};

END