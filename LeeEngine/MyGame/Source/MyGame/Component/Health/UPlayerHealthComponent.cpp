#include "CoreMinimal.h"
#include "UPlayerHealthComponent.h"

#include "MyGame/Character/Player/AMyGameCharacterBase.h"
#include "MyGame/Widget/UMyGameWidgetBase.h"


void UPlayerHealthComponent::MarkHealthToWidget()
{
	if (!MyGameCharacter)
	{
		MyGameCharacter = dynamic_cast<AMyGameCharacterBase*>(GetOwner());
	}

	if (const std::shared_ptr<UMyGameWidgetBase>& Widget = MyGameCharacter->GetCharacterWidget())
	{
		Widget->SetHealthBarWidget(CurrentHealth, MaxHealth);
	}
}
