#include "CoreMinimal.h"
#include "ATownGameMode.h"

#include "UMyGameInstance.h"
#include "Engine/World/UWorld.h"
#include "MyGame/Character/Enemy/AEnemyBase.h"
#include "MyGame/Character/Player/AGideonCharacter.h"
#include "MyGame/Character/Player/ASanhwaCharacter.h"
#include "MyGame/Widget/Login/ULoginWidget.h"
#include "MyGame/Widget/Town/UEquipmentStatusWidget.h"

void ATownGameMode::Tick(float DeltaSeconds)
{
	AGameMode::Tick(DeltaSeconds);

	if (ImGui::IsKeyPressed(ImGuiKey_9))
	{
		const std::shared_ptr<AActor>& Actor = GetWorld()->GetPersistentLevel()->FindActorByName("Gideon");
		GetWorld()->GetPlayerController()->OnPossess(std::dynamic_pointer_cast<ACharacter>(Actor).get());
	}
	if (ImGui::IsKeyPressed(ImGuiKey_0))
	{
		const std::shared_ptr<AActor>& Actor = GetWorld()->GetPersistentLevel()->FindActorByName("Sanhwa");
		GetWorld()->GetPlayerController()->OnPossess(std::dynamic_pointer_cast<ACharacter>(Actor).get());
	}

}

void ATownGameMode::BeginPlay()
{
	AGameMode::BeginPlay();

	const std::shared_ptr<AGideonCharacter>& Gideon = std::dynamic_pointer_cast<AGideonCharacter>(
		GetWorld()->SpawnActor("AGideonCharacter", FTransform{XMFLOAT3{ -500.0f,0,-0.0f}, XMFLOAT4{0.0f,0.0f,0.0f,1.0f}, {1.0f,1.0f,1.0f}}, "Gideon")
	);
	GetWorld()->GetPlayerController()->OnPossess(Gideon.get());
	GideonCharacter = Gideon;

	const std::shared_ptr<ASanhwaCharacter>& Sanhwa = std::dynamic_pointer_cast<ASanhwaCharacter>(
		GetWorld()->SpawnActor("ASanhwaCharacter", FTransform{XMFLOAT3{ -600,0.0f,-500.0f}, XMFLOAT4{0.0f,0.0f,0.0f,1.0f}, XMFLOAT3{1.0f,1.0f,1.0f}}, "Sanhwa")
	);
	SanhwaCharacter = Sanhwa;
	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		EquipmentStatusWidget = std::make_shared<UEquipmentStatusWidget>();
		PC->CreateWidget("EquipmentStatus", EquipmentStatusWidget);
		PC->AddToViewport("EquipmentStatus", EquipmentStatusWidget);
	}
}

void ATownGameMode::StartGame()
{
	AGameMode::StartGame();

	
}

void ATownGameMode::EndGame()
{
	AGameMode::EndGame(); 
}

EEquipUpgradeResult ATownGameMode::AddEquipLevel(EEquipType Type)
{
	UMyGameInstance* GameInstance = UMyGameInstance::GetInstance<UMyGameInstance>();
	if (!GameInstance)
	{
		return EEquipUpgradeResult::DbError;
	}

	EEquipUpgradeResult Result = GameInstance->EnchantEquipLevel(Type);
	if (Result != EEquipUpgradeResult::Success)
	{
		MY_LOG("LOG", EDebugLogLevel::DLL_Warning, "Equip upgrade failed");
		return Result;
	}

	const std::array<int, static_cast<int>(EEquipType::Count)>& EquipLevel = GameInstance->GetEquipLevel();

	// 강화 비용 같은거도 적용하기

	MY_LOG("LOG",EDebugLogLevel::DLL_Warning, std::to_string(static_cast<int>(Type)) + " Enchant Success");

	if (const std::shared_ptr<ASanhwaCharacter>& Sanhwa = SanhwaCharacter.lock())
	{
		Sanhwa->SetEquipmentLevel(EquipLevel);
	}
	if (const std::shared_ptr<AGideonCharacter>& Gideon = GideonCharacter.lock())
	{
		Gideon->SetEquipmentLevel(EquipLevel);
	}

	if (EquipmentStatusWidget)
	{
		EquipmentStatusWidget->UpdateEquipmentData();
	}

	return Result;
}

