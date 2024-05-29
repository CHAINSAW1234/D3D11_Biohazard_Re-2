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

	XMFLOAT3					bind_pos_ws(const XMMATRIX& model)
	{
		XMMATRIX m = XMMatrixMultiply(XMMatrixInverse(nullptr, inverse_bind_pose),model);

		XMFLOAT3 result;
		result.x = XMVectorGetX(m.r[3]);
		result.y = XMVectorGetY(m.r[3]);
		result.z = XMVectorGetZ(m.r[3]);

		return result;
	}
};

class Skeleton
{
public:
    static Skeleton*			create(const aiScene* scene);

    Skeleton();
    ~Skeleton();
    int32_t						find_joint_index(const std::string& channel_name);

    inline uint32_t				num_bones() { return m_num_joints; }
    inline class Joint*			joints() { return &m_joints[0]; }

private:
    void						build_bone_list(aiNode* node, const aiScene* scene, std::vector<aiBone*>& temp_bone_list, std::unordered_set<std::string>& bone_map);
    void						build_skeleton(aiNode* node, int bone_index, const aiScene* scene, std::vector<aiBone*>& temp_bone_list);

private:
    uint32_t					m_num_joints;
    std::vector<Joint>			m_joints;
};

END