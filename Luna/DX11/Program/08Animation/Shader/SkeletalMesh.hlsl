#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "LightHelper.hlsl"

Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

Texture2D gShadowMap : register(t1);
SamplerState gShadowSampler : register(s1);

cbuffer cbPerFrame : register(b0)
{
	matrix View;
	matrix Projection;
    matrix LightView;
    matrix LightProj;
}

#define MAX_BONES 100
#define MAX_BONE_INFLUENCE 4
cbuffer cbPerObject : register(b1)
{
	matrix World;
    float4x4 WorldInvTranspose;
    Material ObjectMaterial;

    float4x4 gBoneFinalTransforms[MAX_BONES];
};

cbuffer cbLight : register(b2)
{
	DirectionalLight gDirLight;
    PointLight gPointLight;
    float3 gEyePosW;
    float pad;
}

struct VS_INPUT
{
    int4 boneIDs : BONEIDS;
    float4 boneWeights : BONEWEIGHTS;
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 PosScreen : SV_POSITION;
    float3 PosWorld : POSITION;
    float4 PosLightSpace : POSITION1;
    float3 NormalW : TEXCOORD1;
    float2 Tex : TEXCOORD;
    float Depth : TEXCOORD2;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( VS_INPUT input )
{
    VS_OUTPUT output = (VS_OUTPUT)0;

	float4 skinnedPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 skinnedNormal = float3(0.0f, 0.0f, 0.0f);
	
    // bone에 따른 최종 위치 계산
    {
		for (int index = 0; index < MAX_BONE_INFLUENCE; ++index)
		{
			if(input.boneIDs[index] <0)
			{
				continue;
			}
            if(input.boneIDs[index] >= MAX_BONES)
            {
	            skinnedPosition = input.Pos;
            }

            float4x4 test = gBoneFinalTransforms[input.boneIDs[index]];
			float weight = input.boneWeights[index];

            float4 localPosition = mul(input.Pos, gBoneFinalTransforms[input.boneIDs[index]]);
			skinnedPosition += localPosition * weight;
    
			float3 localNormal = mul(input.Normal, gBoneFinalTransforms[input.boneIDs[index]]);
			skinnedNormal += localNormal * weight;
           
		}
    }
    
    input.Pos = skinnedPosition;
    input.Normal = skinnedNormal;

    output.PosScreen = mul( input.Pos, World );
    // 픽셀 셰이더 내에서 라이팅을 위해 
    output.PosWorld = output.PosScreen.xyz;

    output.PosScreen = mul( output.PosScreen, View );
    output.Depth = output.PosScreen.z;
    output.PosScreen = mul( output.PosScreen, Projection );

    // 노말벡터를 월드좌표계로
    output.NormalW = mul(input.Normal, (float3x3)WorldInvTranspose);
    output.NormalW = normalize(output.NormalW);

    output.Tex = input.TexCoord;


    // light source에서 버텍스로의 position
    output.PosLightSpace = mul(input.Pos, World);
    output.PosLightSpace = mul(output.PosLightSpace, LightView);
    output.PosLightSpace = mul(output.PosLightSpace, LightProj);



    return output;
}
