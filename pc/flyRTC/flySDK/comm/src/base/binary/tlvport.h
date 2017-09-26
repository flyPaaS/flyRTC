#pragma once

// linux platform
//#ifdef linux
#include "iLog.h"
#include "iErrno.h"
#include "iQsUin.h"

#if 0
#define TLV_LOG_ERR       LogErr
#define TLV_LOG_DEBUG     LogDebug
#define TLV_LOG_IN        LogInternal
#endif

#define TLV_WRITE_TO_FILE WriteToFile

#define TLV_LOG_ERR       printf
#define TLV_LOG_DEBUG     printf
#define TLV_LOG_IN        printf


//#endif

// iphone/osx platform
#ifdef MACOS
#include "iErrno.h"
#include <string.h>


#include "iFile.h"

#define TLV_LOG_ERR       
#define TLV_LOG_DEBUG     
#define TLV_LOG_IN
inline int WriteToFile(const char* pFile, const char* pInput, int nLen)
{
		return -1;
}
#define TLV_WRITE_TO_FILE WriteToFile
#endif

// S60 platform
#ifdef __SYMBIAN32__
#include "iErrno.h"
#include <string.h>
#include <stdlib.h>

#define TLV_LOG_ERR       
#define TLV_LOG_DEBUG     
#define TLV_LOG_IN
inline int WriteToFile(const char* pFile, const char* pInput, int nLen)
{
		return -1;
}

typedef unsigned long long uint64_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#define TLV_WRITE_TO_FILE WriteToFile
#endif


#ifdef android
#include "iErrno.h"
#include <string.h>

#define TLV_LOG_ERR       
#define TLV_LOG_DEBUG     
#define TLV_LOG_IN
inline int WriteToFile(const char* pFile, const char* pInput, int nLen)
{
		return -1;
}
#define TLV_WRITE_TO_FILE WriteToFile
#endif

