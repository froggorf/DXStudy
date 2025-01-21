#pragma target 4.0
#pragma enable_d3d11_debug_symbols

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
    PointLight gPointLight;
    float3 gEyePosW;
    float pad;
}

cbuffer cbFog : register(b3)
{
	float4 fogColor;
    float fogStart;
    float fogEnd;
    float pad1;
    float pad2;
}

struct VS_OUTPUT
{
    float4 PosScreen : SV_POSITION;
    float3 PosWorld : POSITION;
    float3 NormalW : TEXCOORD1;
    float2 Tex : TEXCOORD;
    float Depth : TEXCOORD2;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.PosScreen = mul( Pos, World );
    // 픽셀 셰이더 내에서 사용하기 위해
    output.PosWorld = output.PosScreen.xyz;

    output.PosScreen = mul( output.PosScreen, View );
    output.Depth = output.PosScreen.z;
    output.PosScreen = mul( output.PosScreen, Projection );

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

    float3 toEye = normalize(gEyePosW - input.PosWorld);

    float4 A, D, S;

    ComputeDirectionalLight(ObjectMaterial, gDirLight, input.NormalW, toEye,A,D,S);
    ambient += A; diffuse += D; spec += S;

    ComputePointLight(ObjectMaterial,gPointLight,input.PosWorld ,input.NormalW, toEye, A,D,S);
    ambient += A;
	diffuse += D;
	spec += S;

    float4 finalColor = color * (ambient + diffuse) + spec;

    float fogFactor = saturate((input.Depth - fogStart) / (fogEnd - fogStart));

    finalColor = lerp(finalColor, fogColor, fogFactor);

    return finalColor;
}