#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Zombie.h"

//	Upper Body			|	MALE										|	BIG_MALE, FEMALE							|		STOMACH			BIG MALE, FEMALE			MALE
//	Stomach				|	Body_mesh0001		Insidebody_mesh0022		|	Body_mesh0020		Insidebody_mesh0000		|		From Chest		Body_mesh0038				Body_mesh0019
//	Chest				|	Body_mesh0000		Insidebody_mesh0021		|	Body_mesh0019		Insidebody_mesh0001		|		From Hips		Body_mesh0039				Body_mesh0020
//	Hips				|	Body_mesh0002		Insidebody_mesh0023		|	Body_mesh0021		Insidebody_mesh0002		|
//						|												|												|
//	R_Arm				|												|												|
//	Upper_Humerous		|	Body_mesh0003		Insidebody_mesh0032		|	Body_mesh0022		Insidebody_mesh0011		|		Deficit_mesh0040 (From Parent)	Deficit_mesh0041 (From Child)		Damage_mesh0066 (From Parent)		Damage_mesh0067 (From Child)
//	Lower_Humerous		|	Body_mesh0004		Insidebody_mesh0033		|	Body_mesh0023		Insidebody_mesh0012		|		Deficit_mesh0042 (From Parent)	Deficit_mesh0043 (From Child)		Damage_mesh0068 (From Parent)		Damage_mesh0069 (From Child)
//	Upper_Radius		|	Body_mesh0005		Insidebody_mesh0034		|	Body_mesh0024		Insidebody_mesh0013		|		Deficit_mesh0044 (From Parent)	Deficit_mesh0045 (From Child)		Damage_mesh0070 (From Parent)		Damage_mesh0071 (From Child)
//	Lower_Radius		|	Body_mesh0006		Insidebody_mesh0035		|	Body_mesh0025		Insidebody_mesh0014		|		Is Last Child (Can't Off)											Is Last Child
//						|												|												|
//	L_Arm				|												|												|
//	Upper_Humerous		|	Body_mesh0007		Insidebody_mesh0036		|	Body_mesh0026		Insidebody_mesh0015		|		Deficit_mesh0046 (From Parent)	Deficit_mesh0047 (From Child)		Damage_mesh0072 (From Parent)		Damage_mesh0073 (From Child)
//	Lower_Humerous		|	Body_mesh0008		Insidebody_mesh0037		|	Body_mesh0027		Insidebody_mesh0016		|		Deficit_mesh0048 (From Parent)	Deficit_mesh0049 (From Child)		Damage_mesh0074 (From Parent)		Damage_mesh0075 (From Child)
//	Upper_Radius		|	Body_mesh0009		Insidebody_mesh0038		|	Body_mesh0028		Insidebody_mesh0017		|		Deficit_mesh0050 (From Parent)	Deficit_mesh0051 (From Child)		Damage_mesh0076 (From Parent)		Damage_mesh0077 (From Child)
//	Lower_Radius		|	Body_mesh0010		Insidebody_mesh0039		|	Body_mesh0029		Insidebody_mesh0018		|		Is Last Child (Can't Off)											Is Last Child
//						|												|												|
//	R_Leg				|												|												|
//	Upper_Femur			|	Body_mesh0011		Insidebody_mesh0024		|	Body_mesh0030		Insidebody_mesh0003		|		Deficit_mesh0052 (From Parent)	Deficit_mesh0053 (From Child)		Damage_mesh0078 (From Parent)		Damage_mesh0079 (From Child)
//	Lower_Femur			|	Body_mesh0012		Insidebody_mesh0025		|	Body_mesh0031		Insidebody_mesh0004		|		Deficit_mesh0054 (From Parent)	Deficit_mesh0055 (From Child)		Damage_mesh0080 (From Parent)		Damage_mesh0081 (From Child)
//	Upper_Tabia			|	Body_mesh0013		Insidebody_mesh0026		|	Body_mesh0032		Insidebody_mesh0005		|		Deficit_mesh0056 (From Parent)	Deficit_mesh0057 (From Child)		Damage_mesh0082 (From Parent)		Damage_mesh0083 (From Child)
//	Lower_Tabia			|	Body_mesh0014		Insidebody_mesh0027		|	Body_mesh0033		Insidebody_mesh0006		|		Deficit_mesh0058 (From Parent)	//	발은 추가적 파괴 가능				Damage_mesh0084 (From Parent)
//						|												|												|
//	L_Leg				|												|												|
//	Upper_Femur			|	Body_mesh0015		Insidebody_mesh0028		|	Body_mesh0034		Insidebody_mesh0007		|		Deficit_mesh0059 (From Parent)	Deficit_mesh0060 (From Child)		Damage_mesh0085 (From Parent)		Damage_mesh0086 (From Child)
//	Lower_Femur			|	Body_mesh0016		Insidebody_mesh0029		|	Body_mesh0035		Insidebody_mesh0008		|		Deficit_mesh0061 (From Parent)	Deficit_mesh0062 (From Child)		Damage_mesh0087 (From Parent)		Damage_mesh0088 (From Child)
//	Upper_Tabia			|	Body_mesh0017		Insidebody_mesh0030		|	Body_mesh0036		Insidebody_mesh0009		|		Deficit_mesh0063 (From Parent)	Deficit_mesh0064 (From Child)		Damage_mesh0089 (From Parent)		Damage_mesh0090 (From Child)
//	Lower_Tabia			|	Body_mesh0018		Insidebody_mesh0031		|	Body_mesh0037		Insidebody_mesh0010		|		Deficit_mesh0065 (From Parent)	//	발은 추가적 파괴 가능				Damage_mesh0091 (From Parent)
//						|												|												|
//	Head				|												|												|
//	Broken Right		|	BokenrHeadA_mesh0104						|												|
//	Broken Center		|	BokenrHeadB_mesh0105						|												|
//	Broken Left			|	BokenrHeadC_mesh0106						|												|


