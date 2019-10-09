#include "HUD.h"
#include "WICTextureLoader.h"
#include "Log.h"
#include <string>
#include <fstream>
#include "../Console/Packets.h"

HUD::HUD()
	:d3dDevice(0), d3dContext(0), hwnd(0), renderTargetView(0),  fontDefault(0), fontERSDeployMode(0), fontERSStore(0),
	fontProcessBar(0), fontThrottleAndBrake(0)
{
	windowFlags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground;

}


HUD::~HUD()
{
	UnInitializePlotter();
}

bool HUD::InitializePlotter(HWND hwnd, ID3D11Device * device, ID3D11DeviceContext * context, IDXGISwapChain *swapChain)
{
	// 保存窗口句柄和d3d设备
	this->hwnd = hwnd;
	this->d3dDevice = device;
	this->d3dContext = context;

	// 获取后备渲染目标
	ID3D11Texture2D *renderTargetTexture = 0;
	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<PVOID*>(&renderTargetTexture));
	if (FAILED(hr))
	{
		Log.Write("HUD.cpp", "Initialize", 0, "Failed to Get swapChain buffer");
		return false;
	}
	device->CreateRenderTargetView(renderTargetTexture, 0, &renderTargetView);
	renderTargetTexture->Release();

	// 初始化ImGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();  (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);

	// 初始化HUD绘图的起点
	origin = { 550, 679 };

	//载入需要的字体
	fontDefault = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", 25);
	fontERSDeployMode = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", 20);
	fontERSStore = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", 14);
	fontProcessBar = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", 9);
	fontThrottleAndBrake = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", 0.5f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_FrameBg, { 1.0f, 1.0f, 1.0f, 0.0f });
	// TODO:: 这里隐藏进度条的边框

	// 获取资源路径
	HANDLE hMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, HUDPlugResourcePathMapViewName);
	void *pBuffer = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	wchar_t *path = ((wchar_t *)pBuffer);
	resourcePath = new wchar_t[wcslen(path) + 1];
	wcscpy(resourcePath, path);
	UnmapViewOfFile(pBuffer);
	CloseHandle(hMap);

	// 从插件资源目录下加载ImGui布局配置
	io.IniFilename = NULL;
	std::wstring iniFileName(resourcePath + std::wstring(L"imgui.ini"));
	DWORD fileSize = 0;
	char *buffer = 0;
	HANDLE pFile = CreateFileW(iniFileName.c_str(), GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,        //打开已存在的文件 
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (pFile == INVALID_HANDLE_VALUE)
	{
		printf("open file error!\n");
		CloseHandle(pFile);
		return NULL;
	}

	fileSize = GetFileSize(pFile, NULL);          //得到文件的大小

	buffer = (char *)malloc(fileSize);
	ZeroMemory(buffer, fileSize);
	DWORD dwBytesToRead = fileSize;
	DWORD dwBytesRead = 0;
	char * tmpBuf = buffer;
	do
	{                                       //循环读文件，确保读出完整的文件    
		ReadFile(pFile, tmpBuf, dwBytesToRead, &dwBytesRead, NULL);

		if (dwBytesRead == 0)
			break;

		dwBytesToRead -= dwBytesRead;
		tmpBuf += dwBytesRead;

	} while (dwBytesToRead > 0);

	ImGui::LoadIniSettingsFromMemory(buffer, fileSize);

	free(buffer);
	CloseHandle(pFile);


	// 载入背景
	if (LoadTexture(L"Background.png", &background.srv, &background.w, &background.h) == false)
	{
		Log.Write("HUD.cpp", "Initialize", 0, "Failed to LoadTexture: bakeground");
	}

	// 载入挡位纹理
	if (LoadTexture(L"Gear.png", &gearTexture.srv, &gearTexture.w, &gearTexture.h) == false)
	{
		Log.Write("HUD.cpp", "Initialize", 0, "Failed to LoadTexture: gearTexture");
	}

	// 载入换挡指示灯
	if (LoadTexture(L"GreenLED.png", &greenLED.srv, &greenLED.w, &greenLED.h) == false)
	{
		Log.Write("HUD.cpp", "Initialize", 0, "Failed to LoadTexture: GreenLED");
	}

	if (LoadTexture(L"RedLED.png", &redLED.srv, &redLED.w, &redLED.h) == false)
	{
		Log.Write("HUD.cpp", "Initialize", 0, "Failed to LoadTexture: redLED");
	}

	if (LoadTexture(L"BlueLED.png", &blueLED.srv, &blueLED.w, &blueLED.h) == false)
	{
		Log.Write("HUD.cpp", "Initialize", 0, "Failed to LoadTexture: blueLED");
	}

	// 载入DRS灯
	if (LoadTexture(L"DRSLights.png", &DRSLights.srv, &DRSLights.w, &DRSLights.h) == false)
	{
		Log.Write("HUD.cpp", "Initialize", 0, "Failed to LoadTexture: DRSLights");
	}

	// 获取车辆数据资源的互斥体
	mutexCarStatus = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, CarStatusMutexName);
	if (mutexCarStatus == NULL)
		return false;

	// 获取车辆数据资源的内存映射句柄
	mapViewCarStatus = OpenFileMappingA(FILE_MAP_ALL_ACCESS, 0, CarStatusMapViewName);
	if (mapViewCarStatus == NULL)
		return false;


	return true;
}

