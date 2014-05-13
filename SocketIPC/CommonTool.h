
#pragma once
#include <string>
#include "../XlDiagTool/Common/tslog.h"
#include "SocketIPCDefine.h"

class CommonTool
{
public:
	//string转换为wstring
	static std::wstring StringToWstring(const std::string& str);
	//wstring转换为string
	static std::string WstringToString(const std::wstring& wstr);

	//从SocketIPC数据中解释出ucFlags和ulType
	static long ParseHeaderFromData(const char* szData, unsigned char &ucFlags, unsigned long &ulType, unsigned long &ulDataLen, LPSOCKETIPC_STREAM lpSocketIpcStream);
	//组合SocketIPC数据
	static long CombineSocketIpcData(const unsigned char &ucFlag, const unsigned long &ulType, LPSOCKETIPC_STREAM lpSocketIpcStream, char* szSocketIpcData);

	//创建SocketIPC数据流对象
	static LPSOCKETIPC_STREAM CreateSocketIpcStream();

	//写入bytes到SocketIPC数据流，以\r\n为分割
	static long StreamWriteBytes(LPSOCKETIPC_STREAM lpSocketIpcStream, const BYTE* lpBytes, int iLen);
	//写入string到SocketIPC数据流，以\r\n为分割
	static long StreamWriteString(LPSOCKETIPC_STREAM lpSocketIpcStream, const char* szData);
	//写入wstring到SocketIPC数据流，以\r\n为分割
	static long StreamWriteWString(LPSOCKETIPC_STREAM lpSocketIpcStream, const wchar_t* wszData);

	//从SocketIPC数据流中读取bytes
	static long StreamReadBytes(LPSOCKETIPC_STREAM lpSocketIpcStream, BYTE* lpBytes, int iLen);
	//从SocketIPC数据流中读取string
	static long StreamReadString(LPSOCKETIPC_STREAM lpSocketIpcStream, char* szData, int iLen);
	//从SocketIPC数据流中读取wstring
	static long StreamReadWString(LPSOCKETIPC_STREAM lpSocketIpcStream, wchar_t* wszData, int iLen);
};

