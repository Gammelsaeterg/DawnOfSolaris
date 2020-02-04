// Fill out your copyright notice in the Description page of Project Settings.

#include "DawnOfSolaris.h"
#include "Modules/ModuleManager.h"
#include "GameFramework/Character.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, DawnOfSolaris, "DawnOfSolaris" );

// Hitstun calculation: hitstun < 0.1f: hitstunAnimationOnly, 0.1f - 0.3f: hitstunFlinch, 0.3f - 0.7f: hitstunFlinchWithKnockback, > 0.7f: hitstunLaunched
void runHitstunAnimations(ACharacter & inCharacter, float inHitstunStrengthReceived, FVector hitDirection)
{
	if (inHitstunStrengthReceived < 0.1f)
	{

	}
	else if (inHitstunStrengthReceived > 0.1f && inHitstunStrengthReceived < 0.3f)
	{

	}
	else if (inHitstunStrengthReceived > 0.3f && inHitstunStrengthReceived < 0.7f)
	{

	}
	else if (inHitstunStrengthReceived > 0.7f)
	{

	}
	else
	{
		// If function reaches this place then something is wrong
		UE_LOG(LogTemp, Warning, TEXT("You dun goofed"))
	}
}

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
