#include "CoreMinimal.h"
#include "AMyGameCharacterBase.h"

#include "Engine/World/UWorld.h"


void IDodgeInterface::LoadAnimMontages()
{
	// Dodge
	{
		AssetManager::GetAsyncAssetCache(DodgeMontageName[static_cast<int>(EDodgeDirection::Forward)], [this](std::shared_ptr<UObject> Object)
			{
				AM_Dodge[static_cast<int>(EDodgeDirection::Forward)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
			});
		AssetManager::GetAsyncAssetCache(DodgeMontageName[static_cast<int>(EDodgeDirection::Backward)], [this](std::shared_ptr<UObject> Object)
			{
				AM_Dodge[static_cast<int>(EDodgeDirection::Backward)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
			});
	}

	// Roll
	{
		AssetManager::GetAsyncAssetCache(RollMontageName[static_cast<int>(EDodgeDirection::Forward)], [this](std::shared_ptr<UObject> Object)
			{
				AM_Roll[static_cast<int>(EDodgeDirection::Forward)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
			});
		AssetManager::GetAsyncAssetCache(RollMontageName[static_cast<int>(EDodgeDirection::Backward)], [this](std::shared_ptr<UObject> Object)
			{
				AM_Roll[static_cast<int>(EDodgeDirection::Backward)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
			});
	}
	

}

void IDodgeInterface::DodgeEnd()
{
	MY_LOG("Log",EDebugLogLevel::DLL_Warning, "Dodge End");
	bIsDodging = false;
}

void IDodgeInterface::RollEnd()
{
	RemoveMonochromePostprocess();
	MY_LOG("Log",EDebugLogLevel::DLL_Warning, "Roll End");
}

void IDodgeInterface::AddMonochromePostprocess()
{
#ifdef WITH_EDITOR
	FPostProcessRenderData MonoPP = FPostProcessRenderData{0, "Monochrome",
		UMaterial::GetMaterialCache("M_Monochrome"),
		EMultiRenderTargetType::Editor_HDR};
#else
	FPostProcessRenderData MonoPP = FPostProcessRenderData{0, "Monochrome",
		UMaterial::GetMaterialCache("M_Monochrome"),
		EMultiRenderTargetType::SwapChain_HDR};
#endif
	MonoPP.SetClearDepthStencilTexture(false);
	MonoPP.SetClearRenderTexture(false);
	MonoPP.SetFuncBeforeRendering({[]()
		{
			// 거리 비례를 바인딩 해주기 위해서 해당 함수 적용
			const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& DeviceContext = GDirectXDevice->GetDeviceContext();
			const std::shared_ptr<UTexture>& ViewPositionTex = GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Deferred)->GetRenderTargetTexture(2);
			DeviceContext->PSSetShaderResources(11, 1, ViewPositionTex->GetSRV().GetAddressOf());

			APlayerController* PC = GEngine->GetWorld()->GetPlayerController();


			XMFLOAT3 Pos = PC->GetMonochromeCenterPos();
			XMMATRIX ViewMat = FRenderCommandExecutor::CurrentSceneData->GetViewMatrix();

			XMVECTOR WorldPos4 = XMVectorSet(Pos.x, Pos.y, Pos.z, 1.0f);
			XMVECTOR ViewPosVec = XMVector4Transform(WorldPos4, ViewMat);
			XMFLOAT4 ViewPos;
			XMStoreFloat4(&ViewPos, ViewPosVec);

			FMonochromeDataConstantBuffer Data;
			Data.Distance = PC->GetMonochromeDistance();
			Data.CenterPos = XMFLOAT3{ViewPos.x, ViewPos.y, ViewPos.z};


			GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_BloomBlur, &Data, sizeof(Data));
		}});
	FScene::AddPostProcess_GameThread(MonoPP);


#ifdef WITH_EDITOR
	FPostProcessRenderData NonMonoPP = FPostProcessRenderData{1, "NotMonochrome",
		UMaterial::GetMaterialCache("M_NotMonochrome"),
		EMultiRenderTargetType::Editor_HDR};
	NonMonoPP.SetClearRenderTexture(false);
	NonMonoPP.SetClearDepthStencilTexture(false);
	FScene::AddPostProcess_GameThread(NonMonoPP);
#else
	FPostProcessRenderData NonMonoPP = FPostProcessRenderData{1, "NotMonochrome",
		UMaterial::GetMaterialCache("M_NotMonochrome"),
		EMultiRenderTargetType::SwapChain_HDR};
	NonMonoPP.SetClearRenderTexture(false);
	NonMonoPP.SetClearDepthStencilTexture(false);
	FScene::AddPostProcess_GameThread(NonMonoPP);
#endif

}

void IDodgeInterface::RemoveMonochromePostprocess()
{
	FScene::RemovePostProcess_GameThread(0, "Monochrome");
	FScene::RemovePostProcess_GameThread(1, "NotMonochrome");
}



AMyGameCharacterBase::AMyGameCharacterBase()
{
	if (!GDirectXDevice) return;

	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = true;
		CharacterMovement->Acceleration = 4096.0f;
		CharacterMovement->RotationRate = XMFLOAT3{0.0f, 1500.0f, 0.0f};
		CharacterMovement->MaxWalkSpeed = 600;
		CharacterMovement->Braking = 4096;
	}

	SpringArm->SetArmLength(250);
	
	AssetManager::GetAsyncAssetCache("SK_Manny_UE4",[this](std::shared_ptr<UObject> Object)
		{
			SkeletalMeshComponent->SetSkeletalMesh(std::static_pointer_cast<USkeletalMesh>(Object));
		});
	SkeletalMeshComponent->SetAnimInstanceClass("UMyGameAnimInstanceBase");
	SkeletalMeshComponent->SetRelativeLocation({0,-85,0});
	SkeletalMeshComponent->SetRelativeRotation(XMFLOAT3{0,180,0});


	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetIsMonochromeObject(false);
	}
}

