
#include "stdafx.h"
#include "CommonTool.h"


std::wstring CommonTool::StringToWstring(const std::string& str)
{
	TSDEBUG(L"Enter");

	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算   
	int iLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);   
	wchar_t* wszBuffer = new wchar_t[iLen + 1];   

	//多字节编码转换成宽字节编码   
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), wszBuffer, iLen);   
	wszBuffer[iLen] = '\0';             //添加字符串结尾   

	std::wstring wstr = wszBuffer;
	TSDEBUG(L"wstr = %s", wstr.c_str());

	delete[] wszBuffer;

	return wstr;
}

std::string CommonTool::WstringToString(const std::wstring& wstr)
{
	TSDEBUG(L"Enter, wstr = %s", wstr.c_str());

	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的   
	int iLen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);   
	char* szBuffer = new char[iLen + 1];  

	//宽字节编码转换成多字节编码   
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), szBuffer, iLen, NULL, NULL);   
	szBuffer[iLen] = '\0'; 

	std::string str = szBuffer;

	delete[] szBuffer;  

	return str; 
}

long CommonTool::ParseHeaderFromData(const char* szData, unsigned char &ucFlags, unsigned long &ulType, unsigned long &ulDataLen, LPSOCKETIPC_STREAM lpSocketIpcStream)
{
	TSDEBUG(L"Enter");

	LPSOCKETIPC_DATA_HEADER lpSocketIpcDataHeader = (LPSOCKETIPC_DATA_HEADER)szData;
	ucFlags = lpSocketIpcDataHeader->ucFlags;
	ulType = lpSocketIpcDataHeader->ulType;
	ulDataLen = lpSocketIpcDataHeader->ulDataLen;
	TSDEBUG(L"ucFlags = %u, ulType = %u, ulDataLen = %u", ucFlags, ulType, ulDataLen);

	int iHeaderSize = sizeof(SOCKETIPC_DATA_HEADER);
	TSDEBUG(L"iHeaderSize = %d", iHeaderSize);

	long lRet = StreamWriteString(lpSocketIpcStream, &(szData[iHeaderSize]));
	lpSocketIpcStream->uiCurPos = 0;

	return lRet;
}

long CommonTool::CombineSocketIpcData(const unsigned char &ucFlag, const unsigned long &ulType, LPSOCKETIPC_STREAM lpSocketIpcStream, char* szSocketIpcData)
{
	TSDEBUG(L"Enter");

	if (NULL == szSocketIpcData)
	{
		//计算所需大小
		long lNeedLen = sizeof(SOCKETIPC_DATA_HEADER) + lpSocketIpcStream->uiCurPos;
		TSDEBUG(L"lNeedLen = %d", lNeedLen);
		return lNeedLen;
	}

	//协议头
	LPSOCKETIPC_DATA_HEADER lpSocketIpcDataHeader = (LPSOCKETIPC_DATA_HEADER)szSocketIpcData;
	memset(lpSocketIpcDataHeader, 0, sizeof(SOCKETIPC_DATA_HEADER));
	lpSocketIpcDataHeader->ucFlags = ucFlag;
	lpSocketIpcDataHeader->ulType = ulType;
	lpSocketIpcDataHeader->ulDataLen = lpSocketIpcStream->uiCurPos;
	//数据
	int iHeaderSize = sizeof(SOCKETIPC_DATA_HEADER);
	TSDEBUG(L"iHeaderSize = %d, uiCurPost = %u", iHeaderSize, lpSocketIpcStream->uiCurPos);
	memcpy(&(szSocketIpcData[iHeaderSize]), lpSocketIpcStream->szBuffer, lpSocketIpcStream->uiCurPos);

	return 0;
}

LPSOCKETIPC_STREAM CommonTool::CreateSocketIpcStream()
{
	TSDEBUG(L"Enter");

	LPSOCKETIPC_STREAM lpSocketIpcStream = new SOCKETIPC_STREAM;
	lpSocketIpcStream->szBuffer = new char[IPCSTREAM_SIZE];
	lpSocketIpcStream->uiTotalSize = IPCSTREAM_SIZE;
	lpSocketIpcStream->uiCurPos = 0;
	lpSocketIpcStream->uiUseSize = 0;

	return lpSocketIpcStream;
}


