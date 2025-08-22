#include "CoreMinimal.h"
#include "Slot.h"

float FCanvasSlot::GetLeft() const
{
	switch (Anchors)
	{
	case ECanvasAnchor::LeftTop:
	case ECanvasAnchor::LeftMiddle:
	case ECanvasAnchor::LeftBottom:
		return Position.x - (Size.x * Alignment.x);

	case ECanvasAnchor::CenterTop:
	case ECanvasAnchor::CenterMiddle:
	case ECanvasAnchor::CenterBottom:
		return (ParentSize.x * 0.5f) + Position.x - (Size.x * Alignment.x);

	case ECanvasAnchor::RightTop:
	case ECanvasAnchor::RightMiddle:
	case ECanvasAnchor::RightBottom:
		return ParentSize.x + Position.x - (Size.x * Alignment.x);

	case ECanvasAnchor::WrapTop:
	case ECanvasAnchor::WrapMiddle:
	case ECanvasAnchor::WrapBottom:
	case ECanvasAnchor::WrapAll:
		return Position.x; 

	case ECanvasAnchor::WrapLeft:
		return Position.x;

	case ECanvasAnchor::WrapCenter:
		return (ParentSize.x * 0.5f) + Position.x - (Size.x * 0.5f);

	case ECanvasAnchor::WrapRight:
		return ParentSize.x - Size.x - Position.x;

	default:
		return Position.x;
	}
}

float FCanvasSlot::GetRight() const
{
	switch (Anchors)
	{
	case ECanvasAnchor::LeftTop:
	case ECanvasAnchor::LeftMiddle:
	case ECanvasAnchor::LeftBottom:
	case ECanvasAnchor::CenterTop:
	case ECanvasAnchor::CenterMiddle:
	case ECanvasAnchor::CenterBottom:
	case ECanvasAnchor::RightTop:
	case ECanvasAnchor::RightMiddle:
	case ECanvasAnchor::RightBottom:
		return GetLeft() + Size.x;

	case ECanvasAnchor::WrapTop:
	case ECanvasAnchor::WrapMiddle:
	case ECanvasAnchor::WrapBottom:
	case ECanvasAnchor::WrapAll:
		return ParentSize.x - Position.x; 

	case ECanvasAnchor::WrapLeft:
	case ECanvasAnchor::WrapCenter:
	case ECanvasAnchor::WrapRight:
		return GetLeft() + Size.x;

	default:
		return GetLeft() + Size.x;
	}
}

float FCanvasSlot::GetTop() const
{
	switch (Anchors)
	{
	case ECanvasAnchor::LeftTop:
	case ECanvasAnchor::CenterTop:
	case ECanvasAnchor::RightTop:
	case ECanvasAnchor::WrapTop:
		return Position.y - (Size.y * Alignment.y);

	case ECanvasAnchor::LeftMiddle:
	case ECanvasAnchor::CenterMiddle:
	case ECanvasAnchor::RightMiddle:
	case ECanvasAnchor::WrapMiddle:
		return (ParentSize.y * 0.5f) + Position.y - (Size.y * Alignment.y);

	case ECanvasAnchor::LeftBottom:
	case ECanvasAnchor::CenterBottom:
	case ECanvasAnchor::RightBottom:
	case ECanvasAnchor::WrapBottom:
		return ParentSize.y + Position.y - (Size.y * Alignment.y);

	case ECanvasAnchor::WrapLeft:
	case ECanvasAnchor::WrapCenter:
	case ECanvasAnchor::WrapRight:
	case ECanvasAnchor::WrapAll:
		return Position.y;  

	default:
		return Position.y;
	}
}

