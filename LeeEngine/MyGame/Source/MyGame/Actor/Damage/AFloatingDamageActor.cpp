#include "CoreMinimal.h"
#include "AFloatingDamageActor.h"

#include "Engine/Components/UWidgetComponent.h"
#include "MyGame/Widget/Number/UNumberWidget.h"

std::shared_ptr<UTexture> AFloatingDamageActor::DefaultDigitTexture = nullptr;

AFloatingDamageActor::AFloatingDamageActor()
{
	FloatingDamageWidget = std::make_shared<UWidgetComponent>();
	FloatingDamageWidget->SetupAttachment(GetRootComponent());
	constexpr float SpawnBoundBoxHalf = 100.0f;

	XMFLOAT3 NewRelativeLocation = {MyMath::FRandRange(-1.0f,1.0f) * SpawnBoundBoxHalf, MyMath::FRandRange(-1.0f,1.0f) * SpawnBoundBoxHalf, MyMath::FRandRange(-1.0f,1.0f) * SpawnBoundBoxHalf };
	FloatingDamageWidget->SetRelativeLocation(NewRelativeLocation);
}

void AFloatingDamageActor::Register()
{
	AActor::Register();

	NumberWidget = std::make_shared<UNumberWidget>();
	FloatingDamageWidget->SetWidget(NumberWidget);
}

void AFloatingDamageActor::BeginPlay()
{
	AActor::BeginPlay();


	GEngine->GetTimerManager()->SetTimer(SelfDestroyTimerHandle, {this, &AFloatingDamageActor::DestroySelf}, DestroyTime);
}

void AFloatingDamageActor::OnDestroy()
{
	AActor::OnDestroy();

	GEngine->GetTimerManager()->ClearTimer(SelfDestroyTimerHandle);
}

void AFloatingDamageActor::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	SetActorLocation(GetActorLocation() + XMFLOAT3{0.0f,DeltaSeconds * 100,0.0f});
}


void AFloatingDamageActor::Setting(const XMFLOAT4& Color, UINT Value, float DigitScale, const std::shared_ptr<UTexture>& DigitTexture)
{
	NumberWidget->SetDigitScale(DigitScale);
	NumberWidget->SetColor(Color);
	if (DigitTexture)
	{
		NumberWidget->SetDigitTexture(DigitTexture);
	}
	else
	{
		if (!DefaultDigitTexture)
		{
			DefaultDigitTexture = UTexture::GetTextureCache("T_Number");
		}
		NumberWidget->SetDigitTexture(DefaultDigitTexture);
	}

	NumberWidget->SetValue(Value);
}
