// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "Engine/Class/Framework/ACharacter.h"


class UNumberWidget;
class UWidgetComponent;

class AFloatingDamageActor : public AActor
{
	MY_GENERATE_BODY(AFloatingDamageActor)
	
	AFloatingDamageActor();
	void Register() override;
	void BeginPlay() override;
	void OnDestroy() override;
	void Tick(float DeltaSeconds) override;

	void Floating();
	void DestroySelf() override;

	void Setting(const XMFLOAT4& Color, UINT Value, float DigitScale = 1.0f, const std::shared_ptr<UTexture>& DigitTexture = nullptr);
public:
protected:
	std::shared_ptr<UWidgetComponent> FloatingDamageWidget;
	std::shared_ptr<UNumberWidget> NumberWidget;

	static std::shared_ptr<UTexture> DefaultDigitTexture;

	FTimerHandle SelfDestroyTimerHandle;
	static constexpr float WaitTime = 0.7f;
	static constexpr float FloatingTime = 0.7f;
	float CurrentFloatingTime = 0.0f;
};

inline void AFloatingDamageActor::DestroySelf()
{
	AActor::DestroySelf();
}
