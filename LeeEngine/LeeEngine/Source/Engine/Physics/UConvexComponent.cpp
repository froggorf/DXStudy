#include "CoreMinimal.h"
#include "UConvexComponent.h"

#include "UPhysicsEngine.h"


UConvexComponent::UConvexComponent()
{
	Rename("ConvexComponent" + PrimitiveID);
}

void UConvexComponent::SetStaticMesh(const std::shared_ptr<UStaticMesh>& InStaticMesh)
{
	if (BaseStaticMesh.lock() && RigidActor)
	{
		UnRegisterPhysics();
	}

	BaseStaticMesh = InStaticMesh;
	RegisterPhysics();	

}


void UConvexComponent::RegisterSceneProxies()
{
}

physx::PxRigidActor* UConvexComponent::CreateRigidActor()
{
	physx::PxRigidActor* Actor = nullptr;
	if (const std::shared_ptr<UStaticMesh>& StaticMesh = BaseStaticMesh.lock())
	{
		bool bIsDynamic = CollisionEnabled == ECollisionEnabled::Physics? true : false;
		Actor = gPhysicsEngine->CreateAndRegisterConvexActor(GetComponentTransform(), StaticMesh, Mass, ConvexMeshVertexBuffer, bIsDynamic);	
	}
	return Actor;
}

void UConvexComponent::DebugDraw_RenderThread() const
{
	if (!ConvexMeshVertexBuffer)
	{
		return;
	}

	ID3D11DeviceContext* DeviceContext = GDirectXDevice->GetDeviceContext().Get();

	UINT stride = sizeof(MyVertexData);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, ConvexMeshVertexBuffer.GetAddressOf(), &stride, &offset);
	
	D3D11_BUFFER_DESC desc = {};
	ConvexMeshVertexBuffer->GetDesc(&desc);
	UINT vertexCount = desc.ByteWidth / sizeof(MyVertexData);

	DeviceContext->Draw(vertexCount, 0);
}

