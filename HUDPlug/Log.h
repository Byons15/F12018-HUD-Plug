#pragma once
#include <string>
using std::string;
using std::wstring;

class CSimpleLog
{
public:
	CSimpleLog(void);
	~CSimpleLog(void);
	void	Write(const char* pSourcePath, const char* pFunName, const long lLine, const char* pLogText);
	void	Write(const char* pSourcePath, const char* pFunName, const long lLine, const wchar_t* pLogText);
	void	ScanfWrite(const char* pSourcePath, const char* pFunName, const long lLine, \
		const char* pLogText, ...);
	void	ScanfWrite(const char* pSourcePath, const char* pFunName, const long lLine, \
		const wchar_t* pLogText, ...);
protected:
	string	GetTime();
	string	U2A(const wstring& str);
};

extern CSimpleLog Log;

#define SL_LOG(x)				Log.Write(__FILE__, __FUNCTION__, __LINE__, x)
#define SL_LOG1(x, p1)			Log.ScanfWrite(__FILE__, __FUNCTION__, __LINE__, x, p1)
#define SL_LOG2(x, p1, p2)		Log.ScanfWrite(__FILE__, __FUNCTION__, __LINE__, x, p1, p2)
#define SL_LOG3(x, p1, p2, p3)	Log.ScanfWrite(__FILE__, __FUNCTION__, __LINE__, x, p1, p2, p3)
