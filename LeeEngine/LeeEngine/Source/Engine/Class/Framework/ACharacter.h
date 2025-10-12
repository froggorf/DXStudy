// 07.08
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "CoreMinimal.h"
#include "USpringArmComponent.h"
#include "Engine/Components/USkeletalMeshComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/GameFramework/AActor.h"
#include "Engine/Physics/UCapsuleComponent.h"

class APlayerController;
class UCameraComponent;

class MyQueryFilterCallback : public physx::PxQueryFilterCallback
{
public:
	// 무시할 actor 저장용 set
	AActor* IgnoreActor;

	virtual physx::PxQueryHitType::Enum preFilter(
		const physx::PxFilterData& /*filterData*/,
		const physx::PxShape* /*shape*/,
		const physx::PxRigidActor* actor,
		physx::PxHitFlags& /*queryFlags*/) override;

	physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit, const physx::PxShape* shape, const physx::PxRigidActor* actor) override
	{
		return physx::PxQueryHitType::eBLOCK;
	}
};

enum class EMovementMode
{
	Walking, Flying, Swimming
};
class UCharacterMovementComponent : public UActorComponent
{
	MY_GENERATE_BODY(UCharacterMovementComponent)
public:
	UCharacterMovementComponent();
	void BeginPlay() override;
	void TickComponent(float DeltaSeconds) override;

	void AddInputVector(XMFLOAT3 WorldAccel, float Power);
	void Jump();
	physx::PxController* PxCharacterController= nullptr;
	physx::PxControllerFilters Filters;
	MyQueryFilterCallback CCTQueryCallBack{};

	float GravityScale = 12.5f;
	float MaxStepHeight = 20.0f;
	float WalkableFloorAngle = 44.5f;
	float MaxWalkSpeed = 500.0f;
	float JumpZVelocity = 450.0f;
	float Braking = 2048.0f;
	float Acceleration = 2048.0f;

	bool bOrientRotationToMovement = true;
	XMFLOAT3 RotationRate = XMFLOAT3{0.0f,0.0f,0.0f};

	physx::PxCapsuleControllerDesc desc{};
	EMovementMode MovementMode = EMovementMode::Walking;

	float CurVelocityY = 0.0f;
	XMFLOAT3 ControlInputVector{};
	XMFLOAT3 Velocity{};
};

class ACharacter : public AActor
{
	MY_GENERATE_BODY(ACharacter)

public:
	ACharacter();
	~ACharacter() override = default;
	void Register() override;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	void AddMovementInput(const XMFLOAT3& WorldDirection, float ScaleValue = 1.0f);
	void AddControllerYawInput(float Val);
	void AddControllerPitchInput(float Val);

	void Jump();

	void SetControlRotation(const XMFLOAT4& NewRot);
	XMFLOAT4 GetControlRotation() const {return ControlRotation;}

	UCharacterMovementComponent* GetCharacterMovement() const {return CharacterMovement.lock().get(); }
	USkeletalMeshComponent* GetSkeletalMeshComponent() const {return SkeletalMeshComponent.get();};
	std::shared_ptr<UCameraComponent> GetCameraComponent() const {return CameraComp;}


	std::shared_ptr<UAnimInstance> GetAnimInstance() const;

	void HandleRootMotion(const XMMATRIX& Root);

	void SetCurPlayingAnimMontage(const std::string& NewPlayingAnimMontage){CurPlayingAnimMontage = NewPlayingAnimMontage;}


	virtual void Death(){};
protected:
	virtual void BindKeyInputs() {}
private:
public:
	// 현재 이 액터를 소유중인 컨트롤러
	APlayerController*  Controller;
protected:
	// 컨트롤러의 Rotation
	XMFLOAT4 ControlRotation;

	std::shared_ptr<UCapsuleComponent> CapsuleComp;
	std::shared_ptr<UStaticMeshComponent> QueryCheckCapsuleComp;

	std::shared_ptr<USkeletalMeshComponent> SkeletalMeshComponent;

	std::shared_ptr<UCameraComponent> CameraComp;
	std::shared_ptr<USpringArmComponent> SpringArm;
private:
	float Radius;
	float HalfHeight;

	std::weak_ptr<UCharacterMovementComponent> CharacterMovement;

	XMMATRIX PreviousRootMatrix;
	float LastUpdateTime = -0.5f;
	std::string LastPlayingAnimMontage;
	std::string CurPlayingAnimMontage;
};