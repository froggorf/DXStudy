// 08.16
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "ChildWidget.h"

class APlayerController;

class UUserWidget : public UObject
{
	MY_GENERATE_BODY(UUserWidget)
public:
	UUserWidget()
	{
		OwnerWidget = std::make_shared<FCanvasWidget>();
	}

	void AddToViewport(){ /* 뷰표트에 추가 */ }

	// 모든 Widget  이 계층적으로 렌더링 되도록
	void Draw()
	{ 
		// TODO: 현재는 그냥 계층적으로 렌더링 하지만,
		// 이후에는 ZOrder 값을 받아서 렌더링을 진행하도록 구현 예정
		OwnerWidget->Draw(); 
	}

private:
	std::shared_ptr<FPanelWidget> OwnerWidget;

	std::shared_ptr<APlayerController> OwningPlayer;
};
