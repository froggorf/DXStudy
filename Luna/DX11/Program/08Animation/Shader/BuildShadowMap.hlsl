#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "TransformHelpers.hlsl"
#include "AnimationHelpers.hlsl"

cbuffer cbLightMatrix : register(b0)
{
    matrix LightView;
    matrix LightProj;
}

cbuffer cbPerObject : register(b1)
{
    matrix World;
}

cbuffer cbBoneFinalMatrices : register(b2)
{
	matrix gBoneFinalTransforms[MAX_BONES];
}

struct VS_OUTPUT
{
    float4 PosScreen : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader ( Static Mesh )
//--------------------------------------------------------------------------------------
VS_OUTPUT VS_StaticMesh_ShadowMap( float4 Pos: POSITION, float3 Normal:NORMAL, float2 Tex : TEXCOORD )
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.PosScreen = CalculateScreenPosition(Pos, World, LightView, LightProj);

    return output;
}

//--------------------------------------------------------------------------------------
// Vertex Shader ( Skeletal Mesh )
//--------------------------------------------------------------------------------------
VS_OUTPUT VS_SkeletalMesh_ShadowMap( int4 boneIDs : BONEIDS, float4 boneWeights : BONEWEIGHTS, float4 Pos : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

    // Pos, Normal 본의 가중치에 맞게 위치 조정
    {
        float4 skinnedPosition;
        float3 skinnedNormal;

        CalculateSkinnedPosition(
            Pos,
            Normal,
            boneIDs,
            boneWeights,
            gBoneFinalTransforms,
            skinnedPosition,
            skinnedNormal
        );

        Pos = skinnedPosition;
        Normal = skinnedNormal;    
    }

    output.PosScreen = CalculateScreenPosition(Pos, World, LightView, LightProj);
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    return float4(0.0f,0.0f,0.0f,0.0f);
}