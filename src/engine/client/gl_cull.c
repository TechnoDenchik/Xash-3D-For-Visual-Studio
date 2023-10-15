/*=======================================================================
	  Copyright (c) 2022 TechnoCorporation
=======================================================================*/

#include "common.h"
#include "client.h"
#include "gl_local.h"
#include "entity_types.h"

/*
=============================================================

FRUSTUM AND PVS CULLING

=============================================================
*/
/*
=================
R_CullBox

Returns true if the box is completely outside the frustum
=================
*/
qboolean R_CullBox( const vec3_t mins, const vec3_t maxs )
{
	return GL_FrustumCullBox( &RI.frustum, mins, maxs, 0 );
}

/*
=================
R_CullSphere

Returns true if the sphere is completely outside the frustum
=================
*/
qboolean R_CullSphere( const vec3_t centre, const float radius )
{
	return GL_FrustumCullSphere( &RI.frustum, centre, radius, 0 );
}

/*
=============
R_CullModel
=============
*/
int R_CullModel( cl_entity_t *e, const vec3_t absmin, const vec3_t absmax )
{
	if( e == &clgame.viewent )
	{
		if( CL_IsDevOverviewMode( ))
			return 1;

		if( RP_NORMALPASS() && !cl.local.thirdperson && cl.viewentity == ( cl.playernum + 1 ))
			return 0;

		return 1;
	}

	// don't reflect this entity in mirrors //Magic Nipples - readding mirrors
	if (FBitSet(e->curstate.effects, EF_NOREFLECT) && FBitSet(RI.params, RP_MIRRORVIEW))
		return 1;

	// draw only in mirrors //Magic Nipples - readding mirrors
	if (FBitSet(e->curstate.effects, EF_REFLECTONLY) && !FBitSet(RI.params, RP_MIRRORVIEW))
		return 1;

	// local client can't view himself if camera or thirdperson is not active
	if( RP_LOCALCLIENT( e ) && !cl.local.thirdperson && cl.viewentity == ( cl.playernum + 1 ))
		return 1;

	if( R_CullBox( absmin, absmax ))
		return 1;

	return 0;
}

/*
=================
R_CullSurface

cull invisible surfaces
=================
*/
int R_CullSurface( msurface_t *surf, gl_frustum_t *frustum, uint clipflags )
{
	cl_entity_t	*e = RI.currententity;

	if( !surf || !surf->texinfo || !surf->texinfo->texture )
		return CULL_OTHER;

	if( r_nocull->value )
		return CULL_VISIBLE;

	// world surfaces can be culled by vis frame too
	if( RI.currententity == clgame.entities && surf->visframe != tr.framecount )
		return CULL_VISFRAME;

	// only static ents can be culled by frustum
	if( !R_StaticEntity( e )) frustum = NULL;

	if( !VectorIsNull( surf->plane->normal ))
	{
		float	dist;

		// can use normal.z for world (optimisation)
		if( RI.drawOrtho )
		{
			vec3_t	orthonormal;

			if( e == clgame.entities ) orthonormal[2] = surf->plane->normal[2];
			else Matrix4x4_VectorRotate( RI.objectMatrix, surf->plane->normal, orthonormal );
			dist = orthonormal[2];
		}
		else dist = PlaneDiff( tr.modelorg, surf->plane );

		//if( glState.faceCull == GL_FRONT )
		if (glState.faceCull == GL_FRONT || (RI.params & RP_MIRRORVIEW)) //Magic Nipples - readding mirrors
		{
			if( FBitSet( surf->flags, SURF_PLANEBACK ))
			{
				if( dist >= -BACKFACE_EPSILON )
					return CULL_BACKSIDE; // wrong side
			}
			else
			{
				if( dist <= BACKFACE_EPSILON )
					return CULL_BACKSIDE; // wrong side
			}
		}
		else if( glState.faceCull == GL_BACK )
		{
			if( FBitSet( surf->flags, SURF_PLANEBACK ))
			{
				if( dist <= BACKFACE_EPSILON )
					return CULL_BACKSIDE; // wrong side
			}
			else
			{
				if( dist >= -BACKFACE_EPSILON )
					return CULL_BACKSIDE; // wrong side
			}
		}
	}

	if( frustum && GL_FrustumCullBox( frustum, surf->info->mins, surf->info->maxs, clipflags ))
		return CULL_FRUSTUM;

	return CULL_VISIBLE;
}