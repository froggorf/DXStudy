#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Class/Light/ULightComponent.h"
#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Physics/UBoxComponent.h"


ATestCube2::ATestCube2()
{
	constexpr float Size = 0.2f;

	Ground = std::make_shared<UStaticMeshComponent>();
	Ground->SetupAttachment(GetRootComponent());
	Ground->SetRelativeScale3D({5000.0f, 5.0f, 5000.0f});
	Ground->SetRelativeLocation({0.0f, -100.0f,0.0f});

	Mesh0 = std::make_shared<UStaticMeshComponent>();
	Mesh0 ->SetupAttachment(GetRootComponent());
	Mesh0 ->SetRelativeScale3D({Size, Size, Size});

	Mesh1= std::make_shared<UStaticMeshComponent>();
	Mesh1->SetupAttachment(GetRootComponent());
	Mesh1->SetRelativeScale3D({Size, Size, Size});

	Mesh2= std::make_shared<UStaticMeshComponent>();
	Mesh2->SetupAttachment(GetRootComponent());
	Mesh2->SetRelativeScale3D({Size, Size, Size});

	Mesh3= std::make_shared<UStaticMeshComponent>();
	Mesh3->SetupAttachment(GetRootComponent());
	Mesh3->SetRelativeScale3D({Size, Size, Size});

	Mesh4= std::make_shared<UStaticMeshComponent>();
	Mesh4->SetupAttachment(GetRootComponent());
	Mesh4->SetRelativeScale3D({Size, Size, Size});

	BoxCollision = std::make_shared<UBoxComponent>();
	BoxCollision->SetupAttachment(GetRootComponent());
	BoxCollision->SetExtent({500,500,500});
	BoxCollision->SetCollisionObjectType(ECollisionChannel::AlwaysOverlap);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::Player, ECollisionResponse::Overlap);
}

void ATestCube2::Register()
{
	AActor::Register();

	AssetManager::GetAsyncAssetCache("SM_Brick",[this](std::shared_ptr<UObject> Object)
		{
			Ground->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Ground->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	AssetManager::GetAsyncAssetCache("SM_Town_Box",[this](std::shared_ptr<UObject> Object)
		{
			Mesh4->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Mesh4->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});

	AssetManager::GetAsyncAssetCache("SM_Town_Builing",[this](std::shared_ptr<UObject> Object)
		{
			Mesh0->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Mesh1->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	AssetManager::GetAsyncAssetCache("SM_Town_Hammer",[this](std::shared_ptr<UObject> Object)
		{
			Mesh1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Mesh1->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});

	AssetManager::GetAsyncAssetCache("SM_Town_Roof1",[this](std::shared_ptr<UObject> Object)
		{
			Mesh2->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Mesh2->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	AssetManager::GetAsyncAssetCache("SM_Town_Roof2",[this](std::shared_ptr<UObject> Object)
		{
			Mesh3->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Mesh3->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	
}
void ATestCube2::BeginPlay()
{
	AActor::BeginPlay();

	if (!BoxCollision)
	{
		return;
	}

	BoxCollision->OnComponentBeginOverlap.Add(this, &ATestCube2::ActorComeToSmithy);
	BoxCollision->OnComponentEndOverlap.Add(this, &ATestCube2::ActorComeOutSmithy);

	MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("========================================"));
	MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("🔍 Checking ALL Actors in Scene"));
	MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("========================================"));

	if (GPhysicsEngine && GPhysicsEngine->GetScene())
	{
		physx::PxScene* Scene = GPhysicsEngine->GetScene();
		physx::PxU32 ActorCount = Scene->getNbActors(
			physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);

		std::vector<physx::PxActor*> Actors(ActorCount);
		Scene->getActors(
			physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC,
			Actors.data(), ActorCount);

		// ✅ %u 사용 (unsigned)
		MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("Total Actors in Scene: %u"), ActorCount);

		for (physx::PxU32 i = 0; i < ActorCount; ++i)  // size_t 대신 PxU32
		{
			physx::PxRigidActor* RigidActor = Actors[i]->is<physx::PxRigidActor>();
			if (!RigidActor) continue;

			MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("----------------------------------------"));
			MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("Actor #%u:"), i);  // %zu → %u

			// userData로 컴포넌트 이름 확인
			if (RigidActor->userData)
			{
				UShapeComponent* Comp = static_cast<UShapeComponent*>(RigidActor->userData);
				MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("  Component: %s"), Comp->GetName().c_str());
				MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("  ObjectType: %d"), static_cast<int>(Comp->GetObjectType()));
			}
			else
			{
				MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("  Component: NULL (no userData)"));
			}

			// Shape의 FilterData 확인
			physx::PxU32 ShapeCount = RigidActor->getNbShapes();
			std::vector<physx::PxShape*> Shapes(ShapeCount);
			RigidActor->getShapes(Shapes.data(), ShapeCount);

			MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("  ShapeCount: %u"), ShapeCount);  // %d → %u

			for (physx::PxU32 j = 0; j < ShapeCount; ++j)  // size_t → PxU32
			{
				physx::PxFilterData FilterData = Shapes[j]->getSimulationFilterData();

				MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("  Shape #%u FilterData:"), j);  // %zu → %u
				MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("    word0: %u (0x%X)"), FilterData.word0, FilterData.word0);
				MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("    word1: %u (0x%X)"), FilterData.word1, FilterData.word1);
				MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("    word2: %u (0x%X)"), FilterData.word2, FilterData.word2);

				if (FilterData.word0 == 0)
				{
					MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("    ❌❌❌ INVALID word0 (0)! ❌❌❌"));
				}
			}
		}

		MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("========================================"));
	}
}

void ATestCube2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}

void ATestCube2::ActorComeToSmithy(UShapeComponent* TargetComp , AActor* OverlapActor , UShapeComponent* OverlapComp)
{
	MY_LOG("LOG",EDebugLogLevel::DLL_Warning,"Come");
	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		//PC->CreateWidget()
	}
}

void ATestCube2::ActorComeOutSmithy(UShapeComponent* TargetComp, AActor* OverlapActor, UShapeComponent* OverlapComp)
{
	MY_LOG("LOG",EDebugLogLevel::DLL_Warning,"Out");
}
