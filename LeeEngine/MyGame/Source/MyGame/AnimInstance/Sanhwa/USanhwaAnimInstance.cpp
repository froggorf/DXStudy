#include "CoreMinimal.h"
#include "USanhwaAnimInstance.h"

#include "Engine/AssetManager/AssetManager.h"
#include "MyGame/Character/AMyGameCharacterBase.h"
#include "MyGame/Component/MotionWarping/UMotionWarpingComponent.h"

USanhwaAnimInstance::USanhwaAnimInstance()
{
	BS_LocomotionName = "BS_UE5MM_Locomotion";

	//for (int i = 0; i < 5; ++i)
	//{
	//	std::string Name = "AS_Sanhwa_BasicAttack" + std::to_string(i);
	//	AssetManager::GetAsyncAssetCache(Name,[this, i](std::shared_ptr<UObject> Object)
	//		{
	//			AS_Test[i] = std::dynamic_pointer_cast<UAnimSequence>(Object);

	//		});	
	//}
	
}

void USanhwaAnimInstance::BeginPlay()
{
	UMyGameAnimInstanceBase::BeginPlay();

	NotifyEvent["MotionWarpingEnd"] = Delegate<>{};
	NotifyEvent["MotionWarpingEnd"].Add(static_cast<UMyGameAnimInstanceBase*>(this), &UMyGameAnimInstanceBase::EndMotionWarping);

	NotifyEvent["SH_Attack0"] = Delegate<>{};
	NotifyEvent["SH_Attack0"].Add(this, &USanhwaAnimInstance::MotionWarping_BasicAttack0);

	NotifyEvent["SH_Attack1"] = Delegate<>{};
	NotifyEvent["SH_Attack1"].Add(this, &USanhwaAnimInstance::MotionWarping_BasicAttack1);

	NotifyEvent["SH_Attack2"] = Delegate<>{};
	NotifyEvent["SH_Attack2"].Add(this, &USanhwaAnimInstance::MotionWarping_BasicAttack2);

	NotifyEvent["SH_Attack3"] = Delegate<>{};
	NotifyEvent["SH_Attack3"].Add(this, &USanhwaAnimInstance::MotionWarping_BasicAttack3);

	NotifyEvent["SH_Attack4"] = Delegate<>{};
	NotifyEvent["SH_Attack4"].Add(this, &USanhwaAnimInstance::MotionWarping_BasicAttack4);
}

bool USanhwaAnimInstance::IsAllResourceOK()
{
	return UMyGameAnimInstanceBase::IsAllResourceOK();// && AS_Test[0] && AS_Test[1] && AS_Test[2]&& AS_Test[3]&& AS_Test[4];
}

void USanhwaAnimInstance::MotionWarping_BasicAttack0()
{
	if (SetMotionWarping())
	{
		if (const std::shared_ptr<UMotionWarpingComponent>& MotionWarpingComp = MyGameCharacter->GetMotionWarpingComponent())
		{
			XMFLOAT3 CurActorLocation = MyGameCharacter->GetActorLocation();
			XMFLOAT3 ForwardVector = MyGameCharacter->GetActorForwardVector();
			
			MotionWarpingComp->SetTargetLocation(CurActorLocation + ForwardVector * 100, 1.0f);
		}
	}
}

void USanhwaAnimInstance::MotionWarping_BasicAttack1()
{
	if (SetMotionWarping())
	{
		if (const std::shared_ptr<UMotionWarpingComponent>& MotionWarpingComp = MyGameCharacter->GetMotionWarpingComponent())
		{
			XMFLOAT3 CurActorLocation = MyGameCharacter->GetActorLocation();
			XMFLOAT3 ForwardVector = MyGameCharacter->GetActorForwardVector();

			MotionWarpingComp->SetTargetLocation(CurActorLocation + ForwardVector * 100, 1.0f);
		}
	}
}

void USanhwaAnimInstance::MotionWarping_BasicAttack2()
{
	if (SetMotionWarping())
	{
		if (const std::shared_ptr<UMotionWarpingComponent>& MotionWarpingComp = MyGameCharacter->GetMotionWarpingComponent())
		{
			XMFLOAT3 CurActorLocation = MyGameCharacter->GetActorLocation();
			XMFLOAT3 ForwardVector = MyGameCharacter->GetActorForwardVector();

			MotionWarpingComp->SetTargetLocation(CurActorLocation + ForwardVector * 150, 1.0f);
		}
	}
}


void USanhwaAnimInstance::MotionWarping_BasicAttack3()
{
	//if (SetMotionWarping())
	//{
	//	if (const std::shared_ptr<UMotionWarpingComponent>& MotionWarpingComp = MyGameCharacter->GetMotionWarpingComponent())
	//	{
	//		XMFLOAT3 CurActorLocation = MyGameCharacter->GetActorLocation();
	//		XMFLOAT3 ForwardVector = MyGameCharacter->GetActorForwardVector();

	//		MotionWarpingComp->SetTargetLocation(CurActorLocation + ForwardVector * 0, 0.5f);
	//	}
	//}
}

void USanhwaAnimInstance::MotionWarping_BasicAttack4()
{
	if (SetMotionWarping())
	{
		if (const std::shared_ptr<UMotionWarpingComponent>& MotionWarpingComp = MyGameCharacter->GetMotionWarpingComponent())
		{
			XMFLOAT3 CurActorLocation = MyGameCharacter->GetActorLocation();
			XMFLOAT3 UpVector = XMFLOAT3{0,1,0};
			XMFLOAT3 Forward = MyGameCharacter->GetActorForwardVector();
			
			MotionWarpingComp->SetTargetLocation(CurActorLocation + UpVector * 300 + Forward*300, 1.0f);
		}
	}
}


void USanhwaAnimInstance::UpdateAnimation(float dt)
{
	UMyGameAnimInstanceBase::UpdateAnimation(dt);
}

