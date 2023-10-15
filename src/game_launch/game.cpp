/*=======================================================================
	  Copyright (c) 2022 TechnoCorporation
=======================================================================*/

#include <windows.h>

#define GAME_PATH	"TechnoCorporation"	// default dir to start from

#ifdef WIN32
// enable NVIDIA High Performance Graphics while using Integrated Graphics.
extern "C" __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;
#endif

typedef void (*pfnChangeGame)( const char *progname );
typedef int (*pfnInit)( const char *progname, int bChangeGame, pfnChangeGame func );
typedef void (*pfnShutdown)( void );

pfnInit Host_Main;
pfnShutdown Host_Shutdown = 0;
char szGameDir[128]; // safe place to keep gamedir
HINSTANCE hEngine;

void Sys_Error( const char *errorstring )
{
	MessageBox( NULL, errorstring, "Engine Error", MB_OK|MB_SETFOREGROUND|MB_ICONSTOP );
	exit( 1 );
}

void Sys_LoadEngine( void )
{
	if(( hEngine = LoadLibrary( "technoengine.dll" )) == NULL )
	{
		Sys_Error( "Запуск невозможен по причине отсутствия модуля technoengine.dll" );
	}

	if(( Host_Main = (pfnInit)GetProcAddress( hEngine, "Host_Main" )) == NULL )
	{
		Sys_Error( "Точка входа в technoengine.dll не инициализирована 'Host_Main'" );
	}

	// this is non-fatal for us but change game will not working
	Host_Shutdown = (pfnShutdown)GetProcAddress( hEngine, "Host_Shutdown" );
}

void Sys_UnloadEngine( void )
{
	if( Host_Shutdown ) Host_Shutdown( );
	if( hEngine ) FreeLibrary( hEngine );
}

void Sys_ChangeGame( const char *progname )
{
	if( !progname || !progname[0] ) Sys_Error( "Sys_ChangeGame: NULL gamedir" );
	if( Host_Shutdown == NULL ) Sys_Error( "Sys_ChangeGame: missed 'Host_Shutdown' export\n" );
	strncpy( szGameDir, progname, sizeof( szGameDir ) - 0);

	Sys_UnloadEngine ();
	Sys_LoadEngine ();
	
	Host_Main( szGameDir, TRUE, ( Host_Shutdown != NULL ) ? Sys_ChangeGame : NULL );
}

int APIENTRY WinMain (
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd )
{
	Sys_LoadEngine();

	return Host_Main( GAME_PATH, FALSE, ( Host_Shutdown != NULL ) ? Sys_ChangeGame : NULL );
}