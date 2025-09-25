#include "CoreMinimal.h"
#include "APlayerController.h"

#include "ACharacter.h"
#include "Engine/UEngine.h"
#include "Engine/Physics/ULineComponent.h"
#include "Engine/RenderCore/EditorScene.h"
#include "Engine/Widget/UUserWidget.h"
#include "Engine/World/UWorld.h"


APlayerController::APlayerController()
{
	static UINT PlayerControllerID = 0;
	Rename("PlayerController_"+ (PlayerControllerID++));

	PlayerInput = std::make_shared<UPlayerInput>();
}

void APlayerController::BeginPlay()
{
	AActor::BeginPlay();

	CameraManager = std::dynamic_pointer_cast<APlayerCameraManager>(GEngine->GetWorld()->GetPersistentLevel()->SpawnActor("APlayerCameraManager", {}));
}

void APlayerController::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	if (PlayerInput)
	{
		PlayerInput->Tick();
	}

	if (Character)
	{
		Character->SetControlRotation(GetActorRotation());
	}
}

void APlayerController::TickWidget(float DeltaSeconds)
{
	for (const auto& UserWidget : UserWidgets | std::views::values)
	{
		UserWidget->Tick(DeltaSeconds);
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
	XMVECTOR UpVector    = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR RightVector = XMVector3Normalize(XMVector3Cross(UpVector, ForwardVector));

	XMVECTOR LocalPitchQuat = XMQuaternionRotationAxis(RightVector, XMConvertToRadians(Val));
	XMVECTOR NewControlRotationQuat = XMQuaternionNormalize(XMQuaternionMultiply(ControlRotation, LocalPitchQuat));

	XMStoreFloat4(&Rot, NewControlRotationQuat);
	SetActorRotation(Rot);
}

void APlayerController::OnPossess(ACharacter* CharacterToPossess)
{
	if (CharacterToPossess != nullptr)
	{
		Character = CharacterToPossess;
		Character->Controller = this;

		if (std::shared_ptr<APlayerCameraManager> CM = CameraManager.lock())
		{
			CM->TargetCamera = Character->GetCameraComponent();
		}
	}
}

//void APlayerController::HandleRootMotion(const XMMATRIX& Root)
//{
//	if (Character && Character->GetCharacterMovement())
//	{
//		static float LastRootMotionTime = -1.0f;
//		static XMMATRIX PrevRootMatrix = XMMatrixIdentity();
//		// 0.1초 이상 적용 안됐었다면
//		if (LastRootMotionTime + 0.1f <= GEngine->GetTimeSeconds())
//		{
//			LastRootMotionTime = GEngine->GetTimeSeconds();
//			PrevRootMatrix = XMMatrixIdentity();
//		}
//		
//		XMMATRIX DeltaMatrix = XMMatrixMultiply(Root, XMMatrixInverse(nullptr, PrevRootMatrix));
//		MY_LOG("Delta", EDebugLogLevel::DLL_Warning, XMFLOAT3_TO_TEXT(XMFLOAT3{DeltaMatrix.r[3].m128_f32[0],DeltaMatrix.r[3].m128_f32[1],DeltaMatrix.r[3].m128_f32[2]}));
//
//		// 위치
//		XMVECTOR Pos = XMVectorSet(DeltaMatrix.r[3].m128_f32[0],DeltaMatrix.r[3].m128_f32[1],-DeltaMatrix.r[3].m128_f32[2],0.0f);
//		XMFLOAT4 ActorRotate = Character->GetActorRotation();
//		Pos = XMVector3Rotate(Pos,  XMLoadFloat4(&ActorRotate));
//		XMFLOAT3 RootPosition;
//		XMStoreFloat3(&RootPosition, Pos);
//
//		// 회전
//		XMVECTOR Quat = XMQuaternionRotationMatrix(DeltaMatrix);
//		XMFLOAT4 RootRotation;
//		XMStoreFloat4(&RootRotation, Quat);
//
//		physx::PxVec3 PxDelta(RootPosition.x,RootPosition.y,-RootPosition.z);
//		
//		// TODO 모듈화하기
//		XMFLOAT3 Start = Character->GetActorLocation();
//		Character->GetCharacterMovement()->PxCharacterController->move(PxDelta,0.01f, GEngine->GetDeltaSeconds(), Character->GetCharacterMovement()->Filters);
//		XMFLOAT3 End = Character->GetActorLocation();
//		std::shared_ptr<ULineComponent> LineComp = std::make_shared<ULineComponent>(false,Start,End,End);	
//
//		PrevRootMatrix = Root;
//		LastRootMotionTime = GEngine->GetTimeSeconds();
//	}
//}

void APlayerController::CreateWidget(const std::string& Name, const std::shared_ptr<UUserWidget>& NewWidget)
{
	if (UserWidgets.contains(Name))
	{
		MY_LOG("Warning", EDebugLogLevel::DLL_Warning, "Already exist widget name");
		return;
	}

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

