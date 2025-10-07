#include "CoreMinimal.h"
#include "ChildWidget.h"

#include "Engine/EditorClient/Panel/ImguiViewport.h"
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
	if (InputEvent.bIsKeyEvent)
	{
		InputEvent.Key;
		
	}
	else
	{
		if (HandleMouseInput(InputEvent))
		{
			return true;
		}
	}

	return false;
}

bool FChildWidget::IsMouseInside(const FInputEvent& InputEvent) const
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
		return true;
	}

	return false;
}

void FChildWidget::GetNDCPos(float& NDC_Left, float& NDC_Top, float& NDC_Right, float& NDC_Bottom) const
{
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

	NDC_Left   = (Left / ScreenSize.x) * 2.0f - 1.0f;
	NDC_Right  = (Right / ScreenSize.x) * 2.0f - 1.0f;
	NDC_Top    = 1.0f - (Top / ScreenSize.y) * 2.0f;      
	NDC_Bottom = 1.0f - (Bottom / ScreenSize.y) * 2.0f;
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
	for (const std::shared_ptr<FChildWidget>& ChildWidget : AttachChildren)
	{
		// 08.22 FButtonWidget의 경우 FChildWidget::HandleInput을 처리하여 마우스 인풋을 처리해야하는데,
		// FPanelWidget이어서 FPanelWidget::HandleInput이 호출되어서 자신의 인풋이 무시되는 현상이 발생함,
		// 따라서, 먼저 PanelWidget의 HandleInput을 처리해주고,
		// 거기서 인풋이 처리되지 않을 경우 자식을 처리하는 것으로 진행
		if (FChildWidget::HandleInput(InputEvent))
		{
			return true;
		}

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

		// 자식의 슬롯 위치를 조정해주면서
		HorizontalBoxSlot->SetPosition(BoxLeft + CurrentX, BoxTop + 0.0f, BoxLeft + CurrentX+SlotWidth,BoxTop + BoxWidthHeight.y);
		// 만약 패널 위젯이면 사이즈를 인식시켜줘야함
		if (const std::shared_ptr<FPanelWidget>& PanelWidget = std::dynamic_pointer_cast<FPanelWidget>(Child))
		{
			PanelWidget->SetCurrentSize({HorizontalBoxSlot->GetRight()-HorizontalBoxSlot->GetLeft(), HorizontalBoxSlot->GetBottom() - HorizontalBoxSlot->GetTop()});
		}
		
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
		if (const std::shared_ptr<FPanelWidget>& PanelWidget = std::dynamic_pointer_cast<FPanelWidget>(Child))
		{
			PanelWidget->SetCurrentSize({VerticalBoxSlot->GetRight()-VerticalBoxSlot->GetLeft(), VerticalBoxSlot->GetBottom() - VerticalBoxSlot->GetTop()});
		}
		CurrentY += SlotHeight;
	}
}

void FButtonWidget::Tick(float DeltaSeconds)
{
	// 일단 Box의 Slot으로 AttachChildren의 Slot을 지정
	std::shared_ptr<FSlot> Slot = GetSlot();
	const float ButtonLeft = Slot->GetLeft();
	const float ButtonTop = Slot->GetTop();
	const float ButtonRight = Slot->GetRight();
	const float ButtonBottom = Slot->GetBottom();

	for (const std::shared_ptr<FChildWidget>& Child : AttachChildren)
	{
		const std::shared_ptr<FButtonSlot>& ButtonSlot = std::dynamic_pointer_cast<FButtonSlot>(Child->GetSlot());
		ButtonSlot->SetLeftTopRightBottom({ButtonLeft,ButtonTop,ButtonRight,ButtonBottom});
	}


	// Panel 로서 업데이트를 해줘야할 뿐 아니라 버튼만의 렌더링도 진행해줘야함
	UINT CurButtonType = static_cast<UINT>(CurrentButtonType);
	if (Style[CurButtonType].Image && GetVisibility())
	{
		float NDC_Left{}, NDC_Right{}, NDC_Top{}, NDC_Bottom{};
		GetNDCPos(NDC_Left, NDC_Top, NDC_Right, NDC_Bottom);

		FWidgetRenderData WidgetRenderData;
		WidgetRenderData.Left = NDC_Left;
		WidgetRenderData.Top = max(NDC_Top,NDC_Bottom);
		WidgetRenderData.Width = NDC_Right - NDC_Left;					
		WidgetRenderData.Height = std::abs(NDC_Bottom - NDC_Top);	
		WidgetRenderData.Texture = Style[CurButtonType].Image;
		WidgetRenderData.Tint = Style[CurButtonType].Tint;
		WidgetRenderData.ZOrder = GetZOrder();

		GEngine->GetCurrentWorld()->AddCurrentFrameWidgetRenderData(WidgetRenderData);	
	}

	// 모든것들이 이뤄진 후 자식들이 Tick 되면서 렌더링 큐에 넣어줘야함
	FPanelWidget::Tick(DeltaSeconds);
}

