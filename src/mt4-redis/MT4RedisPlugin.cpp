
#include "stdafx.h"
#include "MT4RedisPlugin.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include"hiredis.h"
#pragma comment(lib, "hiredis.lib")
#pragma comment(lib, "ws2_32.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define REDIS_SERVER "127.0.0.1"
#define REDIS_PORT   6379
#define CONNECT_TIMEOUT 3000
#define COMMAND_BUFFER 1024

//+------------------------------------------------------------------+
//| Excute Redis Command "GET key"                                   |
//+------------------------------------------------------------------+
MT4_EXPFUNC wchar_t* __stdcall RedisGet(const wchar_t* key)
  {
	  return RedisGetWithTimeout(key,CONNECT_TIMEOUT);
  }

//+------------------------------------------------------------------+
//| Excute Redis Command "GET key"                                   |
//+------------------------------------------------------------------+
MT4_EXPFUNC wchar_t* __stdcall RedisGetWithTimeout(const wchar_t* key,int timeout)
  {
	  	// 构造windows socket对象
		//WSADATA wsaData;
		//WSAStartup(MAKEWORD(2,1), &wsaData);

		// 设置超时
		struct timeval tv;
		tv.tv_sec = timeout/1000;
		tv.tv_usec = timeout *1000;
		redisContext* context = redisConnectWithTimeout(REDIS_SERVER,REDIS_PORT,tv);
		
		
		//判断连接是否有错误
		if (context->err) {
			printf("Failed to connect redis server[%s:%d]\n",REDIS_SERVER,REDIS_PORT); 
			redisFree(context);
			return NULL;
		}

		//构造Redis命令
		//获取size
		size_t len = wcslen(key) + 20;
		//命令长度
		if (len > COMMAND_BUFFER) {
			printf("Command is too long, Command string must short than %d",COMMAND_BUFFER); 
			return NULL;
		}
		// 构建Comand
		wchar_t command[COMMAND_BUFFER];
		//构造Redis命令
		wsprintf(command,L"GET %s",key);

		//字符串转换
		const char* command1 = WStr2CStr(command);

		//执行Redis命令
	    printf("Ready to execute command[%s]\n", command1);
		
		// 执行命令
		redisReply* reply = (redisReply*)redisCommand(context, command1);  
      
		//没有Redis响应
		if( NULL == reply)  
		{  
			printf("Failed to execute command[%s]\n",command1);    
			redisFree(context);  
			return NULL;  
		}  

		//响应状态
		if ( reply->type != REDIS_REPLY_STRING)  
		{  
			printf("Failed to execute command[%s]\n",command1);  
			freeReplyObject(reply);  
			redisFree(context);  
			return NULL;  
		}  
		printf("The value of \"%s\" is \"%s\"\n", key, reply->str);   

		//字符串转换
		wchar_t * result = CStr2WStr(reply->str);

		//释放Reply对象
		freeReplyObject(reply); 
		//释放Redis连接
		redisFree(context); 
		printf("Succeed to execute command[%s]\n", command1);
		delete command1;
		command1 = NULL;
		//返回结果字符串
		return result;
  } 

//+------------------------------------------------------------------+
//| Excute Redis Command "SET key value"                             |
//+------------------------------------------------------------------+
MT4_EXPFUNC int __stdcall RedisSet(const wchar_t* key, const wchar_t* value)
  {
	  return RedisSetWithTimeout(key, value, CONNECT_TIMEOUT);
  }

//+------------------------------------------------------------------+
//| Excute Redis Command "SET key value"                             |
//+------------------------------------------------------------------+
MT4_EXPFUNC int __stdcall RedisSetWithTimeout(const wchar_t* key, const wchar_t* value, int timeout)
  {
	    //构造Redis命令
		//获取size
		size_t len = wcslen(key)+wcslen(value) + 20;
		//命令长度
		if (len > COMMAND_BUFFER) {
			printf("Command is too long, Command string must short than %d",COMMAND_BUFFER); 
			return MT4_REDIS_CMD_FAILED;
		}
		// 构建Comand
		wchar_t command[COMMAND_BUFFER];
		//构造Redis命令
		wsprintf(command,L"SET %s %s",key,value);
	    return RedisCommandWithTimeout(command,timeout);
  }

