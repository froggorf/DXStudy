#include "CoreMinimal.h"
#include "APortal.h"

#include "Engine/AssetManager/AssetManager.h"
#include "Engine/Class/Framework/APlayerController.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Physics/UBoxComponent.h"
#include "Engine/World/UWorld.h"

APortal::APortal()
{
	constexpr XMFLOAT3 PortalScale{380.0f, 1.0f, 560.0f};
	constexpr XMFLOAT3 PortalRotation{90.0f, 0.0f, 0.0f};
	constexpr XMFLOAT3 TriggerExtent{260.0f, 320.0f, 180.0f};

	PortalMesh = std::make_shared<UStaticMeshComponent>();
	PortalMesh->SetupAttachment(GetRootComponent());
	PortalMesh->SetRelativeScale3D(PortalScale);
	PortalMesh->SetRelativeRotation(PortalRotation);
	PortalMesh->SetIsMonochromeObject(false);

	TriggerBox = std::make_shared<UBoxComponent>();
	TriggerBox->SetupAttachment(GetRootComponent());
	TriggerBox->SetExtent(TriggerExtent);
	TriggerBox->SetObjectType(ECollisionChannel::AlwaysOverlap);

	for (int i = 0; i < static_cast<int>(ECollisionChannel::Count); ++i)
	{
		TriggerBox->SetResponseToChannel(static_cast<ECollisionChannel>(i), ECollisionResponse::Ignore);
	}
	TriggerBox->SetResponseToChannel(ECollisionChannel::Player, ECollisionResponse::Overlap);
	TriggerBox->SetResponseToChannel(ECollisionChannel::Pawn, ECollisionResponse::Overlap);
}

void APortal::Register()
{
	AActor::Register();

	PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	static std::shared_ptr<UStaticMesh> PortalStaticMesh;
	if (!PortalStaticMesh)
	{
		AssetManager::GetAsyncAssetCache("SM_PortalPlane",[this](std::shared_ptr<UObject> Object)
			{
				PortalStaticMesh = std::dynamic_pointer_cast<UStaticMesh>(Object);
				PortalMesh->SetStaticMesh(PortalStaticMesh);
			});
	}
	else
	{
		PortalMesh->SetStaticMesh(PortalStaticMesh);
	}
}

void APortal::BeginPlay()
{
	AActor::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.Add(this, &APortal::OnPortalBeginOverlap);
	}
}

void APortal::OnPortalBeginOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{
	if (bTriggered || !OtherActor)
	{
		return;
	}

	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		const std::shared_ptr<AActor>& Player = PC->GetPlayerCharacter();
		if (Player && Player.get() == OtherActor)
		{
			bTriggered = true;
			GEngine->ChangeLevelByName("DungeonLevel");
		}
	}
}