void HUD::UnInitializePlotter()
{
	if (renderTargetView) renderTargetView->Release();
	if (background.srv) background.srv->Release();
	if (greenLED.srv) greenLED.srv->Release();
	if (blueLED.srv) blueLED.srv->Release();
	if (redLED.srv) redLED.srv->Release();
	if (gearTexture.srv) gearTexture.srv->Release();
	if (DRSLights.srv) DRSLights.srv->Release();

	delete resourcePath;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void HUD::Update()
{
	//互斥进入共享内存中获取carStatus
	if (WaitForSingleObject(mutexCarStatus, 8) != WAIT_OBJECT_0)
		return;

	void *buffer = MapViewOfFile(mapViewCarStatus, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	Status = *((CarStatus *)buffer);

	UnmapViewOfFile(buffer);
	ReleaseMutex(mutexCarStatus);
}

void HUD::Render()
{
	if (!Status.show)
		return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ShowBackground();
	ShowSpeed();
	ShowRevLightsPercent();
	ShowGear();
	ShowERSDeployMode();
	ShowERSStoreEnergy();
	ShowERSDeployedThisLap();
	ShowERSHarvestedThisLap();
	ShowThrottleAndBrake();

	//ImGui::ShowDemoWindow();

	ImGui::EndFrame();
	ImGui::Render();
	d3dContext->OMSetRenderTargets(1, &renderTargetView, 0);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool HUD::LoadTexture(const wchar_t * fileName, ID3D11ShaderResourceView ** srv, int * width, int * height)
{
	wchar_t file[MAX_PATH + 1] = { 0 };
	wcscpy(file, resourcePath);
	wcscat(file, fileName);

	ID3D11Resource *texture = 0;
	HRESULT hr = DirectX::CreateWICTextureFromFile(d3dDevice, file, &texture, srv);
	if (FAILED(hr))
	{
		if (texture) texture->Release();

		return false;
	}

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	static_cast<ID3D11Texture2D *>(texture)->GetDesc(&desc);

	*width = desc.Width;
	*height = desc.Height;

	return true;
}

bool HUD::LoadTextureFromResource(const wchar_t * name, ID3D11ShaderResourceView ** srv, int * width, int * height)
{
	auto hModule = GetModuleHandle(L"HUDPlug.dll");
	auto hRerc = FindResource(hModule, name, L"PNG");
	auto size = SizeofResource(hModule, hRerc);
	auto hGlobal = LoadResource(hModule, hRerc);
	auto pBuffer = LockResource(hGlobal);

	Log.ScanfWrite("", "", 0, "load background size: %d", size);

	ID3D11Resource *texture = 0;
	HRESULT hr = DirectX::CreateWICTextureFromMemory(d3dDevice, (const uint8_t *)pBuffer, size, 0, srv);
	if (FAILED(hr))
	{
		if (texture) texture->Release();

		return false;
	}

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	static_cast<ID3D11Texture2D *>(texture)->GetDesc(&desc);

	*width = desc.Width;
	*height = desc.Height;

	return true;
}

/*
绘制背景
*/
void HUD::ShowBackground()
{
	ImGui::Begin("bakground", 0, windowFlags);

	ImGui::Image(background.srv, { (float)background.w, (float)background.h });

	ImGui::End();
}

/*
绘制速度
*/
void HUD::ShowSpeed()
{
	ImGui::PushFont(fontDefault);
	ImGui::Begin("speed", 0, windowFlags);
	ImGui::TextColored({ 255, 255, 255, 255 }, "%d", Status.speed);
	ImGui::End();
	ImGui::PopFont();
}

/*
绘制换挡指示灯
*/
void HUD::ShowRevLightsPercent()
{
	switch (Status.teamID)
	{
	case MERCEDES:
	case WILLIAMS:
	case RENAULT:
	case TOROROSSO:
	case MCLAREN:
		{
			UINT8 LEDCount = ((UINT8)ceil((float)Status.revLightsPercent / 100.0f * 15.0f));
			LEDCount = (LEDCount >= 10) ? 15 : LEDCount;
			ShowRevLightsPercent(5, LEDCount);
		}
		break;
	case HASS:
	case FERRARI:
		{
			UINT8 LEDCount = ((UINT8)ceil((float)Status.revLightsPercent / 100.0f * 15.0f));
			ShowRevLightsPercent(5, LEDCount);
		}
		break;
	case REDBULL:
		{
			UINT8 LEDCount = ((UINT8)ceil((float)Status.revLightsPercent / 100.0f * 3.0f)) * 5;
			ShowRevLightsPercent(0, LEDCount);
		}
		break;
	case FORCEINDIA:
	case SAUBER:
	default:
		{
			UINT8 LEDCount = ((UINT8)ceil((float)Status.revLightsPercent / 100.0f * 15.0f));
			ShowRevLightsPercent(0, LEDCount);
		}
		break;
	}
}

void HUD::ShowRevLightsPercent(unsigned beginLEDIndex, unsigned endLEDIndex)
{
	Texture *LED = &greenLED;
	char title[] = "ARevLightsPercent";

	for (UINT8 i = beginLEDIndex; i < endLEDIndex; ++i)
	{
		LED = (i >= 5) ? &redLED : LED;
		LED = (i >= 10) ? &blueLED : LED;
		title[0] = 'A' + i;

		ImGui::Begin(title, 0, windowFlags);

		ImGui::Image(LED->srv, { (float)LED->w, (float)LED->h });

		ImGui::End();
	}
}

void HUD::ShowRevLightsPercentFormStlye2()
{
	UINT8 LEDCount = ((UINT8)ceil((float)Status.revLightsPercent / 100.0f * 3.0f)) * 5;
	Texture *LED = &greenLED;
	char title[] = "0RevLightsPercent";

	for (UINT8 i = 0; i != LEDCount; ++i)
	{
		LED = (i >= 5) ? &redLED : LED;
		LED = (i >= 10) ? &blueLED : LED;
		title[0] = title[0] + i;

		ImGui::Begin(title, 0, windowFlags);

		ImGui::Image(LED->srv, { (float)LED->w, (float)LED->h });

		ImGui::End();
	}
}

void HUD::ShowRevLightsPercentFormStlye3()
{
	UINT8 LEDCount = ((UINT8)ceil((float)Status.revLightsPercent / 100.0f * 15.0f));
	Texture *LED = &greenLED;
	char title[] = "0RevLightsPercent";

	LEDCount = (LEDCount >= 10) ? 15 : LEDCount;

	for (UINT8 i = 5; i != LEDCount; ++i)
	{
		LED = (i >= 5) ? &redLED : LED;
		LED = (i >= 10) ? &blueLED : LED;
		title[0] = title[0] + i;

		ImGui::Begin(title, 0, windowFlags);

		ImGui::Image(LED->srv, { (float)LED->w, (float)LED->h });

		ImGui::End();
	}
}

/*
绘制挡位
*/
void HUD::ShowGear()
{
	if (Status.drs)
	{
		ImGui::Begin("DRSLights", 0, windowFlags);
		ImGui::Image(DRSLights.srv, { (float)DRSLights.w, (float)DRSLights.h });
		ImGui::End();
	}

	ImVec2 uv0, uv1;
	uv0.x = (float)(Status.gear + 2) / 11.0f;
	uv1.x = uv0.x + (1.0f / 11.0f);
	uv0.y = 0;
	uv1.y = 1.05f;
	gearTexture.w = 50;

	ImGui::Begin("gear", 0, windowFlags);
	ImGui::Image(gearTexture.srv, { (float)gearTexture.w, (float)gearTexture.h }, uv0, uv1);

	ImGui::End();
}

/*
绘制ERS模式
*/
void HUD::ShowERSDeployMode()
{
	ImGui::PushFont(fontERSDeployMode);
	ImGui::Begin("ERSDeployMode", 0, windowFlags);

	ImGui::TextColored({ 0.263f, 0.651f, 0.753f, 1.0f }, "%d", Status.ersDeployMode);

	ImGui::End();
	ImGui::PopFont();
}

/*
绘制当圈ERS部署量
*/
void HUD::ShowERSDeployedThisLap()
{
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, { 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushFont(fontProcessBar);
	ImGui::Begin("ERSDeployedThisLap", 0, windowFlags);
	ImGui::ProgressBar(1.0f - (Status.ersDeployedThisLap / 4000000.0f), { -1, 0 }, " ");
	ImGui::End();
	ImGui::PopFont();
	ImGui::PopStyleColor();
}

/*
绘制当圈ERS获取量
*/
void HUD::ShowERSHarvestedThisLap()
{
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, { 1.0f, 0.965f, 0.0f, 1.0f });
	ImGui::PushFont(fontProcessBar);
	ImGui::Begin("ERSHarvestedThisLap", 0, windowFlags);
	ImGui::ProgressBar(Status.ersHarvestedThisLap / 4000000.0f, { -1, 0 }, " ");
	ImGui::End();
	ImGui::PopFont();
	ImGui::PopStyleColor();
}

/*
绘制ERS当前能量
*/
void HUD::ShowERSStoreEnergy()
{
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, { 0.263f, 0.651f, 0.753f, 1.0f });
	ImGui::PushFont(fontProcessBar);
	ImGui::Begin("ERSStoreEnergy", 0, windowFlags);
	ImGui::ProgressBar(Status.ersStoreEnergy / 4000000.f, { -1, 0 }, " ");
	ImGui::End();
	ImGui::PopFont();
	ImGui::PopStyleColor();

	char buffer[10] = { 0 };
	ImGui::PushFont(fontERSStore);
	sprintf(buffer, "%d%%%%", unsigned(roundf((float)Status.ersStoreEnergy / 40000.0f)));
	ImVec2 size = ImGui::CalcTextSize(buffer);
	ImGui::SetNextWindowPos({ 1920 / 2, 823 }, 0, { 0.35f, 0 });
	ImGui::Begin("ERSStoreEnergyNum", 0, windowFlags | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize(size);
	ImGui::TextColored({ 0.263f, 0.651f, 0.753f, 1.0f }, buffer);
	ImGui::End();
	ImGui::PopFont();
}

void HUD::ShowThrottleAndBrake()
{
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, { 0.0f, 0.6f, 0.267f, 1.0f });
	ImGui::PushFont(fontThrottleAndBrake);
	ImGui::Begin("throttle", 0, windowFlags);
	ImGui::ProgressBar((float)Status.throttle / 100.0f, { -1, 0 }, " ");
	ImGui::End();
	ImGui::PopStyleColor();

	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, { 0.718f, 0.118f, 0.0f, 1.0f });

	ImGui::Begin("brake", 0, windowFlags);
	ImGui::ProgressBar((float)Status.brake / 100.0f, { -1, 0 }, " ");
	ImGui::End();
	ImGui::PopFont();
	ImGui::PopStyleColor();
}
