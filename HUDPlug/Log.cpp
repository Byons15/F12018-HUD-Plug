#include "Log.h"
#include "StdAfx.h"
#include "Log.h"
#include <Windows.h>

CSimpleLog	Log;
const char* g_pLogPath = "lo.log";
CSimpleLog::CSimpleLog(void)
{
}

CSimpleLog::~CSimpleLog(void)
{
}

void CSimpleLog::Write(const char* pSourcePath, const char* pFunName, \
	const long lLine, const char* pLogText)
{
	if (pLogText == NULL)
		return;
	int nLogLen = strlen(pLogText);
	if (nLogLen == 0)
		return;
	int nSourceLen = strlen(pSourcePath);
	int nFunLen = strlen(pFunName);
	char szLine[10] = { 0 };
	sprintf_s(szLine, "%ld", lLine);
	int nLineLen = strlen(szLine);
	int nSpaceLen = 80 - nSourceLen - nFunLen - nLineLen;
	string strTime = GetTime();
	FILE* fp = NULL;
	fopen_s(&fp, g_pLogPath, "a+");
	fwrite(strTime.c_str(), strTime.size(), 1, fp);
	fwrite(" ", 1, 1, fp);
	fwrite(pSourcePath, nSourceLen, 1, fp);
	for (int i = 0; i<nSpaceLen; ++i)
		fwrite(" ", 1, 1, fp);
	fwrite(pFunName, nFunLen, 1, fp);
	fwrite(" ", 1, 1, fp);
	fwrite(szLine, nLineLen, 1, fp);
	fwrite(": ", 2, 1, fp);
	fwrite(pLogText, nLogLen, 1, fp);
	fwrite("\n", 1, 1, fp);
	fclose(fp);
}

void CSimpleLog::Write(const char* pSourcePath, const char* pFunName, const long lLine, const wchar_t* pLogText)
{
	string strLogText = U2A(pLogText);
	Write(pSourcePath, pFunName, lLine, strLogText.c_str());
}

string CSimpleLog::GetTime()
{
	SYSTEMTIME st;
	::GetLocalTime(&st);
	char szTime[26] = { 0 };
	sprintf_s(szTime, "%04d-%02d-%02d %02d:%02d:%02d %d  ", st.wYear, st.wMonth, st.wDay, st.wHour, \
		st.wMinute, st.wSecond, st.wMilliseconds);
	return szTime;
}

string CSimpleLog::U2A(const wstring& str)
{
	string strDes;
	if (str.empty())
		goto __end;
	int nLen = ::WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), NULL, 0, NULL, NULL);
	if (0 == nLen)
		goto __end;
	char* pBuffer = new char[nLen + 1];
	memset(pBuffer, 0, nLen + 1);
	::WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), pBuffer, nLen, NULL, NULL);
	pBuffer[nLen] = '\0';
	strDes.append(pBuffer);
	delete[] pBuffer;
__end:
	return strDes;
}

void CSimpleLog::ScanfWrite(const char* pSourcePath, const char* pFunName, const long lLine, \
	const char* pLogText, ...)
{
	va_list pArgs;
	va_start(pArgs, pLogText);
	char szBuffer[1024] = { 0 };
	_vsnprintf_s(szBuffer, 1024, pLogText, pArgs);
	va_end(pArgs);
	Write(pSourcePath, pFunName, lLine, szBuffer);
}

void CSimpleLog::ScanfWrite(const char* pSourcePath, const char* pFunName, const long lLine, \
	const wchar_t* pLogText, ...)
{
	va_list pArgs;
	va_start(pArgs, pLogText);
	wchar_t szBuffer[1024] = { 0 };
	_vsnwprintf_s(szBuffer, 1024, pLogText, pArgs);
	va_end(pArgs);
	Write(pSourcePath, pFunName, lLine, szBuffer);
}
