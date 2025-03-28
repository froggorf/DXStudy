// 03.24
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "USkeletalMeshComponent.h"

#include "Animation/Animator.h"
#include "Engine/Mesh/USkeletalMesh.h"
#include "Engine/SceneProxy/FSkeletalMeshSceneProxy.h"


USkeletalMeshComponent::USkeletalMeshComponent()
{
	Rename("SkeletalMeshComponent_" + std::to_string(ComponentID));
	SkinnedAnimator = std::make_unique<Animator>();
	

}

void USkeletalMeshComponent::Register()
{
	USkinnedMeshComponent::Register();

	if(SkinnedAnimator)
	{
		SkinnedAnimator->UpdateAnimation(0.0f, GetPrimitiveID());
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

void USkeletalMeshComponent::SetAnimation(Animation* InAnim)
{
	if(SkinnedAnimator)
	{
		SkinnedAnimator->SetAnimation(InAnim);
		SkinnedAnimator->UpdateAnimation(0.0f, GetPrimitiveID());
	}
}

void USkeletalMeshComponent::TickComponent(float DeltaSeconds)
{
	USkinnedMeshComponent::TickComponent(DeltaSeconds);

	if(SkinnedAnimator)
	{
		SkinnedAnimator->UpdateAnimation(DeltaSeconds, GetPrimitiveID());
	}
}
