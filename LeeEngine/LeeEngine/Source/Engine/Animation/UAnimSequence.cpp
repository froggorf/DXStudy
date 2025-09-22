// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#include "CoreMinimal.h"
#include "UAnimSequence.h"

#include "Bone.h"
#include "Engine/UEngine.h"
#include "Engine/RenderCore/EditorScene.h"

UAnimSequence::UAnimSequence(const UAnimSequence& Other)
{
	Duration       = Other.Duration;
	TicksPerSecond = Other.TicksPerSecond;
	RootNode       = Other.RootNode;
	Bones          = Other.Bones;
	BoneInfoMap    = Other.BoneInfoMap;
}

Bone* UAnimSequence::FindBone(const std::string& name)
{
	const auto iter = std::find_if(Bones.begin(),
									Bones.end(),
									[&](const Bone& bone)
									{
										return bone.GetBoneName() == name;
									});

	if (iter == Bones.end())
	{
		return nullptr;
	}

	return &(*iter);
}

void UAnimSequence::GetBoneTransform(float CurrentAnimTime, std::vector<XMMATRIX>& FinalBoneMatrices, bool* bPlayRootMotion)
{
	if (CurrentAnimTime == 0.0f && bIsCachedFirstFrameBoneMatrices)
	{
		for (int i = 0; i < MAX_BONES; ++i)
		{
			FinalBoneMatrices[i] = CachedFirstFrameBoneMatrices[i];
		}
		return;
	}

	*bPlayRootMotion = bEnableRootMotion;

	// 09.23) 재생 속도 비율을 적용시켜줌
	CurrentAnimTime *= RateScale;
	CurrentAnimTime = fmod(CurrentAnimTime, Duration);

	std::vector<XMMATRIX> GlobalTransform(BoneHierarchy.size(), XMMatrixIdentity());

	// 계층별로 작업이 일어나야하므로 싱글쓰레드에서 진행
	for (int HierarchyIndex = 0; HierarchyIndex < BoneHierarchy.size(); ++HierarchyIndex)
	{
		const FPrecomputedBoneData& BoneData       = BoneHierarchy[HierarchyIndex];
		XMMATRIX                    LocalTransform = XMMatrixIdentity();
		if (BoneData.Bone)
		{
			BoneData.Bone->Update(CurrentAnimTime);
			LocalTransform = BoneData.Bone->GetLocalTransform();
		}

		if (BoneData.ParentIndex >= 0)
		{
			GlobalTransform[HierarchyIndex] = XMMatrixMultiply(LocalTransform, GlobalTransform[BoneData.ParentIndex]);
		}
		else
		{
			GlobalTransform[HierarchyIndex] = LocalTransform;
		}

		if (BoneData.BoneInfo.id >= 0 && BoneData.BoneInfo.id < MAX_BONES)
		{
			FinalBoneMatrices[BoneData.BoneInfo.id] = XMMatrixMultiply(BoneData.BoneInfo.offset, GlobalTransform[HierarchyIndex]);
		}
	}
}

void UAnimSequence::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UAnimCompositeBase::LoadDataFromFileData(AssetData);

	if (!GetAnimationSkeleton())
	{
		return;
	}

	ReadMyAssetFile(AssetData["AnimationDataPath"], GetAnimationSkeleton().get());

	if (AssetData.contains("RootMotion"))
	{
		int Val = AssetData["RootMotion"];
		bEnableRootMotion = Val;
	}

	if (AssetData.contains("Rate"))
	{
		float Val = AssetData["Rate"];
		RateScale = Val;
	}

	PrecomputeAnimationData(GetAnimationSkeleton()->GetName());
}

