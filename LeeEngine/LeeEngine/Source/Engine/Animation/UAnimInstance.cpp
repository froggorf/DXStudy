#include "CoreMinimal.h"
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#include "UAnimInstance.h"

#include "UAnimSequence.h"
#include "Bone.h"
#include "Engine/Components/USkeletalMeshComponent.h"
#include "Engine/RenderCore/renderingthread.h"

UAnimInstance::UAnimInstance()
{
	CurrentAnimation=nullptr;
	CurrentTime = 0.0f;
	DeltaTime = 0.0f;
	FinalBoneMatrices.reserve(MAX_BONES);

	for(int i = 0; i < MAX_BONES; ++i)
	{
		FinalBoneMatrices.push_back(DirectX::XMMatrixIdentity());
	}
}


void UAnimInstance::BeginPlay()
{
	UObject::BeginPlay();

	NativeInitializeAnimation();
}

void UAnimInstance::NativeInitializeAnimation()
{
}

void UAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
}

void UAnimInstance::UpdateAnimation(float dt)
{
	//DeltaTime = dt;
	//if(CurrentAnimation && CurrentSkeletalMeshComponent)
	//{
	//	CurrentTime = CurrentTime + CurrentAnimation->GetTicksPerSecond() * dt;
	//	CurrentTime = fmod(CurrentTime, CurrentAnimation->GetDuration());
	//	CalculateBoneTransform(&CurrentAnimation->GetRootNode(), DirectX::XMMatrixIdentity());
	//
	//	FScene::UpdateSkeletalMeshAnimation_GameThread(CurrentSkeletalMeshComponent->GetPrimitiveID() , GetFinalBoneMatrices());
	//}	
	
	

}

void UAnimInstance::Tick(float DeltaSeconds)
{
	NativeUpdateAnimation(DeltaSeconds);
	UpdateAnimation(DeltaSeconds);
}

void UAnimInstance::PlayAnimation(const std::shared_ptr<UAnimSequence>& InAnimation)
{
	CurrentAnimation = InAnimation;
	CurrentTime = 0.0f;
}


void UAnimInstance::CalculateBoneTransform(const AssimpNodeData* node, DirectX::XMMATRIX parentTransform)
{
	std::string nodeName = node->name;
	DirectX::XMMATRIX nodeTransform = node->transformation;

	Bone* bone = CurrentAnimation->FindBone(nodeName);
	if (bone)
	{
		bone->Update(CurrentTime);
		nodeTransform = bone->GetLocalTransform();
	}

	DirectX::XMMATRIX globalTransform = parentTransform;
	//DirectX::XMMATRIX globalTransform = DirectX::XMMatrixMultiply(nodeTransform,parentTransform);

	auto boneInfoMap = CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		globalTransform  = DirectX::XMMatrixMultiply(nodeTransform,parentTransform);
		int index = boneInfoMap[nodeName].id;
		DirectX::XMMATRIX offset = boneInfoMap[nodeName].offset;
		if(index < MAX_BONES)
		{
			FinalBoneMatrices[index] =  DirectX::XMMatrixMultiply(offset,globalTransform);	
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
	CurrentAnimation = InAnimation;	
	CurrentTime = 0.0f;


	for(int i = 0; i < MAX_BONES; ++i)
	{
		FinalBoneMatrices[i] = XMMatrixIdentity();
	}
}


AActor* UAnimInstance::TryGetPawnOwner() const
{
	if (CurrentSkeletalMeshComponent)
	{
		return CurrentSkeletalMeshComponent->GetOwner();
	}
	return nullptr;
}