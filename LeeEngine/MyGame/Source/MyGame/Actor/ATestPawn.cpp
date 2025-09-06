#include "CoreMinimal.h"
#include "ATestPawn.h"

#include "ATestCube.h"
#include "ATestCube2.h"
#include "Engine/EditorClient/Panel/ImguiViewport.h"
#include "Engine/Physics/UBoxComponent.h"
#include "Engine/World/UWorld.h"


ATestPawn::ATestPawn()
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = true;
		CharacterMovement->Acceleration = 4096.0f;
		CharacterMovement->RotationRate = XMFLOAT3{0.0f, 1500.0f, 0.0f};
		CharacterMovement->MaxWalkSpeed = 600.0f;
		CharacterMovement->Braking = 4096;
	}

	SpringArm->SetArmLength(250);
	//SpringArm->SetRelativeLocation({0,50,-50.0f});

	AssetManager::GetAsyncAssetCache("SK_LowPoly",[this](std::shared_ptr<UObject> Object)
		{
			SkeletalMeshComponent->SetSkeletalMesh(std::static_pointer_cast<USkeletalMesh>(Object));
		});
	SkeletalMeshComponent->SetAnimInstanceClass("UMyAnimInstance");
	SkeletalMeshComponent->SetRelativeLocation({0,-85,0});
	SkeletalMeshComponent->SetRelativeRotation(XMFLOAT3{0,180,0});

	SMSword = std::make_shared<UStaticMeshComponent>();
	SMSword->SetupAttachment(SkeletalMeshComponent, "Hand_R");
	AssetManager::GetAsyncAssetCache("SM_Sword", [this](std::shared_ptr<UObject> Object)
	{
			SMSword->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(Object));
	});
	SMSword->SetRelativeScale3D({0.2f,0.2f,0.2f});
	SMSword->SetRelativeLocation({-9.898,5.167, 8.479});
	SMSword->SetRelativeRotation(XMFLOAT4{0.765, 0.644, 0.017, 0.014});

	TestComp = std::dynamic_pointer_cast<UTestComponent>(CreateDefaultSubobject("TestActorComp", "UTestComponent"));
}



void ATestPawn::BeginPlay()
{
	GEngine->GetWorld()->GetPlayerController()->OnPossess(this);

	ACharacter::BeginPlay();


	//CapsuleComp->SetDebugDraw(true);
	

	SMSword->GetBodyInstance()->OnComponentBeginOverlap.Add(this, &ATestPawn::AttackStart);
	SMSword->GetBodyInstance()->OnComponentHit.Add(this, &ATestPawn::OnComponentHitEvent);


	SMSword->GetBodyInstance()->SetSimulatePhysics(false);
	SMSword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	for (UINT i = 0; i < static_cast<UINT>(ECollisionChannel::Count); ++i)
	{
		SMSword->GetBodyInstance()->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(i), ECollisionResponse::Overlap);
	}
	SMSword->GetBodyInstance()->SetObjectType(ECollisionChannel::Pawn);

	AssetManager::GetAsyncAssetCache("AM_Sword", [this](std::shared_ptr<UObject> Object)
		{
			AM_Sword = std::static_pointer_cast<UAnimMontage>(Object);
		});
	AssetManager::GetAsyncAssetCache("AM_Backstep", [this](std::shared_ptr<UObject> Object)
		{
			AM_Smash = std::static_pointer_cast<UAnimMontage>(Object);
		});

	
}

