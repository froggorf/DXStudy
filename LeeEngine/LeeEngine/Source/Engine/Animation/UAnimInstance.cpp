#include "CoreMinimal.h"
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#include "UAnimInstance.h"

#include <stack>

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
	//std::string nodeName = node->name;
	//DirectX::XMMATRIX nodeTransform = node->transformation;
	//
	//Bone* bone = Animation->FindBone(nodeName);
	//if (bone)
	//{
	//	bone->Update(CurrentTime);
	//	nodeTransform = bone->GetLocalTransform();
	//}

	//DirectX::XMMATRIX globalTransform = DirectX::XMMatrixMultiply(nodeTransform,parentTransform);

	//auto boneInfoMap = Animation->GetBoneIDMap();
	//if (boneInfoMap.contains(nodeName))
	//{
	//	//globalTransform  = DirectX::XMMatrixMultiply(nodeTransform,parentTransform);
	//	int index = boneInfoMap[nodeName].id;
	//	DirectX::XMMATRIX offset = boneInfoMap[nodeName].offset;
	//	if(index < MAX_BONES)
	//	{
	//		FinalBoneMatrices[index] = DirectX::XMMatrixMultiply(offset,globalTransform);	
	//	}
	//	
	//}
	//else
	//{
	//	for (int i = 0; i < node->childrenCount; ++i)
	//	{
	//		CalculateBoneTransform(Animation, &node->children[i], parentTransform,CurrentTime, FinalBoneMatrices);
	//	}
	//	return;
	//}

	//for (int i = 0; i < node->children.size(); ++i)
	//{
	//	CalculateBoneTransform(Animation, &node->children[i], globalTransform,CurrentTime, FinalBoneMatrices);
	//}

	std::stack<std::pair<const AssimpNodeData*, XMMATRIX>> nodeStack;
	nodeStack.push({node, XMMatrixIdentity()});

	auto boneInfoMap = Animation->GetBoneIDMap();

	while (!nodeStack.empty()) {
		auto [node, parentTransform] = nodeStack.top();
		nodeStack.pop();

		std::string nodeName = node->name;
		XMMATRIX nodeTransform = node->transformation;

		Bone* bone = Animation->FindBone(nodeName);
		if (bone) {
			bone->Update(CurrentTime);
			nodeTransform = bone->GetLocalTransform();
		}

		XMMATRIX globalTransform = XMMatrixMultiply(nodeTransform, parentTransform);

		if (boneInfoMap.contains(nodeName)) {
			int index = boneInfoMap[nodeName].id;
			XMMATRIX offset = boneInfoMap[nodeName].offset;
			if (index < MAX_BONES) {
				FinalBoneMatrices[index] = XMMatrixMultiply(offset, globalTransform);
			}
		}
		else
		{
			for (int i = 0; i < node->children.size(); ++i) {
				nodeStack.push({&node->children[i], parentTransform});
			}
			continue;
		}

		for (int i = 0; i < node->children.size(); ++i) {
			nodeStack.push({&node->children[i], globalTransform});
		}
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