#pragma once
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _DLL_
	// 通过宏来控制是导入还是导出
#ifdef _DLL_
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#endif

	// 导出/导入函数声明

DECLSPEC void __stdcall SetHook(DWORD threadId);

#undef DLL_SAMPLE_API

#ifdef __cplusplus
}
#endif


