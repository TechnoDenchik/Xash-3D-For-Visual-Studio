//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#pragma once
#if !defined ( EV_HLDMH )
#define EV_HLDMH

// bullet types
#include "weapons_const.h"

enum
{
	SMOKE_WALLPUFF = 0,
	SMOKE_RIFLE,
	SMOKE_PISTOL,
	SMOKE_BLACK
};

namespace cl {

void EV_HLDM_GunshotDecalTrace(pmtrace_t *pTrace, char *decalName , char chTextureType);
void EV_HLDM_DecalGunshot(pmtrace_t *pTrace, int iBulletType, float scale, int r, int g, int b, bool bCreateSparks, char cTextureType, bool isSky);
void EV_HLDM_FireBullets(int idx,
						 float *forward, float *right, float *up,
						 int cShots,
						 float *vecSrc, float *vecDirShooting, float *vecSpread,
						 float flDistance, int iBulletType, int iPenetration);
void EV_CS16Client_KillEveryRound( struct tempent_s *te, float frametime, float currenttime );
void EV_CS16Client_CreateSmoke(int type, Vector origin, Vector dir, int speed, float scale, int r, int g, int b , bool wind, Vector velocity = Vector(0, 0, 0), int framerate = 35 );

}

#endif // EV_HLDMH
