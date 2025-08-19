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

	void Tick(float DeltaSeconds)
	{
		if (OwnerWidget)
		{
			OwnerWidget->Tick(DeltaSeconds);
		}
	}

	virtual void NativeConstruct();

	void SetPlayerController(APlayerController* OwningPC) { OwningPlayer = OwningPC;}

	void AddToViewport(){ /* 뷰표트에 추가 */ }

	

protected:
	std::shared_ptr<FPanelWidget> OwnerWidget;

	APlayerController* OwningPlayer;
};
