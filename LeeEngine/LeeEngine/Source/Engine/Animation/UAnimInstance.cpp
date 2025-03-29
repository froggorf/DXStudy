#include "CoreMinimal.h"
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#include "UAnimInstance.h"

#include "UAnimSequence.h"
#include "Bone.h"
#include "Engine/Components/USkeletalMeshComponent.h"
#include "Engine/RenderCore/renderingthread.h"

UAnimInstance::UAnimInstance()
{
	DeltaTime = 0.0f;
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
}

void UAnimInstance::Tick(float DeltaSeconds)
{
	NativeUpdateAnimation(DeltaSeconds);
	UpdateAnimation(DeltaSeconds);
}


void UAnimInstance::CalculateBoneTransform(const std::shared_ptr<UAnimSequence>& Animation, const AssimpNodeData* node, DirectX::XMMATRIX parentTransform, float CurrentTime, std::vector<XMMATRIX>& FinalBoneMatrices)
{
	std::string nodeName = node->name;
	DirectX::XMMATRIX nodeTransform = node->transformation;

	Bone* bone = Animation->FindBone(nodeName);
	if (bone)
	{
		bone->Update(CurrentTime);
		nodeTransform = bone->GetLocalTransform();
	}

	DirectX::XMMATRIX globalTransform = parentTransform;
	//DirectX::XMMATRIX globalTransform = DirectX::XMMatrixMultiply(nodeTransform,parentTransform);

	auto boneInfoMap = Animation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		globalTransform  = DirectX::XMMatrixMultiply(nodeTransform,parentTransform);
		int index = boneInfoMap[nodeName].id;
		DirectX::XMMATRIX offset = boneInfoMap[nodeName].offset;
		if(index < MAX_BONES)
		{
			FinalBoneMatrices[index] = DirectX::XMMatrixMultiply(offset,globalTransform);	
		}
		
	}

	for (int i = 0; i < node->childrenCount; ++i)
	{
		CalculateBoneTransform(Animation, &node->children[i], globalTransform,CurrentTime, FinalBoneMatrices);
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