//+------------------------------------------------------------------+
//| Excute Redis Command                                             |
//+------------------------------------------------------------------+
MT4_EXPFUNC int __stdcall RedisCommand(const wchar_t* command)
  {
	    //执行超时设置
	    return RedisCommandWithTimeout(command, CONNECT_TIMEOUT);
  }

//+------------------------------------------------------------------+
//| Excute Redis Command                                             |
//+------------------------------------------------------------------+
MT4_EXPFUNC int __stdcall RedisCommandWithTimeout(const wchar_t* command,int timeout)
  {
	    // 构造windows socket对象
		//WSADATA wsaData;
		//WSAStartup(MAKEWORD(2,1), &wsaData);

		// 设置超时
		struct timeval tv;
		tv.tv_sec = timeout/1000;
		tv.tv_usec = timeout *1000;
		redisContext* context = redisConnectWithTimeout(REDIS_SERVER,REDIS_PORT,tv);

		//判断连接是否有错误
		if (context->err) {
			printf("Failed to connect redis server[%s:%d]\n",REDIS_SERVER,REDIS_PORT); 
			redisFree(context);
			return MT4_REDIS_CMD_FAILED;
		}

		// 命令长度
		size_t len = wcslen(command) + 1;
		if (len > COMMAND_BUFFER) {
			printf("Command is too long, Command string must short than %d",COMMAND_BUFFER); 
			redisFree(context);
			return MT4_REDIS_CMD_FAILED;
		}

		//字符串装换
		const char* command1 = WStr2CStr(command);

		//执行Redis命令
	    printf("Ready to execute command[%s]\n", command1);
		
		// 执行命令
		redisReply* reply = (redisReply*)redisCommand(context, command1);  

		//没有Redis响应
		if( NULL == reply)  
		{  
			printf("Failed to execute command[%s]\n",command1);    
			redisFree(context);  
			return MT4_REDIS_CMD_FAILED;  
		}  

		//响应状态
		if( !(reply->type == REDIS_REPLY_STATUS && _stricmp(reply->str,"OK")==0))  
		{  
			printf("Failed to execute command[%s]\n",command1);  
			freeReplyObject(reply);  
			redisFree(context);  
			return MT4_REDIS_CMD_FAILED;  
		}

		//释放Reply对象
		freeReplyObject(reply); 
		//释放Redis连接
		redisFree(context);
		printf("Succeed to execute command[%s]\n", command1);
		delete command1;
		command1 = NULL;
		//返回成功状态
		return MT4_REDIS_CMD_SUCCESSED;
  }

//+------------------------------------------------------------------+
//| Test Redis if is working                                         |
//+------------------------------------------------------------------+
MT4_EXPFUNC int __stdcall RedisTest(const char* server, const int port)
{
    redisContext* c = redisConnect(server, port);  
    if ( c->err)  
    {  
        redisFree(c);  
        printf("Connect to redisServer faile\n");  
        return MT4_REDIS_CMD_FAILED;  
    }  
    printf("Connect to redisServer Success\n");  
      
    const char* command1 = "set TESTKEY TESTVALUE";  
    redisReply* r = (redisReply*)redisCommand(c, command1);  
      
    if( NULL == r)  
    {  
        printf("Execut command1 failure\n");  
        redisFree(c);  
        return MT4_REDIS_CMD_FAILED;  
    }  
    if( !(r->type == REDIS_REPLY_STATUS && _stricmp(r->str,"OK")==0))  
    {  
        printf("Failed to execute command[%s]\n",command1);  
        freeReplyObject(r);  
        redisFree(c);  
        return MT4_REDIS_CMD_FAILED;  
    }     
    freeReplyObject(r);  
    printf("Succeed to execute command[%s]\n", command1);  

	redisFree(c); 

	return 1;
}

