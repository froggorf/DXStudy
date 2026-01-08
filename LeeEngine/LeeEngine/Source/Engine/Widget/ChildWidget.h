// 08.16
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Slot.h"
#include "Engine/FAudioDevice.h"
#include "Engine/Class/Framework/UPlayerInput.h"
#include "Engine/Misc/Delegate.h"

class UUserWidget;
class UMaterialInterface;

class IPanelContainer
{
public:
	virtual ~IPanelContainer() = default;

	//AttachChildren에 따라 Slot 값이 다르게 제공됨
	virtual std::shared_ptr<FSlot> CreateSlot() = 0;
	void AddChild(const std::shared_ptr<class FChildWidget>& NewChild);
	void ClearChildren() {AttachChildren.clear();}

	void SetChildrenVisibility(bool NewVisible);
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
	void SetSlot(const std::shared_ptr<FSlot>& NewSlot) {Slot = NewSlot;}

	UINT WidgetID = -1;

	std::shared_ptr<class FCanvasWidget> GetRootWidget();
	std::shared_ptr<FChildWidget> GetOwner() const
	{
		if (OwnerWidget.expired())
		{
			return nullptr;
		}
		return OwnerWidget.lock();
	}

	void SetScaleFactor(const XMFLOAT2& NewScaleFactor) {ScaleFactor = NewScaleFactor;}

	virtual bool HandleInput(const FInputEvent& InputEvent);
	bool IsMouseInside(const FInputEvent& InputEvent) const;
	virtual bool HandleMouseInput(const FInputEvent& InputEvent) = 0;
	virtual bool HandleKeyboardInput(const FInputEvent& InputEvent) { return false;}

	float GetZOrder() const {return ZOrder;}
	void SetZOrder(float NewZOrder) {ZOrder = NewZOrder;}

	bool GetVisibility() const {return bShow; }
	virtual void SetVisibility(bool NewVisible) {bShow = NewVisible;}
protected:
	// 해당 위젯이 그려질 NDC 좌표계의 위치 반환
	void GetNDCPos(float& NDC_Left, float& NDC_Top, float& NDC_Right, float& NDC_Bottom) const;
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


	void SetVisibility(bool NewVisible) override
	{
		SetChildrenVisibility(NewVisible);
	}

	void SetOwnerUserWidget(UUserWidget* NewOwner) {OwnerUserWidget = NewOwner;}
	UUserWidget* GetOwnerUserWidget() const {return OwnerUserWidget;}
	std::vector<std::shared_ptr<UUserWidget>> AttachedUserWidget;
protected:
	// 디자인 해상도(루트)
	XMFLOAT2 DesignResolution = {500.0f,500.0f};

	// 해당 위젯의 사이즈
	XMFLOAT2 CurrentSize = {0.0f,0.0f};

	UUserWidget* OwnerUserWidget = nullptr;

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

struct FImageBrush{
	std::shared_ptr<UTexture> Image;
	XMFLOAT4 Tint = XMFLOAT4{1.0f,1.0f,1.0f,1.0f};
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

	void SetStyle(EButtonType StyleType, const FImageBrush& NewStyle) { Style[static_cast<UINT>(StyleType)] = NewStyle; }
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

	std::array<FImageBrush, static_cast<UINT>(EButtonType::Count)> Style;

};



class FImageWidget : public FChildWidget
{
public:
	FImageWidget(const FImageBrush& NewBrush)
	{
		Brush = NewBrush;
	}
	~FImageWidget() override = default;

	void SetBrush(const FImageBrush& NewBrush) {Brush = NewBrush;}
	void SetTexture(const std::shared_ptr<UTexture>& NewTexture) {Brush.Image = NewTexture;}
	void SetColor(const XMFLOAT4& NewColor) {Brush.Tint = NewColor;}

	const XMFLOAT4& GetColor() const {return Brush.Tint;}

	bool HandleMouseInput(const FInputEvent& InputEvent) override;

	void Tick(float DeltaSeconds) override;

	void SetSystemParam(const FSystemParamConstantBuffer& NewSystemParam)
	{
		bSetSystemValue = true;
		SystemParam = NewSystemParam;
	}
	void SetOverrideMaterial(const std::shared_ptr<UMaterialInterface>& NewMat)
	{
		bOverrideMaterial = true;
		OverrideMaterial = NewMat;
	}

	Delegate<> OnMouseButtonDown;  
private:
	FImageBrush Brush;
	bool bSetSystemValue = false;
	bool bOverrideMaterial = false;
	std::shared_ptr<UMaterialInterface> OverrideMaterial = nullptr;
	FSystemParamConstantBuffer SystemParam;

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

enum class EProgressBarFillMode
{
	Radial_12To3,	// 0(Radial_12To3), 위치 고정되어야함 셰이더와 공유 
	Radial_12To9,	// 1(Radial_12To9), 위치 고정되어야함 셰이더와 공유
	Radial_9To12,	// 2(Radial_9To12), 위치 고정되어야함 셰이더와 공유
	LeftToRight,
	RightToLeft,
	FillFromCenter,
	FillFromCenterHorizontal,
	FillFromCenterVertical,
	TopToBottom,
	BottomToTop,
	Count
};
class FProgressBarWidget : public FChildWidget
{
public:
	FProgressBarWidget();
	~FProgressBarWidget() override = default;
	void SetFillMode(EProgressBarFillMode NewMode) {FillMode = NewMode;}
	void SetBackgroundImageBrush(const FImageBrush& NewImageBrush){BackgroundBrush=NewImageBrush;}
	void SetFillImageBrush(const FImageBrush& NewImageBrush){FillBrush = NewImageBrush;}

