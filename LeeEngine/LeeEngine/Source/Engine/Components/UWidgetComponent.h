#pragma once
#include "USceneComponent.h"

class FCanvasWidget;
class UUserWidget;

// Note: UUserWidget을 월드좌표를 기반으로 띄우는 함수
// 언리얼에서 Space - Screen 만 구현함
class UWidgetComponent : public USceneComponent
{
	MY_GENERATE_BODY(UWidgetComponent)

public:
	void Register() override;

	// 위젯 설정 + 위젯의 DesignResolution으로 DrawSize 설정 + 부착
	void SetWidget(const std::shared_ptr<UUserWidget>& WidgetInstance);
	std::shared_ptr<UUserWidget>& GetWidget() {return Widget;}
	void SetDrawSize(const XMFLOAT2& NewDrawSize) {DrawSize = NewDrawSize;}
	void TickComponent(float DeltaSeconds) override;
	
protected:

private:
	std::shared_ptr<FCanvasWidget> MainCanvas;
	std::shared_ptr<UUserWidget> Widget;
	XMFLOAT2 DrawSize = {100.0f,30.0f};
};