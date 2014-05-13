
#pragma once
#include <DbgHelp.h>
#include <vector>
#include "CommonDefine.h"
#include <Psapi.h>

// 获得Thunder.exe的进程ID、主线程ID
long GetProcessId(const wchar_t* wszProcessName, DWORD &dwProcessId);

// 通过创建远程线程注入DLL
long InjectDllByCreateRemoteThread(HANDLE hProcess, const wchar_t* wszDllPath);

// 获得MiniDump
long WriteMiniDumpFile(HANDLE hProcess, DWORD dwProcessId, MINIDUMP_TYPE minidumpType, const wchar_t* wszFilePath);

// 压缩文件
long CompressFiles(const wchar_t* wszDesFilePath, const std::vector<std::wstring>& vFileList);

// 判断窗口是否卡死，如迅雷7则传：XLUEFrameHostWnd, 迅雷7
long IsHungWindow(const wchar_t* wszClassName, const wchar_t* wszWindowName, BOOL &bRet);

// 时间转换：将FILETIME转换为ULONGLONG
ULONGLONG FileTime2ULONGLONG(const FILETIME* pFileTime);

// 获得进程CPU占用率（单位：%），注：如第一次使用会返回错误码XLDIAGTOOL_FAILED_HAVENOTINIT
long GetProcessCpuUsage(HANDLE hProcess, int &iCpuUsage);

// 获得进程内存占用（单位：字节）
long GetProcessMemoryUsage(HANDLE hProcess, ULONGLONG &ullWorkingSetSize, ULONGLONG &ullPageFileUsage);

// 获得Thunder版本号
long GetThunderVersion(std::wstring& wstrVersion);

// 获得Thunder安装目录
long GetThunderInstDir(std::wstring& wstrInstDir);

// 打开并保存信息到TXT文件
long SaveInfoToFile(const wchar_t* wszFilePath, const char* szData);