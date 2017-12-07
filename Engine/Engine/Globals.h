#ifndef _GLOBALS_H_
#define _GLOBALS_H_

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define RELEASE(x) x != NULL ? delete x, x = NULL : LOG("_RELEASE_ERROR_")
#define RELEASE_ARRAY(x) x != NULL ? delete[] x, x = NULL : LOG("_RELEASE_ARRAY_ERROR_")

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI 3.1415926535897932384f

#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )

#define GET_Y_LNG(size,angle) ((size * sinf(angle * DEGTORAD) / 2.0f)
#define GET_X_LNG(size,angle) ((size * cosf(angle * DEGTORAD) / 2.0f)

#define GET_CURVE_X(w_size,angle) (w_size * (sinf(angle *DEGTORAD)))
#define GET_CURVE_Z(h_size,angle) (h_size * (sinf(angle *DEGTORAD)))

#define START(timer) timer.Start()
#define PEEK(timer) LOG("%s took %f ms", __FUNCTION__, timer.ReadSec())

#define LIBRARY_TEXTURES_FOLDER "Library\\Materials\\"
#define LIBRARY_MESH_FOLDER "Library\\Meshes\\"
#define LIBRARY_SCRIPTS_FOLDER "Library\\Scripts\\"
#define LIBRARY_META_FOLDER "Library\\Metas\\"
#define LIBRARY_SCENE_FOLDER "Library\\Scenes\\"
#define LIBRARY_FOLDER "Library\\"
#define SETTINGS_FOLDER "Settings\\"
#define ASSETS_FOLDER "Assets\\"

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

typedef unsigned int uint;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
typedef unsigned char uchar;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 1024
#define SCREEN_SIZE 1

#endif // !_GLOBALS_H_