void UAnimSequence::ReadMyAssetFile(const std::string& FilePath, USkeletalMesh* SkeletalMesh)
{
	std::string      path = GEngine->GetDirectoryPath() + std::string(FilePath);
	Assimp::Importer importer;
	const aiScene*   scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!scene || !scene->mRootNode)
	{
		assert(nullptr && "Wrong");
		//std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	aiAnimation* animation = scene->mAnimations[0];

	Duration       = static_cast<float>(animation->mDuration);
	TicksPerSecond = static_cast<float>(animation->mTicksPerSecond);
	ReadHierarchyData(RootNode, scene->mRootNode);
	ReadMissingBones(animation, SkeletalMesh->GetSkeletalMeshRenderData()->ModelBoneInfoMap);
}

void UAnimSequence::ReadMissingBones(const aiAnimation* animation, std::map<std::string, BoneInfo>& modelBoneInfoMap)
{
	UINT channelSize = animation->mNumChannels;
	int  boneCount   = static_cast<int>(modelBoneInfoMap.size());

	for (UINT index = 0; index < channelSize; ++index)
	{
		auto        channel  = animation->mChannels[index];
		std::string boneName = channel->mNodeName.C_Str();

		if (boneName.contains("mixamorig:"))
		{
			boneName.replace(boneName.begin(), boneName.begin() + 10, "");
		}

		// Missing Bone 추가
		if (!modelBoneInfoMap.contains(boneName))
		{
			modelBoneInfoMap[boneName]    = {boneCount, XMMatrixIdentity()};
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
	if (dest.name.contains("mixamorig:"))
	{
		dest.name.replace(dest.name.begin(), dest.name.begin() + 10, "");
	}
	aiMatrix4x4 aiMat   = src->mTransformation;
	dest.transformation = XMMATRIX(aiMat.a1,
									aiMat.b1,
									aiMat.c1,
									aiMat.d1,
									// 1열
									aiMat.a2,
									aiMat.b2,
									aiMat.c2,
									aiMat.d2,
									// 2열
									aiMat.a3,
									aiMat.b3,
									aiMat.c3,
									aiMat.d3,
									// 3열
									aiMat.a4,
									aiMat.b4,
									aiMat.c4,
									aiMat.d4 // 4열
	);
	dest.childrenCount = src->mNumChildren;

	for (int i = 0; i < dest.childrenCount; ++i)
	{
		AssimpNodeData newData;
		ReadHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}

void UAnimSequence::TraverseTreeHierarchy(const AssimpNodeData* NodeData, int ParentIndex)
{
	if (!NodeData)
	{
		return;
	}

	FPrecomputedBoneData BoneData;
	BoneData.BoneName    = NodeData->name;
	BoneData.ParentIndex = ParentIndex;
	BoneData.Bone        = FindBone(BoneData.BoneName);
	BoneData.BoneTransform = NodeData->transformation;
	if (BoneInfoMap.contains(BoneData.BoneName))
	{
		BoneData.BoneInfo = BoneInfoMap[BoneData.BoneName];
	}
	else
	{
		BoneData.BoneInfo.id     = -1;
		BoneData.BoneInfo.offset = XMMatrixIdentity();
	}

	int CurrentIndex = static_cast<int>(BoneHierarchy.size());
	BoneHierarchy.push_back(BoneData);

	size_t ChildCount = NodeData->children.size();
	for (int ChildIndex = 0; ChildIndex < ChildCount; ++ChildIndex)
	{
		TraverseTreeHierarchy(&NodeData->children[ChildIndex], CurrentIndex);
	}
}

void UAnimSequence::PrecomputeAnimationData(const std::string& Name)
{
	// 트리 구조의 계층을 벡터 구조로 변환
	BoneHierarchy.clear();
	TraverseTreeHierarchy(&RootNode, -1);

	if (!GetSkeletonBoneHierarchyMap().contains(Name))
	{
		GetSkeletonBoneHierarchyMap()[Name] = BoneHierarchy;
	}

	// 첫 프레임의 본 변환 행렬을 캐시
	CachedFirstFrameBoneMatrices = std::vector<XMMATRIX>(MAX_BONES, XMMatrixIdentity());
	bool Dummy;
	GetBoneTransform(0.0f, CachedFirstFrameBoneMatrices, &Dummy);
	bIsCachedFirstFrameBoneMatrices = true;
}
