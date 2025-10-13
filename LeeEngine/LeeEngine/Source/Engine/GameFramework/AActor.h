// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "CoreMinimal.h"
#include "Engine/UObject/UObject.h"
#include "Engine/Components/UActorComponent.h"
#include "Engine/Physics/UPhysicsEngine.h"


class USceneComponent;

struct FDamageEvent
{
	virtual ~FDamageEvent() = default;
	std::string DamageType;
};

struct FPointDamage
{
	FHitResult HitInfo;
	XMFLOAT3 ShotDirection;
};

struct FRadialDamage
{
	XMFLOAT3 WorldOrigin;
	float BaseDamage;
	float MinimumDamage;
	float InnerRadius;
	float OuterRadius;
	float DamageFallOff;
	std::vector<FHitResult> ComponentHits;

};


class AActor : public UObject, public std::enable_shared_from_this<AActor>
{
	MY_GENERATE_BODY(AActor)


	AActor();
	~AActor() override = default;
	void Init() override;
	void Register() override;
	void UnRegister() override;
	virtual void OnDestroy(){};
	void BeginPlay() override;

	const std::shared_ptr<USceneComponent>& GetRootComponent() const
	{
		return RootComponent;
	}
	void SetRootComponent(const std::shared_ptr<USceneComponent>& NewRootComp);

	XMFLOAT3 GetActorLocation() const;
	XMFLOAT4 GetActorRotation() const;
	XMFLOAT3 GetActorScale3D() const;
	void     SetActorLocation(const XMFLOAT3& NewLocation) const;
	void     SetActorRotation(const XMFLOAT4& NewRotation) const;
	void     SetActorScale3D(const XMFLOAT3& NewScale3D) const;

	XMFLOAT3 GetActorForwardVector() const;
	XMFLOAT3 GetActorRightVector() const;

	virtual void Tick(float DeltaSeconds);
	virtual void Tick_Editor(float DeltaSeconds);

	void SaveDataFromAssetToFile(nlohmann::json& Json) override;
	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	std::shared_ptr<UActorComponent> CreateDefaultSubobject(const std::string& SubobjectName, const std::string& ClassToCreateByDefault);

	// OwnedComponents의 GetClass()를 인자로 받은 Class와 비교하는것을 N번 진행함
	// Tick에서는 사용하면 성능감소가 일어날 수 있음
	std::shared_ptr<UActorComponent> FindComponentByClass(const std::string& Class) const;

	const std::unordered_set<std::shared_ptr<UActorComponent>>& GetComponents() const
	{
		return OwnedComponents;
	}

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser){ return DamageAmount;}
	void SetTickRate(float NewTickRate) {TickRate = NewTickRate;}
	float GetTickRate() const {return TickRate;}

	bool GetPendingKill() const {return bPendingKill;}
	void NotePendingKill(){bPendingKill = true;}
	virtual void DestroySelf();
protected:
	// 월드 내에서 Transform (Loc, Rot, Scale) 의 정보를 다루는 컴퍼넌트
	// 모든 다른 컴퍼넌트는 해당 컴퍼넌트에 부착해야함
	std::shared_ptr<USceneComponent> RootComponent;

	unsigned int ActorID = 0;

	float TickRate = 1.0f;

	bool bPendingKill = false;
private:
	// 액터가 소유중인 모든 액터 컴퍼넌트를 관리하는 컨테이너
	// 언리얼엔진의 경우 많은 수의 컴퍼넌트를 가질 수 있으므로 TSet을 사용
	std::unordered_set<std::shared_ptr<UActorComponent>> OwnedComponents;
};
