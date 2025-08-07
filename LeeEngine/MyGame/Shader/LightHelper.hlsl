//***************************************************************************************
// LightHelper.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Structures and functions for lighting calculations.
//***************************************************************************************

#define PI 3.141592

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


// ============================
// 아래는 레거시 코드




struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float pad;
};

struct PointLight
{ 
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Att;
	float pad;
};

struct SpotLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Direction;
	float Spot;

	float3 Att;
	float pad;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w = SpecPower
	float4 Reflect;
};


//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a directional light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeDirectionalLight(Material mat, DirectionalLight L, 
                             float3 normal, float3 toEye,
					         out float4 ambient,
						     out float4 diffuse,
						     out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector aims opposite the direction the light rays travel.
	// 조명 계산 시 빛이 표면에 도달하는 방향을 사용하므로 반전
	float3 lightVec = -L.Direction;

	// Add ambient term.
	ambient = mat.Ambient * L.Ambient;	

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	// [flatten] -> HLSL에서 조건문이 있다면 분기를 만들어 처리하지만, 해당 지시어를 사용 시 분기를 제거하여 성능이 약간 향상
	[flatten]
	if( diffuseFactor > 0.0f )
	{
		// 빛의 반사 벡터
		float3 v         = reflect(-lightVec, normal);
		// dot(v, toEye) -> 빛이 카메라에 들어오는 정도
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
					
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec    = specFactor * mat.Specular * L.Specular;
	}
}


//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a point light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,
				   out float4 ambient, out float4 diffuse, out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	if( d > L.Range )
		return;
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Ambient term.
	ambient = mat.Ambient * L.Ambient;	

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if( diffuseFactor > 0.0f )
	{
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
					
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec    = specFactor * mat.Specular * L.Specular;
	}

	// Attenuate (거리에 따른 감쇠)
	float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));

	diffuse *= att;
	spec    *= att;
}


//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a spotlight.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye,
				  out float4 ambient, out float4 diffuse, out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	if( d > L.Range )
		return;
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Ambient term.
	ambient = mat.Ambient * L.Ambient;	

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if( diffuseFactor > 0.0f )
	{
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
					
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec    = specFactor * mat.Specular * L.Specular;
	}
	
	// Scale by spotlight factor and attenuate.
	// Spot Light의 범위 안에 있는지.
	float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);

	// Scale by spotlight factor and attenuate.
	// 거리에 따른 감쇠까지
	float att = spot / dot(L.Att, float3(1.0f, d, d*d));

	ambient *= spot;
	diffuse *= att;
	spec    *= att;
}

 
 