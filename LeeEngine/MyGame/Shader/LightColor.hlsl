#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "Global.fx"
#include "TransformHelpers.hlsl"

Texture2D    txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

Texture2D    gShadowMap : register(t1);
SamplerState gShadowSampler : register(s1);

struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;

	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

struct VS_OUTPUT
{
	float4 PosScreen : SV_POSITION;
	float3 PosWorld : POSITION;
	float4 PosLightSpace : POSITION1;
	float3 NormalW : TEXCOORD1;
	float2 Tex : TEXCOORD;
	float  Depth : TEXCOORD2;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT Input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	// 픽셀 셰이더 내에서 라이팅을 위해 
	output.PosWorld = mul(Input.Pos, World).xyz;

	// PosScreen
	output.PosScreen = CalculateScreenPosition(Input.Pos, World, gView, gProjection);
	output.Depth     = output.PosScreen.z;

	// 노말벡터를 월드좌표계로
	output.NormalW = mul(Input.Normal, (float3x3)WorldInvTranspose);
	output.NormalW = normalize(output.NormalW);

	output.Tex = Input.TexCoord;

	// light source에서 버텍스로의 position
	//output.PosLightSpace = CalculateScreenPosition(Input.Pos, World, gLightView, gLightProj);

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 color = txDiffuse.Sample(samLinear, input.Tex);

	// 조명 계산시 사용될 변수 초기화
	float4 ambient = 0;
	float4 diffuse = 0;
	float4 spec    = 0;

	float3 toEye = normalize(gEyePosW - input.PosWorld);

	float4 A, D, S;

	ComputeDirectionalLight(ObjectMaterial, gDirLight, input.NormalW, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	ComputePointLight(ObjectMaterial, gPointLight, input.PosWorld, input.NormalW, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	// 조명 적용
	color.rgb = color.rgb * (ambient + diffuse) + spec;

	// 그림자 매핑
	float3 shadowCoord = input.PosLightSpace.xyz / input.PosLightSpace.w;
	// [-1~1] -> [0~1] 텍스쳐 NDC
	shadowCoord.x = shadowCoord / 2 + 0.5f;
	shadowCoord.y = -shadowCoord.y / 2 + 0.5f;
	if ((saturate(shadowCoord.x) == shadowCoord.x) && (saturate(shadowCoord.y) == shadowCoord.y))
	{
		float depthValue = gShadowMap.Sample(gShadowSampler, shadowCoord.xy).r;

		// TODO: 03.09 임시적 수정 추후 꼭 정상화하기
		float bias            = 0.001f;
		float lightDepthValue = shadowCoord.z - bias;

		//if(lightDepthValue > depthValue)
		//{
		//	float shadowfactor = 0.5f;
		//	color.rgb = color.rgb * shadowfactor;
		//}
	}

	return color;
}

cbuffer cbTest : register(b4)
{
	float  TestSpeedX;
	float  TestSpeedY;
	float2 Pad;
}

float4 TestWater(VS_OUTPUT input) : SV_Target
{
	float2 NewWaterUV = input.Tex;
	NewWaterUV.x      = NewWaterUV.x + gTime * TestSpeedX;
	NewWaterUV.y      = NewWaterUV.y + gTime * TestSpeedY;
	float4 color      = txDiffuse.Sample(samLinear, NewWaterUV);

	//float4 color = txDiffuse.Sample( samLinear, input.Tex );

	// 조명 계산시 사용될 변수 초기화
	float4 ambient = 0;
	float4 diffuse = 0;
	float4 spec    = 0;

	float3 toEye = normalize(gEyePosW - input.PosWorld);

	float4 A, D, S;

	ComputeDirectionalLight(ObjectMaterial, gDirLight, input.NormalW, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	ComputePointLight(ObjectMaterial, gPointLight, input.PosWorld, input.NormalW, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	// 조명 적용
	color.rgb = color.rgb * (ambient + diffuse) + spec;

	// 그림자 매핑
	float3 shadowCoord = input.PosLightSpace.xyz / input.PosLightSpace.w;
	// [-1~1] -> [0~1] 텍스쳐 NDC
	shadowCoord.x = shadowCoord / 2 + 0.5f;
	shadowCoord.y = -shadowCoord.y / 2 + 0.5f;
	if ((saturate(shadowCoord.x) == shadowCoord.x) && (saturate(shadowCoord.y) == shadowCoord.y))
	{
		float depthValue = gShadowMap.Sample(gShadowSampler, shadowCoord.xy).r;

		// TODO: 03.09 임시적 수정 추후 꼭 정상화하기
		float bias            = 0.001f;
		float lightDepthValue = shadowCoord.z - bias;

		//if(lightDepthValue > depthValue)
		//{
		//	float shadowfactor = 0.5f;
		//	color.rgb = color.rgb * shadowfactor;
		//}
	}

	return color;
}