	void Tick(float DeltaSeconds) override;

	void SetValue(float NewValue)  { Value =  std::clamp(NewValue, 0.0f,1.0f); }
	float GetValue() const {return Value;}

	static const std::shared_ptr<UMaterialInterface>& GetRadialPBMaterial(EProgressBarFillMode FillMode);
private:
	void CalculateFillImagePosition(float BGLeft, float BGTop, float BGRight, float BGBottom, float& FillLeft, float& FillTop, float& FillRight, float& FillBottom);

public:
	bool HandleMouseInput(const FInputEvent& InputEvent) override { return false;}
	void SetSlider(const FImageBrush& NewBrush, const XMFLOAT2& SliderSize);
private:
	EProgressBarFillMode FillMode = EProgressBarFillMode::LeftToRight;

	float Value = 0.0f;
protected:
	FImageBrush BackgroundBrush;
	FImageBrush FillBrush;

	bool bIsSlider = false;
	FImageBrush SliderBrush = {nullptr, {1,1,1,0}};
	XMFLOAT2 SliderSize;

	static std::shared_ptr<UMaterialInterface> M_RadialPBMaterial;
	static std::shared_ptr<UMaterialInterface> M_RadialPBMaterial_9To12;
};

// 텍스트 입력 위젯
class FEditableTextWidget : public FChildWidget
{
public:
	FEditableTextWidget();
	~FEditableTextWidget() override;

	void Tick(float DeltaSeconds) override;
	bool HandleMouseInput(const FInputEvent& InputEvent) override;
	bool HandleKeyboardInput(const FInputEvent& InputEvent) override;

	// 텍스트 설정/조회
	void SetText(const std::wstring& NewText);
	std::wstring GetText() const { return Text; }
	std::string GetText_String() const;
	void ClearText() { Text.clear(); CursorPosition = 0; }

	// 힌트 텍스트 (플레이스홀더)
	void SetHintText(const std::wstring& NewHintText) { HintText = NewHintText; }

	// 스타일 설정
	void SetFontSize(float NewFontSize) { FontSize = NewFontSize; }
	void SetFontColor(const XMFLOAT4& NewColor) { FontColor = NewColor; }
	void SetBackgroundBrush(const FImageBrush& NewBrush) { BackgroundBrush = NewBrush; }
	void SetFocusedBrush(const FImageBrush& NewBrush) { FocusedBrush = NewBrush; }

	// 텍스트 정렬
	void SetHorizontalAlignment(ETextHorizontalAlignment Alignment);
	void SetVerticalAlignment(ETextVerticalAlignment Alignment);

	// 입력 제한
	void SetMaxLength(int32 NewMaxLength) { MaxLength = NewMaxLength; }
	void SetIsPassword(bool bNewIsPassword) { bIsPassword = bNewIsPassword; }
	void SetIsReadOnly(bool bNewReadOnly) { bIsReadOnly = bNewReadOnly; }

	// 포커스
	bool IsFocused() const { return bIsFocused; }
	void SetFocus(bool bNewFocused);

	// 델리게이트
	Delegate<const std::wstring&> OnTextChanged;
	Delegate<const std::wstring&> OnTextCommitted;  // Enter 입력 시

private:
	void UpdateCursorBlink(float DeltaTime);
	void InsertCharacter(wchar_t Character);
	void DeleteCharacter();
	void DeleteCharacterBackward();
	void MoveCursor(int32 Delta);
	std::wstring GetDisplayText() const;

	void                                      RenderBackground();
	void                                      RenderText();
	void                                      RenderCursor();
	Microsoft::WRL::ComPtr<IDWriteTextFormat> GetTextFormat();

	static void ClearGlobalFocus();
	static FEditableTextWidget* GetCurrentFocusedWidget() { return CurrentFocusedWidget; }
private:
	// 텍스트 데이터
	std::wstring Text;
	std::wstring HintText = L"텍스트 입력...";

	// 커서
	int32 CursorPosition = 0;
	float CursorBlinkTimer = 0.0f;
	bool bShowCursor = true;
	static constexpr float CursorBlinkInterval = 0.5f;

	// 스타일
	FImageBrush BackgroundBrush;
	FImageBrush FocusedBrush;
	std::wstring FontName = L"맑은 고딕";
	float FontSize = 16.0f;
	XMFLOAT4 FontColor = {0.0f, 0.0f, 0.0f, 1.0f};
	XMFLOAT4 HintTextColor = {0.5f, 0.5f, 0.5f, 1.0f};

	DWRITE_TEXT_ALIGNMENT TextHorizontalAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
	DWRITE_PARAGRAPH_ALIGNMENT TextVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;

	// 상태
	bool bIsFocused = false;
	bool bIsPassword = false;
	bool bIsReadOnly = false;
	int32 MaxLength = 100;

	// 패딩
	float TextPaddingLeft = 10.0f;
	float TextPaddingRight = 10.0f;

	static FEditableTextWidget* CurrentFocusedWidget;
};
