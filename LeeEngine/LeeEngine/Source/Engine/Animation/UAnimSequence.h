// 03.27
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UAnimCompositeBase.h"
#include "Bone.h"

struct aiNode;
struct aiAnimation;

struct AssimpNodeData
{
	DirectX::XMMATRIX transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

struct FPrecomputedBoneData
{
	std::string BoneName;
	Bone* Bone;
	int ParentIndex;
	BoneInfo BoneInfo;
};

class UAnimSequence : public UAnimCompositeBase
{
	MY_GENERATED_BODY(UAnimSequence)
public:
	UAnimSequence( ) = default;
	UAnimSequence(const UAnimSequence& Other);
	~UAnimSequence() override {};

	Bone* FindBone(const std::string& name);


	inline float GetTicksPerSecond() const { return TicksPerSecond; }
	inline float GetDuration() const { return Duration; }
	inline const AssimpNodeData& GetRootNode() const { return RootNode; }
	inline const std::map<std::string, BoneInfo>& GetBoneIDMap() const { return BoneInfoMap; }
	inline static const std::shared_ptr<UAnimSequence>& GetAnimationAsset(const std::string& AnimationName){return std::dynamic_pointer_cast<UAnimSequence>(UAnimationAsset::GetAnimationAsset(AnimationName));}

	// 특정 애니메이션 시간의 본 Matrices를 반환받는 함수
	void GetBoneTransform(float CurrentAnimTime, std::vector<XMMATRIX>& FinalBoneMatrices);


	void LoadDataFromFileData(const nlohmann::json& AssetData) override;
protected:
private:
	// 누락된 본 데이터 추가 및 모델의 boneInfoMap 업데이트
	void ReadMissingBones(const aiAnimation* animation, std::map<std::string, BoneInfo>& modelBoneInfoMap);
	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);

	// 애니메이션의 본 계층 구조를 vector 데이터로 만드는 함수
	void TraverseTreeHierarchy(const AssimpNodeData* NodeData, int ParentIndex);
	void PrecomputeAnimationData();

	

public:
protected:
private:
	float Duration;
	float TicksPerSecond;
	std::vector<Bone> Bones;
	AssimpNodeData RootNode;
	std::map<std::string, BoneInfo> BoneInfoMap;

	std::vector<FPrecomputedBoneData> BoneHierarchy;

	std::vector<XMMATRIX> CachedFirstFrameBoneMatrices;
	bool bIsCachedFirstFrameBoneMatrices;
};
