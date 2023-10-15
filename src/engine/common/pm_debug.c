/*=======================================================================
	  Copyright (c) 2022 TechnoCorporation
=======================================================================*/

#include "common.h"
#include "mathlib.h"
#include "pm_local.h"

// expand debugging BBOX particle hulls by this many units.
#define BOX_GAP	0.0f    

/*
===============
PM_ParticleLine

draw line from particles
================
*/
void PM_ParticleLine( const vec3_t start, const vec3_t end, int pcolor, float life, float zvel )
{
	float	len, curdist;
	vec3_t	diff, pos;

	// determine distance
	VectorSubtract( end, start, diff );
	len = VectorNormalizeLength( diff );
	curdist = 0;

	while( curdist <= len )
	{
		VectorMA( start, curdist, diff, pos );
		CL_Particle( pos, pcolor, life, 0, zvel );
		curdist += 2.0f;
	}
}

/*
================
PM_DrawRectangle

================
*/
static void PM_DrawRectangle( const vec3_t tl, const vec3_t bl, const vec3_t tr, const vec3_t br, int pcolor, float life )
{
	PM_ParticleLine( tl, bl, pcolor, life, 0 );
	PM_ParticleLine( bl, br, pcolor, life, 0 );
	PM_ParticleLine( br, tr, pcolor, life, 0 );
	PM_ParticleLine( tr, tl, pcolor, life, 0 );
}

/*
================
PM_DrawBBox

================
*/
void PM_DrawBBox( const vec3_t mins, const vec3_t maxs, const vec3_t origin, int pcolor, float life )
{
	vec3_t	p[8], tmp;
	float	gap = BOX_GAP;
	int	i;

	for( i = 0; i < 8; i++ )
	{
		tmp[0] = (i & 1) ? mins[0] - gap : maxs[0] + gap;
		tmp[1] = (i & 2) ? mins[1] - gap : maxs[1] + gap ;
		tmp[2] = (i & 4) ? mins[2] - gap : maxs[2] + gap ;

		VectorAdd( tmp, origin, tmp );
		VectorCopy( tmp, p[i] );
	}

	for( i = 0; i < 6; i++ )
	{
		PM_DrawRectangle( p[boxpnt[i][1]], p[boxpnt[i][0]], p[boxpnt[i][2]], p[boxpnt[i][3]], pcolor, life );
	}
}