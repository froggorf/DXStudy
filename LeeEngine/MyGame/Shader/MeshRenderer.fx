#include "Global.fx"
#include "ParticleHelper.fx"

Texture2D Texture0 : register(t0);
Texture2D Texture1 : register(t1);
Texture2D Texture2 : register(t2);
Texture2D Texture3 : register(t3);
Texture2D Texture4 : register(t4);
Texture2D Texture5 : register(t5);
Texture2D Texture6 : register(t6);
Texture2D Texture7 : register(t7);

SamplerState DefaultSampler : register(s0);

struct VS_IN
{
	float3 vPos : POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
	uint   InstID : SV_InstanceID;
};

struct VS_OUT
{
	float3 vPos : POSITION; // NDC 좌표계 위치값을 전달
	float2 UV: TEXCOORD;
	float3 Normal : NORMAL;
	uint   InstID : FOG;
};

VS_OUT VS_MeshParticle(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;

	output.vPos   = _in.vPos;
	output.InstID = _in.InstID;
	output.UV     = _in.UV;
	output.Normal = _in.Normal;
	return output;
}

// GeometryShader
// 1.OutStream 에 정점 출력
//  만약에 아무것도 출력하지 않으면, 레스터라이저로 전달되는 정점이 하나도 없게 됨 ==> 파이프라인 종료
struct GS_OUT
{
	float4 Position : SV_Position;
	float2 UV : TEXCOORD;
	float3 WorldPixelPos : TEXCOORD1;
	float3 Normal : NORMAL;
	uint   InstID : FOG;
};

[maxvertexcount(3)]void GS_Mesh(triangle VS_OUT _in[3], inout TriangleStream<GS_OUT> _OutStream)
{
	// 파티클 데이터 가져오기
	FParticleData particle = gParticle[_in[0].InstID];

	// 1. 비활성화면 종료
	if (!particle.Active)
		return;

	float3x3 rotMat = QuaternionRotationMatrix(particle.WorldRotation);

	// 4. 각 꼭짓점 변환
	GS_OUT verts[3];
	for (int i = 0; i < 3; ++i)
	{
		// 스케일 적용 (비균일 스케일일 경우)
		float3 normalScaled = _in[i].Normal / particle.WorldScale; 
	    // 회전 적용
		float3 normalRotated = mul(normalScaled, rotMat);
		// 정규화
		verts[i].Normal = normalize(normalRotated);

		// 스케일 적용
		float3 scaled = _in[i].vPos * particle.WorldScale;
		// 회전 적용
		float3 rotated = mul(scaled, rotMat);
		// 위치 적용
		float3 worldPos = rotated + particle.WorldPos;

		// MVP 변환
		float4 clipPos = mul(float4(worldPos, 1.0), gView);
		clipPos        = mul(clipPos, gProjection);

		verts[i].Position = clipPos;
		verts[i].UV       = _in[i].UV;
		verts[i].InstID    = _in[0].InstID;
		verts[i].WorldPixelPos = worldPos;
		
	}

	// 5. 삼각형 출력 (0-1-2)
	_OutStream.Append(verts[0]);
	_OutStream.Append(verts[1]);
	_OutStream.Append(verts[2]);
	_OutStream.RestartStrip();
}

float4 PS_Particle(GS_OUT _in) : SV_Target
{
	float4 vColor = float4(1.f, 0.f, 1.f, 1.f);

	vColor = Texture0.Sample(DefaultSampler, _in.UV);

	vColor *= gParticle[_in.InstID].Color;
	return vColor;
}

float4 PS_ParticleOnlyColor(GS_OUT _in) : SV_Target
{
	return gParticle[_in.InstID].Color;

}

