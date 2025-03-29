// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "Engine/Components/UActorComponent.h"

class UTestComponent : public UActorComponent
{
	MY_GENERATED_BODY(UTestComponent)
public:
	UTestComponent();



#ifdef WITH_EDITOR
	void DrawDetailPanel(UINT ComponentDepth) override;
#endif
protected:
private:
public:
	float TestValue1 = 0.0f;
protected:
private:
};