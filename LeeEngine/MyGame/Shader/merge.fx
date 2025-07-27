#include "Global.fx"
#include "TransformHelpers.hlsl"

Texture2D    txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_Merge(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // (Vx, Vy, Vz, 1.f) x matProj ==> (PX*Vz, PY*Vz, PZ*Vz, Vz)
    
    // RectMesh 가 -0.5 ~ 0.5 범위에 정점이 있기 때문에, 
    // -1 ~ 1 범위로 확장해서 전체 픽셀에 대해서 픽셀 쉐이더가 호출되도록 함
    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Merge(VS_OUT _in) : SV_Target
{
    float4 OutColor = (float4) 0.f;
       
    OutColor = txDiffuse.Sample(samLinear, _in.vUV);
    
    return OutColor;
}
