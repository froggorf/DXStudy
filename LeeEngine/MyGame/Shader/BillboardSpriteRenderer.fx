#include "Global.fx"
#include "ParticleHelper.fx"

Texture2D    gParticleTexture : register(t0);
SamplerState DefaultSampler : register(s0);

struct VS_IN
{
	float3 vPos : POSITION;
	uint   InstID : SV_InstanceID;
};

struct VS_OUT
{
	float3 vPos : POSITION; // NDC 좌표계 위치값을 전달
	uint   InstID : FOG;
};

VS_OUT VS_BillboardSpriteParticle(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;

	output.vPos   = _in.vPos;
	output.InstID = _in.InstID;

	return output;
}

// GeometryShader
// 1.OutStream 에 정점 출력
//  만약에 아무것도 출력하지 않으면, 레스터라이저로 전달되는 정점이 하나도 없게 됨 ==> 파이프라인 종료

// 2. PointMesh 를 이용해서 RectMesh 로 정점분할을 한다.
//  GS 단계에서 View Space 기준으로 사각형을 만든다.
//  Billboard 효과 구현
struct GS_OUT
{
	float4 vPosition : SV_Position;
	float2 vUV : TEXCOORD;
	uint   InstID : FOG;
};

[maxvertexcount(6)]void GS_Billboard(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
	FParticleData particle = gParticle[_in[0].InstID];

	if (false == particle.Active)
		return;

	float4 vViewPos = mul(float4(particle.WorldPos, 1.f), gView);

	float2 halfScale  = particle.WorldScale.xy * 0.5f;
	float2 offsets[4] = {float2(-halfScale.x, halfScale.y), float2(halfScale.x, halfScale.y), float2(halfScale.x, -halfScale.y), float2(-halfScale.x, -halfScale.y)};

	// 회전 매트릭스 (예: Z축 회전만)
	float    angle  = particle.WorldRotation.z * (PI / 180.0f);
	float2x2 rotMat = float2x2(cos(angle), -sin(angle), sin(angle), cos(angle));

	GS_OUT arrOut[4];
	for (int i = 0; i < 4; ++i)
	{
		float2 rotated      = mul(offsets[i], rotMat);
		arrOut[i].vPosition = float4(vViewPos.x + rotated.x, vViewPos.y + rotated.y, vViewPos.z, 1.f);
	}

	arrOut[0].vUV = float2(0.f, 0.f);
	arrOut[1].vUV = float2(1.f, 0.f);
	arrOut[2].vUV = float2(1.f, 1.f);
	arrOut[3].vUV = float2(0.f, 1.f);

	// ViewSpace -> Projection
	for (int i = 0; i < 4; ++i)
	{
		arrOut[i].vPosition = mul(arrOut[i].vPosition, gProjection);
		arrOut[i].InstID    = _in[0].InstID;
	}

	_OutStream.Append(arrOut[0]);
	_OutStream.Append(arrOut[1]);
	_OutStream.Append(arrOut[2]);
	_OutStream.RestartStrip();

	_OutStream.Append(arrOut[0]);
	_OutStream.Append(arrOut[2]);
	_OutStream.Append(arrOut[3]);
	_OutStream.RestartStrip();
}

float4 PS_Particle(GS_OUT _in) : SV_Target
{
	float4 vColor = float4(1.f, 0.f, 1.f, 1.f);

	vColor = gParticleTexture.Sample(DefaultSampler, _in.vUV);
	vColor.rgb *= vColor.a;

	vColor *= gParticle[_in.InstID].Color;
	return vColor;
}

float4 PS_Particle_UV(GS_OUT _in) : SV_Target
{
	float4 vColor = float4(1.f, 0.f, 1.f, 1.f);

	// 1. 총 프레임 수
	int totalFrame = gParticle[_in.InstID].UCount * gParticle[_in.InstID].VCount;

	// 2. 현재 프레임 인덱스 (0 ~ totalFrame-1)
	int frameIndex = (int)(gParticle[_in.InstID].NormalizedAge * totalFrame);
	frameIndex     = clamp(frameIndex, 0, totalFrame - 1);

	// 3. 2D 인덱스 변환
	int uIndex = frameIndex % gParticle[_in.InstID].UCount;
	int vIndex = frameIndex / gParticle[_in.InstID].UCount;

	// 4. 한 칸의 크기
	float du = 1.0 / gParticle[_in.InstID].UCount;
	float dv = 1.0 / gParticle[_in.InstID].VCount;

	// 5. 최종 UV 계산
	float2 atlasUV = float2(uIndex * du, vIndex * dv) + _in.vUV * float2(du, dv);

	// 6. 텍스처 샘플링
	vColor = gParticleTexture.Sample(DefaultSampler, atlasUV);

	vColor *= gParticle[_in.InstID].Color;

	return vColor;
}
