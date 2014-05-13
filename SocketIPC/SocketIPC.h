
#pragma once

//定义实例
typedef void* SIPC_INSTANCE;
typedef void* SIPC_STREAM;

//错误码
#define SIPC_SUCC	0	//成功

//服务端：创建IPC服务端
SIPC_INSTANCE SIPC_CreateIpcServer();
//服务端：删除IPC服务端
long SIPC_DeleteIpcServer(SIPC_INSTANCE hSIpcInstance);
//服务端：初始化IPC服务端
long SIPC_InitIpcServer(SIPC_INSTANCE hSIpcInstance);
//服务端：监听
typedef long (__stdcall *LISTENCALLBACK)(DWORD dwErrCode, void* pUserData);
long SIPC_ListenClient(SIPC_INSTANCE hSIpcInstance, LISTENCALLBACK pfnOnListenCallback, void* pUserData);

//客户端：创建IPC客户端
SIPC_INSTANCE SIPC_CreateIpcClient();
//客户端：删除IPC服务端
long SIPC_DeleteIpcClient(SIPC_INSTANCE hSIpcInstance);
//客户端：初始化IPC客户端
long SIPC_InitIpcClient(SIPC_INSTANCE hSIpcInstance);
//客户端：连接
typedef long (__stdcall *CONNECTCALLBACK)(DWORD dwErrCode, void* pUserData);
long SIPC_ConnectServer(SIPC_INSTANCE hSIpcInstance, CONNECTCALLBACK pfnOnConnectCallback, void* pUserData);

//发送
typedef long (__stdcall *SENDCALLBACK)(DWORD dwErrCode, DWORD dwId, void* pUserData);
long SIPC_Send(SIPC_INSTANCE hSIpcInstance, DWORD dwId, SIPC_STREAM hSIpcStream, SENDCALLBACK pfnOnSendCallback, void* pUserData);

//接收
typedef long (__stdcall *RECVCALLBACK)(DWORD dwErrCode, DWORD dwId, SIPC_STREAM hSIpcStream, void* pUserData);
long SIPC_RegisterRecvCallback(SIPC_INSTANCE hSIpcInstance, RECVCALLBACK pfnOnRecvCallback, void* pUserData);

//断开回调
typedef long (__stdcall *DISLINKCALLBACK)(DWORD dwErrCode, void* pUserData);
long SIPC_RegisterDislinkCallback(SIPC_INSTANCE hSIpcInstance, DISLINKCALLBACK pfnDisconnectCallback, void* pUserData);

//创建SocketIPC数据流对象
SIPC_STREAM SIPC_CreateSocketIpcStream();

//写入bytes到SocketIPC数据流
long SIPC_StreamWriteBytes(SIPC_STREAM hSIpcStream, const BYTE* lpBytes, int iLen);
//写入string到SocketIPC数据流
long SIPC_StreamWriteString(SIPC_STREAM hSIpcStream, const char* szData);
//写入wstring到SocketIPC数据流
long SIPC_StreamWriteWString(SIPC_STREAM hSIpcStream, const wchar_t* wszData);

//从SocketIPC数据流中读取bytes
long SIPC_StreamReadBytes(SIPC_STREAM hSIpcStream, BYTE* lpBytes, int iLen);
//从SocketIPC数据流中读取string
long SIPC_StreamReadString(SIPC_STREAM hSIpcStream, char* szData, int iLen);
//从SocketIPC数据流中读取wstring
long SIPC_StreamReadWString(SIPC_STREAM hSIpcStream, wchar_t* wszData, int iLen);
