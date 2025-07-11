﻿// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include "Engine/MyEngineUtils.h"

#include "UActorComponent.h"
#include "Engine/UEngine.h"

class USceneComponent : public UActorComponent, public std::enable_shared_from_this<USceneComponent>
{
	MY_GENERATE_BODY(USceneComponent)
	USceneComponent();

	void Register() override;
	void BeginPlay() override;
	void TickComponent(float DeltaSeconds) override;

	const std::shared_ptr<USceneComponent>& GetAttachParent() const
	{
		return AttachParent;
	}

	std::string GetAttachSocketName() const
	{
		return AttachSocketName;
	}

	// 월드 내 배치 되기 전 컴퍼넌트를 붙이는 함수
	void SetupAttachment(const std::shared_ptr<USceneComponent>& InParent, std::string_view InSocketName = "");

	// Relative Transform 과 부모 변환행렬을 이용해 월드 Transform을 만들어내는 함수
	void UpdateComponentToWorld() final
	{
		UpdateComponentToWorldWithParent(GetAttachParent(), GetAttachSocketName());

		Make_Transform_Dirty()
	}

	XMFLOAT3 GetWorldLocation() const
	{
		return ComponentToWorld.GetTranslation();
	}

	XMFLOAT3 GetRelativeLocation() const
	{
		return RelativeLocation;
	}

	XMFLOAT4 GetRelativeRotation() const
	{
		return RelativeRotation;
	}

	XMFLOAT3 GetRelativeScale3D() const
	{
		return RelativeScale3D;
	}

	void SetRelativeLocation(const XMFLOAT3& NewRelLocation);
	void SetRelativeRotation(const XMFLOAT3& NewRelRotationPitchYawRoll);
	void SetRelativeRotation(const XMFLOAT4& NewRelRotation);
	void SetRelativeRotation(const XMVECTOR& NewRelRotation);
	void SetRelativeScale3D(const XMFLOAT3& NewRelScale3D);

	void AddWorldOffset(const XMFLOAT3& DeltaLocation);
	void AddWorldRotation(const XMFLOAT3& DeltaRotation);

	void SetWorldLocation(const XMFLOAT3& NewLocation);
	void SetWorldRotation(const XMVECTOR& NewRotation);

	const FTransform& GetComponentTransform() const
	{
		return ComponentToWorld;
	}

	// 특정 본 / 소켓의 Transform을 구하는 함수
	virtual FTransform GetSocketTransform(const std::string& InSocketName);

	const std::vector<std::shared_ptr<USceneComponent>>& GetAttachChildren() const
	{
		return AttachChildren;
	}

	// Primitive Component 전용
	virtual UINT GetPrimitiveID() const
	{
		return PrimitiveID;
	}

	virtual bool IsPrimitive() const
	{
		return bIsPrimitive;
	}

#ifdef WITH_EDITOR
	void DrawDetailPanel(UINT ComponentDepth) override;
#endif

private:
	void SetAttachParent(const std::shared_ptr<USceneComponent>& NewAttachParent);
	void SetAttachSocketName(std::string_view NewSocketName);

	void UpdateComponentToWorldWithParent(const std::shared_ptr<USceneComponent>& Parent,  const std::string& SocketName);

	// 월드 Rotation으로부터 Relative Rotation 계산
	XMVECTOR GetRelativeRotationFromWorld(const XMVECTOR& NewWorldRotation);

protected:
	bool bIsPrimitive = false;
	UINT PrimitiveID  = -1;

private:
	// Relative Transform
	XMFLOAT3 RelativeLocation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4 RelativeRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT3 RelativeScale3D  = XMFLOAT3(1.0f, 1.0f, 1.0f);

	// 컴퍼넌트 월드 좌표
	FTransform ComponentToWorld;

	// 현재 부착된 부모 컴퍼넌트
	std::shared_ptr<USceneComponent> AttachParent;

	// 현재 부착된 소켓
	std::string AttachSocketName;

	// 부착된 자식 컴퍼넌트들
	std::vector<std::shared_ptr<USceneComponent>> AttachChildren;
};
