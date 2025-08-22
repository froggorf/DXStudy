// 08.16
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Slot.h"
#include "Engine/FAudioDevice.h"
#include "Engine/Class/Framework/UPlayerInput.h"
#include "Engine/Misc/Delegate.h"

class IPanelContainer
{
public:
	virtual ~IPanelContainer() = default;

	//AttachChildren에 따라 Slot 값이 다르게 제공됨
	virtual std::shared_ptr<FSlot> CreateSlot() = 0;
	void AddChild(const std::shared_ptr<class FChildWidget>& NewChild);
protected:
	std::vector<std::shared_ptr<class FChildWidget>> AttachChildren;
};

// 모든 위젯의 부모 클래스
class FChildWidget : public std::enable_shared_from_this<FChildWidget>
{
public:
	FChildWidget();
	virtual ~FChildWidget() = default;
	FChildWidget(const FChildWidget& Other) = default;
	FChildWidget(FChildWidget&& Other) = default;
	FChildWidget& operator=(const FChildWidget& Other) = default;
	FChildWidget& operator=(FChildWidget&& Other) = default;

	void AttachToWidget(const std::shared_ptr<FChildWidget>& NewOwnerWidget){
		std::shared_ptr<IPanelContainer> PanelOwner = std::dynamic_pointer_cast<IPanelContainer>(NewOwnerWidget);
		if(!PanelOwner) {
			return;
		}
		OwnerWidget = NewOwnerWidget;

		ZOrder = NewOwnerWidget->ZOrder;

		Slot = PanelOwner->CreateSlot();
		std::shared_ptr<FChildWidget> ThisShared = shared_from_this();
		PanelOwner->AddChild(ThisShared);
	}
	virtual void Tick(float DeltaSeconds);

	const std::shared_ptr<FSlot>& GetSlot() const {return Slot;}

	UINT WidgetID = -1;

	std::shared_ptr<class FCanvasWidget> GetRootWidget();
	std::shared_ptr<FChildWidget> GetOwner() const {return OwnerWidget.lock();}

	void SetScaleFactor(const XMFLOAT2& NewScaleFactor) {ScaleFactor = NewScaleFactor;}

	virtual bool HandleInput(const FInputEvent& InputEvent);
	bool IsMouseInside(const FInputEvent& InputEvent) const;
	virtual bool HandleMouseInput(const FInputEvent& InputEvent) = 0;
	virtual void HandleKeyboardInput(const FInputEvent& InputEvent) {}

	float GetZOrder() const {return ZOrder;}
	void SetZOrder(float NewZOrder) {ZOrder = NewZOrder;}
protected:
	// 매프레임마다 계산되는 값
	// Designed Resolution -> Render Resolution 이 되는 값
	XMFLOAT2 ScaleFactor = {0,0};
private:
	bool bShow = true;
	std::shared_ptr<FSlot> Slot;
	std::weak_ptr<FChildWidget> OwnerWidget;

	float ZOrder = 0.0f;
};

class FPanelWidget : public FChildWidget, public IPanelContainer
{
public:
	~FPanelWidget() override = default;

	void SetDesignResolution(XMFLOAT2 Resolution) { DesignResolution = Resolution; }

	void SetCurrentSize(const XMFLOAT2 NewSize) { CurrentSize = NewSize; };
	XMFLOAT2 GetCurrentSize() const {return CurrentSize;}
	void Tick(float DeltaSeconds) override;

	XMFLOAT2 GetDesignResolution() const {return DesignResolution;}

	bool HandleInput(const FInputEvent& InputEvent) override;

	void CollectAllWidgets(std::vector<std::shared_ptr<FChildWidget>>& Widgets);
protected:
	// 디자인 해상도(루트)
	XMFLOAT2 DesignResolution = {500.0f,500.0f};

	// 해당 위젯의 사이즈
	XMFLOAT2 CurrentSize = {0.0f,0.0f};
};

class FCanvasWidget : public FPanelWidget{
public:
	~FCanvasWidget() override = default;
	void Tick(float DeltaSeconds) override;
	bool HandleMouseInput(const FInputEvent& InputEvent) override { return false; }

	std::shared_ptr<FSlot> CreateSlot() override { return std::make_shared<FCanvasSlot>(); }
};


class FHorizontalBoxWidget : public FPanelWidget{
public:
	~FHorizontalBoxWidget() override = default;
	void Tick(float DeltaSeconds) override;
	bool HandleMouseInput(const FInputEvent& InputEvent) override { return false; }
	std::shared_ptr<FSlot> CreateSlot() override {return std::make_shared<FHorizontalBoxSlot>();}
protected:
	void CalculateChildrenSlots();
};

