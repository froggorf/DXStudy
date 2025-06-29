// 03.24
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once
#include "Engine/MyEngineUtils.h"
#include "USkinnedMeshComponent.h"
#include "Engine/Animation/UAnimInstance.h"
#include "Engine/Mesh/USkeletalMesh.h"

class USkeletalMesh;

class USkeletalMeshComponent : public USkinnedMeshComponent
{
	MY_GENERATE_BODY(USkeletalMeshComponent)
	USkeletalMeshComponent();
	void                                               BeginPlay() override;
	void                                               Register() override;
	std::vector<std::shared_ptr<FPrimitiveSceneProxy>> CreateSceneProxy() override;
	virtual bool                                       SetSkeletalMesh(const std::shared_ptr<USkeletalMesh>& NewMesh);

	const std::shared_ptr<USkeletalMesh>& GetSkeletalMesh() const
	{
		return SkeletalMesh;
	}

	FTransform GetSocketTransform(const std::string& InSocketName) override;

	void SetAnimInstanceClass(const std::string& InAnimInstanceClass);

	void TickComponent(float DeltaSeconds) override;

private:
	std::shared_ptr<USkeletalMesh> SkeletalMesh;
	std::shared_ptr<UAnimInstance> AnimInstance;
};
