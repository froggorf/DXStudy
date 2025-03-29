// 03.24
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "USkeletalMeshComponent.h"

#include "Engine/Mesh/USkeletalMesh.h"
#include "Engine/SceneProxy/FSkeletalMeshSceneProxy.h"


USkeletalMeshComponent::USkeletalMeshComponent()
{
	Rename("SkeletalMeshComponent_" + std::to_string(ComponentID));
	AnimInstance = std::make_unique<UAnimInstance>();
	AnimInstance->SetSkeletalMeshComponent(this);

}

void USkeletalMeshComponent::Register()
{
	USkinnedMeshComponent::Register();

	if(AnimInstance)
	{
		AnimInstance->UpdateAnimation(0.0f);
	}
}

std::shared_ptr<FPrimitiveSceneProxy> USkeletalMeshComponent::CreateSceneProxy() const
{
	std::shared_ptr<FSkeletalMeshSceneProxy> SceneProxy = std::make_shared<FSkeletalMeshSceneProxy>(PrimitiveID,SkeletalMesh);
	return SceneProxy;

}

bool USkeletalMeshComponent::SetSkeletalMesh(const std::shared_ptr<USkeletalMesh>& NewMesh)
{
	if(nullptr == NewMesh)
	{
		MY_LOG("SetStaticMesh", EDebugLogLevel::DLL_Warning, "nullptr StaticMesh");
		return false;
	}

	if(NewMesh.get() == GetSkeletalMesh().get())
	{
		return false;
	}

	SkeletalMesh = NewMesh;

	return true;
}

void USkeletalMeshComponent::DrawDetailPanel(UINT ComponentDepth)
{
	USkinnedMeshComponent::DrawDetailPanel(ComponentDepth);

	if(ImGui::Button("Anim1"))
	{
		if(AnimInstance)
		{
			AnimInstance->SetAnimation(GEngine->TestAnim1);
		}
	}
	if(ImGui::Button("Anim2"))
	{
		if(AnimInstance)
		{
			AnimInstance->SetAnimation(GEngine->TestAnim2);
		}
	}
}

void USkeletalMeshComponent::SetAnimation(const std::shared_ptr<UAnimSequence>& InAnim)
{
	if(AnimInstance)
	{
		AnimInstance->SetAnimation(InAnim);
		AnimInstance->UpdateAnimation(0.0f);
	}
}

void USkeletalMeshComponent::TickComponent(float DeltaSeconds)
{
	USkinnedMeshComponent::TickComponent(DeltaSeconds);

	if(AnimInstance)
	{
		AnimInstance->UpdateAnimation(DeltaSeconds);
	}
}
