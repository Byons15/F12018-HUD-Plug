// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "HUDPlug.h"
#include <d3d11.h>
#include <d3dx11.h>
#include "detours.h"
#include "Log.h"
#include "HUD.h"
#include <Windows.h>
#include <process.h>

#pragma comment(lib, "detours.lib")

typedef HRESULT(__stdcall *D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
HRESULT __stdcall hookD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
HRESULT GetDeviceAndCtxFromSwapchain(IDXGISwapChain *pSwapChain, ID3D11Device **ppDevice, ID3D11DeviceContext **ppContext);

D3D11PresentHook OldSwapChainPresent;
static HWND hwnd = 0;
static char *hHookMapName = "hHook data map";
static HUD hud;

/*
	DLL初始化部分
*/
unsigned int __stdcall DllStart(void *argv)
{

	//获取游戏窗口句柄
	HANDLE hMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, gameHwndMapViewName);
	if (hMap == NULL)
	{
		Log.Write("dllmain.cpp", "hookProc", 0, "failed on get game hwnd");
	}
	void *pBuffer = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	hwnd = *((HWND *)pBuffer);
	UnmapViewOfFile(pBuffer);
	CloseHandle(hMap);

	WNDCLASSEX windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = L"HUDPlug";
	windowClass.hIconSm = NULL;

	::RegisterClassEx(&windowClass);

	HWND tempWindow = ::CreateWindow(windowClass.lpszClassName, L"HUDPlug DirectX Window", 
		WS_OVERLAPPEDWINDOW | WS_DISABLED | WS_ICONIC, 0, 0, 100, 100, NULL, NULL, windowClass.hInstance, NULL);

	// 创建一个交换链
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));   //填充0
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = tempWindow;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;//有时候这个也有可能导致hook崩溃等等-.-
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL featrueLevel = D3D_FEATURE_LEVEL_11_0;
	IDXGISwapChain *pSwapChain = 0;
	ID3D11Device *pDevice = 0;
	ID3D11DeviceContext *pContext = 0;

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featrueLevel, 1,
		D3D11_SDK_VERSION, &scd, &pSwapChain, &pDevice, NULL, &pContext)))
	{
		DestroyWindow(tempWindow);
		UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
		Log.Write("HUDPlug.dll", "hookProc", 0, L"Succeed in create d3d Resource!");
		return -1;
	}

	// 从交换链的虚表中获取Present
	DWORD_PTR* pSwapChainVT = NULL;
	DWORD_PTR* pDeviceVT = NULL;
	DWORD_PTR* pContextVT = NULL;
	pSwapChainVT = reinterpret_cast<DWORD_PTR*>(pSwapChain);
	pSwapChainVT = reinterpret_cast<DWORD_PTR*>(pSwapChainVT[0]);//拿对象
	OldSwapChainPresent = reinterpret_cast<D3D11PresentHook>(pSwapChainVT[8]);//拿到这个 就是Present

	DestroyWindow(tempWindow);
	UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
																			  //HOOK
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(reinterpret_cast<PVOID *>(&OldSwapChainPresent), hookD3D11Present);
	DetourTransactionCommit();

	return 0;
}

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
		{
			CloseHandle((HANDLE)(_beginthreadex(0, 0, DllStart, 0, 0, 0)));
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
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

	//第一次调用初始化部分
	static bool firstCall = true;
	if (firstCall == true)
	{
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
		swapChain->GetDesc(&desc);
		hwnd = desc.OutputWindow;

		//初始化HUD
		hud.InitializePlotter(hwnd, device, context, swapChain);

		firstCall = false;
	}

	hud.Update();
	hud.Render();

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

