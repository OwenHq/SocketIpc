
#pragma once
#include <DbgHelp.h>
#include <vector>
#include "CommonDefine.h"
#include <Psapi.h>

// ���Thunder.exe�Ľ���ID�����߳�ID
long GetProcessId(const wchar_t* wszProcessName, DWORD &dwProcessId);

// ͨ������Զ���߳�ע��DLL
long InjectDllByCreateRemoteThread(HANDLE hProcess, const wchar_t* wszDllPath);

// ���MiniDump
long WriteMiniDumpFile(HANDLE hProcess, DWORD dwProcessId, MINIDUMP_TYPE minidumpType, const wchar_t* wszFilePath);

// ѹ���ļ�
long CompressFiles(const wchar_t* wszDesFilePath, const std::vector<std::wstring>& vFileList);

// �жϴ����Ƿ�������Ѹ��7�򴫣�XLUEFrameHostWnd, Ѹ��7
long IsHungWindow(const wchar_t* wszClassName, const wchar_t* wszWindowName, BOOL &bRet);

// ʱ��ת������FILETIMEת��ΪULONGLONG
ULONGLONG FileTime2ULONGLONG(const FILETIME* pFileTime);

// ��ý���CPUռ���ʣ���λ��%����ע�����һ��ʹ�û᷵�ش�����XLDIAGTOOL_FAILED_HAVENOTINIT
long GetProcessCpuUsage(HANDLE hProcess, int &iCpuUsage);

// ��ý����ڴ�ռ�ã���λ���ֽڣ�
long GetProcessMemoryUsage(HANDLE hProcess, ULONGLONG &ullWorkingSetSize, ULONGLONG &ullPageFileUsage);

// ���Thunder�汾��
long GetThunderVersion(std::wstring& wstrVersion);

// ���Thunder��װĿ¼
long GetThunderInstDir(std::wstring& wstrInstDir);

// �򿪲�������Ϣ��TXT�ļ�
long SaveInfoToFile(const wchar_t* wszFilePath, const char* szData);