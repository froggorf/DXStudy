// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#include "UAnimSequence.h"


#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Bone.h"


UAnimSequence::UAnimSequence(const std::string& animationPath, std::map<std::string, BoneInfo>& modelBoneInfoMap)
{
	std::string path = animationPath;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded );
	if(!scene || !scene->mRootNode)
	{
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	aiAnimation* animation = scene->mAnimations[0];
	
	Duration = animation->mDuration;
	TicksPerSecond = animation->mTicksPerSecond;
	ReadHierarchyData(RootNode, scene->mRootNode);
	ReadMissingBones(animation, modelBoneInfoMap);

}

UAnimSequence::UAnimSequence(const UAnimSequence& Other)
{
	Duration = Other.Duration;
	TicksPerSecond = Other.TicksPerSecond;
	RootNode = Other.RootNode;
	Bones = Other.Bones;
	BoneInfoMap = Other.BoneInfoMap;
}


Bone* UAnimSequence::FindBone(const std::string& name)
{
	const auto iter = std::find_if(Bones.begin(), Bones.end(),
		[&](const Bone& bone)
		{
			return bone.GetBoneName() == name;
		}
	);

	if (iter == Bones.end())
	{
		return nullptr;
	}

	return &(*iter);
}

void UAnimSequence::ReadMissingBones(const aiAnimation* animation, std::map<std::string, BoneInfo>& modelBoneInfoMap)
{
	int channelSize = animation->mNumChannels;
	int boneCount = modelBoneInfoMap.size();

	for( int index = 0; index < channelSize; ++index)
	{
		auto channel = animation->mChannels[index];
		std::string boneName = channel->mNodeName.C_Str();

		if(boneName.contains("mixamorig:"))
		{
			boneName.replace(boneName.begin(),boneName.begin()+10, "");
		}

		// Missing Bone 추가
		if (modelBoneInfoMap.find(boneName) == modelBoneInfoMap.end())
		{
			modelBoneInfoMap[boneName] = { boneCount, DirectX::XMMatrixIdentity() };
			modelBoneInfoMap[boneName].id = boneCount;
			++boneCount;
			std::cout << "Missing Bone Added:" << boneName << std::endl;
		}
		Bones.push_back(Bone(boneName, modelBoneInfoMap[boneName].id, channel));	
		
	}
	BoneInfoMap = modelBoneInfoMap;
}

void UAnimSequence::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.C_Str();
	if(dest.name.contains("mixamorig:"))
	{
		dest.name.replace(dest.name.begin(),dest.name.begin()+10, "");
	}
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
