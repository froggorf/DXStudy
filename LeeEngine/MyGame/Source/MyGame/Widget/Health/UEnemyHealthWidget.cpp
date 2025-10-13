#include "CoreMinimal.h"
#include "UEnemyHealthWidget.h"

void UEnemyHealthWidget::NativeConstruct()
{
	UHealthWidgetBase::NativeConstruct();

	PB_HealthBar->SetFillImageBrush({UTexture::GetTextureCache("T_White"), {1,0,0,1}});
}
