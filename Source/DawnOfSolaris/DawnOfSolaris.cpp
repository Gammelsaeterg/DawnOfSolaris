// Fill out your copyright notice in the Description page of Project Settings.

#include "DawnOfSolaris.h"
#include "Modules/ModuleManager.h"
#include "GameFramework/Character.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, DawnOfSolaris, "DawnOfSolaris" );

bool canDamageInteract(ECombatAlignment selfAlignment, ECombatAlignment otherAlignment)
{
	switch (selfAlignment)
	{
	case ECombatAlignment::Neutral:
		return true;
		break;
	case ECombatAlignment::Human:
		if (ECombatAlignment::Alien == otherAlignment || ECombatAlignment::Player == otherAlignment || ECombatAlignment::Neutral == otherAlignment) { return true; }
		else { return false; }
		break;
	case ECombatAlignment::Alien:
		if (ECombatAlignment::Human == otherAlignment || ECombatAlignment::Neutral == otherAlignment) { return true; }
		else { return false; }
		break;
	case ECombatAlignment::Player:
		if (ECombatAlignment::Human == otherAlignment || ECombatAlignment::Player == otherAlignment || ECombatAlignment::Neutral == otherAlignment) { return true; }
		else { return false; }
		break;
	default:
		return false; // Should not be possible to reach this line of code
	}
}

float calculateKnockbackLength(float inMultiplier)
{
	if (inMultiplier > flinchAndKnockbackMinLimit && inMultiplier < launchMinLimit)
	{
		// Turn inMulitplier to value between 0.f to 1.f based on flinchAndKnockbackMinLimit and launchMinLimit
		float inMultiplierInPercent = (inMultiplier - flinchAndKnockbackMinLimit) / (launchMinLimit - flinchAndKnockbackMinLimit);
		
		float minKnockbackValue = 100.f;
		float maxKnockbackValue = 1500.f;
		float knockbackDifferene = maxKnockbackValue - minKnockbackValue;

		return minKnockbackValue + (inMultiplierInPercent * knockbackDifferene);
	}
	else
	{
		return 0.f;
	}

}

float calculateLaunchLength(float inMultiplier)
{
	if (inMultiplier > launchMinLimit)
	{
		float BaseLaunchValue = 1000.f;
		return (inMultiplier * BaseLaunchValue);
	}
	else
	{
		return 0.f;
	}
}