//	Shirt

//	R_Arm	29
//	L_Arm	39
//	Shirt	Shirt
//	Tshirt	Tshirt

//	Pants

//	R_Pants 49			|	R_Shoes				44						|	
//	L_Pants 59			|	L_Shoes				54						|
//	Pants	60
																														
BEGIN(Client)

class CPart_Breaker_Zombie final : public CBase
{
public:
	typedef struct tagPartBreakDesc
	{
		CModel*					pBodyModel = { nullptr };
		CModel*					pFaceModel = { nullptr };
		CModel*					pPants_Model = { nullptr };
		CModel*					pShirts_Model = { nullptr };
		_int					iBodyType = { -1 };
		_int					iClothesModelID_Shirts = { -1 };
		_int					iClothesModelID_Pants = { -1 };
	}PART_BREAKER_DESC;
	
private:
	CPart_Breaker_Zombie();
	CPart_Breaker_Zombie(const CPart_Breaker_Zombie& rhs);
	virtual ~CPart_Breaker_Zombie() = default;

public:
	virtual HRESULT									Initialize(void* pArg);

public:
	HRESULT											Compute_MeshParts_Types_Indices_Male(vector<vector<_int>>& MeshPartsTypesIndices, const vector<string>& MeshTags);
	HRESULT											Compute_MeshParts_Types_Indices_Female_Male_Big(vector<vector<_int>>& MeshPartsTypesIndices, const vector<string>& MeshTags);
	HRESULT											Compute_MeshParts_Types_Indices_Shirt(vector<vector<_int>>& MeshPartsTypesIndices, const vector<string>& MeshTags);
	HRESULT											Compute_MeshParts_Types_Indices_Pants(vector<vector<_int>>& MeshPartsTypesIndices, const vector<string>& MeshTags);

	HRESULT											SetUp_Additional_Child_Meshes();

public:
	_bool											Is_BreaKPart(BREAK_PART ePart);
	_bool											Is_RagDoll_Mesh_Body(_uint iMeshIndex);
	_bool											Is_RagDoll_Mesh_Shirt(_uint iMeshIndex);
	_bool											Is_RagDoll_Mesh_Pants(_uint iMeshIndex);

public:
	_bool											Is_L_Arm_Mesh(_uint iMeshIndex);
	_bool											Is_R_Arm_Mesh(_uint iMeshIndex);
	_bool											Is_L_Leg_Mesh(_uint iMeshIndex);
	_bool											Is_R_Leg_Mesh(_uint iMeshIndex);

public:
	_bool											Attack(BREAK_PART ePart);
	_bool											Break(BREAK_PART ePart);
	
private:
	_uint											m_iBodyType = { 0 };

	vector<_bool>									m_isBreakParts;			//	Part_Index
	vector<_uint>									m_HPs;
	unordered_set<_uint>							m_RagDollMeshIndices_Body;	//	Part_Index
	unordered_set<_uint>							m_AnimMeshIndices_Body;		//	Part_Index

	unordered_set<_uint>							m_RagDollMeshIndices_Shirt;	//	Part_Index
	unordered_set<_uint>							m_AnimMeshIndices_Shirt;	//	Part_Index

	unordered_set<_uint>							m_RagDollMeshIndices_Pants;	//	Part_Index
	unordered_set<_uint>							m_AnimMeshIndices_Pants;	//	Part_Index

	vector<class CPart_Mesh_Info_Zombie*>			m_PartMeshInfos_Body;
	vector<class CPart_Mesh_Info_Zombie*>			m_PartMeshInfos_Shirt;
	vector<class CPart_Mesh_Info_Zombie*>			m_PartMeshInfos_Pants;

	CModel*											m_pBody_Model = { nullptr };
	CModel*											m_pFace_Model = { nullptr };
	CModel*											m_pPants_Model = { nullptr };
	CModel*											m_pShirts_Model = { nullptr };

	_int											m_iClothesModelID_Shirts = { -1 };
	_int											m_iClothesModelID_Pants = { -1 };

public:
	static CPart_Breaker_Zombie* Create(void* pArg);
	virtual void Free() override;
};

END