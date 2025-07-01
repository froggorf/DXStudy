#include "CoreMinimal.h"
#include "UMyAnimInstance.h"

#include <Engine/RenderCore/EditorScene.h>

#include "../Actor/ATestActor2.h"
#include "../Actor/ATestCube2.h"
#include "../Actor/ATestPawn.h"
#include "Engine/Components/USkeletalMeshComponent.h"

UMyAnimInstance* UMyAnimInstance::MyAnimInstance;

UMyAnimInstance::UMyAnimInstance()
{
	AssetManager::GetAsyncAssetCache("BS_MyUEFN_LocomotionRPG", [this](std::shared_ptr<UObject> Object)
		{
			BS_MyUEFN_Locomotion = std::dynamic_pointer_cast<UBlendSpace>(Object);
		});

	AssetManager::GetAsyncAssetCache("AS_MyUEFN_Idle", [this](std::shared_ptr<UObject> Object)
		{
			AS_Test0 = std::dynamic_pointer_cast<UAnimSequence>(Object);
		});
	AssetManager::GetAsyncAssetCache("AS_Aim", [this](std::shared_ptr<UObject> Object)
		{
			AS_Test1 = std::dynamic_pointer_cast<UAnimSequence>(Object);
		});
	AssetManager::GetAsyncAssetCache("AS_Pistol", [this](std::shared_ptr<UObject> Object)
		{
			AS_Test2 = std::dynamic_pointer_cast<UAnimSequence>(Object);
		});

}

void UMyAnimInstance::BeginPlay()
{
	UAnimInstance::BeginPlay();
}

void UMyAnimInstance::NativeInitializeAnimation()
{
	UAnimInstance::NativeInitializeAnimation();

	if (AActor* OwnerActor = GetSkeletalMeshComponent()->GetOwner())
	{
		if (auto TestActor = dynamic_cast<ATestPawn*>(OwnerActor))
		{
			if (const std::shared_ptr<UTestComponent>& OwnerTestComp = std::dynamic_pointer_cast<UTestComponent>(TestActor->FindComponentByClass("UTestComponent")))
			{
				TestComp = OwnerTestComp;
			}
		}
	}
	MyAnimInstance = this;
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	UAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	if (TestComp)
	{
	}
}

void UMyAnimInstance::UpdateAnimation(float dt)
{
	UAnimInstance::UpdateAnimation(dt);

	if (GetSkeletalMeshComponent() && TestComp&& BS_MyUEFN_Locomotion  && AS_Test0 && AS_Test1 && AS_Test2)
	{
		std::vector<XMMATRIX>         FinalBoneMatrices(MAX_BONES, XMMatrixIdentity());
		std::vector<FAnimNotifyEvent> FinalNotifies;

		// BlendSpace_Locomotion
		std::vector<XMMATRIX> BS_IdleWalkRunMatrices(MAX_BONES, XMMatrixIdentity());

		BS_MyUEFN_Locomotion->GetAnimationBoneMatrices(XMFLOAT2{0.0f, TestComp->TestSpeed}, CurrentTime, BS_IdleWalkRunMatrices, FinalNotifies);

		// 애니메이션 시퀀스 계산
		std::vector<XMMATRIX> AS_Matrices(MAX_BONES, XMMatrixIdentity());
		switch (TestComp->TargetAnim)
		{
		case 0:
			AS_Test0->GetBoneTransform(CurrentTime, AS_Matrices);	
			break;
		case 1:
			AS_Test1->GetBoneTransform(CurrentTime, AS_Matrices);
			break;
		case 2:
			AS_Test2->GetBoneTransform(CurrentTime, AS_Matrices);
			break;
		default:
			break;
		}
		

		// 레이어 블렌딩
		std::vector<XMMATRIX> ResultMatrices(MAX_BONES, XMMatrixIdentity());
		LayeredBlendPerBone(BS_IdleWalkRunMatrices, AS_Matrices, "spine_01", 1.0f, ResultMatrices);

		// 몽타쥬 연결
		{
			std::string SlotName = "DefaultSlot";
			PlayMontage(SlotName, ResultMatrices, FinalNotifies);
		}

		for (int i = 0; i < FinalNotifies.size(); ++i)
		{
			FinalNotifies[i].Notify->Notify();
		}


		FScene::UpdateSkeletalMeshAnimation_GameThread(GetSkeletalMeshComponent()->GetPrimitiveID(), ResultMatrices);

		// 06.30 Note : 마지막에 해당 코드를 꼭 넣어줘야지만 GetSocketTransform 등을 사용할 수 있음
		// 다만 해당부분은 작성하는데 놓칠 수 있으므로 추후 반환값을 ResultMatrices를 반환하게 하여 상위클래스에서 자동으로 받는다던가 하는식으로 수정이 가능할것으로 보임
		for (UINT i = 0; i < MAX_BONES; ++i)
		{
			 LastFrameAnimMatrices[i]= ResultMatrices[i];
		}
	}
}
