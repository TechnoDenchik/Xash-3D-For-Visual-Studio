/*=======================================================================
      Copyright (c) 2022 TechnoCorporation
=======================================================================*/

#ifndef COM_STRINGS_H
#define COM_STRINGS_H

// default colored message headers
#define S_NOTE			"^2Note:^7 "
#define S_WARN			"^3Warning:^7 "
#define S_ERROR			"^1Error:^7 "
#define S_USAGE			"Usage: "

#define S_OPENGL_NOTE		"^2OpenGL Note:^7 "
#define S_OPENGL_WARN		"^3OpenGL Warning:^7 "
#define S_OPENGL_ERROR		"^3OpenGL Error:^7 "

// end game final default message
#define DEFAULT_ENDGAME_MESSAGE	"The End"

// path to the hash-pak that contain custom player decals
#define CUSTOM_RES_PATH		"custom.hpk"

// path to default playermodel in GoldSrc
#define DEFAULT_PLAYER_PATH_HALFLIFE	"models/player.mdl"

// path to default playermodel in Quake
#define DEFAULT_PLAYER_PATH_QUAKE	"progs/player.mdl"

// debug beams
#define DEFAULT_LASERBEAM_PATH	"sprites/laserbeam.spr"

#define DEFAULT_INTERNAL_PALETTE	"gfx/palette.lmp"

#define DEFAULT_EXTERNAL_PALETTE	"gfx/palette.pal"

// path to folders where placed all sounds
#define DEFAULT_SOUNDPATH		"sound/"

// path store saved games
#define DEFAULT_SAVE_DIRECTORY	"save/"

// fallback to this skybox
#define DEFAULT_SKYBOX_PATH		"gfx/env/desert"

// playlist for startup videos
#define DEFAULT_VIDEOLIST_PATH	"media/StartupVids.txt"

#define CVAR_GLCONFIG_DESCRIPTION	"enable or disable %s"

#define DEFAULT_BSP_BUILD_ERROR	"%s can't be loaded in this build. Please rebuild engine with enabled SUPPORT_BSP2_FORMAT\n"

#endif//COM_STRINGS_H