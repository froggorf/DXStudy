
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
	if(GetSkeletalMeshComponent())
	{

		std::vector<std::vector<XMMATRIX>> AnimsFinalBoneMatrices;
		AnimsFinalBoneMatrices.resize(3);
		for(int i =0; i< 3; ++i)
		{
			for(int j=  0; j< MAX_BONES;++j)
			{
				AnimsFinalBoneMatrices[i].push_back(XMMatrixIdentity());
			}
		}


		

		std::vector<std::vector<float>> AnimClipPos;
		
		AnimClipPos.push_back({0.0f,0.0f});
		AnimClipPos.push_back({150.0f,0.0f});
		AnimClipPos.push_back({600.0f,0.0f});
		AnimClipPos.push_back({800.0f,0.0f});
		AnimClipPos.push_back({1000.0f,0.0f});


		
		std::vector<DirectX::XMMATRIX> FinalBoneMatrices(MAX_BONES);

		CurrentTime = CurrentTime + AnimSequences[0]->GetTicksPerSecond() * dt;
		


		/*const auto& NearestIter = Distance.begin();
		float A = min(AnimClipPos[Distance.begin()->Index][0],AnimClipPos[(++Distance.begin())->Index][0]);
		float B = max(AnimClipPos[Distance.begin()->Index][0],AnimClipPos[(++Distance.begin())->Index][0]);
		float t = std::clamp((CurrentSpeed - A) / (B-A), 0.0f, 1.0f);

		for (int index = 0; index < MAX_BONES; ++index)
		{
			XMVECTOR r1 = XMVectorLerp(AnimsFinalBoneMatrices[0][index].r[0], AnimsFinalBoneMatrices[1][index].r[0], t);
			XMVECTOR r2 = XMVectorLerp(AnimsFinalBoneMatrices[0][index].r[1], AnimsFinalBoneMatrices[1][index].r[1], t);
			XMVECTOR r3 = XMVectorLerp(AnimsFinalBoneMatrices[0][index].r[2], AnimsFinalBoneMatrices[1][index].r[2], t);
			XMVECTOR r4 = XMVectorLerp(AnimsFinalBoneMatrices[0][index].r[3], AnimsFinalBoneMatrices[1][index].r[3], t);
			FinalBoneMatrices[index] = XMMATRIX(r1, r2, r3, r4);
		}*/
		
		FScene::UpdateSkeletalMeshAnimation_GameThread(GetSkeletalMeshComponent()->GetPrimitiveID() , FinalBoneMatrices);
	}	
}
