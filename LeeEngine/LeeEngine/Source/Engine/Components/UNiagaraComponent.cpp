#include "CoreMinimal.h"
#include "UNiagaraComponent.h"

#include "Engine/SceneProxy/FNiagaraSceneProxy.h"
#include "Engine/World/UWorld.h"

UNiagaraComponent::UNiagaraComponent()
{
	Rename("NiagaraComponent_" + std::to_string(ComponentID));
}

std::vector<std::shared_ptr<FPrimitiveSceneProxy>> UNiagaraComponent::CreateSceneProxy()
{
	if (nullptr == NiagaraAsset)
	{
		return {};
	}

	const auto& RenderData = NiagaraAsset->CreateDynamicRenderData();

	std::vector<std::shared_ptr<FPrimitiveSceneProxy>> SceneProxies;
	SceneProxies.reserve(RenderData.size());
	this->SceneProxies.reserve(RenderData.size());
	for (int i = 0; i < RenderData.size(); ++i)
	{
		auto NiagaraSceneProxy = std::make_shared<FNiagaraSceneProxy>(GetPrimitiveID(), RenderData[i]);
		SceneProxies.emplace_back(NiagaraSceneProxy);
		this->SceneProxies.emplace_back(NiagaraSceneProxy);
	}

	return SceneProxies;
}

void UNiagaraComponent::TickComponent(float DeltaSeconds)
{
	UPrimitiveComponent::TickComponent(DeltaSeconds);

	if (bIsActivate)
	{
		for (const auto& SceneProxy : SceneProxies)
		{
			GEngine->GetCurrentWorld()->AddToBeTickedNiagaraSceneProxy(SceneProxy);
		}
	}
}

void UNiagaraComponent::Activate()
{
	if (bIsActivate)
	{
		MY_LOG("Activate", EDebugLogLevel::DLL_Warning, "effect is already activate, if you want to reset, call activate after Deactivate");
		return;
	}
	bIsActivate = true;

	FScene::SetNiagaraEffectActivate_GameThread(SceneProxies, true);
}

void UNiagaraComponent::Deactivate()
{
	if (!bIsActivate)
	{
		MY_LOG("Deactivate", EDebugLogLevel::DLL_Warning, "effect is already deactivate");
		return;
	}
	bIsActivate = false;

	FScene::SetNiagaraEffectActivate_GameThread(SceneProxies, false);
}

void UNiagaraComponent::SetDelegateOnTickCS(const std::function<void()>& NewFunction)
{
	for (std::shared_ptr<FNiagaraSceneProxy>& SceneProxy : SceneProxies)
	{
		SceneProxy->DelegateOnTickCS = NewFunction;
	}
}

#ifdef WITH_EDITOR
void UNiagaraComponent::DrawDetailPanel(UINT ComponentDepth)
{
	if (ImGui::Button("Activate"))
	{
		Activate();
	}

	if (ImGui::Button("Deactivate"))
	{
		Deactivate();
	}
}
#endif