//+------------------------------------------------------------------+
//| Show Messagebox                                                  |
//+------------------------------------------------------------------+
static void MT4RedisMsgBox(const wchar_t* msg)
{
	MessageBox(NULL,msg,L"MT4Redis",NULL);
}


//+------------------------------------------------------------------+
//| char to WCHAR 、wchar_t、LPWSTR etc                              |
//+------------------------------------------------------------------+
static wchar_t * CStr2WStr(const char *cStr)  
{     
   // MultiByteToWideChar( CP_ACP, 0, chr,  
   //     strlen(chr)+1, wchar, size/sizeof(wchar[0]) );  

	// First: get count of multi-byte string.
    const DWORD cCh = MultiByteToWideChar(CP_ACP,           // Character Page.
                                          0,                // Flag, always be 0.
                                          cStr,             // Multi-byte string.
                                          -1,               // '-1' is to determind length automatically.
                                          NULL,             // 'NULL' means ignore result. This is based
                                                            // on next argument is '0'.
                                          0);               // '0' is to get the count of character needed 
                                                            // instead of to translate.

    // Second: allocate enough memory to save result in wide character.
    wchar_t* wStr = new wchar_t[cCh];
    ZeroMemory(wStr, cCh * sizeof(wStr[0]));

    // Third: Translate it!
    MultiByteToWideChar(CP_ACP,                             // Character Page.
                        0,                                  // Flag, always be 0.
                        cStr,                               // Multi-byte string.
                        -1,                                 // Count of character of string above.
                        wStr,                               // Target wide character buffer.
                        cCh);                               // Count of character of wide character string buffer.

	return wStr;
}  


//+------------------------------------------------------------------+
//| WCHAR 、wchar_t、LPWSTR to char                                  |
//+------------------------------------------------------------------+
static char* WStr2CStr(const wchar_t *wchar)
{  
    //WideCharToMultiByte( CP_ACP, 0, wchar, -1,  
    //    chr, length, NULL, NULL );  

	const DWORD cCh = WideCharToMultiByte(CP_ACP,            
                                          0,
                                          wchar, 
                                          -1,
                                          NULL,
                                          0,
                                          NULL,             // When the wide character to be traslated
                                                            // is not in code page, proc will use this 
                                                            // variable to fill with. We usually set it
                                                            // to NULL.
                                          NULL);            // This variable will save the number of 
                                                            // character which can't be tranlated. We
                                                            // usually set it to NULL.

    // Second: allocate enough memory to save result.
    char* cStr = new char[cCh];
    ZeroMemory(cStr, cCh * sizeof(cStr[0]));

    // Third: Translate it!
    WideCharToMultiByte(CP_ACP,
                        0,
                        wchar,
                        -1,
                        cStr,
                        cCh,
                        NULL,
                        NULL);

	return cStr;
}  

//+------------------------------------------------------------------+
//| char to WCHAR 、wchar_t、LPWSTR etc                              |
//+------------------------------------------------------------------+
//static char* WStr2CStr(const wchar_t* WStr)
//  {
//	     长度设置
//		size_t len = wcslen(WStr) + 1;
//		size_t converted = 0;
//		 准备转换的对象
//		char *CStr;
//		CStr=(char*)malloc(len*sizeof(char));
//		转换
//		wcstombs_s(&converted, CStr, len, WStr, _TRUNCATE);
//		返回
//		return CStr;
//  }
//
//+------------------------------------------------------------------+
//| WCHAR 、wchar_t、LPWSTR to char                                  |
//+------------------------------------------------------------------+
//static wchar_t* CStr2WStr(const char* CStr)
//  {
//		 长度设置
//		size_t len = strlen(CStr) + 1;
//		size_t converted = 0;
//		 准备转换的对象
//		wchar_t *WStr;
//		WStr=(wchar_t*)malloc(len*sizeof(wchar_t));
//		转换
//		mbstowcs_s(&converted, WStr, len, CStr, _TRUNCATE);
//		返回
//		return WStr;
//  }