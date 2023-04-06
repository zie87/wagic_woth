#ifndef _DEBUG_H_
#define _DEBUG_H_

#if defined(WIN32) && defined(_MSC_VER) 
#define snprintf sprintf_s
#endif 

#include "limits.h"

#if defined (_DEBUG) && defined (WIN32) && defined(_MSC_VER)
#include "crtdbg.h"
#define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW new
#endif

#ifndef RESPATH
#define RESPATH "Res"
#endif

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

// Debug options - comment/uncomment as needed
//#define DEBUG_CACHE
#ifdef _DEBUG
#if defined(WIN32) && defined(_MSC_VER)
#define AI_CHANGE_TESTING
#endif
//#define RENDER_AI_STATS
#endif

#endif
