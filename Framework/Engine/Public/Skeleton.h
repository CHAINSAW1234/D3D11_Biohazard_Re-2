#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

struct Joint
{
	std::string					name;
	_matrix						inverse_bind_pose;
	_matrix						offset_from_parent;
	_vector						original_rotation;
	int32_t						parent_index = 0;
	int32_t						parent_index_BreakPart = 0;
	int32_t						parent_index_BreakPart_Cloth_Arm_L = 0;
	int32_t						parent_index_BreakPart_Cloth_Arm_R = 0;
	int32_t						parent_index_BreakPart_Cloth_Leg_L = 0;
	int32_t						parent_index_BreakPart_Cloth_Leg_R = 0;

	XMFLOAT3					bind_pos_ws(const XMMATRIX& model)
	{
		XMMATRIX m = XMMatrixMultiply(XMMatrixInverse(nullptr, inverse_bind_pose),model);

		XMFLOAT3 result;
		result.x = XMVectorGetX(m.r[3]);
		result.y = XMVectorGetY(m.r[3]);
		result.z = XMVectorGetZ(m.r[3]);

		return result;
	}
	_bool						Compare_Name(const _char* pBoneName)
	{
		return !strcmp(name.c_str(), pBoneName);
	}
};

class Skeleton
{
public:
    static Skeleton*			create(const aiScene* scene, ofstream& ofs);
    static Skeleton*			create(ifstream& ifs);

    Skeleton();
    ~Skeleton();
    int32_t						find_joint_index(const std::string& channel_name);

    inline size_t				num_bones() { return m_num_joints; }
    inline struct Joint*		joints() { return &m_joints[0]; }
	void						SetNumJoint();
public:
    void						build_bone_list(aiNode* node, const aiScene* scene, std::vector<aiBone*>& temp_bone_list, std::unordered_set<std::string>& bone_map);
    void						build_skeleton(aiNode* node, int bone_index, const aiScene* scene, std::vector<aiBone*>& temp_bone_list);
	_int                        Find_BoneIndex(const string& strRootTag);

private:
    size_t					m_num_joints;
    std::vector<Joint>			m_joints;
};

END