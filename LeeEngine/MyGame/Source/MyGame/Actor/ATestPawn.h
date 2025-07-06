// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "../Component/UTestComponent.h"
#include "Engine/Components/USkeletalMeshComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/GameFramework/AActor.h"
#include "Engine/Physics/UCapsuleComponent.h"


class MyQueryFilterCallback : public physx::PxQueryFilterCallback
{
public:
	// 무시할 actor 저장용 set
	std::weak_ptr<AActor> IgnoreActor;

	virtual physx::PxQueryHitType::Enum preFilter(
		const physx::PxFilterData& /*filterData*/,
		const physx::PxShape* /*shape*/,
		const physx::PxRigidActor* actor,
		physx::PxHitFlags& /*queryFlags*/) override
	{
		if (actor->userData)
		{
			if (IgnoreActor.lock().get() == static_cast<UShapeComponent*>(actor->userData)->GetOwner())
			{
				return physx::PxQueryHitType::eNONE; // 충돌 무시
			}	
		}
		
		return physx::PxQueryHitType::eBLOCK;
	}

	physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit, const physx::PxShape* shape, const physx::PxRigidActor* actor) override
	{
		return physx::PxQueryHitType::eBLOCK;
	}
};

class ATestPawn : public AActor
{
	MY_GENERATE_BODY(ATestPawn)

	ATestPawn();

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void OnComponentHitEvent(UShapeComponent* HitComponent, AActor* OtherActor, UShapeComponent* OtherComp, const FHitResult& HitResults);
	void AttackStart(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp);
	void AttackEnd(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp);

	void SetAttackStart();
	void SetAttackEnd();
protected:
	

	std::shared_ptr<UTestComponent> TestComp;

	std::shared_ptr<USkeletalMeshComponent> SKComp;

	std::shared_ptr<UStaticMeshComponent> SMSword;

	std::shared_ptr<UCapsuleComponent> CapsuleComp;

	physx::PxControllerManager* Manager;
	physx::PxController* Controller;

	float Radius;
	float HalfHeight;

	physx::PxControllerFilters Filters;
	MyQueryFilterCallback CCTQueryCallBack;

private:
	
};