class FVerticalBoxWidget : public FPanelWidget
{
public:
	~FVerticalBoxWidget() override = default;
	void Tick(float DeltaSeconds) override;
	bool HandleMouseInput(const FInputEvent& InputEvent) override { return false; }
	std::shared_ptr<FSlot> CreateSlot() override {return std::make_shared<FVerticalBoxSlot>();}
protected:
	void CalculateChildrenSlots();
};

struct FButtonStyle
{
	std::shared_ptr<UTexture> Image;
	XMFLOAT4 Tint;
};
enum class EButtonType
{
	Normal, Hovered, Pressed, Disabled, Count
};

class FButtonWidget : public FPanelWidget
{
public:
	FButtonWidget()
	{
		for (size_t i = 0; i < static_cast<size_t>(EButtonType::Count); ++i)
		{
			Style[i].Image = UTexture::GetTextureCache("T_White");
		}
		Style[static_cast<UINT>(EButtonType::Normal)].Tint = XMFLOAT4{0.8f,0.8f,0.8f,1.0f};
		Style[static_cast<UINT>(EButtonType::Hovered)].Tint = XMFLOAT4{1.0f,1.0f,1.0f,1.0f};
		Style[static_cast<UINT>(EButtonType::Pressed)].Tint = XMFLOAT4{0.6f,0.6f,0.6f,1.0f};
		Style[static_cast<UINT>(EButtonType::Disabled)].Tint = XMFLOAT4{0.5f,0.5f,0.5f,1.0f};
	}
	~FButtonWidget() override = default;
	void Tick(float DeltaSeconds) override;
	bool HandleMouseInput(const FInputEvent& InputEvent) override;

	std::shared_ptr<FSlot> CreateSlot() override { return std::make_shared<FButtonSlot>();}

	void SetStyle(EButtonType StyleType, const FButtonStyle& NewStyle) { Style[static_cast<UINT>(StyleType)] = NewStyle; }
	void SetDisabled(bool bNewDisabled);


	Delegate<> OnClicked;
	Delegate<> OnHovered;
	Delegate<> OnUnhovered;
	Delegate<> OnPressed;
	Delegate<> OnReleased;

	std::shared_ptr<USoundBase> PressedSound;
	std::shared_ptr<USoundBase> HoveredSound;
protected:
	void SetButtonType(EButtonType NewButtonType) { CurrentButtonType = NewButtonType;}

private:

	EButtonType CurrentButtonType = EButtonType::Normal;
	bool bIsButtonDisabled = false;

	std::array<FButtonStyle, static_cast<UINT>(EButtonType::Count)> Style;

};

struct FImageBrush{
	std::shared_ptr<UTexture> Image;
};

class FImageWidget : public FChildWidget
{
public:
	FImageWidget(const FImageBrush& NewBrush, XMFLOAT4 NewColor)
	{
		Brush = NewBrush;
		ColorAndOpacity = NewColor;
	}
	~FImageWidget() override = default;

	void SetBrush(const FImageBrush& NewBrush) {Brush = NewBrush;}
	void SetColor(const XMFLOAT4& NewColor) {ColorAndOpacity = NewColor;}

	const XMFLOAT4& GetColor() const {return ColorAndOpacity;}

	bool HandleMouseInput(const FInputEvent& InputEvent) override;

	void Tick(float DeltaSeconds) override;

	Delegate<> OnMouseButtonDown;  
private:
	FImageBrush Brush;
	XMFLOAT4 ColorAndOpacity;

};

enum class ETextHorizontalAlignment
{
	Left, Center, Right
};
enum class ETextVerticalAlignment
{
	Top,Center,Bottom
};
class FTextWidget : public FChildWidget
{
public:
	FTextWidget() = default;
	~FTextWidget() override = default;
	void SetText(const std::wstring& NewText) { Text = NewText; }
	void SetFontSize(float NewFontSize) { FontSize = NewFontSize; }
	void SetFontColor(const XMFLOAT4& NewFontColor) {FontColor = NewFontColor;}
	void SetHorizontalAlignment(ETextHorizontalAlignment HorizontalAlignment);
	void SetVerticalAlignment(ETextVerticalAlignment VerticalAlignment);
	bool HandleMouseInput(const FInputEvent& InputEvent) override { return false; }
	void Tick(float DeltaSeconds) override;

private:
	std::wstring Text = L"";
	std::wstring FontName = L"맑은 고딕";
	float FontSize = 16.0f;
	XMFLOAT4 FontColor = {0.0f,0.0f,0.0f,1.0f};

	DWRITE_TEXT_ALIGNMENT TextHorizontalAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
	DWRITE_PARAGRAPH_ALIGNMENT TextVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
};

