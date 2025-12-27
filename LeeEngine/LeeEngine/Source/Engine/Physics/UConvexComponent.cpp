#include "CoreMinimal.h"
#include "UConvexComponent.h"

#include "UPhysicsEngine.h"


UConvexComponent::UConvexComponent()
{
	Rename("ConvexComponent" + std::to_string(PrimitiveID));
}

void UConvexComponent::SetStaticMesh(const std::shared_ptr<UStaticMesh>& InStaticMesh)
{
	if (BaseStaticMesh.lock() && RigidActor)
	{
		UnRegisterPhysics();
	}

	if (ECollisionEnabled::NoCollision == CollisionEnabled)
	{
		return;
	}

	BaseStaticMesh = InStaticMesh;
	RegisterPhysics();	

}

std::shared_ptr<UStaticMesh> UConvexComponent::GetStaticMesh() const
{
	return BaseStaticMesh.lock();
}


void UConvexComponent::RegisterSceneProxies()
{
}

physx::PxRigidActor* UConvexComponent::CreateRigidActor()
{
	physx::PxRigidActor* Actor = nullptr;
	if (const std::shared_ptr<UStaticMesh>& StaticMesh = BaseStaticMesh.lock())
	{
		if (bSimulatePhysics)
		{
			TriangleMeshVertexBuffer = nullptr;
			Actor = GPhysicsEngine->CreateConvexActor(GetComponentTransform(), StaticMesh, Mass, ConvexMeshVertexBuffer, bSimulatePhysics);		
		}
		else
		{
			ConvexMeshVertexBuffer = nullptr;
			Actor =GPhysicsEngine->CreateTriangleMeshActor(GetComponentTransform(), StaticMesh , Mass, TriangleMeshVertexBuffer, bSimulatePhysics);
		}
		
		
	}
	else
	{
		MY_LOG("Warning", EDebugLogLevel::DLL_Warning, "No Valid StaticMesh");
	}
	return Actor;
}

void UConvexComponent::DebugDraw_RenderThread() const
{
	const Microsoft::WRL::ComPtr<ID3D11Buffer>& VertexBuffer = bSimulatePhysics? ConvexMeshVertexBuffer : TriangleMeshVertexBuffer;
	if (!VertexBuffer)
	{
		return;
	}

	if (bRenderNavMesh)
	{
		GDirectXDevice->SetRSState(ERasterizerType::RT_TwoSided);
	}

	ID3D11DeviceContext* DeviceContext = GDirectXDevice->GetDeviceContext().Get();

	UINT stride = sizeof(MyVertexData);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);
	
	D3D11_BUFFER_DESC desc = {};
	VertexBuffer->GetDesc(&desc);
	UINT vertexCount = desc.ByteWidth / sizeof(MyVertexData);

	DeviceContext->Draw(vertexCount, 0);

	if (bRenderNavMesh)
	{
		GDirectXDevice->SetRSState(ERasterizerType::RT_WireFrame);
	}
}