void ATestPawn::BindKeyInputs()
{
	if (Controller)
	{
		if (std::shared_ptr<UPlayerInput> InputSystem = Controller->GetPlayerInput())
		{
			InputSystem->BindAction(EKeys::Num1, ETriggerEvent::Started, this, &ATestPawn::Attack);
			InputSystem->BindAction(EKeys::Num2, ETriggerEvent::Started, this, &ATestPawn::Backstep);
			InputSystem->BindAction(EKeys::LShift, ETriggerEvent::Started, this, &ATestPawn::SetWalk);
			InputSystem->BindAction(EKeys::LShift, ETriggerEvent::Released, this, &ATestPawn::SetRun);
			InputSystem->BindAxis2D(EKeys::W, ETriggerEvent::Trigger, 1, 0,this, &ATestPawn::Move);
			InputSystem->BindAxis2D(EKeys::S, ETriggerEvent::Trigger, -1, 0,this, &ATestPawn::Move);
			InputSystem->BindAxis2D(EKeys::D, ETriggerEvent::Trigger, 0, 1,this, &ATestPawn::Move);
			InputSystem->BindAxis2D(EKeys::A, ETriggerEvent::Trigger, 0, -1,this, &ATestPawn::Move);

			InputSystem->BindAction<ATestPawn>(EKeys::Space, ETriggerEvent::Started, this, &ATestPawn::Jump);

			InputSystem->BindAxis2D(EKeys::MouseXY2DAxis, ETriggerEvent::Trigger, 0,0, this, &ATestPawn::Look);

			InputSystem->BindAction(EKeys::MouseRight, ETriggerEvent::Started, this, &ATestPawn::MouseRotateStart);
			InputSystem->BindAction(EKeys::MouseRight, ETriggerEvent::Released, this, &ATestPawn::MouseRotateEnd);

			//InputSystem->BindAction(EKeys::MouseLeft, ETriggerEvent::Started, this, &ATestPawn::PressLeftButton);
			InputSystem->BindAction(EKeys::MouseWheelUp, Started, this, &ATestPawn::WheelUp);
			InputSystem->BindAction(EKeys::MouseWheelDown, Started, this, &ATestPawn::WheelDown);
		}
		
	}
}

void ATestPawn::OnComponentHitEvent(UShapeComponent* HitComponent, AActor* OtherActor, UShapeComponent* OtherComp, const FHitResult& HitResults)
{
	MY_LOG("Hit",EDebugLogLevel::DLL_Warning, OtherActor->GetName() + " -> " + OtherComp->GetName());
}


void ATestPawn::AttackStart(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{
	if (ATestCube2* TestCube = dynamic_cast<ATestCube2*>(OtherActor))
	{
		
		
	}
}
void ATestPawn::AttackEnd(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{

	MY_LOG("End",EDebugLogLevel::DLL_Warning, OtherActor->GetName());
}

void ATestPawn::SetAttackStart()
{
	SMSword->GetBodyInstance()->SetSimulatePhysics(true);
	SMSword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::Physics);
	SMSword->GetBodyInstance()->SetKinematicRigidBody(true);
	SMSword->GetBodyInstance()->SetDebugDraw(true);
}

void ATestPawn::SetAttackEnd()
{
	SMSword->GetBodyInstance()->SetSimulatePhysics(false);
	SMSword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SMSword->GetBodyInstance()->SetDebugDraw(false);
}

void ATestPawn::Attack()
{
	if (SkeletalMeshComponent)
	{
		if (const std::shared_ptr<UAnimInstance>& AnimInstance = SkeletalMeshComponent->GetAnimInstance())
		{
			if (AM_Sword)
			{
				AnimInstance->Montage_Play(AM_Sword, 0.0f);
			}
		}
	}
}

void ATestPawn::Backstep()
{
	if (SkeletalMeshComponent)
	{
		if (const std::shared_ptr<UAnimInstance>& AnimInstance = SkeletalMeshComponent->GetAnimInstance())
		{
			if (AM_Smash)
			{
				AnimInstance->Montage_Play(AM_Smash, 0.0f);
			}
		}
	}
}

void ATestPawn::Move(float X, float Y)
{
	bMustFindPath = false;
	XMFLOAT3 ForwardDirection = Controller->GetActorForwardVector();
	XMFLOAT3 RightDirection = Controller->GetActorRightVector();

	AddMovementInput(ForwardDirection, X);
	AddMovementInput(RightDirection, Y);
}

void ATestPawn::Look(float X, float Y)
{
	// 우클릭이 되어있다면
	if (bRightButtonPressed && Controller && Controller->GetPlayerInput())
	{
		XMFLOAT2 Delta = Controller->GetPlayerInput()->LastMouseDelta;
		AddControllerYawInput(Delta.x);
		AddControllerPitchInput(Delta.y);
	}
}

void ATestPawn::MouseRotateStart()
{
	bRightButtonPressed = true;
}

void ATestPawn::MouseRotateEnd()
{
	bRightButtonPressed = false;
}

void ATestPawn::SetWalk()
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 150.0f;
	}
}

