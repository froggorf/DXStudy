#include "CoreMinimal.h"
#include "UConvexComponent.h"

#include "UPhysicsEngine.h"


void UConvexComponent::SetStaticMesh(const std::shared_ptr<UStaticMesh>& InStaticMesh, ECollisionType Type)
{
	if (BaseStaticMesh.lock() && RigidActor)
	{
		// TODO: Unregister 해줘야함
		UnRegisterPhysics();
	}

	if (Type!= ECollisionType::NoCollision)
	{
		BaseStaticMesh = InStaticMesh;
		bIsDynamic = Type == ECollisionType::Dynamic;
		RegisterPhysics();	
	}
	
}


void UConvexComponent::RegisterSceneProxies()
{
	// TODO: 디버깅 드로우용
}

physx::PxRigidActor* UConvexComponent::CreateRigidActor()
{
	physx::PxRigidActor* Actor = nullptr;
	if (const std::shared_ptr<UStaticMesh>& StaticMesh = BaseStaticMesh.lock())
	{
		Actor = gPhysicsEngine->CreateAndRegisterConvexActor(GetComponentTransform(), StaticMesh, Mass, bIsDynamic);	
	}
	return Actor;
}

void UConvexComponent::DebugDraw()
{
}
