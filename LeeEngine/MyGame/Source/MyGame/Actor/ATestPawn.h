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
	std::unordered_set<const physx::PxRigidActor*> IgnoreActors;

	// 추가 함수: 무시할 actor 등록
	void AddIgnoreActor(const physx::PxRigidActor* actor)
	{
		IgnoreActors.insert(actor);
	}

	// 추가 함수: 무시할 actor 해제
	void RemoveIgnoreActor(const physx::PxRigidActor* actor)
	{
		IgnoreActors.erase(actor);
	}

	// 쿼리 프리필터 구현
	virtual physx::PxQueryHitType::Enum preFilter(
		const physx::PxFilterData& /*filterData*/,
		const physx::PxShape* /*shape*/,
		const physx::PxRigidActor* actor,
		physx::PxHitFlags& /*queryFlags*/) override
	{
		// IgnoreActors에 있으면 무시
		if (IgnoreActors.contains(actor))
		{
			return physx::PxQueryHitType::eNONE; // 충돌 무시
		}


		// 아니면 충돌 허용
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