void ATestPawn::SetRun()
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 600.0f;
	}
}

void ATestPawn::PressLeftButton()
{
	if (Controller)
	{
		XMFLOAT2 MousePos = Controller->GetPlayerInput()->LastMousePosition;
		XMFLOAT2 NDC;
#ifdef WITH_EDITOR
		MousePos.x -= FImguiLevelViewport::LevelViewportPos.x;
		MousePos.y -= FImguiLevelViewport::LevelViewportPos.y;

		NDC.x = (MousePos.x / FImguiLevelViewport::PreviousViewPortSize.x) * 2.0f - 1.0f;
		NDC.y = 1.0f - (MousePos.y / FImguiLevelViewport::PreviousViewPortSize.y) * 2.0f;
#else
		//TODO: 화면 길이 구하는것 해야함
#endif
		

		XMVECTOR ScreenPos = XMLoadFloat2(&NDC);
		if (CameraComp)
		{
			XMVECTOR ClipNear = XMVectorSet(NDC.x, NDC.y, 0.0f, 1.0f); // near
			XMVECTOR ClipFar  = XMVectorSet(NDC.x, NDC.y, 1.0f, 1.0f); // far

			XMMATRIX Proj = CameraComp->GetViewMatrices().GetProjectionMatrix();
			XMMATRIX InvProj = XMMatrixInverse(nullptr, Proj);
			XMVECTOR ViewNear = XMVector4Transform(ClipNear, InvProj);
			XMVECTOR ViewFar  = XMVector4Transform(ClipFar,  InvProj);

			// w 나누기
			ViewNear = XMVectorScale(ViewNear, 1.0f / XMVectorGetW(ViewNear));
			ViewFar  = XMVectorScale(ViewFar,  1.0f / XMVectorGetW(ViewFar));

			XMMATRIX ViewMatrix = CameraComp->GetViewMatrices().GetViewMatrix();
			XMMATRIX InvView = XMMatrixInverse(nullptr, ViewMatrix);
			XMVECTOR WorldNear = XMVector4Transform(ViewNear, InvView);
			XMVECTOR WorldFar  = XMVector4Transform(ViewFar,  InvView);

			XMFLOAT3 Start, End;
			XMStoreFloat3(&Start, WorldNear);
			XMStoreFloat3(&End,   WorldFar);

			std::vector<ECollisionChannel> Channel(static_cast<UINT>(ECollisionChannel::Count));
			for (size_t i = 0 ; i < Channel.size(); ++i)
			{
				Channel[i] = static_cast<ECollisionChannel>(i);
			}
			FHitResult HitResult;
			if (gPhysicsEngine->LineTraceSingleByChannel(Start,End, Channel, HitResult, 5))
			{
				bMustFindPath = true;
				ArriveLoc = HitResult.Location;
				MY_LOG("LOG",EDebugLogLevel::DLL_Warning, XMFLOAT3_TO_TEXT(ArriveLoc));
			}

		}


	}


}

void ATestPawn::WheelUp()
{
	if (SpringArm)
	{
		SpringArm->SetArmLength(SpringArm->TargetArmLength - 10.0f);
	}
}

void ATestPawn::WheelDown()
{
	if (SpringArm)
	{
		SpringArm->SetArmLength(SpringArm->TargetArmLength + 10.0f);
	}
}


