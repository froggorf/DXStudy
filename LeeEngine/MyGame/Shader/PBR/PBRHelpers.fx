struct PBR_PS_INPUT
{
	float4 PosScreen : SV_POSITION;
	float3 WorldPos : POSITION1;
	float2 TexCoord : TEXCOORD;
	float3 ViewPosition : POSITION;
	float3 ViewTangent : TANGENT;
	float3 ViewNormal : NORMAL;
	float3 ViewBinormal : BINORMAL;
};
// Textures
Texture2D AlbedoTexture     : register(t0);
Texture2D NormalTexture     : register(t1);
Texture2D MetallicTexture   : register(t2);
Texture2D RoughnessTexture  : register(t3);
Texture2D AOTexture         : register(t4);
TextureCube EnvironmentMap  : register(t200);

// Sampler
SamplerState DefaultSampler : register( s0 );
SamplerState CubeSampler : register(s2);
// =======================
// PBR Helper Functions
// =======================

// Normal Distribution Function (GGX/Trowbridge-Reitz)
float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

// Geometry Function (Smith's method)
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// Fresnel Equation (Schlick's approximation)
float3 FresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Fresnel with roughness for IBL
float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * 
		pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Get Normal from Normal Map
float3 GetNormalFromMap(PBR_PS_INPUT input)
{
	int bNormalTexBind = bTexBind_0_3.g;
	if (!bNormalTexBind)
	{
		return input.ViewNormal;
	}

	float3 tangentNormal = NormalTexture.Sample(DefaultSampler, input.TexCoord).xyz * 2.0 - 1.0;

	float3 N = normalize(input.ViewNormal);
	float3 T = normalize(input.ViewTangent);
	float3 B = normalize(input.ViewBinormal);
	float3x3 TBN = float3x3(T, B, N);

	return normalize(mul(tangentNormal, TBN));
}