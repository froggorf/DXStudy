#include "Global.fx"
#include "TransformHelpers.hlsl"

Texture2D    txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_OUTPUT
{
	float4 PosScreen : SV_POSITION;
	float3 PosWorld : POSITION;
	float4 PosLightSpace : POSITION1;
	float3 NormalW : TEXCOORD1;
	float2 Tex : TEXCOORD;
	float  Depth : TEXCOORD2;
};



struct PS_OUT
{
    float4 vColor       : SV_Target;
    float4 vNormal      : SV_Target1;
    float4 vPosition    : SV_Target2;
    float4 vEmissive    : SV_Target3;
    float4 vCustom      : SV_Target4;
};


PS_OUT PS_Std3D_Deferred(VS_OUTPUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
       
    output.vColor = float4(0.7f, 0.7f, 0.7f, 1.f);
    //float3 vViewNormal = _in.vViewNormal;
    
    output.vColor = txDiffuse.Sample(samLinear, _in.Tex);
        
    output.vNormal = float4(1.f,0.f,0.f, 1.f);    
    output.vPosition = float4(0.f, 1.f, 0.f, 1.f);
    output.vEmissive = float4(0.f, 0.f, 1.f, 1.f);
    output.vCustom = float4(1.f, 0.f, 1.f, 1.f);
           
    return output;
}
