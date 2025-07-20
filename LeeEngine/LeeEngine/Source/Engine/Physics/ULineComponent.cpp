#include "CoreMinimal.h"
#include "ULineComponent.h"

ULineComponent::ULineComponent(bool bIsHit, const XMFLOAT3& Start, const XMFLOAT3& End, const XMFLOAT3& HitLocation, const XMFLOAT3& InTraceColor, const XMFLOAT3& InTraceHitColor)
{
    TraceColor = InTraceColor;
    TraceHitColor = InTraceHitColor;
	this->bIsHit = bIsHit;
    SetWorldLocation(XMFLOAT3{0,0,0});
    SetWorldRotation(XMVectorSet(0,0,0,1));
    

    CreateVertexBuffer(Start,End,HitLocation);
}

void ULineComponent::DebugDraw_RenderThread() const
{
    ID3D11DeviceContext* DeviceContext = GDirectXDevice->GetDeviceContext().Get();
    UINT stride = sizeof(MyVertexData);
    UINT offset = 0;

	

    GDirectXDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
    
	// NotHitLine
    {
        FDebugColor ColorBuffer;
        ColorBuffer.DebugColor = XMFLOAT4{TraceColor.x,TraceColor.y,TraceColor.z, 1.0f};
        GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_DebugDraw, &ColorBuffer, sizeof(ColorBuffer));

        DeviceContext->IASetVertexBuffers(0, 1, NonHitLineVertexBuffer.GetAddressOf(), &stride, &offset);
        DeviceContext->Draw(2, 0);    
    }

    // HitLine
    if (bIsHit){
        FDebugColor ColorBuffer;
        ColorBuffer.DebugColor = XMFLOAT4{TraceHitColor.x,TraceHitColor.y,TraceHitColor.z, 1.0f};
        GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_DebugDraw, &ColorBuffer, sizeof(ColorBuffer));

        DeviceContext->IASetVertexBuffers(0, 1, HitLineVertexBuffer.GetAddressOf(), &stride, &offset);
        DeviceContext->Draw(2, 0);    
    }

    // Topology를 바꿔주기 위해 bIsHit을 두번으로 나누어 렌더링
    GDirectXDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    GDirectXDevice->SetRSState(ERasterizerType::RT_TwoSided);
    // HitBox
    if (bIsHit)
    {
        FDebugColor ColorBuffer;
        ColorBuffer.DebugColor = XMFLOAT4{TraceColor.x,TraceColor.y,TraceColor.z, 1.0f};
        GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_DebugDraw, &ColorBuffer, sizeof(ColorBuffer));
        DeviceContext->IASetVertexBuffers(0, 1, HitLocationBoxVertexBuffer.GetAddressOf(), &stride, &offset);
        DeviceContext->Draw(36, 0);
    }
    GDirectXDevice->SetRSState(ERasterizerType::RT_WireFrame);
}

void ULineComponent::CreateVertexBuffer(const XMFLOAT3& Start, const XMFLOAT3& End, const XMFLOAT3& HitLocation)
{
    // Trace Line VertexBuffer
	{
        XMFLOAT3 TraceStart = Start;
        // 맞았으면 HitLocation까지만 빨간 트레이스, 아니면 끝까지 빨간트레이스
        XMFLOAT3 TraceEnd = bIsHit? HitLocation : End;

        std::vector<MyVertexData> VertexData;
        VertexData.push_back({TraceStart,{0,0,1},{0,0}});
        VertexData.push_back({TraceEnd,{0,0,1},{1,1}});

        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = static_cast<UINT>(sizeof(MyVertexData) * VertexData.size());
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA InitData = {};
        InitData.pSysMem = VertexData.data();

        GDirectXDevice->GetDevice()->CreateBuffer(&bd, &InitData, NonHitLineVertexBuffer.GetAddressOf());   
	}
    


    if (bIsHit)
    {
        // Trace Hit
		{
            XMFLOAT3 TraceStart = HitLocation;
            // 맞았으면 HitLocation까지만 빨간 트레이스, 아니면 끝까지 빨간트레이스
            XMFLOAT3 TraceEnd = End;

            std::vector<MyVertexData> VertexData;
            VertexData.push_back({TraceStart});
            VertexData.push_back({TraceEnd});

            D3D11_BUFFER_DESC bd = {};
            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.ByteWidth = static_cast<UINT>(sizeof(MyVertexData) * VertexData.size());
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = 0;

            D3D11_SUBRESOURCE_DATA InitData = {};
            InitData.pSysMem = VertexData.data();

            GDirectXDevice->GetDevice()->CreateBuffer(&bd, &InitData, HitLineVertexBuffer.GetAddressOf());   
		}

        // Hit Box
        {
            std::vector<MyVertexData> VertexData;
            float BoxSize = 20.f;
            XMFLOAT3 BoxExtent = {BoxSize,BoxSize,BoxSize };
            XMFLOAT3 Point[8] =
            {
                XMFLOAT3(HitLocation.x-BoxExtent.x, HitLocation.y-BoxExtent.y, HitLocation.z-BoxExtent.z), // 0: 좌하뒤
                XMFLOAT3(HitLocation.x+BoxExtent.x, HitLocation.y-BoxExtent.y, HitLocation.z-BoxExtent.z), // 1: 우하뒤
                XMFLOAT3(HitLocation.x+BoxExtent.x, HitLocation.y+BoxExtent.y, HitLocation.z-BoxExtent.z), // 2: 우상뒤
                XMFLOAT3(HitLocation.x-BoxExtent.x, HitLocation.y+BoxExtent.y, HitLocation.z-BoxExtent.z), // 3: 좌상뒤
                XMFLOAT3(HitLocation.x-BoxExtent.x, HitLocation.y-BoxExtent.y, HitLocation.z+BoxExtent.z), // 4: 좌하앞
                XMFLOAT3(HitLocation.x+BoxExtent.x, HitLocation.y-BoxExtent.y, HitLocation.z+BoxExtent.z), // 5: 우하앞
                XMFLOAT3(HitLocation.x+BoxExtent.x, HitLocation.y+BoxExtent.y, HitLocation.z+BoxExtent.z), // 6: 우상앞
                XMFLOAT3(HitLocation.x-BoxExtent.x, HitLocation.y+BoxExtent.y, HitLocation.z+BoxExtent.z)  // 7: 좌상앞
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

            D3D11_BUFFER_DESC bd = {};
            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.ByteWidth = static_cast<UINT>(sizeof(MyVertexData) * VertexData.size());
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = 0;

            D3D11_SUBRESOURCE_DATA InitData = {};
            InitData.pSysMem = VertexData.data();

            GDirectXDevice->GetDevice()->CreateBuffer(&bd, &InitData, HitLocationBoxVertexBuffer.GetAddressOf());    
        }
    }
}
