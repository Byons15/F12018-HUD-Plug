#pragma once
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _DLL_
	// ͨ�����������ǵ��뻹�ǵ���
#ifdef _DLL_
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#endif

	// ����/���뺯������

DECLSPEC void __stdcall SetHook(DWORD threadId);

#undef DLL_SAMPLE_API

#ifdef __cplusplus
}
#endif