long CommonTool::StreamWriteBytes(LPSOCKETIPC_STREAM lpSocketIpcStream, const BYTE* lpBytes, int iLen)
{
	TSDEBUG(L"Enter");

	if (NULL == lpBytes)
	{
		TSDEBUG(L"Invalided param, lpBytes = NULL");
		return -1;
	}
	TSDEBUG(L"iLen = %d", iLen);

	if (lpSocketIpcStream->uiCurPos + iLen + 2 > lpSocketIpcStream->uiTotalSize)
	{
		lpSocketIpcStream->uiTotalSize = ((lpSocketIpcStream->uiCurPos + iLen + 2) / IPCSTREAM_SIZE + 1) * IPCSTREAM_SIZE;
		TSDEBUG(L"lpSocketIpcStream->uiBufferSize = %u", lpSocketIpcStream->uiTotalSize);

		char* szNewBuffer = new char[lpSocketIpcStream->uiTotalSize];
		memcpy(szNewBuffer, lpSocketIpcStream->szBuffer, lpSocketIpcStream->uiCurPos);
		delete lpSocketIpcStream->szBuffer;
		lpSocketIpcStream->szBuffer = szNewBuffer;
	}

	memcpy(&(lpSocketIpcStream->szBuffer[lpSocketIpcStream->uiCurPos]), lpBytes, iLen);
	lpSocketIpcStream->uiCurPos += iLen;
	lpSocketIpcStream->szBuffer[lpSocketIpcStream->uiCurPos] = '\r';
	lpSocketIpcStream->uiCurPos += 1;
	lpSocketIpcStream->szBuffer[lpSocketIpcStream->uiCurPos] = '\n';
	lpSocketIpcStream->uiCurPos += 1;
	TSDEBUG(L"uiCurPos = %u", lpSocketIpcStream->uiCurPos);

	return 0;
}

long CommonTool::StreamWriteString(LPSOCKETIPC_STREAM lpSocketIpcStream, const char* szData)
{
	TSDEBUG(L"Enter");

	if (NULL == szData)
	{
		TSDEBUG(L"Invalided param, szData = NULL");
		return -1;
	}

	int iDataLen = strlen(szData);
	TSDEBUG(L"iDataLen = %d, uiCurPos = %u, uiTotalSize = %u", iDataLen, lpSocketIpcStream->uiCurPos, lpSocketIpcStream->uiTotalSize);

	return StreamWriteBytes(lpSocketIpcStream, (BYTE*)szData, iDataLen);
}

long CommonTool::StreamWriteWString(LPSOCKETIPC_STREAM lpSocketIpcStream, const wchar_t* wszData)
{
	TSDEBUG(L"Enter");

	std::string strData = WstringToString(wszData);
	return StreamWriteString(lpSocketIpcStream, strData.c_str());
}

long CommonTool::StreamReadBytes(LPSOCKETIPC_STREAM lpSocketIpcStream, BYTE* lpBytes, int iLen)
{
	TSDEBUG(L"Enter");

	if (NULL == lpBytes)
	{
		std::string str = lpSocketIpcStream->szBuffer;
		std::size_t uiPos = str.find("\r\n", lpSocketIpcStream->uiCurPos);
		TSDEBUG(L"str: size = %d, length = %d, uiPos = %d", str.size(), str.length(), uiPos);
		if (std::string::npos == uiPos)
		{
			TSDEBUG(L"Can not find decollator");
			return -1;
		}

		return uiPos - lpSocketIpcStream->uiCurPos;
	}

	memcpy(lpBytes, &(lpSocketIpcStream->szBuffer[lpSocketIpcStream->uiCurPos]), iLen);
	lpSocketIpcStream->uiCurPos += iLen;
	lpSocketIpcStream->uiCurPos += 2;

	return 0;
}

long CommonTool::StreamReadString(LPSOCKETIPC_STREAM lpSocketIpcStream, char* szData, int iLen)
{
	TSDEBUG(L"Enter");

	if (NULL == szData)
	{
		long lLen = StreamReadBytes(lpSocketIpcStream, (BYTE*)szData, iLen);
		return lLen + 1;
	}
	else
	{
		long lRet = StreamReadBytes(lpSocketIpcStream, (BYTE*)szData, iLen - 1);
		if (0 == lRet)
		{
			szData[iLen - 1] = '\0';
		}
		
		return lRet;
	}

	return StreamReadBytes(lpSocketIpcStream, (BYTE*)szData, iLen);
}

long CommonTool::StreamReadWString(LPSOCKETIPC_STREAM lpSocketIpcStream, wchar_t* wszData, int iLen)
{
	TSDEBUG(L"Enter");

	if (NULL == wszData)
	{
		return StreamReadString(lpSocketIpcStream, NULL, iLen);
	}
	else
	{
		char* szData = new char[iLen];
		long lRet = StreamReadString(lpSocketIpcStream, szData, iLen);
		if (0 != lRet)
		{
			return lRet;
		}

		std::wstring wstrData = StringToWstring(szData);
		memcpy(wszData, wstrData.c_str(), iLen * 2);
		
		delete [] szData;
		szData = NULL;
	}

	return 0;
}