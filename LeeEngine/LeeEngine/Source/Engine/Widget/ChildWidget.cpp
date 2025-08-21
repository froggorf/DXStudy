#include "CoreMinimal.h"
#include "ChildWidget.h"

#include "Engine/UEngine.h"
#include "Engine/EditorClient/Panel/ImguiViewport.h"
#include "Engine/RenderCore/EditorScene.h"
#include "Engine/World/UWorld.h"

void IPanelContainer::AddChild(const std::shared_ptr<FChildWidget>& NewChild)
{
	AttachChildren.emplace_back(NewChild);
}

UINT WidgetCount = 0;
FChildWidget::FChildWidget()
{
	WidgetID = WidgetCount++;
}

void FChildWidget::Tick(float DeltaSeconds)
{
	std::shared_ptr<FPanelWidget> PanelWidget = std::dynamic_pointer_cast<FPanelWidget>(GetOwner());
	if (PanelWidget)
	{
		const XMFLOAT2& ParentPanelSize = PanelWidget->GetCurrentSize();
		GetSlot()->SetParentSize(ParentPanelSize);	
	}
}

std::shared_ptr<FCanvasWidget> FChildWidget::GetRootWidget()
{
	if (nullptr == GetOwner())
	{
		return std::dynamic_pointer_cast<FCanvasWidget>(shared_from_this()) ;
	}
	else
	{
		return GetRootWidget();
	}
}

bool FChildWidget::HandleInput(const FInputEvent& InputEvent)
{
	if (InputEvent.bKeyEvent)
	{
		InputEvent.Key;
		
	}
	else
	{
		const std::shared_ptr<FSlot>& Slot = GetSlot();
		const float Left = Slot->GetLeft() * ScaleFactor.x;
		const float Top = Slot->GetTop() * ScaleFactor.y;
		const float Right = Slot->GetRight() * ScaleFactor.x;
		const float Bottom = Slot->GetBottom() * ScaleFactor.y;

		XMFLOAT2 MousePos = InputEvent.CurPosition;
#ifdef WITH_EDITOR
		const XMFLOAT2& EditorOffset = FImguiLevelViewport::LevelViewportPos;
		MousePos.x -= EditorOffset.x;
		MousePos.y -= EditorOffset.y;
		MousePos.y += WindowTitleBarHeight;
#endif
		if (Left <= MousePos.x && MousePos.x <= Right
				&& Top <= MousePos.y && MousePos.y <= Bottom)
		{
			
			HandleMouseInput(InputEvent);
			return true;
		}
	}

	return false;
}

void FPanelWidget::Tick(float DeltaSeconds)
{
	FChildWidget::Tick(DeltaSeconds);

	XMFLOAT2 CurrentWidgetSize{};
	// 현재 해상도를 업데이트하고
	{
		// 루트 위젯이면 화면을 전부다 덮을 수 있도록 해상도를 설정해주고
		if (!GetOwner())
		{
			const XMFLOAT2& Resolution = GDirectXDevice->GetCurrentResolution();
			CurrentWidgetSize = DesignResolution;

			XMFLOAT2 ScaleFactor = {
				Resolution.x / DesignResolution.x,
				Resolution.y / DesignResolution.y
			};
			SetScaleFactor(ScaleFactor);
		}
		else
		{
			std::shared_ptr<FPanelWidget> ParentPanel = std::dynamic_pointer_cast<FPanelWidget>(GetOwner());
			if (ParentPanel)
			{
				XMFLOAT2 ParentSize = ParentPanel->GetCurrentSize();
				GetSlot()->SetParentSize(ParentSize);

				// 슬롯으로부터 크기 계산
				float Left = GetSlot()->GetLeft();
				float Top = GetSlot()->GetTop();
				float Right = GetSlot()->GetRight();
				float Bottom = GetSlot()->GetBottom();

				CurrentWidgetSize = {Right - Left, Bottom - Top};
			}
		
		}
	}

	// 현재 크기 저장
	SetCurrentSize(CurrentWidgetSize);

	for (const std::shared_ptr<FChildWidget>& ChildWidget : AttachChildren)
	{
		// 루트에서부터 매프레임마다 계산되어있음
		ChildWidget->SetScaleFactor(ScaleFactor);
	}


	// 모든 자식들을 Tick 돌면서 Tick 및 렌더링 되도록 업데이트를 올리기
	for (const std::shared_ptr<FChildWidget>& ChildWidget : AttachChildren)
	{
		ChildWidget->Tick(DeltaSeconds);
	}
}

