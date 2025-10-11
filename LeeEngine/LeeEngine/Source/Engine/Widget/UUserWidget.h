// 08.16
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "ChildWidget.h"

class APlayerController;

class UUserWidget : public UObject, public std::enable_shared_from_this<UUserWidget>
{
	MY_GENERATE_BODY(UUserWidget)
public:
	UUserWidget();

	virtual void Tick(float DeltaSeconds);
	// 자신이 AttachedChildWidgets 에 부착되었을 때 실행되는 Tick
	virtual void Tick_AttachedUserWidgetVersion(float DeltaSeconds) {};

	virtual void NativeConstruct();

	void SetPlayerController(APlayerController* OwningPC) { OwningPlayer = OwningPC;}

	void AddToViewport(){ /* 뷰표트에 추가 */ }

	void CollectAllWidgets(std::vector<std::shared_ptr<FChildWidget>>& Widgets);

	// 부모 Widget의 특정 FPanelWidget 에 부착
	// AttachToCanvas 함수 내에서 NativeConstruct 실행됨
	void AttachToPanel(const std::shared_ptr<FPanelWidget>& ParentPanelWidget);

	const std::shared_ptr<FPanelWidget>& GetMainCanvasWidget() const {return MainCanvasWidget;}
protected:
	std::shared_ptr<FPanelWidget> MainCanvasWidget;

	APlayerController* OwningPlayer = nullptr;
};
