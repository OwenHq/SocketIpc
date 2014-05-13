
#include "stdafx.h"
#include "Common.h"
#include <TlHelp32.h>


long GetProcessId(const wchar_t* wszProcessName, DWORD &dwProcessId)
{
	TSDEBUG(L"Enter");

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"CreateToolhelp32Snapshot failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	BOOL bRet = ::Process32First(hProcessSnap, &pe32);
	if (!bRet)
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"Process32First failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	while(bRet)
	{
		//判断是否为Thunder.exe
		TSDEBUG(L"pe32.szExeFile = %s", pe32.szExeFile);
		if (0 == wcsicmp(pe32.szExeFile, wszProcessName))
		{
			dwProcessId = pe32.th32ProcessID;
			TSDEBUG(L"dwProcessId = %u", dwProcessId);
			::CloseHandle(hProcessSnap);
			return XLDIAGTOOL_SUCC;
		}

		bRet = ::Process32Next(hProcessSnap, &pe32);
	}
	::CloseHandle(hProcessSnap);

	return XLDIAGTOOL_FAILED_CANNOTFINDPROCESS;
}

long InjectDllByCreateRemoteThread(HANDLE hProcess, const wchar_t* wszDllPath)
{
	TSDEBUG(L"Enter, wszDllPath = %s", wszDllPath);

	//计算DLL路径的字节长度
	int iLen = (wcslen(wszDllPath) + 1) * sizeof(wchar_t);
	TSDEBUG(L"iLen = %d", iLen);

	//在进程中为DLL路径分配地址空间
	wchar_t *pwszRemoteMemory = (wchar_t*)::VirtualAllocEx(hProcess, NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
	if (pwszRemoteMemory == NULL)
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"VirtualAllocEx failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	//复制DLL路径到新分配的地址空间中
	if (!::WriteProcessMemory(hProcess, pwszRemoteMemory, (void*)wszDllPath, iLen, NULL))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"WriteProcessMemory failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	//获得LoadLibraryW在Kernel32.dll中的地址
	PTHREAD_START_ROUTINE pfnLoadLibraryW = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(TEXT("Kernel32.dll")), "LoadLibraryW");
	if (pfnLoadLibraryW == NULL)
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"GetProcAddress failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	//创建远程线程，执行加载DLL操作
	HANDLE hRemoteThread = ::CreateRemoteThread(hProcess, NULL, 0, pfnLoadLibraryW, pwszRemoteMemory, 0, NULL);
	if (hRemoteThread == NULL)
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"CreateRemoteThread failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	//等待远程线程结束
	::WaitForSingleObject(hRemoteThread, INFINITE);

	//清理工作
	if (pwszRemoteMemory)
	{
		::VirtualFreeEx(hProcess, pwszRemoteMemory, 0, MEM_RELEASE);
	}
	if (hRemoteThread)
	{
		::CloseHandle(hRemoteThread);
	}

	return XLDIAGTOOL_SUCC;
}

long WriteMiniDumpFile(HANDLE hProcess, DWORD dwProcessId, MINIDUMP_TYPE minidumpType, const wchar_t* wszFilePath)
{
	TSDEBUG(L"Enter");

	if (NULL == wszFilePath)
	{
		TSDEBUG(L"wszFilePath is NULL");
		return XLDIAGTOOL_FAILED_INVALIDPARAM;
	}

	HANDLE hFile = ::CreateFile(wszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"CreateFile failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	HMODULE hDbgHelp = ::LoadLibrary(L"dbghelp.dll");
	if (hDbgHelp == NULL)
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"LoadLibrary failed, error code is %u", dwErrCode);
		return dwErrCode;
	}
	typedef BOOL (WINAPI *LPMINIDUMPWRITEDUMP)(
		HANDLE hProcess, 
		DWORD ProcessId, 
		HANDLE hFile, 
		MINIDUMP_TYPE DumpType, 
		CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, 
		CONST PMINIDUMP_USER_STREAM_INFORMATION UserEncoderParam, 
		CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);
	LPMINIDUMPWRITEDUMP pfnMiniDumpWriteDump = (LPMINIDUMPWRITEDUMP)::GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
	if (pfnMiniDumpWriteDump == NULL)
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"GetProcAddress failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	if (!pfnMiniDumpWriteDump(hProcess, dwProcessId, hFile, minidumpType, NULL, NULL, NULL))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"MiniDumpWriteDump failed, error code is %u", dwErrCode);
		::CloseHandle(hFile);
		return dwErrCode;
	}
	::CloseHandle(hFile);

	return XLDIAGTOOL_SUCC;
}

long CompressFiles(const wchar_t* wszDesFilePath, const std::vector<std::wstring>& vFileList)
{
	TSDEBUG(L"Enter");

	return XLDIAGTOOL_SUCC;
}

