﻿// 06.25
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/Components/UPrimitiveComponent.h"

/*
 * 렌더링 쓰레드의 SceneProxy에 1:1 매칭 되는 UPrimitiveComponent 는
 * 충돌체인 UShapeComponent를 가질 수 있음
 * UShapeComponent는
 * UBoxComponent / USphereComponent / UConvexComponent / ... 가 상속받음
 */
inline bool bShowCollision = false;

class UShapeComponent : public UPrimitiveComponent
{
	MY_GENERATE_BODY(UShapeComponent)
public:
	UShapeComponent();
	~UShapeComponent() override = default;

	physx::PxRigidActor* GetRigidActor() const {return RigidActor;}
protected:
	// 충돌체에 대한 오브젝트
	physx::PxRigidActor* RigidActor = nullptr;

	float Mass = 1.0f;

public:
	// UShapeComponent는 Register시 Collision Scene에 추가해줘야함
	void Register() override;
	void UnRegister() override;
	virtual void RegisterPhysics();
	virtual void UnRegisterPhysics();
	void ResetPhysics();

	void TickComponent(float DeltaSeconds) override;

	virtual physx::PxRigidActor* CreateRigidActor(){ return nullptr; }

	virtual Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const {return nullptr; }
	
	void AddForce(const XMFLOAT3& Force) const;

	void SetWorldTransform(const FTransform& NewTransform);

	// 기존 PrimitiveComponent는 오브젝트의 렌더링을 등록하지만,
	// ShapeComponent 는 디버깅 드로우만 연결해줘야함
	// UShapeComponent를 상속받는 클래스는 해당 함수를 재정의 해줘야함
	void RegisterSceneProxies() override {}


	// 콜리젼 타입을 변경하는 함수
	void SetObjectType(ECollisionChannel Channel);
	// 콜리젼 반응을 변경하는 함수
	void SetResponseToChannel(ECollisionChannel Channel, ECollisionResponse NewResponse);
	void UpdatePhysicsFilterData();
	void SetSimulatePhysics(bool bNewSimulatePhysics);
	void SetKinematicRigidBody(bool bNewKinematic);

	virtual void DebugDraw_RenderThread() const {}



	void SetDebugDraw(bool bNewDrawDebug)
	{
		bDrawDebug = bNewDrawDebug;
	}

	ECollisionEnabled CollisionEnabled;
	ECollisionEnabled GetCollisionEnabled() const {return CollisionEnabled;}
	void SetCollisionEnabled(ECollisionEnabled NewType);
protected:
	UINT LastDrawDebugRenderThreadFrame = -1;
	bool bDrawDebug = false;
	ECollisionChannel ObjectType = ECollisionChannel::Visibility;
	bool bSimulatePhysics = false;
	std::array<ECollisionResponse, static_cast<UINT>(ECollisionChannel::Count)> CollisionResponse;
};