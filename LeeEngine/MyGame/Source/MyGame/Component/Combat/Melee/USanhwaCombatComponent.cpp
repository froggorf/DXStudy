#include "CoreMinimal.h"
#include "USanhwaCombatComponent.h"

#include "Engine/RenderCore/EditorScene.h"
#include "MyGame/Character/AMyGameCharacterBase.h"
#include "MyGame/Widget/Sanhwa/USanhwaWidget.h"

USanhwaCombatComponent::USanhwaCombatComponent()
{
	BasicAttackMoveDistance = {
		200, 200, 200, -200, 500
	};
}

void USanhwaCombatComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	UMeleeBaseComponent::Initialize(MyCharacter);

	SetBasicAttackMontages({
		"AM_Sanhwa_BasicAttack0",
		"AM_Sanhwa_BasicAttack1",
		"AM_Sanhwa_BasicAttack2",
		"AM_Sanhwa_BasicAttack3",
		"AM_Sanhwa_BasicAttack4",
	});

	SetBasicAttackRange({
		{250,250,250},
		{250,250,250},
		{250,250,250},
		{250,250,250},
		{250,250,250},
	});
}

void USanhwaCombatComponent::BasicAttack()
{
	UMeleeBaseComponent::BasicAttack();
}

void USanhwaCombatComponent::Attack4Success()
{
	// 비어있는 게이지 칸 탐색
	std::vector<int> EmptyGauge;
	EmptyGauge.reserve(GaugeSize);
	for (size_t i = 0; i < SanhwaGauge.size(); ++i)
	{
		if (!SanhwaGauge[i])
		{
			EmptyGauge.emplace_back(i);
		}
	}

	// 비어있는 게이지가 없는것
	if (EmptyGauge.empty())
	{
		return;
	}

	int NewGaugeIndex = MyMath::RandVector(EmptyGauge);
	SanhwaGauge[NewGaugeIndex] = true;

	if (OwnerCharacter)
	{
		if (const std::shared_ptr<USanhwaWidget>& SanhwaWidget = std::static_pointer_cast<USanhwaWidget>(OwnerCharacter->GetCharacterWidget()))
		{
			SanhwaWidget->SetGaugeUI(SanhwaGauge);
		}
	}
}

