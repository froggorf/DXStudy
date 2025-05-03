#include "Global.fx"

struct VS_IN
{
    float3 vPos : POSITION;  
    uint InstID : SV_InstanceID;
};

struct VS_OUT
{
    float3 vPos : POSITION; // NDC 좌표계 위치값을 전달
    uint InstID : FOG;
};

VS_OUT VS_BillboardSpriteParticle(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.vPos   = float3(0.0f,0.0f,0.0f);
    output.InstID = _in.InstID;

    return output;
}


// GeometryShader
// 1.OutStream 에 정점 출력
//  만약에 아무것도 출력하지 않으면, 레스터라이저로 전달되는 정점이 하나도 없게 됨 ==> 파이프라인 종료

// 2. PointMesh 를 이용해서 RectMesh 로 정점분할을 한다.
//  GS 단계에서 View Space 기준으로 사각형을 만든다.
//  Billboard 효과 구현
struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV       : TEXCOORD;
    uint   InstID    : FOG;
};

[maxvertexcount(6)]
void GS_Billboard(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
    //tParticle particle = g_Particle[_in[0].InstID];

    //if (false == particle.Active)
    //    return;

    // WorldSpace -> ViewSpace
    //float4 vViewPos = mul(float4(particle.WorldPos, 1.f), g_matView);
	float4 vViewPos = mul(float4(0.0f,0.0f,0.0f, 1.f), gView);

    // 정점 4개 위치 설정
    // 0 -- 1
    // | \  |
    // 3 -- 2
    GS_OUT arrOut[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };

    //arrOut[0].vPosition = float4(vViewPos.x - particle.WorldScale.x / 2.f, vViewPos.y + particle.WorldScale.y / 2.f, vViewPos.z, 1.f);
    //arrOut[1].vPosition = float4(vViewPos.x + particle.WorldScale.x / 2.f, vViewPos.y + particle.WorldScale.y / 2.f, vViewPos.z, 1.f);
    //arrOut[2].vPosition = float4(vViewPos.x + particle.WorldScale.x / 2.f, vViewPos.y - particle.WorldScale.y / 2.f, vViewPos.z, 1.f);
    //arrOut[3].vPosition = float4(vViewPos.x - particle.WorldScale.x / 2.f, vViewPos.y - particle.WorldScale.y / 2.f, vViewPos.z, 1.f);

    arrOut[0].vPosition = float4(vViewPos.x - 5000.0f / 2.f, vViewPos.y + 3000.0f / 2.f, vViewPos.z, 1.f);
    arrOut[1].vPosition = float4(vViewPos.x + 5000.0f / 2.f, vViewPos.y + 3000.0f / 2.f, vViewPos.z, 1.f);
    arrOut[2].vPosition = float4(vViewPos.x + 5000.0f / 2.f, vViewPos.y - 3000.0f / 2.f, vViewPos.z, 1.f);
    arrOut[3].vPosition = float4(vViewPos.x - 5000.0f / 2.f, vViewPos.y - 3000.0f / 2.f, vViewPos.z, 1.f);

    arrOut[0].vUV = float2(0.f, 0.f);
    arrOut[1].vUV = float2(1.f, 0.f);
    arrOut[2].vUV = float2(1.f, 1.f);
    arrOut[3].vUV = float2(0.f, 1.f);    

    // ViewSpace -> Projection
    for (int i = 0; i < 4; ++i)
    {
        arrOut[i].vPosition = mul(arrOut[i].vPosition, gProjection);
        arrOut[i].InstID = _in[0].InstID;
    }    

    _OutStream.Append(arrOut[0]);
    _OutStream.Append(arrOut[1]);
    _OutStream.Append(arrOut[2]);
    _OutStream.RestartStrip();

    _OutStream.Append(arrOut[0]);
    _OutStream.Append(arrOut[2]);
    _OutStream.Append(arrOut[3]);
    _OutStream.RestartStrip();
}


float4 PS_Particle(GS_OUT _in) : SV_Target
{  
    float4 vColor = float4(1.f, 0.f, 1.f, 1.f);

	return vColor;    
}