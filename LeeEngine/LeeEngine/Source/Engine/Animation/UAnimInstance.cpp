#include "CoreMinimal.h"
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#include "UAnimInstance.h"

#include "UAnimSequence.h"
#include "Bone.h"
#include "Engine/RenderCore/renderingthread.h"

UAnimInstance::UAnimInstance()
{
	m_CurrentAnimation=nullptr;
	m_CurrentTime = 0.0f;
	m_DeltaTime = 0.0f;
	m_FinalBoneMatrices.reserve(MAX_BONES);

	for(int i = 0; i < MAX_BONES; ++i)
	{
		m_FinalBoneMatrices.push_back(DirectX::XMMatrixIdentity());
	}
}

UAnimInstance::UAnimInstance(const std::shared_ptr<UAnimSequence>& InAnimation)
{
	m_CurrentAnimation = InAnimation;
	m_CurrentTime = 0.0f;
	m_DeltaTime = 0.0f;
	m_FinalBoneMatrices.reserve(MAX_BONES);

	for(int i = 0; i < MAX_BONES; ++i)
	{
		m_FinalBoneMatrices.push_back(DirectX::XMMatrixIdentity());
	}
}

void UAnimInstance::UpdateAnimation(float dt, float OwnerPrimitiveID)
{
	m_DeltaTime = dt;
	if(m_CurrentAnimation)
	{
		m_CurrentTime = m_CurrentTime + m_CurrentAnimation->GetTicksPerSecond() * dt;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), DirectX::XMMatrixIdentity());

		FScene::UpdateSkeletalMeshAnimation_GameThread(OwnerPrimitiveID, GetFinalBoneMatrices());
	}

}

void UAnimInstance::PlayAnimation(const std::shared_ptr<UAnimSequence>& InAnimation)
{
	m_CurrentAnimation = InAnimation;
	m_CurrentTime = 0.0f;
}


void UAnimInstance::CalculateBoneTransform(const AssimpNodeData* node, DirectX::XMMATRIX parentTransform)
{
	std::string nodeName = node->name;
	DirectX::XMMATRIX nodeTransform = node->transformation;

	Bone* bone = m_CurrentAnimation->FindBone(nodeName);
	if (bone)
	{
		bone->Update(m_CurrentTime);
		nodeTransform = bone->GetLocalTransform();
	}

	DirectX::XMMATRIX globalTransform = parentTransform;
	//DirectX::XMMATRIX globalTransform = DirectX::XMMatrixMultiply(nodeTransform,parentTransform);

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		globalTransform  = DirectX::XMMatrixMultiply(nodeTransform,parentTransform);
		int index = boneInfoMap[nodeName].id;
		DirectX::XMMATRIX offset = boneInfoMap[nodeName].offset;
		if(index < MAX_BONES)
		{
			m_FinalBoneMatrices[index] =  DirectX::XMMatrixMultiply(offset,globalTransform);	
		}
		
	}

	for (int i = 0; i < node->childrenCount; ++i)
	{
		CalculateBoneTransform(&node->children[i], globalTransform);
	}
}

void UAnimInstance::SetAnimation(const std::shared_ptr<UAnimSequence>& InAnimation, float InBlendTime)
{
	if (InBlendTime > FLT_EPSILON)
	{
		// 블렌딩에 대한 변수들 조정

	}
	m_CurrentAnimation = InAnimation;	
	m_CurrentTime = 0.0f;


	for(int i = 0; i < MAX_BONES; ++i)
	{
		m_FinalBoneMatrices[i] = XMMatrixIdentity();
	}
}


