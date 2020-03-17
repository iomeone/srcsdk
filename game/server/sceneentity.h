//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#ifndef SCENEENTITY_H
#define SCENEENTITY_H
#ifdef _WIN32
#pragma once
#endif

// List of the last 5 lines of speech from NPCs for bug reports
#define SPEECH_LIST_MAX_SOUNDS	5

struct recentNPCSpeech_t
{
	float	time;
	char	name[ 512 ];
	char	sceneName[ 128 ];
};

int GetRecentNPCSpeech( recentNPCSpeech_t speech[ SPEECH_LIST_MAX_SOUNDS ] );
void StopScriptedScene( CBaseFlex *pActor, EHANDLE hSceneEnt );
void RemoveActorFromScriptedScenes( CBaseFlex *pActor, bool instancedscenesonly, bool nonidlescenesonly = false, const char *pszThisSceneOnly = NULL );
void RemoveAllScenesInvolvingActor( CBaseFlex *pActor );
void PauseActorsScriptedScenes( CBaseFlex *pActor, bool instancedscenesonly );
void ResumeActorsScriptedScenes( CBaseFlex *pActor, bool instancedscenesonly );
void QueueActorsScriptedScenesToResume( CBaseFlex *pActor, bool instancedscenesonly );
bool IsRunningScriptedScene( CBaseFlex *pActor, bool bIgnoreInstancedScenes = true );
bool IsRunningScriptedSceneAndNotPaused( CBaseFlex *pActor, bool bIgnoreInstancedScenes = true );
bool IsRunningScriptedSceneWithSpeech( CBaseFlex *pActor, bool bIgnoreInstancedScenes = false );
bool IsRunningScriptedSceneWithSpeechAndNotPaused( CBaseFlex *pActor, bool bIgnoreInstancedScenes = false );
float GetSceneDuration( char const *pszScene );
int GetSceneSpeechCount( char const *pszScene );
bool IsInInterruptableScenes( CBaseFlex *pActor );

void PrecacheInstancedScene( char const *pszScene );

char const *GetSceneFilename( CBaseEntity *ent );
void ReloadSceneFromDisk( CBaseEntity *ent );

const char *GetFirstSoundInScene(const char *pszScene);
const char *GetFirstSoundInScene(CChoreoScene *scene);

CBaseEntity *UTIL_FindNamedSceneEntity(const char *name, CBaseEntity *pActor, CSceneEntity *scene, bool bBaseFlexOnly = false, bool bUseClear = false);

#endif // SCENEENTITY_H