float FCanvasSlot::GetBottom() const
{
	switch (Anchors)
	{
	case ECanvasAnchor::LeftTop:
	case ECanvasAnchor::CenterTop:
	case ECanvasAnchor::RightTop:
	case ECanvasAnchor::LeftMiddle:
	case ECanvasAnchor::CenterMiddle:
	case ECanvasAnchor::RightMiddle:
	case ECanvasAnchor::LeftBottom:
	case ECanvasAnchor::CenterBottom:
	case ECanvasAnchor::RightBottom:
		return GetTop() + Size.y;

	case ECanvasAnchor::WrapTop:
	case ECanvasAnchor::WrapMiddle:
	case ECanvasAnchor::WrapBottom:
		return GetTop() + Size.y;

	case ECanvasAnchor::WrapLeft:
	case ECanvasAnchor::WrapCenter:
	case ECanvasAnchor::WrapRight:
	case ECanvasAnchor::WrapAll:
		return ParentSize.y - Position.y;  

	default:
		return GetTop() + Size.y;
	}
	
}

float FHorizontalBoxSlot::GetLeft() const
{
	return LeftTop.x + HorizontalPadding.x;
}

float FHorizontalBoxSlot::GetRight() const
{
	return RightBottom.x - HorizontalPadding.y; 
}

float FHorizontalBoxSlot::GetTop() const
{
	float ContentTop = LeftTop.y + VerticalPadding.x;  // Top 패딩 적용
	float ContentBottom = RightBottom.y - VerticalPadding.y;  // Bottom 패딩 적용
	float AvailableHeight = ContentBottom - ContentTop;

	// 자식 위젯의 원하는 높이 (임시로 30.0f)
	constexpr float DesiredHeight = 30.0f;

	switch (VerticalAlignment)
	{
	case EVerticalAlignment::Top:
		return ContentTop;

	case EVerticalAlignment::Middle:
		return ContentTop + (AvailableHeight - DesiredHeight) * 0.5f;

	case EVerticalAlignment::Bottom:
		return ContentBottom - DesiredHeight;

	case EVerticalAlignment::Wrap:  // Fill과 동일
	default:
		return ContentTop;
	}
}

float FHorizontalBoxSlot::GetBottom() const
{
	float ContentTop = LeftTop.y + VerticalPadding.x;
	float ContentBottom = RightBottom.y - VerticalPadding.y;
	float AvailableHeight = ContentBottom - ContentTop;

	constexpr float DesiredHeight = 30.0f; 

	switch (VerticalAlignment)
	{
	case EVerticalAlignment::Top:
		return ContentTop + DesiredHeight;

	case EVerticalAlignment::Middle:
		return ContentTop + (AvailableHeight + DesiredHeight) * 0.5f;

	case EVerticalAlignment::Bottom:
		return ContentBottom;

	case EVerticalAlignment::Wrap:  // Fill과 동일
	default:
		return ContentBottom;
	}
}

float FVerticalBoxSlot::GetLeft() const
{
	float ContentLeft = LeftTop.x + HorizontalPadding.x;    
	float ContentRight = RightBottom.x - HorizontalPadding.y; 
	float AvailableWidth = ContentRight - ContentLeft;

	constexpr float DesiredWidth = 50.0f;

	switch (HorizontalAlignment)
	{
	case EHorizontalAlignment::Left:
		return ContentLeft;

	case EHorizontalAlignment::Center:
		return ContentLeft + (AvailableWidth - DesiredWidth) * 0.5f;

	case EHorizontalAlignment::Right:
		return ContentRight - DesiredWidth;

	case EHorizontalAlignment::Wrap:
	default:
		return ContentLeft;
	}
}

float FVerticalBoxSlot::GetRight() const
{
	float ContentLeft = LeftTop.x + HorizontalPadding.x;
	float ContentRight = RightBottom.x - HorizontalPadding.y;
	float AvailableWidth = ContentRight - ContentLeft;

	constexpr float DesiredWidth = 50.0f;

	switch (HorizontalAlignment)
	{
	case EHorizontalAlignment::Left:
		return ContentLeft + DesiredWidth;

	case EHorizontalAlignment::Center:
		return ContentLeft + (AvailableWidth + DesiredWidth) * 0.5f;

	case EHorizontalAlignment::Right:
		return ContentRight;

	case EHorizontalAlignment::Wrap:
	default:
		return ContentRight;
	}
}

float FVerticalBoxSlot::GetTop() const
{
	return LeftTop.y + VerticalPadding.x;  
}

float FVerticalBoxSlot::GetBottom() const
{
	return RightBottom.y - VerticalPadding.y;
}

