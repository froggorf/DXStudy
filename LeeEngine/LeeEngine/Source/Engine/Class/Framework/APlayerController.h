// 07.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UPlayerInput.h"
#include "Engine/Class/Camera/APlayerCameraManager.h"
#include "Engine/Components/UAnimMontage.h"
#include "Engine/GameFramework/AActor.h"

class FBTNode;
class UBehaviorTree;
class ACharacter;
class UUserWidget;

class AController : public AActor
{
	MY_GENERATE_BODY(AController)

public:
	AController();
	~AController() override = default;
	void Tick(float DeltaSeconds) override;

	XMFLOAT4 GetControlRotation() const {return GetActorRotation();}

	bool IsPlayRootMotion() const {return bPlayRootMotion;}
	void SetPlayRootMotion(bool NewRootMotion) {bPlayRootMotion = NewRootMotion;}

	virtual void OnPossess(ACharacter* CharacterToPossess);
	ACharacter* GetCharacter() const {return Character;}
protected:
	ACharacter* Character = nullptr;
	// 현재 소유중인 캐릭터가 루트모션 애니메이션을 플레이하는지에 대한 변수
	bool bPlayRootMotion = false;

};

class AAIController : public AController
{
	MY_GENERATE_BODY(AAIController)
public:
	AAIController();
	~AAIController() override = default;
	void Tick(float DeltaSeconds) override;

	void SetAIActivate(bool NewActive) { bAIActivate = NewActive; }
	void SetBehaviorTree(const std::shared_ptr<UBehaviorTree>& NewBT);
	const std::shared_ptr<UBehaviorTree>& GetBehaviorTree() const {return BehaviorTree;}

protected:
	bool bAIActivate = true;

	std::shared_ptr<UBehaviorTree> BehaviorTree;
};

class APlayerController : public AController
{
	MY_GENERATE_BODY(APlayerController)
	

public:
	APlayerController();
	~APlayerController() override = default;
	void Register() override;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void TickWidget(float DeltaSeconds);
	std::shared_ptr<AActor> GetPlayerCharacter() const {return OwningPlayerCharacter;}

	void AddYawInput(float Val);
	void AddPitchInput(float Val);

	void OnPossess(ACharacter* CharacterToPossess) override;
	

	std::shared_ptr<APlayerCameraManager> GetCameraManager() const;
	const std::shared_ptr<UPlayerInput>& GetPlayerInput() const {return PlayerInput;}

	std::shared_ptr<UUserWidget> GetWidget(const std::string& WidgetName);

	void CreateWidget(const std::string& Name, const std::shared_ptr<UUserWidget>& NewWidget);
	void AddToViewport(const std::string& Name, const std::shared_ptr<UUserWidget>& NewWidget);
	void RemoveFromParent(const std::string& Name);
	// 해당 위젯에서 Input을 소모했는지를 bool값으로 반환
	bool WidgetHandleInput(const FInputEvent& InputEvent);

private:
	std::weak_ptr<APlayerCameraManager> CameraManager;
	// 플레이어 인풋을 관리하는 오브젝트
	std::shared_ptr<UPlayerInput> PlayerInput;
	std::map<std::string, std::shared_ptr<UUserWidget>> UserWidgets;
	std::shared_ptr<UAnimMontage> CurPlayingAnimMontage;

	// AActor* Character 에 대한 shared_ptr를 따로 관리해둠
	std::shared_ptr<AActor> OwningPlayerCharacter;

};
