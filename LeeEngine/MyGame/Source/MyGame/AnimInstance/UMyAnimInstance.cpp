
#include "UMyAnimInstance.h"

#include <Engine/RenderCore/EditorScene.h>

#include "../Actor/ATestActor2.h"
#include "Engine/Components/USkeletalMeshComponent.h"



UMyAnimInstance::UMyAnimInstance()
{
	if(std::shared_ptr<USkeletalMesh> PaladinSkeleton = USkeletalMesh::GetSkeletalMesh("SK_Paladin"))
	{
		AnimSequences.push_back(std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin_Idle.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap));

		AnimSequences.push_back(std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin_Walking.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap));
		AnimSequences.push_back(std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin_Running.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap));
		AnimSequences.push_back(std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin_Idle.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap));
		AnimSequences.push_back(std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin_Running.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap));
	}
	

}

void UMyAnimInstance::BeginPlay()
{
	UAnimInstance::BeginPlay();
}

void UMyAnimInstance::NativeInitializeAnimation()
{
	UAnimInstance::NativeInitializeAnimation();
	
	if(AActor* OwnerActor = GetSkeletalMeshComponent()->GetOwner())
	{
		if(ATestActor2* TestActor = dynamic_cast<ATestActor2*>(OwnerActor))
		{
			if(const std::shared_ptr<UTestComponent>& OwnerTestComp = std::dynamic_pointer_cast<UTestComponent>(TestActor->FindComponentByClass("UTestComponent")))
			{
				TestComp = OwnerTestComp;
			}
		}
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	UAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	if(TestComp)
	{
		CurrentSpeed = TestComp->TestValue1;
		
	}
}

void UMyAnimInstance::UpdateAnimation(float dt)
{
	UAnimInstance::UpdateAnimation(dt);
	DeltaTime = dt;
	if(GetSkeletalMeshComponent()&&TestComp)
	{
		const auto& Edges = TestComp->GetCurrentEdge();
		const auto& Triangles = TestComp->GetCurrentTriangles();
		
		std::vector<DirectX::XMMATRIX> FinalBoneMatrices(MAX_BONES, DirectX::XMMatrixIdentity());
		
		CurrentTime = CurrentTime + AnimSequences[0]->GetTicksPerSecond() * dt;




		FScene::UpdateSkeletalMeshAnimation_GameThread(GetSkeletalMeshComponent()->GetPrimitiveID() , FinalBoneMatrices);
	}	
}
