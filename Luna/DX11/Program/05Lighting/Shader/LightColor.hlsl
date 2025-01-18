#include "LightHelper.hlsl"



Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer cbPerFrame : register(b0)
{
	matrix View;
	matrix Projection;
}

cbuffer cbPerObject : register(b1)
{
	matrix World;
    float4x4 WorldInvTranspose;
    Material ObjectMaterial;
};

cbuffer cbLight : register(b2)
{
	DirectionalLight gDirLight;
    float3 gEyePosW;
    float pad;
}

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 NormalW : TEXCOORD1;
    float2 Tex : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );

    // 노말벡터를 월드좌표계로
    output.NormalW = mul(Normal, (float3x3)WorldInvTranspose);
    output.NormalW = normalize(output.NormalW);

    output.Tex = TexCoord;
    
		
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    float4 color = txDiffuse.Sample( samLinear, input.Tex );

    // 조명 계산시 사용될 변수 초기화
    float4 ambient = 0;
    float4 diffuse = 0;
    float4 spec    = 0;

    float3 toEye = normalize(gEyePosW - input.Pos.xyz);

    // 디렉셔널 라이트 계산
    ComputeDirectionalLight(ObjectMaterial, gDirLight, input.NormalW, toEye,ambient,diffuse,spec);

    float4 finalColor = color * (ambient + diffuse) + spec;
    


    return finalColor;
}