void AMyGameCharacterBase::Register()
{
	ACharacter::Register();

	IDodgeInterface::LoadAnimMontages();
}


void AMyGameCharacterBase::BeginPlay()
{
	GEngine->GetWorld()->GetPlayerController()->OnPossess(this);

	ACharacter::BeginPlay();

	CapsuleComp->SetCollisionObjectType(ECollisionChannel::Pawn);
	CapsuleComp->SetObjectType(ECollisionChannel::Pawn);
}

void AMyGameCharacterBase::BindKeyInputs()
{
	if (Controller)
	{
		if (std::shared_ptr<UPlayerInput> InputSystem = Controller->GetPlayerInput())
		{
			// Walk / Run Toggle
			//InputSystem->BindAction(EKeys::LShift, ETriggerEvent::Started, this, &AMyGameCharacterBase::SetWalk);
			//InputSystem->BindAction(EKeys::LShift, ETriggerEvent::Released, this, &AMyGameCharacterBase::SetRun);
			InputSystem->BindAction(EKeys::LShift, ETriggerEvent::Started, this, &AMyGameCharacterBase::Dodge);


			// Locomotion
			InputSystem->BindAxis2D(EKeys::W, ETriggerEvent::Trigger, 1, 0,this, &AMyGameCharacterBase::Move);
			InputSystem->BindAxis2D(EKeys::S, ETriggerEvent::Trigger, -1, 0,this, &AMyGameCharacterBase::Move);
			InputSystem->BindAxis2D(EKeys::D, ETriggerEvent::Trigger, 0, 1,this, &AMyGameCharacterBase::Move);
			InputSystem->BindAxis2D(EKeys::A, ETriggerEvent::Trigger, 0, -1,this, &AMyGameCharacterBase::Move);

			// Jump
			InputSystem->BindAction<AMyGameCharacterBase>(EKeys::Space, ETriggerEvent::Started, this, &AMyGameCharacterBase::Jump);
			// LookRotate
			InputSystem->BindAxis2D(EKeys::MouseXY2DAxis, ETriggerEvent::Trigger, 0,0, this, &AMyGameCharacterBase::Look);
			InputSystem->BindAction(EKeys::MouseRight, ETriggerEvent::Started, this, &AMyGameCharacterBase::MouseRotateStart);
			InputSystem->BindAction(EKeys::MouseRight, ETriggerEvent::Released, this, &AMyGameCharacterBase::MouseRotateEnd);

			// TargetArmLength Wheel
			InputSystem->BindAction(EKeys::MouseWheelUp, Started, this, &AMyGameCharacterBase::WheelUp);
			InputSystem->BindAction(EKeys::MouseWheelDown, Started, this, &AMyGameCharacterBase::WheelDown);
		}
		
	}
}

