// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#pragma once
#include <DirectXMath.h>
#include <map>
#include <string>
#include <vector>

#include "DirectX/d3dUtil.h"

struct aiNode;
struct aiAnimation;
class Bone;

struct AssimpNodeData
{
	DirectX::XMMATRIX transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;
	Animation(const std::string& animationPath, std::map<std::string, BoneInfo>& modelBoneInfoMap );
	~Animation();
	Bone* FindBone(const std::string& name);

	inline float GetTicksPerSecond() const { return m_TicksPerSecond; }
	inline float GetDuration() const { return m_Duration; }
	inline const AssimpNodeData& GetRootNode() const { return m_RootNode; }
	inline const std::map<std::string, BoneInfo>& GetBoneIDMap() const { return m_BoneInfoMap; }


protected:
private:
	// 누락된 본 데이터 추가 및 모델의 boneInfoMap 업데이트
	void ReadMissingBones(const aiAnimation* animation, std::map<std::string, BoneInfo>& modelBoneInfoMap);
	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);
public:
protected:
private:
	float m_Duration;
	float m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
};