bool FPanelWidget::HandleInput(const FInputEvent& InputEvent)
{
	for (const std::shared_ptr<FChildWidget> ChildWidget : AttachChildren)
	{
		if (ChildWidget->HandleInput(InputEvent))
		{
			return true;
		}
	}


	return false;
}

void FPanelWidget::CollectAllWidgets(std::vector<std::shared_ptr<FChildWidget>>& Widgets)
{
	for (const std::shared_ptr<FChildWidget>& Child : AttachChildren)
	{
		Widgets.emplace_back(Child);
	}
}

void FCanvasWidget::Tick(float DeltaSeconds)
{
	FPanelWidget::Tick(DeltaSeconds);
}

void FHorizontalBoxWidget::Tick(float DeltaSeconds)
{
	// 자식들의 Slot LeftRightTopBottom 을 미리 계산해줘야함
	CalculateChildrenSlots();

	FPanelWidget::Tick(DeltaSeconds);
}

void FHorizontalBoxWidget::CalculateChildrenSlots()
{
	if (AttachChildren.empty())
	{
		return;
	}

	constexpr float SlotDefaultSize = 50.0f;

	const XMFLOAT2& BoxWidthHeight = GetCurrentSize();

	// Auto 슬롯들이 사용한 너비
	float UsedWidth = 0.0f;
	// Fill 슬롯들의 총 비율
	float TotalFillRatio = 0.0f;

	for (const std::shared_ptr<FChildWidget>& Child : AttachChildren)
	{
		const std::shared_ptr<FHorizontalBoxSlot>& HorizontalBoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(Child->GetSlot());
		if (!HorizontalBoxSlot)
		{
			continue;
		}

		// Auto
		if (HorizontalBoxSlot->GetFillSize() == 0.0f)
		{
			UsedWidth += SlotDefaultSize;
		}
		// Fill
		else
		{
			TotalFillRatio += HorizontalBoxSlot->GetFillSize();
		}
	}

	// Fill 슬롯들이 쓰고 남은 공간
	float AvailableWidth = BoxWidthHeight.x - UsedWidth;

	float CurrentX = 0.0f;

	const float BoxLeft = GetSlot()->GetLeft();
	const float BoxTop = GetSlot()->GetTop();

	for (const std::shared_ptr<FChildWidget>& Child : AttachChildren)
	{
		const std::shared_ptr<FHorizontalBoxSlot>& HorizontalBoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(Child->GetSlot());
		if (!HorizontalBoxSlot)
		{
			continue;
		}

		float SlotWidth = 0.0f;
		// Auto
		if (HorizontalBoxSlot->GetFillSize() == 0.0f)
		{
			SlotWidth = SlotDefaultSize;
		}
		// Fill
		else
		{
			SlotWidth = AvailableWidth * (HorizontalBoxSlot->GetFillSize() / TotalFillRatio);
		}

		

		HorizontalBoxSlot->SetPosition(BoxLeft + CurrentX, BoxTop + 0.0f, BoxLeft + CurrentX+SlotWidth,BoxTop + BoxWidthHeight.y);
		CurrentX += SlotWidth;
	}
}

void FVerticalBoxWidget::Tick(float DeltaSeconds)
{
	// 자식들의 Slot LeftRightTopBottom 을 미리 계산해줘야함
	CalculateChildrenSlots();

	FPanelWidget::Tick(DeltaSeconds);
}

