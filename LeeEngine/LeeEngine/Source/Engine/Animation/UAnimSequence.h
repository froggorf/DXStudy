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

class UAnimSequence : public UAnimCompositeBase
{
	MY_GENERATED_BODY(UAnimSequence)
public:
	UAnimSequence() = default;
	UAnimSequence(const std::string& animationPath, std::map<std::string, BoneInfo>& modelBoneInfoMap );
	UAnimSequence(const UAnimSequence& Other);
	~UAnimSequence() override {};
	Bone* FindBone(const std::string& name);

	inline float GetTicksPerSecond() const { return TicksPerSecond; }
	inline float GetDuration() const { return Duration; }
	inline const AssimpNodeData& GetRootNode() const { return RootNode; }
	inline const std::map<std::string, BoneInfo>& GetBoneIDMap() const { return BoneInfoMap; }


protected:
private:
	// 누락된 본 데이터 추가 및 모델의 boneInfoMap 업데이트
	void ReadMissingBones(const aiAnimation* animation, std::map<std::string, BoneInfo>& modelBoneInfoMap);
	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);
public:
protected:
private:
	float Duration;
	float TicksPerSecond;
	std::vector<Bone> Bones;
	AssimpNodeData RootNode;
	std::map<std::string, BoneInfo> BoneInfoMap;
};
