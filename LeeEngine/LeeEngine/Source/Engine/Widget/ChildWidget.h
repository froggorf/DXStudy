// 08.16
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Slot.h"
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
protected:
	// 매프레임마다 계산되는 값
	// Designed Resolution -> Render Resolution 이 되는 값
	XMFLOAT2 ScaleFactor = {0,0};
private:
	bool bShow = true;
	std::shared_ptr<FSlot> Slot;
	std::weak_ptr<FChildWidget> OwnerWidget;
};

class FPanelWidget : public FChildWidget, public IPanelContainer
{
public:
	void SetDesignResolution(XMFLOAT2 Resolution) { DesignResolution = Resolution; }

	void SetCurrentSize(const XMFLOAT2 NewSize) { CurrentSize = NewSize; };
	XMFLOAT2 GetCurrentSize() const {return CurrentSize;}
	void Tick(float DeltaSeconds) override;

	XMFLOAT2 GetDesignResolution() const {return DesignResolution;}
protected:
	// 디자인 해상도(루트)
	XMFLOAT2 DesignResolution = {500.0f,500.0f};

	// 해당 위젯의 사이즈
	XMFLOAT2 CurrentSize = {0.0f,0.0f};
};


class FCanvasWidget : public FPanelWidget{
public:
	void Tick(float DeltaSeconds) override;

	std::shared_ptr<FSlot> CreateSlot() override { return std::make_shared<FCanvasSlot>(); }
};


class FHorizontalBoxWidget : public FPanelWidget{
public:
	void Tick(float DeltaSeconds) override;
	std::shared_ptr<FSlot> CreateSlot() override {return std::make_shared<FHorizontalBoxSlot>();}
protected:
	void CalculateChildrenSlots();
};

class FVerticalBoxWidget : public FPanelWidget
{
public:
	void Tick(float DeltaSeconds) override;
	std::shared_ptr<FSlot> CreateSlot() override {return std::make_shared<FVerticalBoxSlot>();}
protected:
	void CalculateChildrenSlots();
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

	void Tick(float DeltaSeconds) override;
private:
	FImageBrush Brush;
	XMFLOAT4 ColorAndOpacity;

	Delegate<> OnMouseButtonDown;  
};

class FTextWidget : public FChildWidget
{
public:
	FTextWidget() = default;

	void SetText(const std::wstring& NewText) { Text = NewText; }
	void SetFontSize(float NewFontSize) { FontSize = NewFontSize; }
	void SetFontColor(const XMFLOAT4& NewFontColor) {FontColor = NewFontColor;}

	void Tick(float DeltaSeconds) override;

private:
	std::wstring Text = L"";
	std::wstring FontName = L"맑은 고딕";
	float FontSize = 16.0f;
	XMFLOAT4 FontColor = {0.0f,0.0f,0.0f,1.0f};
};