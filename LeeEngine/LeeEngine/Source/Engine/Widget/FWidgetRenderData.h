// 08.18
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
class UMaterialInterface;

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

	std::wstring TextData = L"";
	std::wstring FontName = L"맑은 고딕";
	float FontSize = 16.0f;

	/*
	DWRITE_TEXT_ALIGNMENT_CENTER; // -> 가로 중앙
	DWRITE_TEXT_ALIGNMENT_JUSTIFIED;  // -> 가로 문단 정렬(양쪽정렬?)
	DWRITE_TEXT_ALIGNMENT_LEADING;	// -> 가로 좌측정렬
	DWRITE_TEXT_ALIGNMENT_TRAILING; // -> 가로 우측정렬

	DWRITE_PARAGRAPH_ALIGNMENT_CENTER; // -> 세로 중앙정렬
	DWRITE_PARAGRAPH_ALIGNMENT_FAR; // -> 세로 하단정렬
	DWRITE_PARAGRAPH_ALIGNMENT_NEAR; // -> 세로 상단정렬
	 */
	DWRITE_TEXT_ALIGNMENT TextHorizontalAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
	DWRITE_PARAGRAPH_ALIGNMENT TextVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;

	bool IsSettingSystemValue() const {return bIsSettingSystemValue;}
	void SetSystemValue(const FSystemParamConstantBuffer& NewValue)
	{
		bIsSettingSystemValue = true;
		memcpy(&SystemParam, &NewValue, sizeof(NewValue));
	}
	const FSystemParamConstantBuffer& GetSystemParamValue() const {return SystemParam;}

	bool HasOverrideMaterial() const {return bHasOverrideMaterial;}
	void SetOverrideWidgetMaterial(const std::shared_ptr<UMaterialInterface>& NewMat)
	{
		if (NewMat == nullptr)
		{
			return;
		}
		bHasOverrideMaterial = true;
		OverrideWidgetMaterial = NewMat;
	}
	const std::shared_ptr<UMaterialInterface>& GetOverrideMaterial() const {return OverrideWidgetMaterial;}
private:
	bool bIsSettingSystemValue = false;
	FSystemParamConstantBuffer SystemParam;
	bool bHasOverrideMaterial = false;
	std::shared_ptr<UMaterialInterface> OverrideWidgetMaterial;
};

