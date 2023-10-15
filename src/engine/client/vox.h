/*=======================================================================
	  Copyright (c) 2022 TechnoCorporation
=======================================================================*/
#ifndef VOX_H
#define VOX_H

#define CVOXWORDMAX			64
#define CVOXZEROSCANMAX		255	// scan up to this many samples for next zero crossing
#define MAX_SENTENCES		4096
#define SENTENCE_INDEX		-99999	// unique sentence index

typedef struct voxword_s
{
	int	volume;		// increase percent, ie: 125 = 125% increase
	int	pitch;		// pitch shift up percent
	int	start;		// offset start of wave percent
	int	end;		// offset end of wave percent
	int	cbtrim;		// end of wave after being trimmed to 'end'
	int	fKeepCached;	// 1 if this word was already in cache before sentence referenced it
	int	samplefrac;	// if pitch shifting, this is position into wav * 256
	int	timecompress;	// % of wave to skip during playback (causes no pitch shift)
	sfx_t	*sfx;		// name and cache pointer
} voxword_t;


typedef struct
{
	char	*pName;
	float	length;
} sentence_t;

void VOX_LoadWord( struct channel_s *pchan );
void VOX_FreeWord( struct channel_s *pchan );

#endif