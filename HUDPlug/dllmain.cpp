// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "HUD.h"
#include <d3d11.h>
#include <d3dx11.h>
#include "detours.h"

#pragma comment(lib, "detours.lib")

LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam);
typedef HRESULT(__stdcall *D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
HRESULT __stdcall hookD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
HRESULT GetDeviceAndCtxFromSwapchain(IDXGISwapChain *pSwapChain, ID3D11Device **ppDevice, ID3D11DeviceContext **ppContext);

HHOOK hhook = 0;
D3D11PresentHook OldSwapChainPresent;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

DECLSPEC void __stdcall SetHook(DWORD threadId)
{
	hhook = SetWindowsHookExA(WH_GETMESSAGE, hookProc, GetModuleHandleA("HUDPlug.dll"), threadId);

	return;
}

LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam)
{
	static bool firstCall = true;
	if (firstCall == true)
	{
		DWORD_PTR hDxgi = (DWORD_PTR)GetModuleHandleA("dxgi.dll");
		OldSwapChainPresent = (D3D11PresentHook)((DWORD_PTR)hDxgi + 0x6DFE0);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(reinterpret_cast<PVOID *>(&OldSwapChainPresent), hookD3D11Present);
		DetourTransactionCommit();

		UnhookWindowsHookEx(hhook);
	}
	return CallNextHookEx(hhook, code, wParam, lParam);
}

HRESULT __stdcall hookD3D11Present(IDXGISwapChain * pSwapChain, UINT SyncInterval, UINT Flags)
{
	static ID3D11Device *device = 0;
	static ID3D11DeviceContext * context = 0;
	IDXGISwapChain *swapChain;

	static bool firstCall = true;
	if (firstCall == true)
	{
		// 获取游戏d3d设备和上下文环境。
		GetDeviceAndCtxFromSwapchain(pSwapChain, &device, &context);

		MessageBoxA(0, "注入成功", 0, MB_OK);
		auto threadID = GetCurrentThreadId();
		auto proId = GetCurrentProcessId();
		//TODO:: 其他绘图代码准备工作。

		firstCall = false;
	}

	//TODO:: 添加绘图代码 

	return OldSwapChainPresent(pSwapChain, SyncInterval, Flags);
}

HRESULT GetDeviceAndCtxFromSwapchain(IDXGISwapChain *pSwapChain, ID3D11Device **ppDevice, ID3D11DeviceContext **ppContext)
{
	HRESULT ret = pSwapChain->GetDevice(__uuidof(ID3D11Device), (PVOID*)ppDevice);

	if (SUCCEEDED(ret))
		(*ppDevice)->GetImmediateContext(ppContext);

	return ret;
}

