#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Zombie.h"

//	Upper Body
//	Chest				Body_mesh0000		Insidebody_mesh0021
//	Stomach				Body_mesh0001		Insidebody_mesh0022
//	Hips				Body_mesh0002		Insidebody_mesh0023

//	R_Arm
//	Upper_Humerous		Body_mesh0003		Insidebody_mesh0032		Deficit_mesh0040 (From Parent)	Deficit_mesh0041 (From Child)		Damage_mesh0066 (From Parent)		Damage_mesh0067 (From Child)
//	Lower_Humerous		Body_mesh0004		Insidebody_mesh0033		Deficit_mesh0042 (From Parent)	Deficit_mesh0043 (From Child)		Damage_mesh0068 (From Parent)		Damage_mesh0069 (From Child)
//	Upper_Radius		Body_mesh0005		Insidebody_mesh0034		Deficit_mesh0044 (From Parent)	Deficit_mesh0045 (From Child)		Damage_mesh0070 (From Parent)		Damage_mesh0071 (From Child)
//	Lower_Radius		Body_mesh0006		Insidebody_mesh0035		Is Last Child (Can't Off)											Is Last Child

//	L_Arm
//	Upper_Humerous		Body_mesh0007		Insidebody_mesh0036		Deficit_mesh0046 (From Parent)	Deficit_mesh0047 (From Child)		Damage_mesh0072 (From Parent)		Damage_mesh0073 (From Child)
//	Lower_Humerous		Body_mesh0008		Insidebody_mesh0037		Deficit_mesh0048 (From Parent)	Deficit_mesh0049 (From Child)		Damage_mesh0074 (From Parent)		Damage_mesh0075 (From Child)
//	Upper_Radius		Body_mesh0009		Insidebody_mesh0038		Deficit_mesh0050 (From Parent)	Deficit_mesh0051 (From Child)		Damage_mesh0076 (From Parent)		Damage_mesh0077 (From Child)
//	Lower_Radius		Body_mesh0010		Insidebody_mesh0039		Is Last Child (Can't Off)											Is Last Child

//	R_Leg
//	Upper_Femur			Body_mesh0011		Insidebody_mesh0024		Deficit_mesh0052 (From Parent)	Deficit_mesh0053 (From Child)		Damage_mesh0078 (From Parent)		Damage_mesh0079 (From Child)
//	Lower_Femur			Body_mesh0012		Insidebody_mesh0025		Deficit_mesh0054 (From Parent)	Deficit_mesh0055 (From Child)		Damage_mesh0080 (From Parent)		Damage_mesh0081 (From Child)
//	Upper_Tabia			Body_mesh0013		Insidebody_mesh0026		Deficit_mesh0056 (From Parent)	Deficit_mesh0057 (From Child)		Damage_mesh0082 (From Parent)		Damage_mesh0083 (From Child)
//	Lower_Tabia			Body_mesh0014		Insidebody_mesh0027		Deficit_mesh0058 (From Parent)	//	발은 추가적 파괴 가능				Damage_mesh0084 (From Parent)

//	L_Leg
//	Upper_Femur			Body_mesh0015		Insidebody_mesh0028		Deficit_mesh0059 (From Parent)	Deficit_mesh0060 (From Child)		Damage_mesh0085 (From Parent)		Damage_mesh0086 (From Child)
//	Lower_Femur			Body_mesh0016		Insidebody_mesh0029		Deficit_mesh0061 (From Parent)	Deficit_mesh0062 (From Child)		Damage_mesh0087 (From Parent)		Damage_mesh0088 (From Child)
//	Upper_Tabia			Body_mesh0017		Insidebody_mesh0030		Deficit_mesh0063 (From Parent)	Deficit_mesh0064 (From Child)		Damage_mesh0089 (From Parent)		Damage_mesh0090 (From Child)
//	Lower_Tabia			Body_mesh0018		Insidebody_mesh0031		Deficit_mesh0065 (From Parent)	//	발은 추가적 파괴 가능				Damage_mesh0091 (From Parent)

//	Head
//	Broken Right		BokenrHeadA_mesh0104
//	Broken Center		BokenrHeadB_mesh0105
//	Broken Left			BokenrHeadC_mesh0106

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
	vector<_bool>							m_isBreakParts;			//	Part_Index
	unordered_set<_uint>					m_RagDollMeshIndices;	//	Part_Index
	unordered_set<_uint>					m_AnimMeshIndices;		//	Part_Index

	vector<class CPart_Mesh_Info_Zombie*>	m_PartMeshInfos;

	CModel*									m_pBody_Model = { nullptr };

public:
	static CPart_Breaker_Zombie* Create(void* pArg);
	virtual void Free() override;
};

END