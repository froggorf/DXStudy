// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#pragma once
#include "CoreMinimal.h"

struct aiNodeAnim;

struct KeyPosition
{
	XMFLOAT3 position;
	float    timeStamp;
};

struct KeyRotation
{
	XMVECTOR orientationQuat;
	float    timeStamp;
};

struct KeyScale
{
	XMFLOAT3 scale;
	float    timeStamp;
};

struct FBoneLocalTransform
{
	XMVECTOR Translation;
	XMVECTOR Rotation;
	XMVECTOR Scale;
};

class Bone
{
public:
	Bone(const std::string& name, int ID, const aiNodeAnim* channel);
	void Update(float animationTime);

	const FBoneLocalTransform& GetLocalTransform() const
	{
		return m_LocalTransform;
	}

	std::string GetBoneName() const
	{
		return m_Name;
	}

	int GetBoneID() const
	{
		return m_ID;
	}

	// animation time에 맞는 m_KeyXXX의 index를 반환
	int GetPositionIndex(float animationTime) const;
	int GetRotationIndex(float animationTime) const;
	int GetScaleIndex(float animationTime) const;

private:
	// lerp, Slerp 등에 사용되는 factor 반환
	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;
	// 선형 보간 후 Translation Matrix 반환
	XMVECTOR InterpolatePosition(float animationTime) const;
	// 구형 보간 후 Rotation Matrix 반환
	XMVECTOR InterpolateRotation(float animationTime) const;
	// 선형 보간 후 Scale Matrix 반환
	XMVECTOR InterpolateScale(float animationTime) const;

	std::vector<KeyPosition> m_KeyPositions;
	std::vector<KeyRotation> m_KeyRotations;
	std::vector<KeyScale>    m_KeyScales;

	FBoneLocalTransform m_LocalTransform;
	std::string m_Name;
	int         m_ID;
};
