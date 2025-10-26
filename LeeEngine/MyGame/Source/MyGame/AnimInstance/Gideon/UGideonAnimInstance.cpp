#include "CoreMinimal.h"
#include "UGideonAnimInstance.h"

#include "MyGame/Character/Player/AGideonCharacter.h"
#include "MyGame/Component/Combat/Range/UGideonCombatComponent.h"
#include "MyGame/Component/Combat/Skill/NormalSkill/Gideon/UGideonSkillComponent.h"

UGideonAnimInstance::UGideonAnimInstance()
{
	BS_LocomotionName = "BS_Gideon_Locomotion";


}

void UGideonAnimInstance::LoadData_OnRegister()
{
	UMyGameAnimInstanceBase::LoadData_OnRegister();

	AssetManager::GetAsyncAssetCache("BS_Gideon_FPSLocomotion", [this](std::shared_ptr<UObject> Object)
		{
			BS_FPSLocomotion = std::dynamic_pointer_cast<UBlendSpace>(Object);
		});
}


bool UGideonAnimInstance::IsAllResourceOK()
{
	return UMyGameAnimInstanceBase::IsAllResourceOK() && BS_FPSLocomotion;
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
		{
			BS_Locomotion->GetAnimationBoneTransforms(XMFLOAT2{0.0f, MovementVelocity }, CurrentTime, BoneTransforms, FinalNotifies);      
		}

		
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
}
