// 루나책의 D3DX11 버전을 D3D11
#pragma once

// warning C4819: 현재 코드 페이지(949)에서 표시할 수 없는 문자가 파 일에 들어 있습니다. 데이터가 손실되지 않게 하려면 해당 파일을 유니코드 형식으로 저장하십시오.
#pragma warning(disable:4819)
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <wrl.h>				// ComPtr
#include <DirectXMath.h>
#include <Directxcolors.h>

#include <iostream>
#include <algorithm>
#include <cmath>

#include <map>
#include <unordered_map>
#include <string>
#include <vector>

#include "LightHelper.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "comctl32.lib")

#if defined(DEBUG) | defined(_DEBUG)
	#pragma comment(lib, "assimp-vc143-mtd.lib")
	#pragma comment(lib, "fmodL_vc.lib")
#else
	#pragma comment(lib, "assimp-vc143-mt.lib")
	#pragma comment(lib, "fmod_vc.lib")
#endif

#pragma comment(lib, "DirectXTex.lib")



// DEBUG, HRESULT 에러 출력
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			LPWSTR output;\
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &output, 0, NULL); \
			MessageBox(NULL, output, TEXT("ERROR"), MB_OK);	   \
		}                                                      \
	}
	#endif



#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif

#define WindowTitleBarHeight 56


#ifndef COMPILE_SHADER_FROM_FILE
#define COMPILE_SHADER_FROM_FILE
//--------------------------------------------------------------------------------------
// 셰이더 로드 함수
//--------------------------------------------------------------------------------------
inline HRESULT CompileShaderFromFile( const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;
    
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

#endif
    
    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile( szFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob );
    if( FAILED(hr) )
    {
        if( pErrorBlob )
        {
            OutputDebugStringA( reinterpret_cast<const char*>( pErrorBlob->GetBufferPointer() ) );
            pErrorBlob->Release();
        }
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}

#endif  //COMPILE_SHADER_FROM_FILE

// ======================== Mesh Data ========================
struct MyVertexData
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT2 TexCoords;
};

#define MAX_BONE_INFLUENCE 4
#define MAX_BONES 100
__declspec(align(16)) struct MySkeletalMeshVertexData
{
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT2 TexCoords;
};
struct BoneInfo
{
    int id;							// 최종 본 매트릭스 내 해당 본의 ID
    DirectX::XMMATRIX offset;       // 모델 root 로부터의 해당 본의 offset matrix
};
// ============================================================


struct ObjConstantBuffer
{
    XMMATRIX World;
    XMMATRIX InvTransposeMatrix;
    Material ObjectMaterial;
};

struct FrameConstantBuffer
{
    XMMATRIX View;
    XMMATRIX Projection;

    XMMATRIX LightView;
    XMMATRIX LightProj;
    float Time;
    float Padding[3];
};


struct SkeletalMeshBoneTransformConstantBuffer
{
    XMMATRIX BoneFinalTransforms[MAX_BONES];
};

struct LightFrameConstantBuffer
{
    DirectionalLight gDirLight;
    PointLight gPointLight;
    XMFLOAT3 gEyePosW;
    float pad;
};

struct ShadowLightMatrixConstantBuffer
{
    XMMATRIX LightView;
    XMMATRIX LightProj;
};

struct ShadowObjConstantBuffer
{
    XMMATRIX ObjWorld;
};


inline DirectX::XMVECTOR ComputeUpVector(const DirectX::XMVECTOR& cameraPos, const DirectX::XMVECTOR& targetPos)
{
    // 바라보는 방향(Look Direction)
    DirectX::XMVECTOR lookDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(targetPos, cameraPos));

    // 월드 공간의 참조 Up 벡터
    DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    float dotProduct = fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(lookDir, worldUp)));

    if (dotProduct > 0.999f) // 평행 또는 거의 평행
    {
	    return DirectX::XMVectorSet(0.0f, 0.0f,1.0f,0.0f);
    }

    // 오른쪽 벡터(Right Vector)
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUp, lookDir));

    // Up 벡터 계산
    DirectX::XMVECTOR up = DirectX::XMVector3Cross(lookDir, right);

    return up;
}

inline void XMMatrixLerp(const XMMATRIX& A, const XMMATRIX& B, float t, XMMATRIX& Out)
{
    
    XMVECTOR V1 = XMVectorLerp(A.r[0], B.r[0], t);
    XMVECTOR V2 = XMVectorLerp(A.r[1], B.r[1], t);
    XMVECTOR V3 = XMVectorLerp(A.r[2], B.r[2], t);
    XMVECTOR V4 = XMVectorLerp(A.r[3], B.r[3], t);


    Out = XMMATRIX{V1,V2,V3,V4};
}