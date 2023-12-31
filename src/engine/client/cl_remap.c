/*=======================================================================
	  Copyright (c) 2022 TechnoCorporation
=======================================================================*/

#include "common.h"
#include "client.h"
#include "gl_local.h"
#include "studio.h"

/*
====================
CL_GetRemapInfoForEntity

Returns remapinfo slot for specified entity
====================
*/
remap_info_t *CL_GetRemapInfoForEntity( cl_entity_t *e )
{
	if( !e ) return NULL;

	if( e == &clgame.viewent )
		return clgame.remap_info[clgame.maxEntities];

	return clgame.remap_info[e->curstate.number];
}

/*
====================
CL_CmpStudioTextures

return true if equal
====================
*/
qboolean CL_CmpStudioTextures( int numtexs, mstudiotexture_t *p1, mstudiotexture_t *p2 )
{
	int	i;

	if( !p1 || !p2 ) return false;

	for( i = 0; i < numtexs; i++, p1++, p2++ )
	{
		if( p1->flags & STUDIO_NF_COLORMAP )
			continue;	// colormaps always has different indexes

		if( p1->index != p2->index )
			return false;
	} 
	return true;
}

/*
====================
CL_CreateRawTextureFromPixels

Convert texture_t struct into mstudiotexture_t prototype
====================
*/
byte *CL_CreateRawTextureFromPixels( texture_t *tx, size_t *size, int topcolor, int bottomcolor )
{
	static mstudiotexture_t	pin;
	byte			*pal;

	Assert( size != NULL );

	*size = sizeof( pin ) + (tx->width * tx->height) + 768;

	// fill header
	if( !pin.name[0] ) Q_strncpy( pin.name, "#raw_remap_image.mdl", sizeof( pin.name ));
	pin.flags = STUDIO_NF_COLORMAP; // just in case :-)
	pin.index = (int)(tx + 1); // pointer to pixels
	pin.width = tx->width;
	pin.height = tx->height;

	// update palette
	pal = (byte *)(tx + 1) + (tx->width * tx->height);
	Image_PaletteHueReplace( pal, topcolor, tx->anim_min, tx->anim_max, 3 );
	Image_PaletteHueReplace( pal, bottomcolor, tx->anim_max + 1, tx->anim_total, 3 );

	return (byte *)&pin;
}

/*
====================
CL_DuplicateTexture

Dupliacte texture with remap pixels
====================
*/
void CL_DuplicateTexture( mstudiotexture_t *ptexture, int topcolor, int bottomcolor )
{
	gl_texture_t	*glt;
	texture_t		*tx = NULL;
	char		texname[128];
	int		i, size, index;
	byte		paletteBackup[768];
	byte		*raw, *pal;

	// save off the real texture index
	index = ptexture->index;
	glt = R_GetTexture( index );
	Q_snprintf( texname, sizeof( texname ), "#%i_%s", RI.currententity->curstate.number, glt->name + 1 );

	// search for pixels
	for( i = 0; i < RI.currentmodel->numtextures; i++ )
	{
		tx = RI.currentmodel->textures[i];
		if( tx->gl_texturenum == index )
			break; // found
	}

	Assert( tx != NULL );

	// backup original palette
	pal = (byte *)(tx + 1) + (tx->width * tx->height);
	memcpy( paletteBackup, pal, 768 );

	raw = CL_CreateRawTextureFromPixels( tx, &size, topcolor, bottomcolor );
	ptexture->index = GL_LoadTexture( texname, raw, size, TF_FORCE_COLOR ); // do copy

	// restore original palette
	memcpy( pal, paletteBackup, 768 );
}

