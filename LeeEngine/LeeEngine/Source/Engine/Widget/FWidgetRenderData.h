// 08.18
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
struct FWidgetRenderData
{
	// NDC 좌표계로 저장할것
	float Left;
	float Top;
	float Width;
	float Height;
	
	std::shared_ptr<UTexture> Texture;

	DirectX::XMFLOAT4 Tint = DirectX::XMFLOAT4{1.0f, 1.0f, 1.0f, 1.0f};

	float ZOrder = 0;
};

