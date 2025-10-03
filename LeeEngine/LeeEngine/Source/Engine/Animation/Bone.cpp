// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#include "CoreMinimal.h"
#include "Bone.h"

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
	: m_LocalTransform(XMMatrixIdentity()), m_Name(name), m_ID(ID)
{
	UINT numPosition = channel->mNumPositionKeys;
	for (UINT positionIndex = 0; positionIndex < numPosition; ++positionIndex)
	{
		aiVector3D  aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float       timeStamp  = static_cast<float>(channel->mPositionKeys[positionIndex].mTime);
		KeyPosition positionData;
		positionData.position  = XMFLOAT3{aiPosition.x, aiPosition.y, aiPosition.z};
		positionData.timeStamp = timeStamp;
		m_KeyPositions.push_back(positionData);
	}

	UINT numRotation = channel->mNumRotationKeys;
	for (UINT rotationIndex = 0; rotationIndex < numRotation; ++rotationIndex)
	{
		aiQuaternion aiQuat    = channel->mRotationKeys[rotationIndex].mValue;
		float        timeStamp = static_cast<float>(channel->mRotationKeys[rotationIndex].mTime);
		KeyRotation  rotationData;
		XMFLOAT4     quat            = {aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w};
		rotationData.orientationQuat = XMVector4Normalize(XMLoadFloat4(&quat));
		rotationData.timeStamp       = timeStamp;
		m_KeyRotations.push_back(rotationData);
	}

	UINT numScale = channel->mNumScalingKeys;
	for (UINT scaleIndex = 0; scaleIndex < numScale; ++scaleIndex)
	{
		aiVector3D aiScale   = channel->mScalingKeys[scaleIndex].mValue;
		float      timeStamp = static_cast<float>(channel->mScalingKeys[scaleIndex].mTime);
		KeyScale   scaleData;
		scaleData.scale     = XMFLOAT3{aiScale.x, aiScale.y, aiScale.z};
		scaleData.timeStamp = timeStamp;
		m_KeyScales.push_back(scaleData);
	}
}

void Bone::Update(float animationTime)
{
	m_LocalTransform.Translation = InterpolatePosition(animationTime);
	m_LocalTransform.Rotation    = InterpolateRotation(animationTime);
	m_LocalTransform.Scale       = InterpolateScale(animationTime);
}

int Bone::GetPositionIndex(float animationTime) const
{
	for (int index = 0; index < m_KeyPositions.size() - 1; ++index)
	{
		if (animationTime < m_KeyPositions[index + 1].timeStamp)
			return index;
	}
	// 일부 애니메이션의 경우 타임스탬프의 시간을 넘는 경우가 있음, 해당 경우에는 마지막 인덱스를 반환
	assert(0);
	return -1;
}

int Bone::GetRotationIndex(float animationTime) const
{
	for (int index = 0; index < m_KeyRotations.size() - 1; ++index)
	{
		if (animationTime < m_KeyRotations[index + 1].timeStamp)
			return index;
	}
	assert(0);
	return -1;
}

int Bone::GetScaleIndex(float animationTime) const
{
	for (int index = 0; index < m_KeyScales.size() - 1; ++index)
	{
		if (animationTime < m_KeyScales[index + 1].timeStamp)
			return index;
	}
	assert(0);
	return -1;
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const
{
	float scaleFactor  = 0.0f;
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff   = nextTimeStamp - lastTimeStamp;

	return midWayLength / framesDiff;
}

XMVECTOR Bone::InterpolatePosition(float animationTime) const
{
	if (1 == m_KeyPositions.size())
	{
		return XMVectorSet(m_KeyPositions[0].position.x, m_KeyPositions[0].position.y, m_KeyPositions[0].position.z,0.0f);
	}

	int      p0Index     = GetPositionIndex(animationTime);
	int      p1Index     = (p0Index + 1) % m_KeyPositions.size();
	float    scaleFactor = GetScaleFactor(m_KeyPositions[p0Index].timeStamp, m_KeyPositions[p1Index].timeStamp, animationTime);
	XMVECTOR vecPosition = XMVectorLerp(XMLoadFloat3(&m_KeyPositions[p0Index].position), XMLoadFloat3(&m_KeyPositions[p1Index].position), scaleFactor);
	return vecPosition;
}

XMVECTOR Bone::InterpolateRotation(float animationTime) const
{
	if (1 == m_KeyRotations.size())
	{
		XMVECTOR quat = m_KeyRotations[0].orientationQuat;
		return quat;
	}
	int      p0Index     = GetRotationIndex(animationTime);
	int      p1Index     = (p0Index + 1) % m_KeyRotations.size();
	float    scaleFactor = GetScaleFactor(m_KeyRotations[p0Index].timeStamp, m_KeyRotations[p1Index].timeStamp, animationTime);
	XMVECTOR finalQuat   = XMQuaternionSlerp(m_KeyRotations[p0Index].orientationQuat, m_KeyRotations[p1Index].orientationQuat, scaleFactor);
	return finalQuat;
}

XMVECTOR Bone::InterpolateScale(float animationTime) const
{
	if (1 == m_KeyScales.size())
	{
		return XMVectorSet(m_KeyScales[0].scale.x, m_KeyScales[0].scale.y, m_KeyScales[0].scale.z, 0.0f);
	}
	int      p0Index     = GetScaleIndex(animationTime);
	int      p1Index     = (p0Index + 1) % m_KeyScales.size();
	float    scaleFactor = GetScaleFactor(m_KeyScales[p0Index].timeStamp, m_KeyScales[p1Index].timeStamp, animationTime);
	XMVECTOR vecScale = XMVectorLerp(XMLoadFloat3(&m_KeyScales[p0Index].scale), XMLoadFloat3(&m_KeyScales[p1Index].scale), scaleFactor);
	return vecScale;
}