/*
====================
CL_UpdateStudioTexture

Update texture top and bottom colors
====================
*/
void CL_UpdateStudioTexture( mstudiotexture_t *ptexture, int topcolor, int bottomcolor )
{
	gl_texture_t	*glt;
	rgbdata_t		*pic;
	texture_t		*tx = NULL;
	char		texname[128], name[128], mdlname[128];
	int		i, size, index;
	byte		paletteBackup[768];
	byte		*raw, *pal;

	// save off the real texture index
	glt = R_GetTexture( ptexture->index );

	// build name of original texture
	Q_strncpy( mdlname, RI.currentmodel->name, sizeof( mdlname ));
	COM_FileBase( ptexture->name, name );
	COM_StripExtension( mdlname );

	Q_snprintf( texname, sizeof( texname ), "#%s/%s.mdl", mdlname, name );
	index = GL_FindTexture( texname );
	if( !index ) return; // couldn't find texture

	// search for pixels
	for( i = 0; i < RI.currentmodel->numtextures; i++ )
	{
		tx = RI.currentmodel->textures[i];
		if( tx->gl_texturenum == index )
			break; // found
	}

	Assert( tx != NULL );

	// backup original palette
	pal = (byte *)(tx + 1) + (tx->width * tx->height);
	memcpy( paletteBackup, pal, 768 );

	raw = CL_CreateRawTextureFromPixels( tx, &size, topcolor, bottomcolor );
	pic = FS_LoadImage( glt->name, raw, size );
	if( !pic )
	{
		Con_DPrintf( S_ERROR "Couldn't update texture %s\n", glt->name );
		return;
	}

	index = GL_UpdateTextureInternal( glt->name, pic, 0 );
	FS_FreeImage( pic );

	// restore original palette
	memcpy( pal, paletteBackup, 768 );

	Assert( index == ptexture->index );
}

/*
====================
CL_UpdateAliasTexture

Update texture top and bottom colors
====================
*/
void CL_UpdateAliasTexture( unsigned short *texture, int skinnum, int topcolor, int bottomcolor )
{
	char	texname[MAX_QPATH];
	rgbdata_t	skin, *pic;
	texture_t	*tx;

	if( !texture || !RI.currentmodel->textures )
		return; // no remapinfo in model

	tx = RI.currentmodel->textures[skinnum];
	if( !tx ) return; // missing texture ?

	if( *texture == 0 )
	{
		Q_snprintf( texname, sizeof( texname ), "%s:remap%i_%i", RI.currentmodel->name, skinnum, RI.currententity->index );
		skin.width = tx->width;
		skin.height = tx->height;
		skin.depth = skin.numMips = 1;
		skin.size = tx->width * tx->height;
		skin.type = PF_INDEXED_24;
		skin.flags = IMAGE_HAS_COLOR|IMAGE_QUAKEPAL;
		skin.encode = DXT_ENCODE_DEFAULT;
		skin.buffer = (byte *)(tx + 1);
		skin.palette = skin.buffer + skin.size;
		pic = FS_CopyImage( &skin ); // because GL_LoadTextureInternal will freed a rgbdata_t at end
		*texture = GL_LoadTextureInternal( texname, pic, TF_KEEP_SOURCE );
	}

	// and now we can remap with internal routines
	GL_ProcessTexture( *texture, -1.0f, topcolor, bottomcolor );
}