long IsHungWindow(const wchar_t* wszClassName, const wchar_t* wszWindowName, BOOL &bRet)
{
	TSDEBUG(L"Enter");

	if (wszClassName == NULL && wszWindowName == NULL)
	{
		TSDEBUG(L"Both wszClassName and wszWindowName are NULL");
		return XLDIAGTOOL_FAILED_INVALIDPARAM;
	}

	HWND hWnd = ::FindWindow(wszClassName, wszWindowName);
	if (NULL == hWnd)
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"FindWindow failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	DWORD dwSendResult = 0;
	LRESULT hr = ::SendMessageTimeout(hWnd, WM_NULL, NULL, NULL, SMTO_BLOCK, 2000, &dwSendResult);
	TSDEBUG(L"hr = %d", hr);
	if (SUCCEEDED(hr))
	{
		bRet = FALSE;
		return XLDIAGTOOL_SUCC;
	}
	else if(::GetLastError() == ERROR_TIMEOUT)
	{
		bRet = TRUE;
		return XLDIAGTOOL_SUCC;
	}
	else
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"SendMessageTimeout failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	return XLDIAGTOOL_SUCC;
}

ULONGLONG FileTime2ULONGLONG(const FILETIME* pFileTime)
{
	TSDEBUG(L"Enter");

	LARGE_INTEGER li;
	li.HighPart = pFileTime->dwHighDateTime;
	li.LowPart = pFileTime->dwLowDateTime;
	return li.QuadPart;
}

long GetProcessCpuUsage(HANDLE hProcess, int &iCpuUsage)
{
	TSDEBUG(L"Enter");

	//cpu数量 
	static int siProcessorCount = 0;	//CPU个数
	static ULONGLONG sullLastTime = 0;		//上一次系统时间
	static ULONGLONG sullLastProcessTotalTimePerProcessor = 0;	//上一次进程平均每个CPU的总耗时

	FILETIME filetimeNow;	//当前系统时间
	FILETIME filetimeProcessCreation;	//进程创建时间
	FILETIME filetimeProcessExit;	//进程退出时间
	FILETIME filetimeProcessKernel;		//进程内核时间
	FILETIME filetimeProcessUser;	//进程用户态时间
	ULONGLONG ullProcessTotalTimePerProcessor;	//进程内核及用户态平均每个CPU总耗时
	ULONGLONG ullTimeNow;		//当前时间
	ULONGLONG ullProcessTotalTimePerProcessorDelta;	//进程内核及用户态平均每个CPU总耗时增量
	ULONGLONG ullTimeDelta;	//当前时间增量

	int cpu = -1; 

	if(siProcessorCount == 0) 
	{ 
		SYSTEM_INFO systemInfo;
		::GetSystemInfo(&systemInfo);
		siProcessorCount = (int)systemInfo.dwNumberOfProcessors;
		TSDEBUG(L"siProcessorCount = %d", siProcessorCount);
	} 

	//算当前时间
	::GetSystemTimeAsFileTime(&filetimeNow); 
	if (!GetProcessTimes(hProcess, &filetimeProcessCreation, &filetimeProcessExit, &filetimeProcessKernel, &filetimeProcessUser)) 
	{ 
		/*
			We don’t assert here because in some cases (such as in the Task Manager) 
			we may call this function on a process that has just exited but we have 
			not yet received the notification.
		*/ 
		iCpuUsage = 0;
		return XLDIAGTOOL_FAILED; 
	} 
	ullProcessTotalTimePerProcessor = (FileTime2ULONGLONG(&filetimeProcessKernel) + FileTime2ULONGLONG(&filetimeProcessUser)) / siProcessorCount; 
	ullTimeNow = FileTime2ULONGLONG(&filetimeNow); 

	if ((sullLastProcessTotalTimePerProcessor == 0) || (sullLastTime == 0)) 
	{ 
		// First call, just set the last values. 
		sullLastProcessTotalTimePerProcessor = ullProcessTotalTimePerProcessor; 
		sullLastTime = ullTimeNow; 
		return XLDIAGTOOL_FAILED_HAVENOTINIT;
	} 

	//算增量
	ullProcessTotalTimePerProcessorDelta = ullProcessTotalTimePerProcessor - sullLastProcessTotalTimePerProcessor; 
	ullTimeDelta = ullTimeNow - sullLastTime; 


	if (ullTimeDelta == 0) 
	{
		TSDEBUG(L"ullTimeDelta = 0");
		return XLDIAGTOOL_FAILED; 
	}
	// We add time_delta / 2 so the result is rounded. 
	iCpuUsage = (int)((ullProcessTotalTimePerProcessorDelta * 100 + ullTimeDelta / 2) / ullTimeDelta); 
	sullLastProcessTotalTimePerProcessor = ullProcessTotalTimePerProcessor; 
	sullLastTime = ullTimeNow; 
	TSDEBUG(L"iCpuUsage = %d", iCpuUsage);

	return XLDIAGTOOL_SUCC;
}