void ATestPawn::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	if (bMustFindPath)
	{
		CurPathFindTime -= DeltaSeconds;
		// 경로 찾기
		if (CurPathFindTime <= 0.0f)
		{
			CurPathFindTime = PathFindCooldown;
			// 2. 시작점, 도착점 지정
			float startPos[3] = {GetActorLocation().x,GetActorLocation().y,-GetActorLocation().z};
			float endPos[3] = {ArriveLoc.x,ArriveLoc.y,-ArriveLoc.z};

			// 3. 폴리곤 찾기 (nearest poly)
			dtPolyRef startRef, endRef;
			float nearestStart[3], nearestEnd[3];
			dtQueryFilter filter;
			filter.setIncludeFlags(0xffffffff);
			filter.setExcludeFlags(0);
			float extents[3] = {10,300,10};
			ATestCube::MyDtNavQuery->findNearestPoly(startPos, extents, &filter, &startRef, nearestStart);
			ATestCube::MyDtNavQuery->findNearestPoly(endPos, extents, &filter, &endRef, nearestEnd);

			if (startRef == 0 || endRef == 0)
			{
				MY_LOG("NotFound" , EDebugLogLevel::DLL_Warning, "No Found NearestPoly" + std::to_string(startRef) + " " + std::to_string(endRef));
			}
			// 4. 경로 찾기 (findPath)
#define MAX_POLYS 3000
			dtPolyRef pathPolys[MAX_POLYS];
			int pathCount = 0;
			ATestCube::MyDtNavQuery->findPath(startRef, endRef, nearestStart, nearestEnd, &filter, pathPolys, &pathCount, MAX_POLYS);

			// 5. 경로 좌표 추출 (findStraightPath)
			float straightPath[MAX_POLYS*3];
			unsigned char straightPathFlags[MAX_POLYS];
			dtPolyRef straightPathPolys[MAX_POLYS];
			int straightPathCount = 0;

			ATestCube::MyDtNavQuery->findStraightPath(nearestStart, nearestEnd, pathPolys, pathCount, straightPath, straightPathFlags, straightPathPolys, &straightPathCount, MAX_POLYS);

			StraightPath.clear();
			for (int i = 0; i < straightPathCount; ++i)
			{
				XMFLOAT3 NewPos = {straightPath[i*3+0],straightPath[i*3+1],straightPath[i*3+2]};
				StraightPath.emplace_back(NewPos);
			}

			if (StraightPath.empty())
			{
				bMustFindPath = false;
				CurPathFindTime = 0.0f;
			}
		}

		if (!StraightPath.empty())
		{
			XMFLOAT3 CurPos = GetActorLocation();
			CurPos.z *=-1;
			XMFLOAT3 Target = StraightPath.front();

			// 목표점까지의 벡터
			XMFLOAT3 Delta = {Target.x - CurPos.x, 0, Target.z - CurPos.z};

			float dist = sqrtf(Delta.x*Delta.x + 0 + Delta.z*Delta.z);
			float moveSpeed = 200.0f; // 1초에 200만큼 이동 (원하는 속도로 조절)
			float step = GetCharacterMovement()->MaxWalkSpeed * DeltaSeconds;

			if (dist < 5.f) // 목표점에 거의 도착했다면
			{
				// 캐릭터를 목표점에 정확히 위치시키고, 다음 점으로 넘어감
				StraightPath.erase(StraightPath.begin());
				if (StraightPath.empty())
				{
					bMustFindPath = false;
					CurPathFindTime = 0.0f;
				}
			}
			else
			{
				XMFLOAT3 MoveDir = {Delta.x / dist, 0, Delta.z / dist};
				XMFLOAT3 MoveStep = {MoveDir.x * step, 0, MoveDir.z * step};
				MoveStep.z *= -1.0f;
				// 애니메이션 속도를 맞추기위해 강제로 MaxWalkSpeed 에 맞추기 위해 값을 조정
				XMStoreFloat3(&MoveStep, XMVectorScale(XMLoadFloat3(&MoveStep), (1.0f/XMVectorGetX(XMVector3Length(XMLoadFloat3(&MoveStep))) * step*5)));
				GetCharacterMovement()->AddInputVector(MoveStep, 1);
			}
		}
	}
}
