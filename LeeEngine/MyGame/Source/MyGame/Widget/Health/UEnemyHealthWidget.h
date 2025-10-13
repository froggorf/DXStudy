#pragma once
#include "UHealthWidgetBase.h"

class UEnemyHealthWidget : public UHealthWidgetBase
{
	MY_GENERATE_BODY(UEnemyHealthWidget)

	UEnemyHealthWidget() = default;
	~UEnemyHealthWidget() override = default;

	void NativeConstruct() override;
public:

protected:
private:
};
