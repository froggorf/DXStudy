#pragma once
#include "MyGame/Widget/UMyGameWidgetBase.h"

class USanhwaWidget : public UMyGameWidgetBase
{
	MY_GENERATE_BODY(USanhwaWidget)

	USanhwaWidget() = default;
	~USanhwaWidget() override = default;

	std::string GetSkillTextureName() override  { return "T_Icon_Sanhwa_Skill";}
	std::string GetBasicAttackTextureName() override { return "T_Icon_Sanhwa_BasicAttack";}
	std::string GetUltimateAttackTextureName() override { return "T_Icon_Sanhwa_Ultimate";}
private:
};
