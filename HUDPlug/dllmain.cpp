// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "HUD.h"
#include <d3d11.h>
#include <d3dx11.h>
#include "detours.h"
#include "Log.h"

#pragma comment(lib, "detours.lib")

LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam);
typedef HRESULT(__stdcall *D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
HRESULT __stdcall hookD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
HRESULT GetDeviceAndCtxFromSwapchain(IDXGISwapChain *pSwapChain, ID3D11Device **ppDevice, ID3D11DeviceContext **ppContext);

HHOOK hhook = 0;
D3D11PresentHook OldSwapChainPresent;

/*
	DLL的入口
*/
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

DECLSPEC void __stdcall SetHook(HWND hwnd)
{
	DWORD processId = 0;
	DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
	hhook = SetWindowsHookExA(WH_CALLWNDPROC, hookProc, GetModuleHandleA("HUDPlug.dll"), threadId);

	PostMessageA(hwnd, WM_CHAR, 0, 0);

	return;
}

/*
	窗口消息Hook处理
*/
LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam)
{
	CWPSTRUCT *cwpStruct = (CWPSTRUCT *)lParam;

	static bool firstCall = true;
	if (firstCall == true)
	{
		// 创建一个交换链
		DXGI_SWAP_CHAIN_DESC scd;
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));   //填充0
		scd.BufferCount = 1;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.OutputWindow = cwpStruct->hwnd;
		scd.SampleDesc.Count = 1;
		scd.Windowed = TRUE;//有时候这个也有可能导致hook崩溃等等-.-
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		D3D_FEATURE_LEVEL featrueLevel = D3D_FEATURE_LEVEL_11_0;
		IDXGISwapChain *pSwapChain = 0;
		ID3D11Device *pDevice = 0;
		ID3D11DeviceContext *pContext = 0;

		if (FAILED(D3D11CreateDeviceAndSwapChain(NULL,D3D_DRIVER_TYPE_HARDWARE,	NULL,NULL, &featrueLevel,1,
			D3D11_SDK_VERSION,&scd,&pSwapChain,&pDevice,NULL,&pContext)))
		{
			Log.Write("HUDPlug.dll", "hookProc", 0, L"Succeed in create d3d Resource!");
			return CallNextHookEx(hhook, code, wParam, lParam);
		}

		// 从交换链的虚表中获取Present
		DWORD_PTR* pSwapChainVT = NULL;
		DWORD_PTR* pDeviceVT = NULL;
		DWORD_PTR* pContextVT = NULL;
		pSwapChainVT = reinterpret_cast<DWORD_PTR*>(pSwapChain);
		pSwapChainVT = reinterpret_cast<DWORD_PTR*>(pSwapChainVT[0]);//拿对象
		OldSwapChainPresent = reinterpret_cast<D3D11PresentHook>(pSwapChainVT[8]);//拿到这个 就是Present

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(reinterpret_cast<PVOID *>(&OldSwapChainPresent), hookD3D11Present);
		DetourTransactionCommit();

		firstCall = false;
	}

	switch (cwpStruct->message)
	{
	default:
		break;
	}

	return CallNextHookEx(hhook, code, wParam, lParam);
}

/*
	IDXGISwapChain::Present的hook处理
*/
HRESULT __stdcall hookD3D11Present(IDXGISwapChain * pSwapChain, UINT SyncInterval, UINT Flags)
{
	ID3D11Device *device = 0;
	ID3D11DeviceContext * context = 0;
	IDXGISwapChain *swapChain = pSwapChain;

	// 获取游戏d3d设备和上下文环境。
	GetDeviceAndCtxFromSwapchain(pSwapChain, &device, &context);

	auto threadID = GetCurrentThreadId();
	auto proId = GetCurrentProcessId();

	if (!device || !context || !swapChain)
	{
		Log.Write("HUDPlug.dll", "hookD3D11Present", 0, L"Failed in get game d3d resource!");
		return OldSwapChainPresent(pSwapChain, SyncInterval, Flags);
	}

	// 第一次调用时要用的初始化代码
	static bool firstCall = true;
	if (firstCall == true)
	{
		Log.Write("HUDPlug.dll", "hookD3D11Present", 0, L"Succeed first call!");

		firstCall = false;
	}

	//TODO:: 添加绘图代码 

	return OldSwapChainPresent(pSwapChain, SyncInterval, Flags);
}

/*
	从交换链获取d3d设备和上下文
*/
HRESULT GetDeviceAndCtxFromSwapchain(IDXGISwapChain *pSwapChain, ID3D11Device **ppDevice, ID3D11DeviceContext **ppContext)
{
	HRESULT ret = pSwapChain->GetDevice(__uuidof(ID3D11Device), (PVOID*)ppDevice);

	if (SUCCEEDED(ret))
		(*ppDevice)->GetImmediateContext(ppContext);

	return ret;
}

