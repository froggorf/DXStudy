
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
		UpdateAnimation(DeltaSeconds);
		
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

		XMFLOAT2 CurrentValue;

		std::vector<std::shared_ptr<FAnimClipPoint>> Points;
		TestComp->GetAnimsForBlend(CurrentValue,Points);

		if(Points.size() == 3)
		{
			TriangleInterpolation(CurrentValue, Points[0],Points[1],Points[2],FinalBoneMatrices);
		}

		FScene::UpdateSkeletalMeshAnimation_GameThread(GetSkeletalMeshComponent()->GetPrimitiveID() , FinalBoneMatrices);
	}	
}

void UMyAnimInstance::TriangleInterpolation(const XMFLOAT2& CurrentValue, const std::shared_ptr<FAnimClipPoint>& P1,
	const std::shared_ptr<FAnimClipPoint>& P2, const std::shared_ptr<FAnimClipPoint>& P3,
	std::vector<XMMATRIX>& AnimMatrices)
{
	XMVECTOR CurrentValueVector = XMVectorSet(CurrentValue.x,CurrentValue.y,0.0f,0.0f);
	XMVECTOR P1Vector = XMVectorSet(P1->Position.x,P1->Position.y,0.0f,0.0f);
	XMVECTOR P2Vector = XMVectorSet(P2->Position.x,P2->Position.y,0.0f,0.0f);
	XMVECTOR P3Vector = XMVectorSet(P3->Position.x,P3->Position.y,0.0f,0.0f);
	

	std::vector<XMMATRIX> P1AnimMatrices(MAX_BONES);
	CalculateBoneTransform(P1->AnimSequence, &P1->AnimSequence->GetRootNode(), XMMatrixIdentity(), fmod(CurrentTime, P1->AnimSequence->GetDuration()), P1AnimMatrices);

	std::vector<XMMATRIX> P2AnimMatrices(MAX_BONES);
	CalculateBoneTransform(P2->AnimSequence, &P2->AnimSequence->GetRootNode(), XMMatrixIdentity(), fmod(CurrentTime, P2->AnimSequence->GetDuration()), P2AnimMatrices);

	std::vector<XMMATRIX> P3AnimMatrices(MAX_BONES);
	CalculateBoneTransform(P3->AnimSequence, &P3->AnimSequence->GetRootNode(), XMMatrixIdentity(), fmod(CurrentTime, P3->AnimSequence->GetDuration()), P3AnimMatrices);

	float P1LengthEst = XMVectorGetX(XMVector2LengthEst(XMVectorSubtract(P1Vector, CurrentValueVector)));
	float P2LengthEst = XMVectorGetX(XMVector2LengthEst(XMVectorSubtract(P2Vector, CurrentValueVector)));
	float P3LengthEst = XMVectorGetX(XMVector2LengthEst(XMVectorSubtract(P3Vector, CurrentValueVector)));
	float P1Weight = 1 - P1LengthEst/(P1LengthEst+P2LengthEst+P3LengthEst);
	float P2Weight = 1 - P2LengthEst/(P1LengthEst+P2LengthEst+P3LengthEst);
	float P3Weight = 1 - P1Weight-P2Weight;
	for(int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
	{
		for(int VectorIndex = 0; VectorIndex < 4; ++VectorIndex)
		{
			XMVECTOR WeightedP1 = XMVectorScale(P1AnimMatrices[BoneIndex].r[VectorIndex], P1Weight);
			XMVECTOR WeightedP2 = XMVectorScale(P2AnimMatrices[BoneIndex].r[VectorIndex], P2Weight);
			XMVECTOR WeightedP3 = XMVectorScale(P3AnimMatrices[BoneIndex].r[VectorIndex], P3Weight);
			AnimMatrices[BoneIndex].r[VectorIndex]= XMVectorAdd(XMVectorAdd(WeightedP1,WeightedP2),WeightedP3);
		}
	}
}
