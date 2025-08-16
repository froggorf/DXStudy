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
	virtual void Tick(float DeltaSeconds) = 0;
	virtual void Draw() = 0;

	const std::shared_ptr<FSlot>& GetSlot() const {return Slot;}

	UINT WidgetID = -1;
private:

	bool bShow = true;

	std::shared_ptr<FSlot> Slot;
	std::weak_ptr<FChildWidget> OwnerWidget;		
	
};

class FPanelWidget : public FChildWidget, public IPanelContainer
{

};


class FCanvasWidget : public FPanelWidget{
public:
	XMFLOAT2 GetScaleFactor() const {
		return {
			CurrentResolution.x / DesignResolution.x,
			CurrentResolution.y / DesignResolution.y
		};
	}
	void SetDesignResolution(XMFLOAT2 Resolution) { DesignResolution = Resolution; }
	void UpdateCurrentResolution(XMFLOAT2 Resolution) { CurrentResolution = Resolution; }

	void Tick(float DeltaSeconds) override
	{
		
	}

	void Draw() override
	{
		for (const std::shared_ptr<FChildWidget>& ChildWidget : AttachChildren)
		{
			ChildWidget->Draw();
		}
	}

	std::shared_ptr<FSlot> CreateSlot() override
	{
		std::shared_ptr<FCanvasSlot> CanvasSlot = std::make_shared<FCanvasSlot>();
		CanvasSlot->Anchors = ECanvasAnchor::LeftTop;
		CanvasSlot->Position = {0,0};
		CanvasSlot->Size = {100,30};
		CanvasSlot->Alignment = {0,0};
		return CanvasSlot;
	}
private:
	XMFLOAT2 DesignResolution = {500.0f,500.0f};
	XMFLOAT2 CurrentResolution{};  // 현재 화면 해상도
};

class FHorizontalBoxWidget : public FPanelWidget{
	// Horizontal Box 기능...
};

class FVerticalBoxWidget : public FPanelWidget
{
	
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

	void Tick(float DeltaSeconds) override
	{
		
	}
	void Draw() override;
private:
	FImageBrush Brush;
	XMFLOAT4 ColorAndOpacity;

	Delegate<> OnMouseButtonDown;  // 추후 기존에 만든 델리게이트와 연결하기
};