#include "CoreMinimal.h"
#include "UGideonAnimInstance.h"

#include "MyGame/Character/Player/AGideonCharacter.h"
#include "MyGame/Component/Combat/Range/UGideonCombatComponent.h"
#include "MyGame/Component/Combat/Skill/NormalSkill/Gideon/UGideonSkillComponent.h"

std::shared_ptr<UAnimMontage> UGideonAnimInstance::Cache_AM_Ult_Loop1;
std::shared_ptr<UAnimMontage> UGideonAnimInstance::Cache_AM_Ult_Loop2;
std::shared_ptr<UAnimMontage> UGideonAnimInstance::Cache_AM_Ult_End;
std::shared_ptr<UBlendSpace> UGideonAnimInstance::Cache_BS_Gideon_FPS_Loco;

UGideonAnimInstance::UGideonAnimInstance()
{
	BS_LocomotionName = "BS_Gideon_Locomotion";
}

void UGideonAnimInstance::LoadData_OnRegister()
{
	UMyGameAnimInstanceBase::LoadData_OnRegister();

	// BS_FPSLocomotion
	if (!Cache_BS_Gideon_FPS_Loco)
	{
		AssetManager::GetAsyncAssetCache("BS_Gideon_FPSLocomotion", [this](std::shared_ptr<UObject> Object)
			{
				Cache_BS_Gideon_FPS_Loco = std::dynamic_pointer_cast<UBlendSpace>(Object);
				BS_FPSLocomotion = Cache_BS_Gideon_FPS_Loco;
			});	
	}
	else
	{
		BS_FPSLocomotion = Cache_BS_Gideon_FPS_Loco;
	}

	// AM_UltLoop1
	if (!Cache_AM_Ult_Loop1)
	{
		AssetManager::GetAsyncAssetCache("AM_Gideon_Ult_Loop1", [this](std::shared_ptr<UObject> Object)
			{
				Cache_AM_Ult_Loop1 = std::dynamic_pointer_cast<UAnimMontage>(Object);
				AM_UltLoop1 = Cache_AM_Ult_Loop1;
			});	
	}
	else
	{
		AM_UltLoop1 = Cache_AM_Ult_Loop1;
	}
	// AM_UltLoop2
	if (!Cache_AM_Ult_Loop2)
	{
		AssetManager::GetAsyncAssetCache("AM_Gideon_Ult_Loop2", [this](std::shared_ptr<UObject> Object)
			{
				Cache_AM_Ult_Loop2 = std::dynamic_pointer_cast<UAnimMontage>(Object);
				AM_UltLoop2 = Cache_AM_Ult_Loop2;
			});	
	}
	else
	{
		AM_UltLoop2 = Cache_AM_Ult_Loop2;
	}

	// AM_UltEnd
	if (!Cache_AM_Ult_End)
	{
		AssetManager::GetAsyncAssetCache("AM_Gideon_Ult_End", [this](std::shared_ptr<UObject> Object)
			{
				Cache_AM_Ult_End = std::dynamic_pointer_cast<UAnimMontage>(Object);
				AM_UltEnd = Cache_AM_Ult_End;
			});	
	}
	else
	{
		AM_UltEnd = Cache_AM_Ult_End;
	}
}


bool UGideonAnimInstance::IsAllResourceOK()
{
	return UMyGameAnimInstanceBase::IsAllResourceOK() && BS_FPSLocomotion && AM_UltEnd && AM_UltLoop1 && AM_UltLoop2;
}

void UGideonAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	UMyGameAnimInstanceBase::NativeUpdateAnimation(DeltaSeconds);

	// 방향 계산하기
	// TODO: 필요시 함수화
	Direction = 0.0f;
	if (MovementVelocity > FLT_EPSILON) 
	{
		XMFLOAT4 ActorRot = MyGameCharacter->GetActorRotation();
		XMVECTOR ActorRotQuat = XMLoadFloat4(&ActorRot);
		XMVECTOR ActorForward = XMVector3Rotate(XMVectorSet(0,0,1,0), ActorRotQuat);

		XMFLOAT3 Velocity = MovementComp->Velocity;

		XMVECTOR Forward2D = XMVector2Normalize(XMVectorSet(XMVectorGetX(ActorForward), XMVectorGetZ(ActorForward), 0, 0));
		XMVECTOR Velocity2D = XMVector2Normalize(XMVectorSet(Velocity.x, Velocity.z, 0, 0));

		// 각도
		float Dot = XMVectorGetX(XMVector2Dot(Forward2D, Velocity2D));
		Dot = std::clamp(Dot, -1.0f, 1.0f);

		float AngleRad = acosf(Dot);
		Direction = XMConvertToDegrees(AngleRad);

		// 좌우 판별
		float Cross = XMVectorGetZ(XMVector2Cross(Forward2D, Velocity2D));
		if (Cross < 0)
		{
			Direction *= -1;
		}
		Direction = std::clamp(Direction, -180.0f, 180.0f);
	}
}

void UGideonAnimInstance::UpdateAnimation(float dt)
{
	UMyGameAnimInstanceBase::UpdateAnimation(dt);

	if (MovementComp->bOrientRotationToMovement)
	{
		BS_FPSLocomotion->GetAnimationBoneTransforms(XMFLOAT2{Direction, MovementVelocity }, CurrentTime, BoneTransforms, FinalNotifies);    
	}
	else
	{
		BS_Locomotion->GetAnimationBoneTransforms(XMFLOAT2{0.0f, MovementVelocity }, CurrentTime, BoneTransforms, FinalNotifies);      
	}

	// UpperBody 몽타쥬
	{
		std::vector<FBoneLocalTransform> UpperBodyBoneTransforms = BoneTransforms;
		if (PlayMontage("UpperBody", UpperBodyBoneTransforms, FinalNotifies))
		{
			LayeredBlendPerBone(BoneTransforms, UpperBodyBoneTransforms, "spine_02", 1.0f, BoneTransforms);	
		}
	}

	// DefaultSlot 몽타쥬
	{
		PlayMontage("DefaultSlot", BoneTransforms, FinalNotifies);
		
	}
}

void UGideonAnimInstance::Ultimate_StartMotionWarping()
{
	if (!SetMotionWarping())
	{
		return;
	}

	const std::shared_ptr<UMotionWarpingComponent>& MotionWarpingComp = MyGameCharacter->GetMotionWarpingComponent();
	XMFLOAT3 NewPos = MyGameCharacter->GetActorLocation()+XMFLOAT3{0.0f,200.0f,0.0f};
	MotionWarpingComp->SetTargetLocation(NewPos, 1.0f, true);
}

void UGideonAnimInstance::Ultimate_ChangeCameraToNormal()
{
	AGideonCharacter* Gideon = dynamic_cast<AGideonCharacter*>(MyGameCharacter);
	if (!Gideon)
	{
		return;
	}

	Gideon->ChangeToNormalCamera(0.5f);
}

void UGideonAnimInstance::Ultimate_ChangeToLoop()
{
	AGideonCharacter* Gideon = dynamic_cast<AGideonCharacter*>(MyGameCharacter);
	if (!Gideon)
	{
		return;
	}

	const std::shared_ptr<UAnimInstance>& AnimInstance = Gideon->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	// GideonCharacter 공격 시작
	Gideon->MeteorStart();

	AnimInstance->Montage_Play(AM_UltLoop1, 0, {this, &UGideonAnimInstance::Ultimate_Loop2});
}

void UGideonAnimInstance::Ultimate_Loop2()
{
	if (!MyGameCharacter)
	{
		return;
	}

	const std::shared_ptr<UAnimInstance>& AnimInstance = MyGameCharacter->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	AnimInstance->Montage_Play(AM_UltLoop2, 0, {this, &UGideonAnimInstance::Ultimate_End});
}

