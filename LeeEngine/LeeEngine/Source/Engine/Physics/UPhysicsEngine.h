// 06.24
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/UObject/UObject.h"

class UPhysicsEngine : public UObject
{
	MY_GENERATE_BODY(UPhysicsEngine)
public:
	UPhysicsEngine();
	~UPhysicsEngine() final;
	 
	void TickPhysics(float DeltaSeconds) const;

	/// Shape
	// Sphere Shape
	physx::PxShape* CreateSphereShape(const float Radius) const;

	/// Actor
	// 정적 물리 오브젝트 생성
	physx::PxRigidActor* CreateAndRegisterActor(const FTransform& Transform, physx::PxShape* InShape, const float Mass, bool bIsDynamic = true) const;
	// 정적 convex hull 물리 오브젝트 생성
	physx::PxRigidActor* CreateAndRegisterConvexActor(const FTransform& Transform, const std::shared_ptr<UStaticMesh>& StaticMesh, const float Mass, Microsoft::WRL::ComPtr<ID3D11Buffer>& OutVertexBuffer, bool bIsDynamic = true) const;
	static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateVertexBufferForConvexActor(const physx::PxConvexMesh* ConvexMesh);
	// Scene
	/*
	 * Note
	 * 해당 함수에서 RemoveActor 를 release 하고 nullptr로 만듦
	 */
	void UnRegisterActor(physx::PxRigidActor* RemoveActor) const;

protected:
	// StaticMesh정보를 통해 ConvexMesh를 만들어 반환해주는 함수
	physx::PxConvexMesh* CreateConvexMesh(const std::shared_ptr<UStaticMesh>& StaticMesh) const;

private:
public:
protected:
private:
	physx::PxDefaultAllocator			gAllocator;
	physx::PxDefaultErrorCallback		gErrorCallback;

	// 커스텀 Deleter를 통해 관리
	physx::PxFoundation* PxFoundation     = nullptr;
	physx::PxPhysics* PxPhysics        = nullptr;
	physx::PxScene* PxScene          = nullptr;
	physx::PxMaterial* DefaultMaterial = nullptr;
};

extern std::unique_ptr<UPhysicsEngine> gPhysicsEngine;