/*
====================
CL_AllocRemapInfo

Allocate new remap info per entity
and make copy of remap textures
====================
*/
void CL_AllocRemapInfo( int topcolor, int bottomcolor )
{
	remap_info_t	*info;
	studiohdr_t	*phdr;
	aliashdr_t	*ahdr;
	mstudiotexture_t	*src, *dst;
	int		i, size;

	if( !RI.currententity ) return;
	i = ( RI.currententity == &clgame.viewent ) ? clgame.maxEntities : RI.currententity->curstate.number;

	if( !RI.currentmodel || ( RI.currentmodel->type != mod_alias && RI.currentmodel->type != mod_studio ))
	{
		// entity has changed model by another type, release remap info
		if( clgame.remap_info[i] )
		{
			CL_FreeRemapInfo( clgame.remap_info[i] );
			clgame.remap_info[i] = NULL;
		}
		return; // missed or hide model, ignore it
	}

	// model doesn't contains remap textures
	if( RI.currentmodel->numtextures <= 0 )
	{
		// entity has changed model with no remap textures
		if( clgame.remap_info[i] )
		{
			CL_FreeRemapInfo( clgame.remap_info[i] );
			clgame.remap_info[i] = NULL;
		}
		return;
	}

	if( RI.currentmodel->type == mod_studio )
	{
		phdr = (studiohdr_t *)Mod_StudioExtradata( RI.currentmodel );
		if( !phdr ) return;	// bad model?

		src = (mstudiotexture_t *)(((byte *)phdr) + phdr->textureindex);
		dst = (clgame.remap_info[i] ? clgame.remap_info[i]->ptexture : NULL); 

		// NOTE: we must copy all the structures 'mstudiotexture_t' for easy access when model is rendering
		if( !CL_CmpStudioTextures( phdr->numtextures, src, dst ) || clgame.remap_info[i]->model != RI.currentmodel )
		{
			// this code catches studiomodel change with another studiomodel with remap textures
			// e.g. playermodel 'barney' with playermodel 'gordon'
			if( clgame.remap_info[i] ) CL_FreeRemapInfo( clgame.remap_info[i] ); // free old info
			size = sizeof( remap_info_t ) + ( sizeof( mstudiotexture_t ) * phdr->numtextures );
			info = clgame.remap_info[i] = Mem_Calloc( clgame.mempool, size );	
			info->ptexture = (mstudiotexture_t *)(info + 1); // textures are immediately comes after remap_info
		}
		else
		{
			// studiomodel is valid, nothing to change
			return;
		}

		info->numtextures = phdr->numtextures;
		info->topcolor = topcolor;
		info->bottomcolor = bottomcolor;

		src = (mstudiotexture_t *)(((byte *)phdr) + phdr->textureindex);
		dst = info->ptexture;

		// copy unchanged first
		memcpy( dst, src, sizeof( mstudiotexture_t ) * phdr->numtextures );

		// make local copies for remap textures
		for( i = 0; i < info->numtextures; i++ )
		{
			if( dst[i].flags & STUDIO_NF_COLORMAP )
				CL_DuplicateTexture( &dst[i], topcolor, bottomcolor );
		}
	}
	else if( RI.currentmodel->type == mod_alias )
	{
		ahdr = (aliashdr_t *)Mod_AliasExtradata( RI.currentmodel );
		if( !ahdr ) return;	// bad model?

		// NOTE: we must copy all the structures 'mstudiotexture_t' for easy access when model is rendering
		if( !clgame.remap_info[i] || clgame.remap_info[i]->model != RI.currentmodel )
		{
			// this code catches studiomodel change with another studiomodel with remap textures
			// e.g. playermodel 'barney' with playermodel 'gordon'
			if( clgame.remap_info[i] ) CL_FreeRemapInfo( clgame.remap_info[i] ); // free old info
			info = clgame.remap_info[i] = Mem_Calloc( clgame.mempool, sizeof( remap_info_t ));	
		}
		else
		{
			// aliasmodel is valid, nothing to change
			return;
		}

		info->numtextures = RI.currentmodel->numtextures;

		// alias remapping is easy
		CL_UpdateRemapInfo( topcolor, bottomcolor );
	}
	else
	{
		// only alias & studio models are supposed for remapping
		return;
	}

	info->model = RI.currentmodel;
}

/*
====================
CL_UpdateRemapInfo

Update all remaps per entity
====================
*/
void CL_UpdateRemapInfo( int topcolor, int bottomcolor )
{
	remap_info_t	*info;
	int		i;

	i = ( RI.currententity == &clgame.viewent ) ? clgame.maxEntities : RI.currententity->curstate.number;
	info = clgame.remap_info[i];
	if( !info ) return; // no remap info

	if( info->topcolor == topcolor && info->bottomcolor == bottomcolor )
		return; // values is valid

	for( i = 0; i < info->numtextures; i++ )
	{
		if( info->ptexture != NULL )
		{
			if( FBitSet( info->ptexture[i].flags, STUDIO_NF_COLORMAP ))
				CL_UpdateStudioTexture( &info->ptexture[i], topcolor, bottomcolor );
		}
		else CL_UpdateAliasTexture( &info->textures[i], i, topcolor, bottomcolor );
	}

	info->topcolor = topcolor;
	info->bottomcolor = bottomcolor;
}

/*
====================
CL_FreeRemapInfo

Release remap info per entity
====================
*/
void CL_FreeRemapInfo( remap_info_t *info )
{
	int	i;

	Assert( info != NULL );

	// release all colormap texture copies
	for( i = 0; i < info->numtextures; i++ )
	{
		if( info->ptexture != NULL )
		{
			if( FBitSet( info->ptexture[i].flags, STUDIO_NF_COLORMAP ))
				GL_FreeTexture( info->ptexture[i].index );
		}

		if( info->textures[i] != 0 )
			GL_FreeTexture( info->textures[i] );
	}

	Mem_Free( info ); // release struct	
}

/*
====================
CL_ClearAllRemaps

Release all remap infos
====================
*/
void CL_ClearAllRemaps( void )
{
	int	i;

	if( clgame.remap_info )
	{
		for( i = 0; i < clgame.maxRemapInfos; i++ )
		{
			if( clgame.remap_info[i] )
				CL_FreeRemapInfo( clgame.remap_info[i] ); 
		}
		Mem_Free( clgame.remap_info );
	}
	clgame.remap_info = NULL;
}