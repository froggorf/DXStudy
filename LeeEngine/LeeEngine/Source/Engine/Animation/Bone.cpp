// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#include "CoreMinimal.h"
#include "Bone.h"

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
	: m_LocalTransform(DirectX::XMMatrixIdentity()), m_Name(name), m_ID(ID)
{
	UINT numPosition = channel->mNumPositionKeys;
	for (int positionIndex = 0; positionIndex < numPosition; ++positionIndex)
	{
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = channel->mPositionKeys[positionIndex].mTime;
		KeyPosition positionData;
		positionData.position = DirectX::XMFLOAT3{aiPosition.x,aiPosition.y,aiPosition.z};
		positionData.timeStamp = timeStamp;
		m_KeyPositions.push_back(positionData);
	}

	UINT numRotation = channel->mNumRotationKeys;
	for (int rotationIndex = 0; rotationIndex < numRotation; ++rotationIndex)
	{
		aiQuaternion aiQuat = channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
		KeyRotation rotationData;
		DirectX::XMFLOAT4 quat = { aiQuat.x,aiQuat.y,aiQuat.z,aiQuat.w };
		rotationData.orientationQuat = DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&quat)) ;
		rotationData.timeStamp = timeStamp;
		m_KeyRotations.push_back(rotationData);
	}

	UINT numScale = channel->mNumScalingKeys;
	for (int scaleIndex = 0; scaleIndex < numScale; ++scaleIndex)
	{
		aiVector3D aiScale = channel->mScalingKeys[scaleIndex].mValue;
		float timeStamp = channel->mScalingKeys[scaleIndex].mTime;
		KeyScale scaleData;
		scaleData.scale = DirectX::XMFLOAT3{ aiScale.x,aiScale.y,aiScale.z };
		scaleData.timeStamp = timeStamp;
		m_KeyScales.push_back(scaleData);
	}
}

void Bone::Update(float animationTime)
{
	DirectX::XMMATRIX translation = InterpolatePosition(animationTime);
	DirectX::XMMATRIX rotation = InterpolateRotation(animationTime);
	DirectX::XMMATRIX scale = InterpolateScale(animationTime);
	m_LocalTransform = scale * rotation * translation;
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
	float scaleFactor = 0.0f;
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;

	return midWayLength / framesDiff;
}

DirectX::XMMATRIX Bone::InterpolatePosition(float animationTime) const
{
	if (1 == m_KeyPositions.size())
	{
		return DirectX::XMMatrixTranslation(m_KeyPositions[0].position.x, m_KeyPositions[0].position.y, m_KeyPositions[0].position.z);
	}

	int p0Index = GetPositionIndex(animationTime);
	int p1Index = (p0Index + 1) % m_KeyPositions.size();
	float scaleFactor = GetScaleFactor(m_KeyPositions[p0Index].timeStamp, m_KeyPositions[p1Index].timeStamp, animationTime);
	DirectX::XMFLOAT3 finalPosition{};
	DirectX::XMVECTOR vecPosition = DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&m_KeyPositions[p0Index].position), DirectX::XMLoadFloat3(&m_KeyPositions[p1Index].position), scaleFactor);
	DirectX::XMStoreFloat3(&finalPosition, vecPosition);
	return DirectX::XMMatrixTranslation(finalPosition.x, finalPosition.y, finalPosition.z);
}

DirectX::XMMATRIX Bone::InterpolateRotation(float animationTime) const
{
	if (1 == m_KeyRotations.size())
	{
		DirectX::XMVECTOR quat = m_KeyRotations[0].orientationQuat;
		return DirectX::XMMatrixRotationQuaternion(quat);
	}
	int p0Index = GetRotationIndex(animationTime);
	int p1Index = (p0Index + 1) % m_KeyRotations.size();
	float scaleFactor = GetScaleFactor(m_KeyRotations[p0Index].timeStamp, m_KeyRotations[p1Index].timeStamp, animationTime);
	DirectX::XMVECTOR finalQuat = DirectX::XMQuaternionSlerp(m_KeyRotations[p0Index].orientationQuat, m_KeyRotations[p1Index].orientationQuat, scaleFactor);
	return DirectX::XMMatrixRotationQuaternion(finalQuat);
}

DirectX::XMMATRIX Bone::InterpolateScale(float animationTime) const
{
	if (1 == m_KeyScales.size())
	{
		return DirectX::XMMatrixScaling(m_KeyScales[0].scale.x, m_KeyScales[0].scale.y, m_KeyScales[0].scale.z);
	}
	int p0Index = GetScaleIndex(animationTime);
	int p1Index = (p0Index + 1) % m_KeyScales.size();
	float scaleFactor = GetScaleFactor(m_KeyScales[p0Index].timeStamp, m_KeyScales[p1Index].timeStamp, animationTime);
	DirectX::XMFLOAT3 finalScale{};
	DirectX::XMVECTOR vecScale = DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&m_KeyScales[p0Index].scale), DirectX::XMLoadFloat3(&m_KeyScales[p1Index].scale), scaleFactor);
	DirectX::XMStoreFloat3(&finalScale, vecScale);
	return DirectX::XMMatrixScaling(finalScale.x, finalScale.y, finalScale.z);
}