bool FButtonWidget::HandleMouseInput(const FInputEvent& InputEvent)
{
	// 버튼이 비활성화 중이면 early return 
	if (bIsButtonDisabled)
	{
		return false;
	}
	

	bool bIsMouseInsideButton = IsMouseInside(InputEvent);

	
	// 마우스 이벤트
	if (!InputEvent.bIsKeyEvent)
	{
		EKeys Key = InputEvent.Key;

		// 버튼이 활성화 중이고 && 2DAxis 마우스면
		if (Key == EKeys::MouseXY2DAxis)
		{
			if (!bIsMouseInsideButton)
			{
				// 호버링 중이었다가 풀리면
				if (CurrentButtonType == EButtonType::Hovered || CurrentButtonType == EButtonType::Pressed)
				{
					CurrentButtonType = bIsButtonDisabled ? EButtonType::Disabled : EButtonType::Normal;
					OnUnhovered.Broadcast();
				}
			}
			else
			{
				// 첫 호버링 됐을 때 호버 이벤트
				if (CurrentButtonType != EButtonType::Hovered && CurrentButtonType != EButtonType::Pressed)
				{
					CurrentButtonType = EButtonType::Hovered;
					OnHovered.Broadcast();
					if (HoveredSound)
					{
						GAudioDevice->PlaySound2D(HoveredSound);
					}		
				}	
			}

			// MouseXY2DAxis 이벤트는 모든 UI에서 사용할 수 있으므로 해당 인풋은 소비되지 않도록 해야함
			return false;
		}

		// Pressed
		if (bIsMouseInsideButton && Key == EKeys::MouseLeft && InputEvent.bKeyDown)
		{
			CurrentButtonType = EButtonType::Pressed;
			OnPressed.Broadcast();
			if (PressedSound)
			{
				GAudioDevice->PlaySound2D(PressedSound);
			}

			// TODO: Consume 할지 설정하기
			return true;
		}
		// Released
		if (bIsMouseInsideButton && Key == EKeys::MouseLeft && !InputEvent.bKeyDown)
		{
			if (CurrentButtonType == EButtonType::Pressed)
			{
				CurrentButtonType = bIsMouseInsideButton ? EButtonType::Hovered : EButtonType::Normal;
				OnClicked.Broadcast();
				OnReleased.Broadcast();
			}

			return true;
		}
	}

	return false;
}

void FButtonWidget::SetDisabled(bool bNewDisabled)
{
	bIsButtonDisabled = bNewDisabled;
	CurrentButtonType =  bIsButtonDisabled ? EButtonType::Disabled : EButtonType::Normal; 
}

bool FImageWidget::HandleMouseInput(const FInputEvent& InputEvent)
{
	// 마우스가 이미지 위에 없으면 나가기
	if (!IsMouseInside(InputEvent))
	{
		return false;
	}

	if (InputEvent.bKeyDown && InputEvent.Key == EKeys::MouseLeft)
	{
		OnMouseButtonDown.Broadcast();
		return true;
	}

	return false;
}

