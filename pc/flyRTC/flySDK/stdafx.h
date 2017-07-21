// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#include <atlbase.h>
#include <atlstr.h>

// TODO: reference additional headers your program requires here
#include "./comm/inc/MediaEngineInterface.h"
#include "./comm/inc/cfg.h"
#include "./ViGo/inc/interface/ViGoEngine.h"

#include "UGo.h"
#include "ugo_test.h"
#include "ugo_auto_test.h"

using namespace gl_media_engine;
//#pragma comment(lib, "../output/ViGo_deb.lib")
//#pragma comment(lib, "../output/UGo_deb.lib")
#pragma comment(lib, "../output/ViGo_res.lib")
#pragma comment(lib, "../output/UGo_res.lib")

#include "../json/exception.h"
#include "../json/reader.h"
#include "../json/json.h"
#pragma comment(lib, "../output/json.lib")

#include "../bson/json.h"
#include "../bson/bsonobjbuilder.h"
#pragma comment(lib, "../output/bson.lib")
using namespace std;
using namespace _bson;

#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
//////////////////////////////////////////////////////////////////////////
#include "Base.h"
#include "flySDK.h"

// UNCODE和UTF8转换
CString StringA2String(CStringA str);
CStringA String2StringA(CString str);
// UNICODE和ANSI转换
CString StringAToString(CStringA str);
CStringA StringToStringA(CString str);