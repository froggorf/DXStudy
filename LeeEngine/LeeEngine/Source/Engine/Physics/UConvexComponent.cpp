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
		Actor = gPhysicsEngine->CreateAndRegisterConvexActor(GetComponentTransform(), StaticMesh, Mass, ConvexMeshVertexBuffer, bSimulatePhysics);	
	}
	else
	{
		MY_LOG("Warning", EDebugLogLevel::DLL_Warning, "No Valid StaticMesh");
	}
	return Actor;
}

void UConvexComponent::DebugDraw_RenderThread() const
{
	if (!ConvexMeshVertexBuffer)
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
	DeviceContext->IASetVertexBuffers(0, 1, ConvexMeshVertexBuffer.GetAddressOf(), &stride, &offset);
	
	D3D11_BUFFER_DESC desc = {};
	ConvexMeshVertexBuffer->GetDesc(&desc);
	UINT vertexCount = desc.ByteWidth / sizeof(MyVertexData);

	DeviceContext->Draw(vertexCount, 0);

	if (bRenderNavMesh)
	{
		GDirectXDevice->SetRSState(ERasterizerType::RT_WireFrame);
	}
}

void UConvexComponent::CreateVertexBufferForNavMesh(rcPolyMeshDetail* dmesh)
{
	if (dmesh->nverts == 0)
	{
		return;
	}

	bRenderNavMesh = true;

	std::vector<MyVertexData> VertexData(dmesh->ntris * 3);
	for (int i = 0; i < dmesh->ntris; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			int vi = dmesh->tris[i*4 + j]; // 정점 인덱스
			VertexData[i*3 + j].Pos.x = dmesh->verts[vi*3 + 0];
			VertexData[i*3 + j].Pos.y = dmesh->verts[vi*3 + 1] + 3.f;
			VertexData[i*3 + j].Pos.z = -dmesh->verts[vi*3 + 2];
		}
	}

	VertexData.shrink_to_fit();

	// DirectX 11 버텍스 버퍼 생성
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = static_cast<UINT>(sizeof(MyVertexData) * VertexData.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = VertexData.data();

	GDirectXDevice->GetDevice()->CreateBuffer(&bd, &InitData, ConvexMeshVertexBuffer.GetAddressOf());
}

