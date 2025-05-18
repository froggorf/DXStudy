// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

// VFX 업데이트, Subsystem업데이트, 레벨 업데이트 같은 전체적인 역할을 관리하는 클래스
#pragma once

#include "Engine/MyEngineUtils.h"
#include "Engine/Level/ULevel.h"
#include "Engine/UObject/UObject.h"

class FNiagaraSceneProxy;
class USceneComponent;
class ULevel;
class AActor;

class UWorld : public UObject, public std::enable_shared_from_this<UWorld>
{
	MY_GENERATED_BODY(UWorld)
	UWorld();
	~UWorld() override;
	void Init() override;
	void PostLoad() override;

	void BeginPlay() override;

	void TickWorld(float DeltaSeconds);
	void Tick();

	std::shared_ptr<ULevel> GetPersistentLevel() const
	{
		return PersistentLevel;
	}

	void SetPersistentLevel(const std::shared_ptr<ULevel>& NewLevel);

	void LoadLevelInstanceByName(const std::string& NewLevelName);

	void AddLevel(const std::shared_ptr<ULevel>& NewLevel);
	void RemoveLevel(const std::shared_ptr<ULevel>& Level);

	void AddToBeTickedNiagaraSceneProxy(const std::shared_ptr<FNiagaraSceneProxy>& NewNiagaraSceneProxy)
	{
		ToBeTickedNiagaraSceneProxies.emplace_back(NewNiagaraSceneProxy);
	}

private:
	// 콜렉션과 관련된 현재 레벨
	std::shared_ptr<ULevel> PersistentLevel;

	// 콜렉션에 있는 모든 레벨정보
	std::set<std::shared_ptr<ULevel>> Levels;

	// Tick을 진행할 이펙트 들에 대한 SceneProxies
	std::vector<std::shared_ptr<FNiagaraSceneProxy>> ToBeTickedNiagaraSceneProxies;

	// 현재 선택된 액터
	// TODO: 추후 에디터 기능으로 분리하기
	std::shared_ptr<AActor>                       CurrentSelectedActor;
	std::vector<std::shared_ptr<USceneComponent>> SelectActorComponents;
	std::vector<std::string>                      SelectActorComponentNames;
	int                                           CurrentSelectedComponentIndex = -1;
};
