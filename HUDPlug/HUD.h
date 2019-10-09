#pragma once

#include <Windows.h>
#include <d3d11.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "HUDPlug.h"

class HUD
{
public:
	HUD();
	~HUD();

	/* 初始化HUD的绘图部分，否则调用Render()会无效 */
	bool InitializePlotter(HWND hwnd, ID3D11Device *device, ID3D11DeviceContext *context, IDXGISwapChain *swapChain);

	void UnInitializePlotter();

	void Update();
	void Render();

	CarStatus Status;

private:
	bool LoadTexture(const wchar_t *fileName, __out ID3D11ShaderResourceView **srv, __out int *width, __out int *height);
	bool LoadTextureFromResource(const wchar_t *name, __out ID3D11ShaderResourceView **srv, __out int *width, __out int *height);

	void ShowBackground();
	void ShowSpeed();
	void ShowGear();
	void ShowERSDeployMode();
	void ShowERSDeployedThisLap();
	void ShowERSHarvestedThisLap();
	void ShowERSStoreEnergy();
	void ShowThrottleAndBrake();

	void ShowRevLightsPercent();
	void ShowRevLightsPercent(unsigned beginLEDIndex, unsigned endLEDIndex);
	void ShowRevLightsPercentFormStlye1();
	void ShowRevLightsPercentFormStlye2();
	void ShowRevLightsPercentFormStlye3();
	struct Texture
	{
		ID3D11ShaderResourceView *srv = 0;
		int w = 0, h = 0;
	};

	HANDLE mutexCarStatus;
	HANDLE mapViewCarStatus;
	HWND hwnd;
	IDXGISwapChain *swapChain;
	ID3D11Device *d3dDevice;
	ID3D11DeviceContext *d3dContext;
	ID3D11RenderTargetView *renderTargetView;
	Texture background;
	Texture gearTexture;
	Texture greenLED, blueLED, redLED;
	Texture DRSLights;
	ImVec2 origin;
	ImGuiWindowFlags windowFlags;
	ImFont *fontDefault, *fontERSDeployMode, *fontERSStore, *fontProcessBar, *fontThrottleAndBrake;
	wchar_t *resourcePath;
};

