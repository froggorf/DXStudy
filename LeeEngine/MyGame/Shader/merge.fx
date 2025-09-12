#include "Global.fx"
#include "TransformHelpers.hlsl"
#include "PBRLightHelper.fx"

Texture2D    ColorTexture : register( t0 );
Texture2D    LightDiffuseTexture : register( t1 );
Texture2D    LightSpecularTexture : register( t2 );
Texture2D    ViewPosTexture : register( t3 );
Texture2D    ViewNormalTexture : register( t4 );
Texture2D    PBRTexture : register( t5 );

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

	float4 Albedo = ColorTexture.Sample(samLinear, _in.vUV);
	float4 Diffuse = LightDiffuseTexture.Sample(samLinear, _in.vUV);
	float4 Specular = LightSpecularTexture.Sample(samLinear, _in.vUV);
	float3 N = ViewNormalTexture.Sample(samLinear, _in.vUV);
	float3 V = ViewPosTexture.Sample(samLinear, _in.vUV);
	float4 PBRData = PBRTexture.Sample(samLinear, _in.vUV);

	
	
	

	float ObjectMetallic = PBRData.r;
	float ObjectSpecular = PBRData.g;
	float ObjectRoughness = PBRData.b;
	float ObjectAO = PBRData.a;

	// PBR 로 변경한 이후, IBL 을 해당 코드에서 처리,
	const float DefaultSpecular = 0.04 *  ObjectSpecular;
	float3 F0 = lerp(float3(DefaultSpecular, DefaultSpecular, DefaultSpecular), Albedo, ObjectMetallic);
	float3 ambient = IBLAmbient(normalize(N), normalize(-V), F0, Albedo, ObjectMetallic, ObjectRoughness, ObjectAO);

	// 빛 먼저 적용시키고,
	float3 FinalDiffuse = Albedo.rgb * Diffuse.rgb * (1.0 - ObjectMetallic);
	float3 FinalSpecular = lerp(Albedo.rgb * Specular.rgb, Specular.rgb, ObjectMetallic);
	// IBL Ambient 를 적용시킨다
	OutColor.rgb = FinalDiffuse + FinalSpecular + ambient;

	OutColor.rgb = ACESFilm(OutColor.rgb);
	// 08.20 백버퍼를 B8G8R8A8_UNORM 으로 바꾸니 알파값이 중요해져서 해당 코드를 추가
	OutColor.a = 1.0f;
    return OutColor;
}