void FImageWidget::Tick(float DeltaSeconds)
{
	FChildWidget::Tick(DeltaSeconds);

	if (!Brush.Image && !GetVisibility())
	{
		return;
	}

	float NDC_Left{}, NDC_Right{}, NDC_Top{}, NDC_Bottom{};
	GetNDCPos(NDC_Left, NDC_Top, NDC_Right, NDC_Bottom);

	FWidgetRenderData WidgetRenderData;
	WidgetRenderData.Left = NDC_Left;
	WidgetRenderData.Top = max(NDC_Top,NDC_Bottom);
	WidgetRenderData.Width = NDC_Right - NDC_Left;					
	WidgetRenderData.Height = std::abs(NDC_Bottom - NDC_Top);	
	WidgetRenderData.Texture = Brush.Image;
	WidgetRenderData.Tint = GetColor();
	WidgetRenderData.ZOrder = GetZOrder();

	GEngine->GetCurrentWorld()->AddCurrentFrameWidgetRenderData(WidgetRenderData);
}

void FTextWidget::SetHorizontalAlignment(ETextHorizontalAlignment HorizontalAlignment)
{
	switch (HorizontalAlignment)
	{
	case ETextHorizontalAlignment::Center:
		TextHorizontalAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
		break;
	case ETextHorizontalAlignment::Left:
		TextHorizontalAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
		break;
	case ETextHorizontalAlignment::Right:
		TextHorizontalAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
		break;
	}
}

void FTextWidget::SetVerticalAlignment(ETextVerticalAlignment VerticalAlignment)
{
	switch (VerticalAlignment)
	{
	case ETextVerticalAlignment::Bottom:
		TextVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
		break;
	case ETextVerticalAlignment::Center:
		TextVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
		break;
	case ETextVerticalAlignment::Top:
		TextVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
		break;
	}
}

void FTextWidget::Tick(float DeltaSeconds)
{
	FChildWidget::Tick(DeltaSeconds);

	if (Text.empty() && !GetVisibility())
	{
		return;
	}

	std::wstring T = Text;

	// 텍스트 같은 경우엔 DWrite 라이브러리를 사용하기에
	// NDC 좌표계로 직접 gpu로 렌더링 요청을 보내는것이 아닌
	// Direct2D DeviceContext->DrawTextW 로 렌더링,
	// 렌더링 시 윈도우 좌표를 요청하므로 Slot의 좌표를 그대로 전달

	float Left = GetSlot()->GetLeft();
	float Top = GetSlot()->GetTop();
	float Right = GetSlot()->GetRight();
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
	WidgetRenderData.TextHorizontalAlignment = TextHorizontalAlignment;
	WidgetRenderData.TextVerticalAlignment= TextVerticalAlignment;

	GEngine->GetCurrentWorld()->AddCurrentFrameWidgetRenderData(WidgetRenderData);
}

FProgressBarWidget::FProgressBarWidget()
{
	FillBrush.Image = UTexture::GetTextureCache("T_White");
	FillBrush.Tint = {0.0f, 0.5f, 1.0f, 1.0f};
	BackgroundBrush.Image = UTexture::GetTextureCache("T_White");
}

