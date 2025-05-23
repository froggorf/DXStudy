// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#include "Animation.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Bone.h"


#define ResourceFolderDirectory "../../Resource/"
Animation::Animation(const std::string& animationPath, std::map<std::string, BoneInfo>& modelBoneInfoMap)
{
	std::string path = ResourceFolderDirectory + animationPath;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded );
	if(!scene || !scene->mRootNode)
	{
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
	}
	aiAnimation* animation = scene->mAnimations[0];
	m_Duration = animation->mDuration;
	m_TicksPerSecond = animation->mTicksPerSecond;
	ReadHierarchyData(m_RootNode, scene->mRootNode);
	ReadMissingBones(animation, modelBoneInfoMap);

}

Animation::~Animation()
{
}

Bone* Animation::FindBone(const std::string& name)
{
	const auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
		[&](const Bone& bone)
		{
			return bone.GetBoneName() == name;
		}
	);

	if (iter == m_Bones.end())
	{
		return nullptr;
	}

	return &(*iter);
}

void Animation::ReadMissingBones(const aiAnimation* animation, std::map<std::string, BoneInfo>& modelBoneInfoMap)
{
	int channelSize = animation->mNumChannels;
	int boneCount = modelBoneInfoMap.size();

	for( int index = 0; index < channelSize; ++index)
	{
		auto channel = animation->mChannels[index];
		std::string boneName = channel->mNodeName.C_Str();

		// Missing Bone 추가
		if (modelBoneInfoMap.find(boneName) == modelBoneInfoMap.end())
		{
			modelBoneInfoMap[boneName] = { boneCount, DirectX::XMMatrixIdentity() };
			modelBoneInfoMap[boneName].id = boneCount;
			++boneCount;
			std::cout << "Missing Bone Added: " << boneName << std::endl;
		}
		m_Bones.push_back(Bone(boneName, modelBoneInfoMap[boneName].id, channel));
	}
	m_BoneInfoMap = modelBoneInfoMap;
}

void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.C_Str();
	aiMatrix4x4 aiMat = src->mTransformation;
	dest.transformation = 	DirectX::XMMATRIX(
		aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,  // 1열
		aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,  // 2열
		aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,  // 3열
		aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4   // 4열
	);
	dest.childrenCount = src->mNumChildren;

	for(int i = 0; i < dest.childrenCount; ++i)
	{
		AssimpNodeData newData;
		ReadHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}
