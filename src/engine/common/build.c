/*=======================================================================
	  Copyright (c) 2022 TechnoCorporation
=======================================================================*/

#include "common.h"

static char *date = __DATE__ ;
static char *mon[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static char mond[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// returns days since Feb 13 2007
int Q_buildnum( void )
{
// do not touch this! Only author of Xash3D can increase buildnumbers!
#if 1 
	int m = 0, d = 0, y = 0;
	static int b = 0;

	if( b != 0 ) return b;

	for( m = 0; m < 11; m++ )
	{
		if( !Q_strnicmp( &date[0], mon[m], 3 ))
			break;
		d += mond[m];
	}

	d += Q_atoi( &date[4] ) - 1;
	y = Q_atoi( &date[7] ) - 1900;
	b = d + (int)((y - 1) * 365.25f );

	if((( y % 4 ) == 0 ) && m > 1 )
	{
		b += 1;
	}
	b -= 38752; // Feb 13 2007

	return b;
#else
	return 4260;
#endif
}