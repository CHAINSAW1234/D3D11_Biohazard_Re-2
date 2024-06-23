#pragma once

#include "Client_Defines.h"
#include "BlackBoard.h"

BEGIN(Engine)
class CTask_Node;
END

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

	_uint							Get_Current_MotionType_Body();
	_uint							Get_Pre_MotionType_Body();

	CModel*							Get_PartModel(_uint iPartType);

public:		/* Anim Controll */
	void							Reset_NonActive_Body(const list<_uint>& ActivePlayingIndices);

public:		/* Public Utility */
	_bool							Compute_Direction_To_Player_World(_float3* pDirection);
	_bool							Compute_Direction_To_Player_Local(_float3* pDirection);
	_bool							Compute_Player_Angle_XZ_Plane_Local(_float* pAngle);


	CTransform*						Get_Transform(CGameObject* pObject);
	CTransform*						Get_Transform_AI();

private:	/* Private Utility */
	CModel*							Find_PartModel(_uint iPartID);


public:		/* Anim Branch Check */
	_bool							Is_Start_Anim(_uint iPartID, _uint iAnimIndex);
	_bool							Is_Loop_Anim(_uint iPartID, _uint iAnimIndex);

	_bool							Is_Move_Anim(_uint iPartID, _uint iAnimIndex);
	_bool							Is_Turn_Anim(_uint iPartID, _uint iAnimIndex);

public:		/* Motion Blend Controll */
	vector<_float>					Get_BlendWeights(_uint iPartID);
	

	
protected:
	class CPathFinder*				m_pPathFinder = { nullptr };
	class CPlayer*					m_pPlayer = { nullptr };
	class CZombie*					m_pAI = { nullptr };

	CTask_Node*						m_pPreNode = { nullptr };

public:
	static CBlackBoard_Zombie* Create();

public:
	virtual void Free() override;
};

END