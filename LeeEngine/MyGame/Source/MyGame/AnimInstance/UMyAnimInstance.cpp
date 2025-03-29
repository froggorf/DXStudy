#include "UMyAnimInstance.h"

#include <Engine/RenderCore/EditorScene.h>

#include "../Actor/ATestActor2.h"
#include "Engine/Components/USkeletalMeshComponent.h"



UMyAnimInstance::UMyAnimInstance()
{
	if(std::shared_ptr<USkeletalMesh> PaladinSkeleton = USkeletalMesh::GetSkeletalMesh("SK_Paladin"))
	{
		TestAnim1 = std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin_Walking.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap);
		TestAnim2 = std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin_Running.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap);
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
	if(TestAnim1 && TestAnim2 && GetSkeletalMeshComponent())
	{
		CurrentTime = CurrentTime + TestAnim1->GetTicksPerSecond() * dt;
		float TestAnim1Time = fmod(CurrentTime, TestAnim1->GetDuration());
		float TestAnim2Time = fmod(CurrentTime, TestAnim2->GetDuration());

		std::vector<DirectX::XMMATRIX> Anim1FinalBoneMatrices(MAX_BONES);
		CalculateBoneTransform(TestAnim1,&TestAnim1->GetRootNode(), DirectX::XMMatrixIdentity(), TestAnim1Time, Anim1FinalBoneMatrices);
		
		std::vector<DirectX::XMMATRIX> Anim2FinalBoneMatrices(MAX_BONES);
		CalculateBoneTransform(TestAnim2,&TestAnim2->GetRootNode(), DirectX::XMMatrixIdentity(),TestAnim2Time,Anim2FinalBoneMatrices);

		std::vector<DirectX::XMMATRIX> FinalBoneMatrices(MAX_BONES);
		float t = std::clamp(CurrentSpeed/600.0f, 0.0f,1.0f);
		for (int index = 0; index < FinalBoneMatrices.size(); ++index)
		{
			XMVECTOR r1 = XMVectorLerp(Anim1FinalBoneMatrices[index].r[0], Anim2FinalBoneMatrices[index].r[0], t);
			XMVECTOR r2 = XMVectorLerp(Anim1FinalBoneMatrices[index].r[1], Anim2FinalBoneMatrices[index].r[1], t);
			XMVECTOR r3 = XMVectorLerp(Anim1FinalBoneMatrices[index].r[2], Anim2FinalBoneMatrices[index].r[2], t);
			XMVECTOR r4 = XMVectorLerp(Anim1FinalBoneMatrices[index].r[3], Anim2FinalBoneMatrices[index].r[3], t);
			FinalBoneMatrices[index] = XMMATRIX(r1, r2, r3, r4);
		}
		
		FScene::UpdateSkeletalMeshAnimation_GameThread(GetSkeletalMeshComponent()->GetPrimitiveID() , FinalBoneMatrices);
	}	
}
