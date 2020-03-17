//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//===========================================================================//

#ifndef IACHIEVEMENTMGR_H
#define IACHIEVEMENTMGR_H
#ifdef _WIN32
#pragma once
#endif

#include "utlmap.h"
#include "vgui_controls/Panel.h"

class CBaseAchievement;

#include "compat_iachievementmgr.h"

// flags for IAchievement::GetFlags

#define ACH_LISTEN_KILL_EVENTS				0x0001
#define ACH_LISTEN_MAP_EVENTS				0x0002
#define ACH_LISTEN_COMPONENT_EVENTS			0x0004
#define ACH_HAS_COMPONENTS					0x0020
#define ACH_SAVE_WITH_GAME					0x0040
#define ACH_SAVE_GLOBAL						0x0080
#define ACH_FILTER_ATTACKER_IS_PLAYER		0x0100
#define ACH_FILTER_VICTIM_IS_PLAYER_ENEMY	0x0200
#define ACH_FILTER_FULL_ROUND_ONLY			0x0400
#define ACH_FILTER_LOCAL_PLAYER_EVENTS		0x0800		// Evaluate player-specific events only

#define ACH_LISTEN_PLAYER_KILL_ENEMY_EVENTS		ACH_LISTEN_KILL_EVENTS | ACH_FILTER_ATTACKER_IS_PLAYER | ACH_FILTER_VICTIM_IS_PLAYER_ENEMY
#define ACH_LISTEN_KILL_ENEMY_EVENTS		ACH_LISTEN_KILL_EVENTS | ACH_FILTER_VICTIM_IS_PLAYER_ENEMY

// Update this for changes in either abstract class in this file
#define ACHIEVEMENTMGR_INTERFACE_VERSION "ACHIEVEMENTMGR_INTERFACE_VERSION001"

#define ACHIEVEMENT_LOCALIZED_NAME_FROM_STR( name ) \
	( g_pVGuiLocalize->Find( CFmtStr( "#%s_NAME", name ) ) )

#define ACHIEVEMENT_LOCALIZED_NAME( pAchievement ) \
	( ACHIEVEMENT_LOCALIZED_NAME_FROM_STR( pAchievement->GetName() ) )

#define ACHIEVEMENT_LOCALIZED_DESC_FROM_STR( name ) \
	( g_pVGuiLocalize->Find( CFmtStr( "#%s_DESC", name ) ) )

#define ACHIEVEMENT_LOCALIZED_DESC( pAchievement ) \
	( ACHIEVEMENT_LOCALIZED_DESC_FROM_STR( pAchievement->GetName() ) )

// Special slot designations
#define SINGLE_PLAYER_SLOT	0
#define STEAM_PLAYER_SLOT	0

#endif // IACHIEVEMENTMGR_H
