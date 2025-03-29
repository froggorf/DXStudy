// 03.27
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UAnimSequence.h"
#include "Engine/UObject/UObject.h"

class USkeletalMeshComponent;

class UAnimInstance : public UObject
{
	MY_GENERATED_BODY(UAnimInstance)
public:
	UAnimInstance();
	~UAnimInstance() override {};

	void BeginPlay() override;

	// AnimInstance가 생성될 때 호출되는 함수
	virtual void NativeInitializeAnimation();
	// 매 프레임마다 호출되어 애니메이션 상태머신에 적용될 변수들을 업데이트하는 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	// NativeUpdateAnimation으로 업데이트된 변수를 이용해 애니메이션 상태머신을 통해 애니메이션을 최종으로 판단하는 함수
	virtual void UpdateAnimation(float dt);
	void Tick(float DeltaSeconds);

	void CalculateBoneTransform(const std::shared_ptr<UAnimSequence>& Animation,const AssimpNodeData* node, DirectX::XMMATRIX parentTransform, float CurrentTime,std::vector<XMMATRIX>& FinalBoneMatrices);


	void SetSkeletalMeshComponent(USkeletalMeshComponent* InOwner) { CurrentSkeletalMeshComponent = InOwner; }
	USkeletalMeshComponent* GetSkeletalMeshComponent() const { return CurrentSkeletalMeshComponent; }
	class AActor* TryGetPawnOwner() const;

protected:
private:
public:
protected:
	float DeltaTime;
private:

	USkeletalMeshComponent* CurrentSkeletalMeshComponent;
};
