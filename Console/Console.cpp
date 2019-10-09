#include "Console.h"
#include "Receiver.h"
#include <qthread.h>

Console::Console(QWidget *parent)
	: QWidget(parent), workingThread(0), receiver(0)
{
	ui.setupUi(this);

	//创建线程开始接受遥测数据
	workingThread = new QThread();
	receiver = new Receiver(workingThread);
	connect(receiver, SIGNAL(FirstReceive()), this, SLOT(SetupHUDPlug()));
	workingThread->start();

}

void Console::Initialize(HWND hwnd)
{
	// 获取HUDPlug.dll的绝对目录和资源目录
	wchar_t path[MAX_PATH + 1] = { 0 };
	GetModuleFileNameW(NULL, path, MAX_PATH);
	(wcsrchr(path, L'\\'))[1] = 0;

	//获取HUDPlug.dll的绝对地址
	wchar_t modulePath[MAX_PATH + 1] = { 0 };
	wcscpy(modulePath, path);
	wcscat(modulePath, L"HUDPlug.dll");
	auto modulePathSize = wcslen(modulePath) + 1;

	// 获取游戏进程ID并获取游戏进程句柄
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);

	// 为游戏进程开辟一个空间，写入dll的绝对地址
	LPVOID pszLibFileRemote = (PWSTR)VirtualAllocEx(hProcess, NULL, modulePathSize, MEM_COMMIT, PAGE_READWRITE);
	DWORD n = WriteProcessMemory(hProcess, pszLibFileRemote, (PVOID)modulePath, modulePathSize * sizeof(wchar_t), NULL);

	// 获取LoadLibraryW的真实地址。
	PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");

	// 创建共享内存，把窗口句柄放进去传给dll， 
	HANDLE hMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(HHOOK), gameHwndMapViewName);
	HWND *pBuffer = (HWND *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	*pBuffer = hwnd;
	UnmapViewOfFile(pBuffer);


	// 创建远程线程调用LoadLibraryW(DLLPathname)
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, pfnThreadRtn, pszLibFileRemote, 0, NULL);

	// 创建共享内存，把资源路径放进去
	wchar_t resourcePath[MAX_PATH + 1] = { 0 };
	wcscpy(resourcePath, path);
	wcscat(resourcePath, L"HUDplug\\");
	auto resourcePathSize = wcslen(resourcePath) + 1;
	HANDLE hResourcePathMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(wchar_t)*resourcePathSize, HUDPlugResourcePathMapViewName);
	wchar_t *pResourcePathBuffer = (wchar_t *)MapViewOfFile(hResourcePathMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	wcscpy(pResourcePathBuffer, resourcePath);
	UnmapViewOfFile(pResourcePathBuffer);
}

Console::~Console()
{
	workingThread->quit();
	workingThread->wait();
	delete workingThread;
}

void Console::SetupHUDPlug()
{
	HWND hwnd = FindWindowW(L"EGO_WindowClass", L"F1 2018 (DirectX 11)");
	Initialize(hwnd);
}
