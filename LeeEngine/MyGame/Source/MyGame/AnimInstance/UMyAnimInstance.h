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

	void TriangleInterpolation(const XMFLOAT2& CurrentValue, const std::shared_ptr<FAnimClipPoint>& P1, const std::shared_ptr<FAnimClipPoint>& P2,const std::shared_ptr<FAnimClipPoint>& P3, std::vector<XMMATRIX>& AnimMatrices);
	void LinearInterpolation(const XMFLOAT2& CurrentValue,const std::shared_ptr<FAnimClipPoint>& P1, const std::shared_ptr<FAnimClipPoint>& P2,std::vector<XMMATRIX>& AnimMatrices);
	void CalculateOneAnimation(const std::shared_ptr<FAnimClipPoint>& Point,
		std::vector<DirectX::XMMATRIX>& AnimMatrices);
public:
protected:
private:

	std::shared_ptr<UTestComponent> TestComp;

	float CurrentTime = 0.0f;
	float CurrentSpeed = 0.0f;

};