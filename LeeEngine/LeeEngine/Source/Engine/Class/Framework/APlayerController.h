// 07.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UPlayerInput.h"
#include "Engine/Class/Camera/APlayerCameraManager.h"
#include "Engine/Components/UAnimMontage.h"
#include "Engine/GameFramework/AActor.h"

class ACharacter;
class UUserWidget;

class APlayerController : public AActor
{
	MY_GENERATE_BODY(APlayerController)
	

public:
	APlayerController();
	~APlayerController() override = default;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void TickWidget(float DeltaSeconds);

	XMFLOAT4 GetControlRotation() const {return GetActorRotation();}
	void AddYawInput(float Val);
	void AddPitchInput(float Val);

	void OnPossess(ACharacter* CharacterToPossess);

	

	std::shared_ptr<APlayerCameraManager> GetCameraManager() const
	{
		if (CameraManager.expired())
		{
			return nullptr;
		}
		return CameraManager.lock();
	}

	ACharacter* GetCharacter() const {return Character;}

	const std::shared_ptr<UPlayerInput>& GetPlayerInput() const {return PlayerInput;}

	bool IsPlayRootMotion() const {return bPlayRootMotion;}
	void SetPlayRootMotion(bool NewRootMotion) {bPlayRootMotion = NewRootMotion;}

	const std::shared_ptr<UUserWidget>& GetWidget(const std::string& WidgetName)
	{
		auto Iter = UserWidgets.find(WidgetName);
		if (Iter != UserWidgets.end())
		{
			return Iter->second;
		}

		return nullptr;
	}

	void CreateWidget(const std::string& Name, const std::shared_ptr<UUserWidget>& NewWidget);
	// 해당 위젯에서 Input을 소모했는지를 bool값으로 반환
	bool WidgetHandleInput(const FInputEvent& InputEvent);

private:
	ACharacter* Character;
	std::weak_ptr<APlayerCameraManager> CameraManager;

	// 플레이어 인풋을 관리하는 오브젝트
	std::shared_ptr<UPlayerInput> PlayerInput;

	// 현재 소유중인 캐릭터가 루트모션 애니메이션을 플레이하는지에 대한 변수
	bool bPlayRootMotion;

	std::map<std::string, std::shared_ptr<UUserWidget>> UserWidgets;

	std::shared_ptr<UAnimMontage> CurPlayingAnimMontage;
};