void UGideonAnimInstance::Ultimate_End()
{
	AGideonCharacter* Gideon = dynamic_cast<AGideonCharacter*>(MyGameCharacter);
	if (!Gideon)
	{
		return;
	}

	const std::shared_ptr<UAnimInstance>& AnimInstance = Gideon->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	// Gideon Character 공격 종료
	Gideon->MeteorEnd();

	const std::shared_ptr<UMotionWarpingComponent>& MotionWarpingComp = MyGameCharacter->GetMotionWarpingComponent();
	XMFLOAT3 NewPos = MyGameCharacter->GetActorLocation()+XMFLOAT3{0.0f, -200.0f,0.0f};
	MotionWarpingComp->SetbIsSetPosition(true);
	MotionWarpingComp->SetTargetLocation(NewPos, 1.0f);

	AnimInstance->Montage_Play(AM_UltEnd, 0);
}

void UGideonAnimInstance::BasicAttack0()
{
	if (!MyGameCharacter)
	{
		return;
	}
	std::shared_ptr<UGideonCombatComponent> GideonCombatComp = std::dynamic_pointer_cast<UGideonCombatComponent>(MyGameCharacter->GetCombatComponent());
	if (!GideonCombatComp)
	{
		return;
	}

	GideonCombatComp->ApplyBasicAttack0();
}

void UGideonAnimInstance::BasicAttack1()
{
	if (!MyGameCharacter)
	{
		return;
	}
	std::shared_ptr<UGideonCombatComponent> GideonCombatComp = std::dynamic_pointer_cast<UGideonCombatComponent>(MyGameCharacter->GetCombatComponent());
	if (!GideonCombatComp)
	{
		return;
	}

	GideonCombatComp->ApplyBasicAttack1();
}

void UGideonAnimInstance::BasicAttack2()
{
	if (!MyGameCharacter)
	{
		return;
	}
	std::shared_ptr<UGideonCombatComponent> GideonCombatComp = std::dynamic_pointer_cast<UGideonCombatComponent>(MyGameCharacter->GetCombatComponent());
	if (!GideonCombatComp)
	{
		return;
	}

	GideonCombatComp->ApplyBasicAttack2();
}

void UGideonAnimInstance::HeavyAttack()
{
	if (!MyGameCharacter)
	{
		return;
	}
	std::shared_ptr<UGideonCombatComponent> GideonCombatComp = std::dynamic_pointer_cast<UGideonCombatComponent>(MyGameCharacter->GetCombatComponent());
	if (!GideonCombatComp)
	{
		return;
	}

	GideonCombatComp->ApplyHeavyAttack();
}

void UGideonAnimInstance::SkillAttack()
{
	if (const std::shared_ptr<UGideonSkillComponent>& GideonSkillComp = std::dynamic_pointer_cast<UGideonSkillComponent>(MyGameCharacter->GetSkillComponent()))
	{
		GideonSkillComp->ApplySkillAttack();
	}
}

void UGideonAnimInstance::SetAnimNotify_BeginPlay()
{
	UMyGameAnimInstanceBase::SetAnimNotify_BeginPlay();

	// BasicAttack
	NotifyEvent["BasicAttack0"] = {this, &UGideonAnimInstance::BasicAttack0};
	NotifyEvent["BasicAttack1"] = {this, &UGideonAnimInstance::BasicAttack1};
	NotifyEvent["BasicAttack2"] = {this, &UGideonAnimInstance::BasicAttack2};
	// HeavyAttack
	NotifyEvent["ChargeAttack"] = {this,&UGideonAnimInstance::HeavyAttack};
	// Skill
	NotifyEvent["G_Skill"] = {this, &UGideonAnimInstance::SkillAttack};
	// Ultimate
	{
		NotifyEvent["UltMotionWarping"] = {this, &UGideonAnimInstance::Ultimate_StartMotionWarping};
		NotifyEvent["Camera"] = {this, &UGideonAnimInstance::Ultimate_ChangeCameraToNormal};
		NotifyEvent["UltLoopStart"] = {this, &UGideonAnimInstance::Ultimate_ChangeToLoop};
	}
}
