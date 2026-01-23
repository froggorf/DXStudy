#include "CoreMinimal.h"
#include "ATownGameMode.h"

#include "UMyGameInstance.h"
#include "Engine/World/UWorld.h"
#include "Engine/Class/Actor/ASkyBox.h"
#include "MyGame/Actor/APortal.h"
#include "MyGame/Character/Enemy/AEnemyBase.h"
#include "MyGame/Character/Player/AGideonCharacter.h"
#include "MyGame/Character/Player/ASanhwaCharacter.h"
#include "Engine/FAudioDevice.h"
#include "MyGame/Widget/Login/ULoginWidget.h"
#include "MyGame/Widget/Dungeon/UDungeonDeathWidget.h"
#include "MyGame/Widget/Town/UEquipmentStatusWidget.h"
#include "MyGame/Widget/Stage/UStageLevelWidget.h"


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
	const bool bF5Pressed = ImGui::IsKeyPressed(ImGuiKey_F5);
	const bool bF6Pressed = ImGui::IsKeyPressed(ImGuiKey_F6);
	if (bF5Pressed || bF6Pressed)
	{
		if (UMyGameInstance* GameInstance = UMyGameInstance::GetInstance<UMyGameInstance>())
		{
			int64_t NewStageLevel = static_cast<int64_t>(GameInstance->GetStageLevel());
			if (bF5Pressed)
			{
				++NewStageLevel;
			}
			if (bF6Pressed)
			{
				--NewStageLevel;
			}
			if (NewStageLevel < 1)
			{
				NewStageLevel = 1;
			}
			if (GameInstance->SetStageLevel(NewStageLevel))
			{
				if (StageLevelWidget)
				{
					StageLevelWidget->UpdateStageLevel();
				}
			}
		}
	}
}

void ATownGameMode::BeginPlay()
{
	AGameMode::BeginPlay();
	PlayBgmByName("SB_BGM_Town_Loop");
	const std::vector<std::shared_ptr<AActor>> SkyBoxes = GetWorld()->GetPersistentLevel()->GetActorsFromClass("ASkyBox");
	if (!SkyBoxes.empty())
	{
		if (const std::shared_ptr<ASkyBox>& SkyBox = std::dynamic_pointer_cast<ASkyBox>(SkyBoxes[0]))
		{
			SkyBox->SetSkyBoxTextureName("T_SkyDawn");
			SkyBox->SetDirectionalLightColor({0.4f, 0.36f, 0.32f});
		}
	}

	const std::shared_ptr<AGideonCharacter>& Gideon = std::dynamic_pointer_cast<AGideonCharacter>(
		GetWorld()->SpawnActor("AGideonCharacter", FTransform{XMFLOAT3{ -500.0f,50.0f,-0.0f}, XMFLOAT4{0.0f,0.0f,0.0f,1.0f}, {1.0f,1.0f,1.0f}}, "Gideon")
	);
	GetWorld()->GetPlayerController()->OnPossess(Gideon.get());
	GideonCharacter = Gideon;

	const std::shared_ptr<ASanhwaCharacter>& Sanhwa = std::dynamic_pointer_cast<ASanhwaCharacter>(
		GetWorld()->SpawnActor("ASanhwaCharacter", FTransform{XMFLOAT3{-1000.0f,-1000.0f,1000000.0f}, XMFLOAT4{0.0f,0.0f,0.0f,1.0f}, XMFLOAT3{1.0f,1.0f,1.0f}}, "Sanhwa")
	);
	SanhwaCharacter = Sanhwa;
	GetWorld()->SpawnActor("APortal", FTransform{XMFLOAT3{0.0f, 0.0f, -1000.0f}, XMFLOAT4{0.0f,0.0f,0.0f,1.0f}, XMFLOAT3{1.0f,1.0f,1.0f}}, "TownPortal");
	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		EquipmentStatusWidget = std::make_shared<UEquipmentStatusWidget>();
		PC->CreateWidget("EquipmentStatus", EquipmentStatusWidget);
		PC->AddToViewport("EquipmentStatus", EquipmentStatusWidget);

		StageLevelWidget = std::make_shared<UStageLevelWidget>();
		PC->CreateWidget("StageLevel", StageLevelWidget);
		PC->AddToViewport("StageLevel", StageLevelWidget);
	}


	const XMFLOAT3 ScarecrowStart = XMFLOAT3{-1500.0f, 50.0f, -300.0f};
	constexpr int ScarecrowCount = 4;
	constexpr float ScarecrowSpacing = 75.0f;
	for (int i = 0; i < ScarecrowCount; ++i)
	{
		const XMFLOAT3 ScarecrowLocation = XMFLOAT3{
			ScarecrowStart.x,
			ScarecrowStart.y,
			ScarecrowStart.z + (ScarecrowSpacing * i)
		};
		const XMFLOAT4 ScarecrowRotation = MyMath::VectorToRotationQuaternion(XMFLOAT3{1.0f, 0.0f , 0.0f});
		GetWorld()->SpawnActor("AScarecrow", FTransform{ScarecrowLocation, ScarecrowRotation});
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

	const bool bF5Pressed = ImGui::IsKeyPressed(ImGuiKey_F5);
	const bool bF6Pressed = ImGui::IsKeyPressed(ImGuiKey_F6);
	if (bF5Pressed || bF6Pressed)
	{
		if (UMyGameInstance* GameInstance = UMyGameInstance::GetInstance<UMyGameInstance>())
		{
			int64_t NewStageLevel = static_cast<int64_t>(GameInstance->GetStageLevel());
			if (bF5Pressed)
			{
				++NewStageLevel;
			}
			if (bF6Pressed)
			{
				--NewStageLevel;
			}
			if (NewStageLevel < 1)
			{
				NewStageLevel = 1;
			}
			if (GameInstance->SetStageLevel(NewStageLevel))
			{
				if (StageLevelWidget)
				{
					StageLevelWidget->UpdateStageLevel();
				}
			}
		}
	}
}

