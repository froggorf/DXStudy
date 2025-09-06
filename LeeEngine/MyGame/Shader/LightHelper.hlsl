#ifndef __LIGHTHELPER_HLSL
#define __LIGHTHELPER_HLSL

struct FLightInfo
{
	int		LightType;
	float3  WorldPos;
	float3  WorldDir;
	float3	LightColor;
	float3  LightAmbient;
	float	Radius;
	float	Angle;
};

StructuredBuffer<FLightInfo> g_LightBuffer : register(t14);

void CalcLight(float4x4 ViewMat, float3 _ViewPos, float3 _ViewNormal, int _Idx
	, inout float3 _LightColor, inout float3 _Specular)
{
	FLightInfo info = g_LightBuffer[_Idx];

	// Directional Light
	if (0 == info.LightType)
	{
		// View 공간상에서 광원이 진행하는 방향
		float3 vLightDir = normalize(mul(float4(normalize(info.WorldDir), 0.f), ViewMat).xyz);
		float fLightPow = saturate(dot(_ViewNormal, -vLightDir));

		// 반사광 세기 구하기
		float3 vReflect = vLightDir + 2.f * dot(-vLightDir, _ViewNormal) * _ViewNormal;
		vReflect = normalize(vReflect);

		// 시선벡터
		float3 vEye = normalize(_ViewPos);

		// 반사벡터랑 시선벡터를 내적해서 반사광 세기 구함
		float fReclectPow = saturate(dot(vReflect, -vEye));
		fReclectPow = pow(fReclectPow, 20);

		_LightColor += info.LightColor * fLightPow + info.LightAmbient;
		_Specular += info.LightColor * fReclectPow;
	}

	// Point Light
	else if (1 == info.LightType)
	{   
		// 광원의 위치를 View 공간으로 가져온다.
		float3 LightViewPos = mul(float4(info.WorldPos, 1.f), ViewMat);

		// 광원이 표면에 오는 방향벡터
		float3 vLightDir = normalize(_ViewPos - LightViewPos);

		// 빛의 진입각도 세기(램버트 코사인법칙)
		float fLightPow = saturate(dot(_ViewNormal, -vLightDir));

		// 빛의 반사벡터 구하기
		float3 vReflect = vLightDir + 2.f * dot(-vLightDir, _ViewNormal) * _ViewNormal;
		vReflect = normalize(vReflect);

		// 시선벡터
		float3 vEye = normalize(_ViewPos);

		// 반사벡터랑 시선벡터를 내적해서 반사광 세기 구함
		float fReclectPow = saturate(dot(vReflect, -vEye));
		fReclectPow = pow(fReclectPow, 20);

		// 광원으로부터 물체가 떨어진 거리에 따른 빛의 감쇄비율
		float Attenuation = saturate(1.f - (length(_ViewPos - LightViewPos) / info.Radius));

		_LightColor += info.LightColor * fLightPow * Attenuation;
		_Specular += info.LightColor * fReclectPow * Attenuation;
	}

	// Spot Light
	else
	{

	}
}

float4x4 CreateLookAt(float3 eye, float3 at, float3 up)
{
	float3 zaxis = normalize(at - eye);
	float3 xaxis = normalize(cross(up, zaxis));
	float3 yaxis = cross(zaxis, xaxis);

	float4x4 result = float4x4(
		float4(xaxis, -dot(xaxis, eye)),
		float4(yaxis, -dot(yaxis, eye)),
		float4(zaxis, -dot(zaxis, eye)),
		float4(0, 0, 0, 1)
	);
	return transpose(result); // HLSL 행렬은 행 우선이므로 transpose 필요
}

float4x4 CreatePerspectiveFov(float fovY, float aspect, float zn, float zf)
{
	float yScale = 1.0f / tan(fovY * 0.5f);
	float xScale = yScale / aspect;

	float4x4 m;
	m[0] = float4( xScale, 0,      0,               0 );
	m[1] = float4( 0,      yScale, 0,               0 );
	m[2] = float4( 0,      0,      zf/(zf-zn),      1 );
	m[3] = float4( 0,      0,      -zn*zf/(zf-zn),  0 );
	return m;
}

float4x4 CalcCubeViewProjMatrix(uint face, float3 lightPos, float nearZ, float farZ, float fovY, float aspect)
{
	// 각 면의 방향과 업벡터
	static const float3 lookDirections[6] = {
		float3( 1, 0, 0), // +X
		float3(-1, 0, 0), // -X
		float3( 0, 1, 0), // +Y
		float3( 0,-1, 0), // -Y
		float3( 0, 0, 1), // +Z
		float3( 0, 0,-1)  // -Z
	};
	static const float3 upVectors[6] = {
		float3(0, 1, 0), float3(0, 1, 0),
		float3(0, 0,-1), float3(0, 0, 1),
		float3(0, 1, 0), float3(0, 1, 0)
	};

	float3 look = lookDirections[face];
	float3 up   = upVectors[face];

	// 뷰 행렬 (LookAt)
	float3 target = lightPos + look;
	float4x4 view = CreateLookAt(lightPos, target, up);

	// 프로젝션 행렬 (90도 FOV, aspect=1)
	float4x4 proj = CreatePerspectiveFov(fovY, aspect, nearZ, farZ);

	return mul(view, proj); // HLSL에서는 보통 view * proj 순서로 곱함
}

#endif