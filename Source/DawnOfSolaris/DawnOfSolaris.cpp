// Fill out your copyright notice in the Description page of Project Settings.

#include "DawnOfSolaris.h"
#include "Modules/ModuleManager.h"

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
		return false; // TODO: Default if all fails, change this!
	}
}