void FProgressBarWidget::Tick(float DeltaSeconds)
{
	FChildWidget::Tick(DeltaSeconds);

	if (!FillBrush.Image && !BackgroundBrush.Image && !GetVisibility())
	{
		return;
	}

	float BG_Left{}, BG_Right{}, BG_Top{}, BG_Bottom{};
	GetNDCPos(BG_Left, BG_Top, BG_Right, BG_Bottom);

	// 백그라운드 먼저 그리고
	{
		FWidgetRenderData WidgetRenderData;
		WidgetRenderData.Left = BG_Left;
		WidgetRenderData.Top = max(BG_Top,BG_Bottom);
		WidgetRenderData.Width = BG_Right - BG_Left;					
		WidgetRenderData.Height = std::abs(BG_Bottom - BG_Top);	
		WidgetRenderData.Texture = BackgroundBrush.Image;
		WidgetRenderData.Tint = BackgroundBrush.Tint;
		WidgetRenderData.ZOrder = GetZOrder();

		GEngine->GetCurrentWorld()->AddCurrentFrameWidgetRenderData(WidgetRenderData);
	}

	// Fill 그리기
	{
		float Fill_Left, Fill_Right, Fill_Top, Fill_Bottom;
		CalculateFillImagePosition(BG_Left, BG_Top, BG_Right, BG_Bottom
			,Fill_Left, Fill_Top, Fill_Right,Fill_Bottom);

		FWidgetRenderData WidgetRenderData;
		WidgetRenderData.Left = Fill_Left;
		WidgetRenderData.Top = max(Fill_Top,Fill_Bottom);
		WidgetRenderData.Width = Fill_Right - Fill_Left;					
		WidgetRenderData.Height = std::abs(Fill_Bottom - Fill_Top);	
		WidgetRenderData.Texture = FillBrush.Image;
		WidgetRenderData.Tint = FillBrush.Tint;
		WidgetRenderData.ZOrder = GetZOrder();

		GEngine->GetCurrentWorld()->AddCurrentFrameWidgetRenderData(WidgetRenderData);
	}
	
}

void FProgressBarWidget::CalculateFillImagePosition(float BGLeft, float BGTop, float BGRight, float BGBottom, float& FillLeft, float& FillTop, float& FillRight, float& FillBottom)
{
	const float BGWidth =  BGRight - BGLeft;
	const float FillWidth = BGWidth * Value;
	const float BGHeight = std::abs(BGBottom - BGTop);
	const float FillHeight = BGHeight * Value;

	const float BGHorizontalCenter = (BGRight + BGLeft)/2;
	const float FillHalfWidth = FillWidth/2;

	const float BGVerticalCenter = (BGBottom + BGTop) / 2;
	const float FillHalfHeight = FillHeight/2;
	switch (FillMode)
	{
		// 왼 -> 오
	case EProgressBarFillMode::LeftToRight:
		{
			FillLeft = BGLeft;
			FillTop = BGTop;
			FillBottom = BGBottom;
			FillRight = FillLeft + FillWidth;
		}
		break;
		// 오 -> 왼
	case EProgressBarFillMode::RightToLeft:
		{
			FillRight = BGRight;
			FillTop = BGTop;
			FillBottom = BGBottom;
			FillLeft = FillRight - FillWidth;
		}
		break;

		// 밑 -> 위
	case EProgressBarFillMode::BottomToTop:
		{
			FillLeft = BGLeft;
			FillRight = BGRight;
			FillBottom = BGBottom;
			FillTop = BGBottom + FillHeight;
		}
		break;
		// 위 -> 밑
	case EProgressBarFillMode::TopToBottom:
		{
			FillLeft = BGLeft;
			FillRight = BGRight;
			FillTop = BGTop;
			FillBottom = FillTop - FillHeight;
		}
		break;

		// 중앙 -> 밖(양옆으로 퍼지듯)
	case EProgressBarFillMode::FillFromCenterHorizontal:
		{
			FillTop = BGTop;
			FillBottom = BGBottom;
			FillLeft = BGHorizontalCenter - FillHalfWidth;
			FillRight = BGHorizontalCenter + FillHalfWidth;
		}
	break;
		// 중앙 -> 밖(위아래로 퍼지듯)
	case EProgressBarFillMode::FillFromCenterVertical:
		{
			FillLeft = BGLeft;
			FillRight = BGRight;
			FillTop = BGVerticalCenter + FillHalfHeight;
			FillBottom = BGVerticalCenter - FillHalfHeight;
		}
	break;
		// 중앙 -> 밖
	case EProgressBarFillMode::FillFromCenter:
		{
			FillTop = BGVerticalCenter + FillHalfHeight;
			FillBottom = BGVerticalCenter - FillHalfHeight;
			FillLeft = BGHorizontalCenter - FillHalfWidth;
			FillRight = BGHorizontalCenter + FillHalfWidth;
		}
		break;
	
	}
}