void FVerticalBoxWidget::CalculateChildrenSlots()
{
	if (AttachChildren.empty())
	{
		return;
	}

	constexpr float SlotDefaultSize = 50.0f;

	const XMFLOAT2& BoxWidthHeight = GetCurrentSize();

	// Auto 슬롯들이 사용한 너비
	float UsedHeight = 0.0f;
	// Fill 슬롯들의 총 비율
	float TotalFillRatio = 0.0f;

	for (const std::shared_ptr<FChildWidget>& Child : AttachChildren)
	{
		const std::shared_ptr<FVerticalBoxSlot>& VerticalBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(Child->GetSlot());
		if (!VerticalBoxSlot)
		{
			continue;
		}

		// Auto
		if (VerticalBoxSlot->GetFillSize() == 0.0f)
		{
			UsedHeight += SlotDefaultSize;
		}
		// Fill
		else
		{
			TotalFillRatio += VerticalBoxSlot->GetFillSize();
		}
	}

	// Fill 슬롯들이 쓰고 남은 공간
	float AvailableHeight = BoxWidthHeight.y - UsedHeight;

	float CurrentY = 0.0f;

	const float BoxLeft = GetSlot()->GetLeft();
	const float BoxTop = GetSlot()->GetTop();

	for (const std::shared_ptr<FChildWidget>& Child : AttachChildren)
	{
		const std::shared_ptr<FVerticalBoxSlot>& VerticalBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(Child->GetSlot());
		if (!VerticalBoxSlot)
		{
			continue;
		}

		float SlotHeight = 0.0f;
		// Auto
		if (VerticalBoxSlot->GetFillSize() == 0.0f)
		{
			SlotHeight = SlotDefaultSize;
		}
		// Fill
		else
		{
			SlotHeight = AvailableHeight * (VerticalBoxSlot->GetFillSize() / TotalFillRatio);
		}

		VerticalBoxSlot->SetPosition(BoxLeft + 0.0f, BoxTop + CurrentY, BoxLeft + BoxWidthHeight.x,BoxTop + CurrentY + SlotHeight);
		CurrentY += SlotHeight;
	}
}

void FImageWidget::HandleMouseInput(const FInputEvent& InputEvent)
{
	if (InputEvent.bKeyDown && InputEvent.Key == EKeys::MouseLeft)
	{
		OnMouseButtonDown.Broadcast();
	}
}

void FImageWidget::Tick(float DeltaSeconds)
{
	FChildWidget::Tick(DeltaSeconds);

	if (!Brush.Image)
	{
		return;
	}

	
	// LeftTopRightBottom 받아서
	float Left = GetSlot()->GetLeft();
	float Right = GetSlot()->GetRight();
	float Top = GetSlot()->GetTop();
	float Bottom = GetSlot()->GetBottom();

	Left *= ScaleFactor.x;
	Top *= ScaleFactor.y;
	Right *= ScaleFactor.x;
	Bottom *= ScaleFactor.y;

	const XMFLOAT2& ScreenSize = GDirectXDevice->GetCurrentResolution();

	float NDC_Left   = (Left / ScreenSize.x) * 2.0f - 1.0f;
	float NDC_Right  = (Right / ScreenSize.x) * 2.0f - 1.0f;
	float NDC_Top    = 1.0f - (Top / ScreenSize.y) * 2.0f;      
	float NDC_Bottom = 1.0f - (Bottom / ScreenSize.y) * 2.0f;   

	FWidgetRenderData WidgetRenderData;
	WidgetRenderData.Left = NDC_Left;
	WidgetRenderData.Top = max(NDC_Top,NDC_Bottom);
	WidgetRenderData.Width = NDC_Right - NDC_Left;					
	WidgetRenderData.Height = std::abs(NDC_Bottom - NDC_Top);	
	WidgetRenderData.Texture = Brush.Image;
	WidgetRenderData.Tint = ColorAndOpacity;
	WidgetRenderData.ZOrder = GetZOrder();

	GEngine->GetWorld()->AddCurrentFrameWidgetRenderData(WidgetRenderData);
}

void FTextWidget::Tick(float DeltaSeconds)
{
	FChildWidget::Tick(DeltaSeconds);

	if (Text.empty())
	{
		return;
	}

	// LeftTopRightBottom 받아서
	float Left = GetSlot()->GetLeft();
	float Right = GetSlot()->GetRight();
	float Top = GetSlot()->GetTop();
	float Bottom = GetSlot()->GetBottom();

	Left *= ScaleFactor.x;
	Top *= ScaleFactor.y;
	Right *= ScaleFactor.x;
	Bottom *= ScaleFactor.y;

	FWidgetRenderData WidgetRenderData;
	WidgetRenderData.Left = Left;
	WidgetRenderData.Top = Top;
	WidgetRenderData.Width = Right-Left;
	WidgetRenderData.Height = Bottom- Top;
	WidgetRenderData.TextData = Text;
	WidgetRenderData.FontName = FontName;
	WidgetRenderData.FontSize = FontSize;
	WidgetRenderData.Tint = FontColor;
	WidgetRenderData.ZOrder = GetZOrder();

	GEngine->GetWorld()->AddCurrentFrameWidgetRenderData(WidgetRenderData);
}
