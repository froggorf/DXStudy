// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include "Engine/MyEngineUtils.h"

#include "UActorComponent.h"

class USceneComponent : public UActorComponent, public std::enable_shared_from_this<USceneComponent>
{
public:
	USceneComponent();

	const std::shared_ptr<USceneComponent>& GetAttachParent() const { return AttachParent; }
	std::string GetAttachSocketName() const { return AttachSocketName; }

	// 월드 내 배치 되기 전 컴퍼넌트를 붙이는 함수
	void SetupAttachment(const std::shared_ptr<USceneComponent>& InParent, std::string_view InSocketName = "");

	// Relative Transform 과 부모 변환행렬을 이용해 월드 Transform을 만들어내는 함수
	virtual void UpdateComponentToWorld() override final
	{
		UpdateComponentToWorldWithParent(GetAttachParent(), GetAttachSocketName());
	}

	XMFLOAT3 GetRelativeLocation() const {return RelativeLocation;}
	XMFLOAT3 GetRelativeRotation() const {return RelativeRotation;}
	XMFLOAT3 GetRelativeScale3D() const {return RelativeScale3D;}

	void SetRelativeLocation(const DirectX::XMFLOAT3& NewRelLocation);
	void SetRelativeRotation(const DirectX::XMFLOAT3& NewRelRotation);
	void SetRelativeScale3D(const DirectX::XMFLOAT3& NewRelScale3D);

	// 언리얼엔진에서 FTransform 값 복사를 통해 반환하도록 구현 (값을 받아 적용시킬 일이 존재할 것으로 추측)
	FTransform GetComponentToWorld() const {return ComponentToWorld;}

	const std::vector<std::shared_ptr<USceneComponent>>& GetAttachChildren() const {return AttachChildren;}

	virtual void TestDraw();
	virtual void TestDrawComponent();
protected:
private:
	void SetAttachParent(const std::shared_ptr<USceneComponent>& NewAttachParent);
	void SetAttachSocketName(std::string_view NewSocketName);

	void UpdateComponentToWorldWithParent(const std::shared_ptr<USceneComponent>& Parent, std::string_view SocketName);
public:
protected:
private:
	// Relative Transform
	DirectX::XMFLOAT3 RelativeLocation = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
	DirectX::XMFLOAT3 RelativeRotation = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
	DirectX::XMFLOAT3 RelativeScale3D = DirectX::XMFLOAT3(1.0f,1.0f,1.0f);

	// 컴퍼넌트 월드 좌표
	FTransform ComponentToWorld;

	// 현재 부착된 부모 컴퍼넌트
	std::shared_ptr<USceneComponent> AttachParent;

	// 현재 부착된 소켓
	std::string AttachSocketName;

	// 부착된 자식 컴퍼넌트들
	std::vector<std::shared_ptr<USceneComponent>> AttachChildren;

};