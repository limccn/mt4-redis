#pragma once

#ifndef __AFXWIN_H__
	#error "PCH ‚"
#endif

#include "resource.h"		// 

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers


#define MT4_REDIS_ERR -1          /* Error */
#define MT4_REDIS_OK 0            /* OK */
#define MT4_REDIS_ERR_IO 1        /* error in read or write */
#define MT4_REDIS_ERR_EOF 3       /* eof */
#define MT4_REDIS_ERR_PROTOCOL 4  /* protocol error */
#define MT4_REDIS_ERR_OTHER 2     /* something else */

#define MT4_REDIS_CMD_SUCCESSED 0 /* Successed */
#define MT4_REDIS_CMD_FAILED   -1 /* Failed */

//---
#define MT4_EXPFUNC __declspec(dllexport)

//+------------------------------------------------------------------+
//| Test Redis if is working                                         |
//+------------------------------------------------------------------+
MT4_EXPFUNC int __stdcall RedisTest(const char* server, const int port);

//+------------------------------------------------------------------+
//| Excute Redis Command "GET key"                                   |
//+------------------------------------------------------------------+
MT4_EXPFUNC wchar_t* __stdcall RedisGet(const wchar_t* key);

//+------------------------------------------------------------------+
//| Excute Redis Command "GET key"                                   |
//+------------------------------------------------------------------+
MT4_EXPFUNC wchar_t* __stdcall RedisGetWithTimeout(const wchar_t* key,int timeout);

//+------------------------------------------------------------------+
//| Excute Redis Command "SET key value"                             |
//+------------------------------------------------------------------+
MT4_EXPFUNC int __stdcall RedisSet(const wchar_t* key, const wchar_t* value);

//+------------------------------------------------------------------+
//| Excute Redis Command "SET key value"                             |
//+------------------------------------------------------------------+
MT4_EXPFUNC int __stdcall RedisSetWithTimeout(const wchar_t* key, const wchar_t* value,int timeout);

//+------------------------------------------------------------------+
//| Excute Redis Command                                             |
//+------------------------------------------------------------------+
MT4_EXPFUNC int __stdcall RedisCommand(const wchar_t* command);

//+------------------------------------------------------------------+
//| Excute Redis Command                                             |
//+------------------------------------------------------------------+
MT4_EXPFUNC int __stdcall RedisCommandWithTimeout(const wchar_t* command,int timeout);

//+------------------------------------------------------------------+
//| char to WCHAR 、wchar_t、LPWSTR etc                              |
//+------------------------------------------------------------------+
static wchar_t * CStr2WStr(const char *cStr);

//+------------------------------------------------------------------+
//| WCHAR 、wchar_t、LPWSTR to char                                  |
//+------------------------------------------------------------------+
static char* WStr2CStr(const wchar_t *wchar);

//+------------------------------------------------------------------+
//| Show Messagebox                                                  |
//+------------------------------------------------------------------+
static void MT4RedisMsgBox(const wchar_t* msg);