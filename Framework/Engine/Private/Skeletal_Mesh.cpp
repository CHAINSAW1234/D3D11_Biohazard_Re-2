#include "Skeletal_Mesh.h"
#include "RagDoll_Physics.h"
#include "Skeleton.h"

#define JOINT_COUNT 10

SkeletalMesh* SkeletalMesh::load(const std::string& name, Skeleton* skeleton)
{
	filesystem::path FullPath(name);

	string strParentsPath = FullPath.parent_path().string();
	string strFileName = FullPath.stem().string();

	string strNewPath = strParentsPath + "/" + strFileName + "_Ragdoll.bin";

	filesystem::path CheckPath(strNewPath);

	if (true == filesystem::exists(CheckPath))
	{
		ifstream ifs(strNewPath.c_str(), ios::binary);

		if (true == ifs.fail())
		{
			MSG_BOX(TEXT("Failed To OpenFile_Radoll"));

			return nullptr;
		}

		SkeletalMesh* skeletal_mesh = new SkeletalMesh();

		if (skeleton)
			skeletal_mesh->m_skeleton = skeleton;
		else
			skeletal_mesh->m_skeleton = Skeleton::create(ifs);

		return skeletal_mesh;
	}
	else
	{
		_uint		iOption = { aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded };

		const aiScene* scene;
		Assimp::Importer importer;
		scene = importer.ReadFile(name, iOption);

		ofstream ofs(strNewPath.c_str(), ios::binary);

		SkeletalMesh* skeletal_mesh = new SkeletalMesh();

		if (skeleton)
			skeletal_mesh->m_skeleton = skeleton;
		else
			skeletal_mesh->m_skeleton = Skeleton::create(scene, ofs);

		return skeletal_mesh;
	}
}

SkeletalMesh::SkeletalMesh()
{
}

SkeletalMesh::~SkeletalMesh()
{
}