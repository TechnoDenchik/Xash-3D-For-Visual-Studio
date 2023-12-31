/*=======================================================================
      Copyright (c) 2022 TechnoCorporation
=======================================================================*/

#ifndef EXTDLL_H
#define EXTDLL_H

// shut-up compiler warnings
#pragma warning(disable : 4305)	// int or float data truncation
#pragma warning(disable : 4201)	// nameless struct/union
#pragma warning(disable : 4514)	// unreferenced inline function removed
#pragma warning(disable : 4100)	// unreferenced formal parameter
#pragma warning(disable : 4244)	// conversion from 'float' to 'int', possible loss of data

#include "windows.h"

// Misc C-runtime library headers
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define bound( min, num, max )	((num) >= (min) ? ((num) < (max) ? (num) : (max)) : (min))
#define SetBits( iBitVector, bits )	((iBitVector) = (iBitVector) | (bits))
#define ClearBits( iBitVector, bits )	((iBitVector) = (iBitVector) & ~(bits))
#define FBitSet( iBitVector, bit )	((iBitVector) & (bit))

typedef int (*cmpfunc)( const void *a, const void *b );

#include "menu_int.h"

#endif//EXTDLL_H