#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Zombie.h"

BEGIN(Client)

class CPart_Breaker_Zombie final : public CBase
{
public:
	typedef struct tagPartBreakDesc
	{
		CModel*					pBodyModel = { nullptr };
	}PART_BREAKER_DESC;
	
private:
	CPart_Breaker_Zombie();
	CPart_Breaker_Zombie(const CPart_Breaker_Zombie& rhs);
	virtual ~CPart_Breaker_Zombie() = default;

public:
	virtual HRESULT				Initialize(void* pArg);

public:
	_bool						Is_BreaKPart(BREAK_PART ePart);
	_bool						Is_RagDoll_Mesh(_uint iMeshIndex);

public:
	void						Break(BREAK_PART ePart);
	

	
private:
	vector<_bool>							m_isBreakParts;
	vector<unordered_set<_uint>>			m_RagDollMeshIndices;
	vector<unordered_set<_uint>>			m_AnimMeshIndices;

	vector<list<_uint>>						m_AppearMeshIndices;
	vector<list<_uint>>						m_HideMeshIndices;

	CModel*									m_pBody_Model = { nullptr };

public:
	static CPart_Breaker_Zombie* Create(void* pArg);
	virtual void Free() override;
};

END