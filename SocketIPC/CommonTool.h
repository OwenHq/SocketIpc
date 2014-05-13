
#pragma once
#include <string>
#include "../XlDiagTool/Common/tslog.h"
#include "SocketIPCDefine.h"

class CommonTool
{
public:
	//stringת��Ϊwstring
	static std::wstring StringToWstring(const std::string& str);
	//wstringת��Ϊstring
	static std::string WstringToString(const std::wstring& wstr);

	//��SocketIPC�����н��ͳ�ucFlags��ulType
	static long ParseHeaderFromData(const char* szData, unsigned char &ucFlags, unsigned long &ulType, unsigned long &ulDataLen, LPSOCKETIPC_STREAM lpSocketIpcStream);
	//���SocketIPC����
	static long CombineSocketIpcData(const unsigned char &ucFlag, const unsigned long &ulType, LPSOCKETIPC_STREAM lpSocketIpcStream, char* szSocketIpcData);

	//����SocketIPC����������
	static LPSOCKETIPC_STREAM CreateSocketIpcStream();

	//д��bytes��SocketIPC����������\r\nΪ�ָ�
	static long StreamWriteBytes(LPSOCKETIPC_STREAM lpSocketIpcStream, const BYTE* lpBytes, int iLen);
	//д��string��SocketIPC����������\r\nΪ�ָ�
	static long StreamWriteString(LPSOCKETIPC_STREAM lpSocketIpcStream, const char* szData);
	//д��wstring��SocketIPC����������\r\nΪ�ָ�
	static long StreamWriteWString(LPSOCKETIPC_STREAM lpSocketIpcStream, const wchar_t* wszData);

	//��SocketIPC�������ж�ȡbytes
	static long StreamReadBytes(LPSOCKETIPC_STREAM lpSocketIpcStream, BYTE* lpBytes, int iLen);
	//��SocketIPC�������ж�ȡstring
	static long StreamReadString(LPSOCKETIPC_STREAM lpSocketIpcStream, char* szData, int iLen);
	//��SocketIPC�������ж�ȡwstring
	static long StreamReadWString(LPSOCKETIPC_STREAM lpSocketIpcStream, wchar_t* wszData, int iLen);
};

