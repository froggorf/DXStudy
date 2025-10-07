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

	void SetParentSize(const XMFLOAT2& InParentSize) { ParentSize = InParentSize; }
protected:
	XMFLOAT2 ParentSize = {0,0};
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

	// Anchors 에 따라 값을 다르게 계산
	float GetLeft() const override;
	float GetRight() const override;
	float GetTop() const override;
	float GetBottom() const override;

	ECanvasAnchor Anchors = ECanvasAnchor::LeftTop;
	XMFLOAT2 Position = {0,0};
	XMFLOAT2 Size = {100,30};
	XMFLOAT2 Alignment = {0,0};
	bool bSizeToContent;   // 아마 구현 안할예정
	
	int ZOrder;


};

enum class EHorizontalAlignment
{
	Left, Center, Right, Wrap
};
enum class EVerticalAlignment
{
	Top, Middle, Bottom, Wrap
};

struct FBoxSlot : FSlot{
	float FillSize = 0.0f;    // 0.0f -> Auto, 그 외 : Fill{x}
	float GetFillSize() const {return FillSize;}

	

	EHorizontalAlignment HorizontalAlignment = EHorizontalAlignment::Wrap;
	EVerticalAlignment VerticalAlignment = EVerticalAlignment::Wrap;

	void SetPosition(float Left, float Top, float Right, float Bottom)
	{
		LeftTop = {Left,Top};
		RightBottom = {Right, Bottom};
	}

	void SetHorizontalPadding(const XMFLOAT2& NewPadding) {HorizontalPadding= NewPadding;}
	void SetVerticalPadding(const XMFLOAT2& NewPadding) {VerticalPadding= NewPadding;}
protected:
	XMFLOAT2 LeftTop {};
	XMFLOAT2 RightBottom {};
	XMFLOAT2 HorizontalPadding{};
	XMFLOAT2 VerticalPadding{};
};

struct FHorizontalBoxSlot : FBoxSlot {
	float GetLeft() const override;
	float GetRight() const override;
	float GetTop() const override;
	float GetBottom() const override;
};

struct FVerticalBoxSlot : FBoxSlot {
	float GetLeft() const override;
	float GetRight() const override;
	float GetTop() const override;
	float GetBottom() const override;
};

struct FButtonSlot : FSlot
{
	// TODO: Padding 이나 Wrap 기능 추가할 가능성이 있음
	float GetLeft() const override { return LeftTopRightBottom.x; }
	float GetTop() const override { return LeftTopRightBottom.y; }
	float GetRight() const override { return LeftTopRightBottom.z; }
	float GetBottom() const override { return LeftTopRightBottom.w; }

	// XMFLOAT4 -> {Left, Top, Right, Bottom}
	void SetLeftTopRightBottom(const XMFLOAT4& NewSize) {LeftTopRightBottom = NewSize;}
private:
	XMFLOAT4 LeftTopRightBottom {};
};