void ATownGameMode::RefreshEquipmentStatusWidget()
{
	if (EquipmentStatusWidget)
	{
		EquipmentStatusWidget->UpdateEquipmentData();
	}
}

void ADungeonGameMode::Tick(float DeltaSeconds)
{
	AGameMode::Tick(DeltaSeconds);
}

void ADungeonGameMode::Register()
{
	AGameMode::Register();
	
}

void ADungeonGameMode::BeginPlay()
{
	AGameMode::BeginPlay();

	const std::shared_ptr<AGideonCharacter>& Gideon = std::dynamic_pointer_cast<AGideonCharacter>(
		GetWorld()->SpawnActor("AGideonCharacter", FTransform{XMFLOAT3{ -0.0f,4000.0f,-0.0f}, XMFLOAT4{0.0f,0.0f,0.0f,1.0f}, {1.0f,1.0f,1.0f}}, "Gideon")
	);
	GetWorld()->GetPlayerController()->OnPossess(Gideon.get());
	Gideon->SetEquipmentLevel(UMyGameInstance::GetInstance<UMyGameInstance>()->GetEquipLevel());

	const std::shared_ptr<ASanhwaCharacter>& Sanhwa = std::dynamic_pointer_cast<ASanhwaCharacter>(
		GetWorld()->SpawnActor("ASanhwaCharacter", FTransform{XMFLOAT3{ -600,0.0f,-500.0f}, XMFLOAT4{0.0f,0.0f,0.0f,1.0f}, XMFLOAT3{1.0f,1.0f,1.0f}}, "Sanhwa")
	);
	Sanhwa->SetEquipmentLevel(UMyGameInstance::GetInstance<UMyGameInstance>()->GetEquipLevel());
}

void ADungeonGameMode::StartGame()
{
	AGameMode::StartGame();
}

void ADungeonGameMode::EndGame()
{
	AGameMode::EndGame();
}

ALoginGameMode::ALoginGameMode()
{
	CameraComp = std::make_shared<UCameraComponent>();
	CameraComp->SetupAttachment(GetRootComponent());
	CameraComp->SetRelativeLocation(XMFLOAT3{0,4000,3000});
	CameraComp->SetRelativeRotation(MyMath::VectorToRotationQuaternion(XMFLOAT3{0,0,1}));
}

void ALoginGameMode::BeginPlay()
{
	AGameMode::BeginPlay();

	static std::shared_ptr<USkeletalMesh> SK_Gideon;
	AssetManager::GetAsyncAssetCache("SK_Gideon",[this](std::shared_ptr<UObject> Object)
		{
			SK_Gideon = (std::static_pointer_cast<USkeletalMesh>(Object));
		});

	static std::shared_ptr<USkeletalMesh> SK_Sanhwa;
	AssetManager::GetAsyncAssetCache("SK_Sanhwa",[this](std::shared_ptr<UObject> Object)
		{
			SK_Sanhwa = (std::static_pointer_cast<USkeletalMesh>(Object));
		});

	if (APlayerCameraManager* CM = GetWorld()->GetCameraManager())
	{
		CM->SetTargetCamera(CameraComp);
	}

	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		std::shared_ptr<UUserWidget> LoginWidget = std::make_shared<ULoginWidget>();
		PC->CreateWidget("Login", LoginWidget);
		PC->AddToViewport("Login", LoginWidget);
	}

	GEngine->SetInputMode(EInputMode::InputMode_UIOnly);
	GEngine->SetMouseLock(EMouseLockMode::DoNotLock);
	GEngine->ShowCursor(true);
}
