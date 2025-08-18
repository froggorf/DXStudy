#include "CoreMinimal.h"
#include "ChildWidget.h"

void IPanelContainer::AddChild(const std::shared_ptr<FChildWidget>& NewChild)
{
	AttachChildren.emplace_back(NewChild);
}

UINT WidgetCount = 0;
FChildWidget::FChildWidget()
{
	WidgetID = WidgetCount++;
}

void FImageWidget::Tick(float DeltaSeconds)
{
	//std::shared_ptr<FSlot> OwnerSlot = GetOwner()->GetSlot();
	//
	//// LeftTopRightBottom 받아서
	//float Left = GetSlot()->GetLeft();
	//float Right = GetSlot()->GetRight();
	//float Top = GetSlot()->GetTop();
	//float Bottom = GetSlot()->GetBottom();
	//
	//
	//// 해당위치에
	//if (Brush.Image)
	//{
	//
	//
	//}
}
