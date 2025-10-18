#include "CoreMinimal.h"
#include "APlayerController.h"

#include "ACharacter.h"
#include "Engine/Widget/UUserWidget.h"
#include "Engine/World/UWorld.h"


AController::AController()
{
	static std::atomic<UINT> ControllerID = 0;
	Rename("PlayerController_"+ (ControllerID++));
}

void AController::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	if (Character)
	{
		Character->SetControlRotation(GetActorRotation());
	}
}

void APlayerController::AddYawInput(float Val)
{
	XMFLOAT4 Rot = GetActorRotation();
	XMVECTOR ControlRotation = XMLoadFloat4(&Rot);
	XMVECTOR ForwardVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), ControlRotation));
	XMVECTOR UpVector    = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR RightVector = XMVector3Normalize(XMVector3Cross(UpVector, ForwardVector));

	XMVECTOR LocalYawQuat   = XMQuaternionRotationAxis(UpVector, XMConvertToRadians(Val));
	XMVECTOR NewControlRotationQuat = XMQuaternionNormalize(XMQuaternionMultiply(ControlRotation, LocalYawQuat));

	XMStoreFloat4(&Rot, NewControlRotationQuat);
	SetActorRotation(Rot);

}

void APlayerController::AddPitchInput(float Val)
{
	XMFLOAT4 Rot = GetActorRotation();
	XMVECTOR ControlRotation = XMLoadFloat4(&Rot);
	XMVECTOR ForwardVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), ControlRotation));
	XMVECTOR UpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR RightVector = XMVector3Normalize(XMVector3Cross(UpVector, ForwardVector));

	XMVECTOR ForwardXZ = XMVector3Normalize(XMVectorSet(XMVectorGetX(ForwardVector), 0.0f, XMVectorGetZ(ForwardVector), 0.0f));
	float Dot = XMVectorGetX(XMVector3Dot(ForwardVector, ForwardXZ));
	Dot = std::clamp(Dot, -1.0f, 1.0f);
	float CurrentAngleRad = acosf(Dot);
	if (XMVectorGetY(ForwardVector) > 0)
		CurrentAngleRad *= -1;

	float MinPitchDeg = -60.0f;
	float MaxPitchDeg = 60.0f;

	if (const std::shared_ptr<APlayerCameraManager>& PCM = CameraManager.lock())
	{
		MinPitchDeg = PCM->ViewPitchMin;
		MaxPitchDeg = PCM->ViewPitchMax;
	}

	float DesiredPitchDeg = XMConvertToDegrees(CurrentAngleRad) + Val;
	float ClampedPitchDeg = std::clamp(DesiredPitchDeg, MinPitchDeg, MaxPitchDeg);

	float DeltaPitchRad = XMConvertToRadians(ClampedPitchDeg) - CurrentAngleRad;
	if (fabs(DeltaPitchRad) < FLT_EPSILON)
		return;

	XMVECTOR PitchQuat = XMQuaternionRotationAxis(RightVector, DeltaPitchRad);
	XMVECTOR NewControlRotation = XMQuaternionMultiply(ControlRotation, PitchQuat);
	NewControlRotation = XMQuaternionNormalize(NewControlRotation);

	XMStoreFloat4(&Rot, NewControlRotation);
	SetActorRotation(Rot);
}
	

void AController::OnPossess(ACharacter* CharacterToPossess)
{
	if (CharacterToPossess != nullptr)
	{
		Character = CharacterToPossess;
		Character->Controller = this;
	}
}

APlayerController::APlayerController()
{
	Character = nullptr;
	bPlayRootMotion = false;
	PlayerInput = std::make_shared<UPlayerInput>();
}

void APlayerController::Register()
{
	AActor::Register();

	CameraManager = std::dynamic_pointer_cast<APlayerCameraManager>(GetWorld()->GetPersistentLevel()->SpawnActor("APlayerCameraManager", {}));
}

void APlayerController::BeginPlay()
{
	AActor::BeginPlay();

	
}

void APlayerController::Tick(float DeltaSeconds)
{
	AController::Tick(DeltaSeconds);

	if (PlayerInput)
	{
		PlayerInput->Tick();
	}

}

void APlayerController::TickWidget(float DeltaSeconds)
{
	for (const auto& UserWidget : UserWidgets | std::views::values)
	{
		UserWidget->Tick(DeltaSeconds);
	}
}


void APlayerController::OnPossess(ACharacter* CharacterToPossess)
{
	AController::OnPossess(CharacterToPossess);

	if (!Character)
	{
		return; 
	}

	if (std::shared_ptr<APlayerCameraManager> CM = CameraManager.lock())
	{

		CM->TargetCamera = Character->GetCameraComponent();
	}
}

std::shared_ptr<APlayerCameraManager> APlayerController::GetCameraManager() const
{
	if (CameraManager.expired())
	{
		return nullptr;
	}
	return CameraManager.lock();
}

std::shared_ptr<UUserWidget> APlayerController::GetWidget(const std::string& WidgetName)
{
	auto Iter = UserWidgets.find(WidgetName);
	if (Iter != UserWidgets.end())
	{
		return Iter->second;
	}

	return nullptr;
}

void APlayerController::CreateWidget(const std::string& Name, const std::shared_ptr<UUserWidget>& NewWidget)
{
	if (UserWidgets.contains(Name))
	{
		MY_LOG("Warning", EDebugLogLevel::DLL_Warning, "Already exist widget name");
		return;
	}

	NewWidget->NativeConstruct();

	UserWidgets[Name] = NewWidget;
}

// 해당 위젯에서 Input을 소모했는지를 bool값으로 반환
bool APlayerController::WidgetHandleInput(const FInputEvent& InputEvent)
{
	std::vector<std::shared_ptr<FChildWidget>> Widgets;
	Widgets.reserve(200);
	for (const auto& UserWidget : UserWidgets | std::views::values)
	{
		UserWidget->CollectAllWidgets(Widgets);
	}

	std::ranges::stable_sort(Widgets, [](const std::shared_ptr<FChildWidget>& A, const std::shared_ptr<FChildWidget>& B)
	{
		return A->GetZOrder() > B->GetZOrder();
	});

	// 계층 구조상 아래부터, 그리고 ZOrder는 큰쪽부터 인풋이 들어가져야함
	for (auto Iter = Widgets.rbegin(); Iter != Widgets.rend(); ++Iter)
	{
		if ((*Iter)->HandleInput(InputEvent))
		{
			return true;
		}
	}

	return false;
}
