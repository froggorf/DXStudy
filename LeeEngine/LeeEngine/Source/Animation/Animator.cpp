// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#include "Animator.h"

#include "Animation.h"
#include "Bone.h"
#include "Engine/RenderCore/renderingthread.h"

Animator::Animator()
{
	m_CurrentAnimation=nullptr;
	m_CurrentTime = 0.0f;
	m_DeltaTime = 0.0f;
	m_FinalBoneMatrices.reserve(100);

	for(int i = 0; i < 100; ++i)
	{
		m_FinalBoneMatrices.push_back(DirectX::XMMatrixIdentity());
	}
}

Animator::Animator(Animation* animation)

{
	m_CurrentAnimation = animation;
	m_CurrentTime = 0.0f;
	m_DeltaTime = 0.0f;
	m_FinalBoneMatrices.reserve(100);

	for(int i = 0; i < 100; ++i)
	{
		m_FinalBoneMatrices.push_back(DirectX::XMMatrixIdentity());
	}
}

Animator::~Animator()
{
}

void Animator::UpdateAnimation(float dt, float OwnerPrimitiveID)
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

void Animator::PlayAnimation(Animation* pAnimation)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, DirectX::XMMATRIX parentTransform)
{
	std::string nodeName = node->name;
	DirectX::XMMATRIX nodeTransform = node->transformation;

	Bone* bone = m_CurrentAnimation->FindBone(nodeName);
	if (bone)
	{
		bone->Update(m_CurrentTime);
		nodeTransform = bone->GetLocalTransform();
	}

	DirectX::XMMATRIX globalTransform = DirectX::XMMatrixMultiply(nodeTransform,parentTransform);

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		DirectX::XMMATRIX offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] =  DirectX::XMMatrixMultiply(offset,globalTransform);
	}
	

	for (int i = 0; i < node->childrenCount; ++i)
	{
		CalculateBoneTransform(&node->children[i], globalTransform);
	}
}

void Animator::SetAnimation(Animation* InAnimation)
{
	m_CurrentTime = 0.0f;
	m_CurrentAnimation = InAnimation;

	for(int i = 0; i < 100; ++i)
	{
		m_FinalBoneMatrices[i] = XMMatrixIdentity();
	}
}


