#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "LightHelper.hlsl"
#include "TransformHelpers.hlsl"

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
VS_OUTPUT VS( float4 Pos : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD )
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    // 픽셀 셰이더 내에서 라이팅을 위해 
    output.PosWorld =  mul( Pos, World ).xyz;

    // PosScreen
    output.PosScreen = CalculateScreenPosition(Pos, World,View,Projection);
    output.Depth = output.PosScreen.z;

    // 노말벡터를 월드좌표계로
    output.NormalW = mul(Normal, (float3x3)WorldInvTranspose);
    output.NormalW = normalize(output.NormalW);

    output.Tex = TexCoord;

    // light source에서 버텍스로의 position
    output.PosLightSpace = CalculateScreenPosition(Pos, World,LightView,LightProj);
		
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

    // 조명 적용
    color.rgb = color.rgb * (ambient + diffuse) + spec;

    // 그림자 매핑
    float3 shadowCoord = input.PosLightSpace.xyz/ input.PosLightSpace.w;
    // [-1~1] -> [0~1] 텍스쳐 NDC
    shadowCoord.x = shadowCoord / 2 + 0.5f;
	shadowCoord.y = -shadowCoord.y / 2 + 0.5f;
    if((saturate(shadowCoord.x) == shadowCoord.x) && (saturate(shadowCoord.y) == shadowCoord.y))
    {
		float depthValue = gShadowMap.Sample(gShadowSampler, shadowCoord.xy).r;

        float bias = 0.001f;
        float lightDepthValue = shadowCoord.z - bias;

        if(lightDepthValue > depthValue)
        {
			float shadowfactor = 0.5f;
			color.rgb = color.rgb * shadowfactor;
        }
    }

    return color;
}