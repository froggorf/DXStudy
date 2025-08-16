// 08.16
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
struct FSlot
{
	FSlot() = default;
	virtual ~FSlot() = default;

	// 상하좌우의 위젯의 위치를 구하는 함수
	virtual float GetLeft() const = 0;
	virtual float GetRight() const = 0;
	virtual float GetTop() const = 0;
	virtual float GetBottom() const = 0;

	XMFLOAT2 GetPosition() const { return {GetLeft(), GetTop()}; }
	XMFLOAT2 GetSize() const { return {GetRight() - GetLeft(), GetBottom() - GetTop()}; }
	XMFLOAT4 GetRect() const { return {GetLeft(), GetTop(), GetRight(), GetBottom()}; }
};

enum class ECanvasAnchor{
	LeftTop, CenterTop, RightTop, WrapTop,
	LeftMiddle, CenterMiddle, RightMiddle, WrapMiddle,
	LeftBottom, CenterBottom, RightBottom, WrapBottom,
	WrapLeft, WrapCenter, WrapRight, WrapAll
};

struct FCanvasSlot : FSlot
{
	FCanvasSlot() = default;
	~FCanvasSlot() override = default;

	ECanvasAnchor Anchors;
	// Anchors 에 따라 값을 다르게 계산
	float GetLeft() const override
	{
		return 0;
	}
	float GetRight() const override
	{
		return 100;
	}		
	float GetTop() const override
	{
		return 0;
	}		
	float GetBottom() const override
	{
		return 100;
	}		

	XMFLOAT2 Position;
	XMFLOAT2 Size;
	XMFLOAT2 Alignment;
	bool bSizeToContent;   // 아마 구현 안할예정

	int ZOrder;
};
//
//enum class EHorizontalAlignment
//{
//	Left, Center, Right, Wrap
//};
//enum class EVerticalAlignment
//{
//	Top, Middle, Bottom, Wrap
//};
//
//struct FBoxSlot : FSlot{
//	float FillSize = 0.0f;    // 0.0f -> Auto, 그 외 : Fill{x}
//	float GetFillSize() const {return FillSize;}
//
//	XMFLOAT2 HorizontalPadding;
//	XMFLOAT2 VerticalPadding;
//
//	EHorizontalAlignment HorizontalAlignment;
//	EVerticalAlignment VerticalAlignment;
//
//}
//
//struct FHorizontalBoxSlot : FBoxSlot {
//	float GetLeft() override { ... }
//	float GetRight() override { ... }		
//	float GetTop() override { ... }		
//	float GetBottom() override { ... }	
//};
//
//struct FVerticalBoxSlot : FBoxSlot {
//	float GetLeft() override { ... }
//	float GetRight() override { ... }		
//	float GetTop() override { ... }		
//	float GetBottom() override { ... }	
//};