void ADungeonGameMode::Register()
{
	AGameMode::Register();
	
}

void ADungeonGameMode::BeginPlay()
{
	AGameMode::BeginPlay();
	PlayBgmByName("SB_BGM_Dungeon_Loop");
	const std::vector<std::shared_ptr<AActor>> SkyBoxes = GetWorld()->GetPersistentLevel()->GetActorsFromClass("ASkyBox");
	if (!SkyBoxes.empty())
	{
		if (const std::shared_ptr<ASkyBox>& SkyBox = std::dynamic_pointer_cast<ASkyBox>(SkyBoxes[0]))
		{
			SkyBox->SetSkyBoxTextureName("T_SkyBox");
			SkyBox->SetDirectionalLightColor({2.5f, 2.5f, 2.5f});
		}
	}

	const std::shared_ptr<AGideonCharacter>& Gideon = std::dynamic_pointer_cast<AGideonCharacter>(
		GetWorld()->SpawnActor("AGideonCharacter", FTransform{XMFLOAT3{ -0.0f,3500,-0.0f}, XMFLOAT4{0.0f,0.0f,0.0f,1.0f}, {1.0f,1.0f,1.0f}}, "Gideon")
	);
	GetWorld()->GetPlayerController()->OnPossess(Gideon.get());
	Gideon->SetEquipmentLevel(UMyGameInstance::GetInstance<UMyGameInstance>()->GetEquipLevel());

	const std::shared_ptr<ASanhwaCharacter>& Sanhwa = std::dynamic_pointer_cast<ASanhwaCharacter>(
		GetWorld()->SpawnActor("ASanhwaCharacter", FTransform{XMFLOAT3{ -600,0.0f,-500.0f}, XMFLOAT4{0.0f,0.0f,0.0f,1.0f}, XMFLOAT3{1.0f,1.0f,1.0f}}, "Sanhwa")
	);
	Sanhwa->SetEquipmentLevel(UMyGameInstance::GetInstance<UMyGameInstance>()->GetEquipLevel());

	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		EquipmentStatusWidget = std::make_shared<UEquipmentStatusWidget>();
		PC->CreateWidget("EquipmentStatus", EquipmentStatusWidget);
		PC->AddToViewport("EquipmentStatus", EquipmentStatusWidget);

		StageLevelWidget = std::make_shared<UStageLevelWidget>();
		PC->CreateWidget("StageLevel", StageLevelWidget);
		PC->AddToViewport("StageLevel", StageLevelWidget);
	}

	const std::vector<std::shared_ptr<AActor>> Dragons = GetWorld()->GetPersistentLevel()->GetActorsFromClass("ADragon");
	for (const std::shared_ptr<AActor>& Actor : Dragons)
	{
		if (const std::shared_ptr<ADragon>& Dragon = std::dynamic_pointer_cast<ADragon>(Actor))
		{
			Dragon->OnDeath.Add(this, &ADungeonGameMode::HandleDragonDeath);
		}
	}
}

void ADungeonGameMode::StartGame()
{
	AGameMode::StartGame();
}

void ADungeonGameMode::EndGame()
{
	AGameMode::EndGame();
}

void ADungeonGameMode::HandleDragonDeath()
{
	if (bReturnPortalSpawned)
	{
		return;
	}

	bReturnPortalSpawned = true;

	const FTransform SpawnTransform{
		XMFLOAT3{11776.0f, 4579.0f, 6277.0f},
		MyMath::ForwardVectorToRotationQuaternion(XMFLOAT3{-1.0f, 0.0f, 0.0f}),
		XMFLOAT3{1.0f, 1.0f, 1.0f}
	};
	const std::shared_ptr<APortal>& Portal = std::dynamic_pointer_cast<APortal>(
		GetWorld()->SpawnActor("APortal", SpawnTransform, "DungeonReturnPortal")
	);
	if (Portal)
	{
		Portal->SetTargetLevelName("TownLevel");
		Portal->SetIncreaseStageOnUse(true);
	}
}

void ADungeonGameMode::HandlePlayerDeath()
{
	if (bPlayerDeathWidgetRequested)
	{
		return;
	}

	bPlayerDeathWidgetRequested = true;
	GEngine->GetTimerManager()->SetTimer(PlayerDeathUITimerHandle, {this, &ADungeonGameMode::ShowPlayerDeathWidget}, 3.0f, false);
}

void ADungeonGameMode::ShowPlayerDeathWidget()
{
	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		PlaySound2DByName("SB_SFX_UI_Error");
		if (!DeathWidget)
		{
			DeathWidget = std::make_shared<UDungeonDeathWidget>();
			PC->CreateWidget("DungeonDeath", DeathWidget);
		}

		PC->AddToViewport("DungeonDeath", DeathWidget);
	}
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
	PlayBgmByName("SB_BGM_Login_Loop");
	const std::vector<std::shared_ptr<AActor>> SkyBoxes = GetWorld()->GetPersistentLevel()->GetActorsFromClass("ASkyBox");
	if (!SkyBoxes.empty())
	{
		if (const std::shared_ptr<ASkyBox>& SkyBox = std::dynamic_pointer_cast<ASkyBox>(SkyBoxes[0]))
		{
			SkyBox->SetSkyBoxTextureName("T_SkyBox");
			SkyBox->SetDirectionalLightColor({2.5f,2.5f,2.5f});
		}
	}

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