long GetProcessMemoryUsage(HANDLE hProcess, ULONGLONG &ullWorkingSetSize, ULONGLONG &ullPageFileUsage)
{
	TSDEBUG(L"Enter");

	PROCESS_MEMORY_COUNTERS pmc;
	if (!::GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"GetProcessMemoryInfo failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	ullWorkingSetSize = pmc.WorkingSetSize;
	ullPageFileUsage = pmc.PagefileUsage;

	return XLDIAGTOOL_SUCC;
}

long GetThunderVersion(std::wstring& wstrVersion)
{
	TSDEBUG(L"Enter");

	//获得安装迅雷的版本号
	HKEY hKey;
	long lRet = ::RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Thunder Network\\ThunderOem\\thunder_backwnd", &hKey);
	if (ERROR_SUCCESS != lRet)
	{
		TSDEBUG(L"RegOpenKey failed, error code is %d", lRet);
		return lRet;
	}

	DWORD dwSize = 0;
	DWORD dwType = REG_EXPAND_SZ;
	lRet = ::RegQueryValueEx(hKey, L"Version", NULL, &dwType, NULL, &dwSize);
	if (ERROR_SUCCESS != lRet)
	{
		TSDEBUG(L"RegQueryValueEx dwType failed, error code is %d", lRet);
		::RegCloseKey(hKey);
		return lRet;
	}

	TCHAR* pszBuffer = (TCHAR*)(new BYTE[dwSize]);
	lRet = ::RegQueryValueEx(hKey, L"Version", NULL, &dwType, (BYTE*)pszBuffer, &dwSize);
	if (ERROR_SUCCESS != lRet)
	{
		TSDEBUG(L"RegQueryValueEx failed, error code is %d", lRet);
		::RegCloseKey(hKey);
		delete [] pszBuffer;
		return lRet;
	}

	wstrVersion = pszBuffer;
	::RegCloseKey(hKey);
	delete [] pszBuffer;

	return XLDIAGTOOL_SUCC;
}

long GetThunderInstDir(std::wstring& wstrInstDir)
{
	TSDEBUG(L"Enter");

	//获得安装迅雷的版本号
	HKEY hKey;
	long lRet = ::RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Thunder Network\\ThunderOem\\thunder_backwnd", &hKey);
	if (ERROR_SUCCESS != lRet)
	{
		TSDEBUG(L"RegOpenKey failed, error code is %d", lRet);
		return lRet;
	}

	DWORD dwSize = 0;
	DWORD dwType = REG_EXPAND_SZ;
	lRet = ::RegQueryValueEx(hKey, L"instdir", NULL, &dwType, NULL, &dwSize);
	if (ERROR_SUCCESS != lRet)
	{
		TSDEBUG(L"RegQueryValueEx dwType failed, error code is %d", lRet);
		::RegCloseKey(hKey);
		return lRet;
	}

	TCHAR* pszBuffer = (TCHAR*)(new BYTE[dwSize]);
	lRet = ::RegQueryValueEx(hKey, L"instdir", NULL, &dwType, (BYTE*)pszBuffer, &dwSize);
	if (ERROR_SUCCESS != lRet)
	{
		TSDEBUG(L"RegQueryValueEx failed, error code is %d", lRet);
		::RegCloseKey(hKey);
		delete [] pszBuffer;
		return lRet;
	}

	wstrInstDir = pszBuffer;
	wstrInstDir = wstrInstDir + L"\\";
	::RegCloseKey(hKey);
	delete [] pszBuffer;

	return XLDIAGTOOL_SUCC;
}

long SaveInfoToFile(const wchar_t* wszFilePath, const char* szData)
{
	TSDEBUG(L"Enter");

	if (NULL == wszFilePath || NULL == szData)
	{
		TSDEBUG(L"Invalided param");
		return XLDIAGTOOL_FAILED_INVALIDPARAM;
	}
	TSDEBUG(L"wszFilePath = %s", wszFilePath);

	HANDLE hFile = ::CreateFile(wszFilePath, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"CreateFile failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	DWORD dwNunberOfBytesWritten;
	DWORD dwDataLen = strlen(szData);
	if (!::WriteFile(hFile, (void*)szData, dwDataLen, &dwNunberOfBytesWritten, NULL))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"WriteFile failed, error code is %u", dwErrCode);
		return dwErrCode;
	}
	if (dwNunberOfBytesWritten != dwDataLen)
	{
		TSDEBUG(L"dwNumberOfBytesWritten != dwDataLen");
		return XLDIAGTOOL_FAILED_WRITEFILE;
	}

	::CloseHandle(hFile);

	return XLDIAGTOOL_SUCC;
}