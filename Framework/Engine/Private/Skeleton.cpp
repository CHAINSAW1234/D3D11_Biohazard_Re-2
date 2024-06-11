#include "Skeleton.h"

_matrix create_offset_transform(const _matrix& a, const _matrix& b)
{
    // Create delta transform
    _vector pos_a = a.r[3];
    pos_a=XMVectorSetW(pos_a, 1.f);
    _vector pos_b = b.r[3];
    pos_b=XMVectorSetW(pos_b, 1.f);

    // Calculate the translation difference
    _vector translation_diff = XMVectorSubtract(pos_b, pos_a);
    _matrix translation_delta_mat = XMMatrixTranslationFromVector(translation_diff);

    // Calculate the rotation difference
    _vector quat_a = XMQuaternionRotationMatrix(a);
    _vector quat_b = XMQuaternionRotationMatrix(b);
    _vector rotation_diff = XMQuaternionMultiply(quat_b, XMQuaternionConjugate(quat_a));
    _matrix rotation_delta_mat = XMMatrixRotationQuaternion(rotation_diff);

    //return XMMatrixMultiply(translation_delta_mat, rotation_delta_mat);
    return XMMatrixMultiply(rotation_delta_mat, translation_delta_mat);
}

Skeleton* Skeleton::create(const aiScene* scene, ofstream& ofs)
{
    Skeleton* skeleton = new Skeleton();

    std::vector<aiBone*>            temp_bone_list(256);
    std::unordered_set<std::string> bone_map;

    skeleton->build_bone_list(scene->mRootNode, scene, temp_bone_list, bone_map);
    skeleton->m_joints.reserve(skeleton->m_num_joints);
    skeleton->build_skeleton(scene->mRootNode, 0, scene, temp_bone_list);

    for (auto& joint : skeleton->m_joints)
    {
        if (joint.parent_index == -1)
        {
            XMMATRIX offsetFromParentMatrix = XMMatrixInverse(nullptr, joint.inverse_bind_pose);
            joint.offset_from_parent = offsetFromParentMatrix;
        }
        else
        {
            XMMATRIX parentGlobalMatrix = XMMatrixInverse(nullptr, skeleton->m_joints[joint.parent_index].inverse_bind_pose);

            XMMATRIX currentGlobalMatrix = XMMatrixInverse(nullptr, joint.inverse_bind_pose);

            // 상대 오프셋 매트릭스 계산
            XMMATRIX offsetFromParentMatrix = create_offset_transform(parentGlobalMatrix, currentGlobalMatrix);
            joint.offset_from_parent= offsetFromParentMatrix;
        }
    }

    for (int i = 0; i < skeleton->m_joints.size(); i++)
    {
        auto& joint = skeleton->m_joints[i];
    }

    auto JointSize = skeleton->m_joints.size();

    ofs.write(reinterpret_cast<_char*>(&JointSize), sizeof(JointSize));

    for (int i = 0; i < JointSize; i++)
    {
        auto& joint = skeleton->m_joints[i];

        ofs.write(reinterpret_cast<_char*>(&joint.inverse_bind_pose), sizeof(joint.inverse_bind_pose));
        ofs.write(reinterpret_cast<_char*>(&joint.original_rotation), sizeof(joint.original_rotation));
        ofs.write(reinterpret_cast<_char*>(&joint.parent_index), sizeof(joint.parent_index));
    }

    return skeleton;
}

Skeleton* Skeleton::create(ifstream& ifs)
{
    Skeleton* skeleton = new Skeleton();

    _uint JointSize = 0;

    ifs.read(reinterpret_cast<_char*>(&JointSize), sizeof(JointSize));

    for (int i = 0; i < JointSize; i++)
    {
        Joint joint;

        joint.name = "None";
        ifs.read(reinterpret_cast<_char*>(&joint.inverse_bind_pose), sizeof(_matrix));
        ifs.read(reinterpret_cast<_char*>(&joint.original_rotation), sizeof(_vector));
        ifs.read(reinterpret_cast<_char*>(&joint.parent_index), sizeof(joint.parent_index));

        skeleton->m_joints.push_back(joint);
    }

    return skeleton;
}

Skeleton::Skeleton()
{
    m_num_joints = 0;
}

Skeleton::~Skeleton()
{
}

void Skeleton::build_bone_list(aiNode* node, const aiScene* scene, std::vector<aiBone*>& temp_bone_list, std::unordered_set<std::string>& bone_map)
{
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* current_mesh = scene->mMeshes[node->mMeshes[i]];

        for (int j = 0; j < current_mesh->mNumBones; j++)
        {
            std::string bone_name = std::string(current_mesh->mBones[j]->mName.C_Str());

            if (bone_map.find(bone_name) == bone_map.end())
            {
                temp_bone_list[m_num_joints] = current_mesh->mBones[j];
                m_num_joints++;

                bone_map.insert(bone_name);
            }
        }
    }

    for (int i = 0; i < node->mNumChildren; i++)
        build_bone_list(node->mChildren[i], scene, temp_bone_list, bone_map);
}

void Skeleton::build_skeleton(aiNode* node, int bone_index, const aiScene* scene, std::vector<aiBone*>& temp_bone_list)
{
    std::string node_name = node->mName.C_Str();

    int count = bone_index;

    for (int i = 0; i < m_num_joints; i++)
    {
        std::string bone_name = temp_bone_list[i]->mName.C_Str();

        if (bone_name == node_name)
        {
            Joint joint;

            joint.name = bone_name;
            _float4x4	OffsetMatrix;
            memcpy(&OffsetMatrix, &temp_bone_list[i]->mOffsetMatrix, sizeof(_float4x4));
            XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
            XMMATRIX offsetMatrix = XMLoadFloat4x4(&OffsetMatrix);
            offsetMatrix = XMMatrixRotationY(PxPi) * offsetMatrix;
            joint.inverse_bind_pose= offsetMatrix;

            XMMATRIX inverseMatrix = XMMatrixInverse(nullptr, joint.inverse_bind_pose);
            XMVECTOR rotationQuat = XMQuaternionRotationMatrix(inverseMatrix);
            joint.original_rotation = rotationQuat;

            aiNode* parent = node->mParent;
            int     index;

            while (parent)
            {
                index = find_joint_index(parent->mName.C_Str());

                if (index == -1)
                    parent = parent->mParent;
                else
                    break;
            }

            joint.parent_index = index;
            m_joints.push_back(joint);

            break;
        }
    }

    for (int i = 0; i < node->mNumChildren; i++)
        build_skeleton(node->mChildren[i], m_num_joints, scene, temp_bone_list);
}

int32_t Skeleton::find_joint_index(const std::string& channel_name)
{
    for (int i = 0; i < m_joints.size(); i++)
    {
        if (m_joints[i].name == channel_name)
            return i;
    }

    return -1;
}
