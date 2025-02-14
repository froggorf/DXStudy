// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once


#include "Engine/MyEngineUtils.h"

class FStaticMeshRenderData
{
	unsigned int MeshCount;

	// Buffer[MeshCount]
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> VertexBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> IndexBuffer;

	// TODO: 임시 텍스쳐 데이터,
	// UMaterial 구현 이전으로 텍스쳐 SRV 데이터를 해당 클래스에서 관리
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> TextureSRV;
};