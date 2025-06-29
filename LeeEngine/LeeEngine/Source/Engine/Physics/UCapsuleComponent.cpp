#include "CoreMinimal.h"
#include "UCapsuleComponent.h"

UCapsuleComponent::UCapsuleComponent()
{
    Rename("CapsuleComponent"+PrimitiveID);

    SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    for (size_t i = 0; i < CollisionResponse.size(); ++i)
    {
        CollisionResponse[i] = ECollisionResponse::Overlap;
    }
}

physx::PxRigidActor* UCapsuleComponent::CreateRigidActor()
{
	CreateVertexBuffer();

    bool bIsDynamic = CollisionEnabled == ECollisionEnabled::Physics? true : false;
	return gPhysicsEngine->CreateAndRegisterActor(GetComponentTransform(), gPhysicsEngine->CreateCapsuleShape(Radius,HalfHeight), Mass, bIsDynamic);
}

void UCapsuleComponent::DebugDraw_RenderThread() const
{
    ID3D11DeviceContext* DeviceContext = GDirectXDevice->GetDeviceContext().Get();

    UINT stride = sizeof(MyVertexData);
    UINT offset = 0;
    DeviceContext->IASetVertexBuffers(0, 1, CapsuleMeshVertexBuffer.GetAddressOf(), &stride, &offset);

    DeviceContext->Draw(CapsuleMeshVertexCount, 0);
}

void UCapsuleComponent::CreateVertexBuffer()
{
/// 06.29 -> AI를 통해 생성된 코드
    int SliceCount = 16;                 // 수평 분할
    int StackCount = 12;                 // 수직 분할 (전체 스택)

    std::vector<MyVertexData> VertexData;

    // (1) 모든 링의 포인트를 미리 생성
    std::vector<std::vector<XMFLOAT3>> Rings;
    for (int stack = 0; stack <= StackCount; ++stack)
    {
        float t   = static_cast<float>(stack) / StackCount;
        float phi = XM_PI * t; // 0 ~ PI

        float y = cosf(phi) * Radius;
        float r = sinf(phi) * Radius;

        // 상단 반구는 y에 +HalfHeight, 하단 반구는 -HalfHeight로 이동
        if (stack < StackCount / 2)
            y += HalfHeight;
        else if (stack > StackCount / 2)
            y -= HalfHeight;
        // 중간(원통)은 y 그대로

        std::vector<XMFLOAT3> ring;
        for (int slice = 0; slice <= SliceCount; ++slice)
        {
            float theta = XM_2PI * float(slice) / SliceCount;
            float x = r * cosf(theta);
            float z = r * sinf(theta);
            ring.push_back(XMFLOAT3(x, y, z));
        }
        Rings.push_back(ring);
    }

    // (2) 삼각형 리스트로 버텍스 풀어서 저장
    for (int stack = 0; stack < StackCount; ++stack)
    {
        const auto& ring0 = Rings[stack];
        const auto& ring1 = Rings[stack + 1];

        for (int slice = 0; slice < SliceCount; ++slice)
        {
            // 첫 삼각형
            VertexData.push_back({ ring0[slice] });
            VertexData.push_back({ ring1[slice] });
            VertexData.push_back({ ring1[slice + 1] });

            // 두 번째 삼각형
            VertexData.push_back({ ring0[slice] });
            VertexData.push_back({ ring1[slice + 1] });
            VertexData.push_back({ ring0[slice + 1] });
        }
    }

    // DirectX 11 버텍스 버퍼 생성
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = static_cast<UINT>(sizeof(MyVertexData) * VertexData.size());
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = VertexData.data();

    GDirectXDevice->GetDevice()->CreateBuffer(&bd, &InitData, CapsuleMeshVertexBuffer.GetAddressOf());

    CapsuleMeshVertexCount = static_cast<UINT>(VertexData.size());
}
