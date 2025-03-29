#pragma once
// 테스트용 AnimInstance

#include "../Component/UTestComponent.h"
#include "Engine/Animation/UAnimInstance.h"

class UMyAnimInstance : public UAnimInstance
{
	MY_GENERATED_BODY(UMyAnimInstance)
public:
	UMyAnimInstance();
	~UMyAnimInstance() override {};

	void BeginPlay() override;
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;
	void UpdateAnimation(float dt) override;
protected:
private:
public:
protected:
private:
	std::shared_ptr<UAnimSequence> TestAnim1;
	std::shared_ptr<UAnimSequence> TestAnim2;

	std::shared_ptr<UTestComponent> TestComp;

};