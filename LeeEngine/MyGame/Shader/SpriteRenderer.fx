#include "Global.fx"
#include "ParticleHelper.fx"

Texture2D gParticleTexture : register(t0);
SamplerState DefaultSampler : register(s0);

struct VS_IN
{
    float3 vPos : POSITION;
    float2 UV : TEXCOORD;
    uint InstID : SV_InstanceID;
};

struct VS_OUT
{
    float3 vPos : POSITION; // NDC 좌표계 위치값을 전달
    float UV: TEXCOORD;
    uint InstID : FOG;
};

VS_OUT VS_SpriteParticle(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.vPos   = _in.vPos;
    output.InstID = _in.InstID;
    output.UV = _in.UV;
    return output;
}

// GeometryShader
// 1.OutStream 에 정점 출력
//  만약에 아무것도 출력하지 않으면, 레스터라이저로 전달되는 정점이 하나도 없게 됨 ==> 파이프라인 종료
// Plane 메쉬를 정점으로 받아 active중이라면 MVP 변환 후 반환, 아니라면 파이프라인 종료
struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV       : TEXCOORD;
    uint   InstID    : FOG;
};

[maxvertexcount(6)]
void GS_Sprite(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
    // 파티클 데이터 가져오기
    FParticleData particle = gParticle[_in[0].InstID];

    // 1. 비활성화면 종료
    if (!particle.Active)
        return;

    // 2. 로컬 평면 꼭짓점 (중심 기준)
    float3 localVerts[4] = {
        float3(-0.5,  0.5, 0.0), // 좌상
        float3( 0.5,  0.5, 0.0), // 우상
        float3( 0.5, -0.5, 0.0), // 우하
        float3(-0.5, -0.5, 0.0)  // 좌하
    };

    float2 uv[4] = {
        float2(0, 0), // 좌상
        float2(1, 0), // 우상
        float2(1, 1), // 우하
        float2(0, 1)  // 좌하
    };

    // 3. 월드 회전 행렬 만들기 (오일러 각 기반, radians 단위라고 가정)
    float3x3 rotMat = QuaternionRotationMatrix(particle.WorldRotation);

    // 4. 각 꼭짓점 변환
    GS_OUT verts[4];
    for (int i = 0; i < 4; ++i)
    {
        // 스케일 적용
        float3 scaled = localVerts[i] * particle.WorldScale;
        // 회전 적용
        float3 rotated = mul(scaled, rotMat);
        // 위치 적용
        float3 worldPos = rotated + particle.WorldPos;

        // MVP 변환
        float4 clipPos = mul(float4(worldPos, 1.0), gView);
        clipPos = mul(clipPos, gProjection);

        verts[i].vPosition = clipPos;
        verts[i].vUV = uv[i];
        verts[i].InstID = _in[0].InstID;
    }

    // 5. 삼각형 2개로 출력 (0-1-2, 0-2-3)
    _OutStream.Append(verts[0]);
    _OutStream.Append(verts[1]);
    _OutStream.Append(verts[2]);
    _OutStream.RestartStrip();

    _OutStream.Append(verts[0]);
    _OutStream.Append(verts[2]);
    _OutStream.Append(verts[3]);
    _OutStream.RestartStrip();
}


float4 PS_Particle(GS_OUT _in) : SV_Target
{  
    float4 vColor = float4(1.f, 0.f, 1.f, 1.f);

vColor = gParticleTexture.Sample(DefaultSampler,  _in.vUV);
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
int frameIndex = (int)(gParticle[_in.InstID].NormalizedAge* totalFrame);
frameIndex = clamp(frameIndex, 0, totalFrame - 1);

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