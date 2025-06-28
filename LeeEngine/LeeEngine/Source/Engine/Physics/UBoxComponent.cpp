#include "CoreMinimal.h"
#include "UBoxComponent.h"

UBoxComponent::UBoxComponent()
{
    Rename("BoxComponent"+PrimitiveID);
    bIsDynamic = false;
}

physx::PxRigidActor* UBoxComponent::CreateRigidActor()
{
    for (size_t i = 0; i < CollisionResponse.size(); ++i)
    {
        CollisionResponse[i] = ECollisionResponse::Overlap;
    }

	CreateVertexBuffer();

	return gPhysicsEngine->CreateAndRegisterActor(GetComponentTransform(), gPhysicsEngine->CreateBoxShape(BoxExtent), Mass, bIsDynamic);
}

void UBoxComponent::DebugDraw_RenderThread() const
{
    ID3D11DeviceContext* DeviceContext = GDirectXDevice->GetDeviceContext().Get();

    UINT stride = sizeof(MyVertexData);
    UINT offset = 0;
    DeviceContext->IASetVertexBuffers(0, 1, BoxMeshVertexBuffer.GetAddressOf(), &stride, &offset);

    DeviceContext->Draw(36, 0);
}

void UBoxComponent::CreateVertexBuffer()
{
	if (GetVertexBuffer())
	{
		GetVertexBuffer()->Release();
	}

    std::vector<MyVertexData> VertexData;

    XMFLOAT3 Point[8] =
    {
        XMFLOAT3(-BoxExtent.x, -BoxExtent.y, -BoxExtent.z), // 0: 좌하뒤
        XMFLOAT3( BoxExtent.x, -BoxExtent.y, -BoxExtent.z), // 1: 우하뒤
        XMFLOAT3( BoxExtent.x,  BoxExtent.y, -BoxExtent.z), // 2: 우상뒤
        XMFLOAT3(-BoxExtent.x,  BoxExtent.y, -BoxExtent.z), // 3: 좌상뒤
        XMFLOAT3(-BoxExtent.x, -BoxExtent.y,  BoxExtent.z), // 4: 좌하앞
        XMFLOAT3( BoxExtent.x, -BoxExtent.y,  BoxExtent.z), // 5: 우하앞
        XMFLOAT3( BoxExtent.x,  BoxExtent.y,  BoxExtent.z), // 6: 우상앞
        XMFLOAT3(-BoxExtent.x,  BoxExtent.y,  BoxExtent.z)  // 7: 좌상앞
    };

    // UV 좌표
    XMFLOAT2 UV[4] = {
        XMFLOAT2(0,1), XMFLOAT2(1,1), XMFLOAT2(1,0), XMFLOAT2(0,0)
    };

    VertexData.clear();
    VertexData.reserve(36);

    XMFLOAT3 FrontNormal(0,0,1);
    VertexData.push_back({Point[4], FrontNormal, UV[0]});
    VertexData.push_back({Point[5], FrontNormal, UV[1]});
    VertexData.push_back({Point[6], FrontNormal, UV[2]});
    VertexData.push_back({Point[4], FrontNormal, UV[0]});
    VertexData.push_back({Point[6], FrontNormal, UV[2]});
    VertexData.push_back({Point[7], FrontNormal, UV[3]});

    XMFLOAT3 BackNormal(0,0,-1);
    VertexData.push_back({Point[1], BackNormal, UV[0]});
    VertexData.push_back({Point[0], BackNormal, UV[1]});
    VertexData.push_back({Point[3], BackNormal, UV[2]});
    VertexData.push_back({Point[1], BackNormal, UV[0]});
    VertexData.push_back({Point[3], BackNormal, UV[2]});
    VertexData.push_back({Point[2], BackNormal, UV[3]});

    XMFLOAT3 RightNormal(1,0,0);
    VertexData.push_back({Point[5], RightNormal, UV[0]});
    VertexData.push_back({Point[1], RightNormal, UV[1]});
    VertexData.push_back({Point[2], RightNormal, UV[2]});
    VertexData.push_back({Point[5], RightNormal, UV[0]});
    VertexData.push_back({Point[2], RightNormal, UV[2]});
    VertexData.push_back({Point[6], RightNormal, UV[3]});

    // 왼쪽면 (-X)
    // 0 4 7 3
    XMFLOAT3 LeftNormal(-1,0,0);
    VertexData.push_back({Point[0], LeftNormal, UV[0]});
    VertexData.push_back({Point[4], LeftNormal, UV[1]});
    VertexData.push_back({Point[7], LeftNormal, UV[2]});
    VertexData.push_back({Point[0], LeftNormal, UV[0]});
    VertexData.push_back({Point[7], LeftNormal, UV[2]});
    VertexData.push_back({Point[3], LeftNormal, UV[3]});

    // 위쪽면 (+Y)
    // 3 7 6 2
    XMFLOAT3 TopNormal(0,1,0);
    VertexData.push_back({Point[3], TopNormal, UV[0]});
    VertexData.push_back({Point[7], TopNormal, UV[1]});
    VertexData.push_back({Point[6], TopNormal, UV[2]});
    VertexData.push_back({Point[3], TopNormal, UV[0]});
    VertexData.push_back({Point[6], TopNormal, UV[2]});
    VertexData.push_back({Point[2], TopNormal, UV[3]});

    // 아래쪽면 (-Y)
    // 0 1 5 4
    XMFLOAT3 BottomNormal(0,-1,0);
    VertexData.push_back({Point[0], BottomNormal, UV[0]});
    VertexData.push_back({Point[1], BottomNormal, UV[1]});
    VertexData.push_back({Point[5], BottomNormal, UV[2]});
    VertexData.push_back({Point[0], BottomNormal, UV[0]});
    VertexData.push_back({Point[5], BottomNormal, UV[2]});
    VertexData.push_back({Point[4], BottomNormal, UV[3]});

	// DirectX 11 버텍스 버퍼 생성
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = static_cast<UINT>(sizeof(MyVertexData) * VertexData.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = VertexData.data();

	GDirectXDevice->GetDevice()->CreateBuffer(&bd, &InitData, BoxMeshVertexBuffer.GetAddressOf());

}