void AMyGameCharacterBase::AttackedWhileDodge()
{
	MY_LOG("TickRate", EDebugLogLevel::DLL_Warning, "Set TickRate To 0.1f")
	SetTickRate(0.1f);
	GEngine->GetTimerManager()->SetTimer(RollingEndHandle, {this, &AMyGameCharacterBase::RollEnd} , 1.0f, false);
	AddMonochromePostprocess();
}



float AMyGameCharacterBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser)
{
	if (bIsDodging)
	{
		ChangeToRoll();
		

		return DamageAmount;
	}
	

	return DamageAmount;
}

void AMyGameCharacterBase::Move(float X, float Y)
{
	XMFLOAT3 ForwardDirection = Controller->GetActorForwardVector();
	XMFLOAT3 RightDirection = Controller->GetActorRightVector();

	AddMovementInput(ForwardDirection, X);
	AddMovementInput(RightDirection, Y);
}

void AMyGameCharacterBase::Look(float X, float Y)
{
	// 우클릭이 되어있다면
	if (bRightButtonPressed && Controller && Controller->GetPlayerInput())
	{
		XMFLOAT2 Delta = Controller->GetPlayerInput()->LastMouseDelta;
		AddControllerYawInput(Delta.x);
		AddControllerPitchInput(Delta.y);
	}
}

void AMyGameCharacterBase::MouseRotateStart()
{
	bRightButtonPressed = true;
}

void AMyGameCharacterBase::MouseRotateEnd()
{
	bRightButtonPressed = false;
}

void AMyGameCharacterBase::Dodge()
{
	std::shared_ptr<UAnimInstance> AnimInstance = GetAnimInstance();
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (!AnimInstance || !MovementComp)
	{
		return;
	}

	bIsDodging = true;
	Delegate<> OnDodgeEnd;
	OnDodgeEnd.Add(this, &AMyGameCharacterBase::DodgeEnd);

	// TODO: 수정하기
	if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_W) || ImGui::IsKeyDown(ImGuiKey_A) || ImGui::IsKeyDown(ImGuiKey_D))
	{
		bIsBackDodge = false;

		AnimInstance->Montage_Play(AM_Dodge[static_cast<int>(EDodgeDirection::Forward)], 0, OnDodgeEnd);
	}
	else
	{
		bIsBackDodge = true;
		AnimInstance->Montage_Play(AM_Dodge[static_cast<int>(EDodgeDirection::Backward)],0, OnDodgeEnd);
	}
}

void AMyGameCharacterBase::DodgeEnd()
{
	IDodgeInterface::DodgeEnd();
}

void AMyGameCharacterBase::ChangeToRoll()
{
	std::shared_ptr<UAnimInstance> AnimInstance = GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}
	
	bIsDodging = false;
	GEngine->GetTimerManager()->SetTimer(AttackedWhileDodgingHandle, {this, &AMyGameCharacterBase::AttackedWhileDodge} , AttackedWhileDodgeTriggerTime, false);

	const std::shared_ptr<UAnimMontage>& PlayedMontage = bIsBackDodge? AM_Roll[static_cast<int>(EDodgeDirection::Backward)] : AM_Roll[static_cast<int>(EDodgeDirection::Forward)];
	GetAnimInstance()->Montage_Play(PlayedMontage);
	
}

void AMyGameCharacterBase::RollEnd()
{
	IDodgeInterface::RollEnd();

	SetTickRate(1.0f);
}

void AMyGameCharacterBase::SetWalk()
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 150.0f;
	}
}

void AMyGameCharacterBase::SetRun()
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 600.0f;
	}
}


void AMyGameCharacterBase::WheelUp()
{
	if (SpringArm)
	{
		SpringArm->SetArmLength(SpringArm->TargetArmLength - 10.0f);
	}
}

void AMyGameCharacterBase::WheelDown()
{
	if (SpringArm)
	{
		SpringArm->SetArmLength(SpringArm->TargetArmLength + 10.0f);
	}
}




void AMyGameCharacterBase::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}
