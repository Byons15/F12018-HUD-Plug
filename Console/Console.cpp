#include "Console.h"
#include "Receiver.h"
#include <qthread.h>

Console::Console(QWidget *parent)
	: QWidget(parent), workingThread(0), receiver(0)
{
	ui.setupUi(this);

	//�����߳̿�ʼ����ң������
	workingThread = new QThread();
	receiver = new Receiver(workingThread);
	connect(receiver, SIGNAL(FirstReceive()), this, SLOT(SetupHUDPlug()));
	workingThread->start();

}

void Console::Initialize(HWND hwnd)
{
	// ��ȡHUDPlug.dll�ľ���Ŀ¼����ԴĿ¼
	wchar_t path[MAX_PATH + 1] = { 0 };
	GetModuleFileNameW(NULL, path, MAX_PATH);
	(wcsrchr(path, L'\\'))[1] = 0;

	//��ȡHUDPlug.dll�ľ��Ե�ַ
	wchar_t modulePath[MAX_PATH + 1] = { 0 };
	wcscpy(modulePath, path);
	wcscat(modulePath, L"HUDPlug.dll");
	auto modulePathSize = wcslen(modulePath) + 1;

	// ��ȡ��Ϸ����ID����ȡ��Ϸ���̾��
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);

	// Ϊ��Ϸ���̿���һ���ռ䣬д��dll�ľ��Ե�ַ
	LPVOID pszLibFileRemote = (PWSTR)VirtualAllocEx(hProcess, NULL, modulePathSize, MEM_COMMIT, PAGE_READWRITE);
	DWORD n = WriteProcessMemory(hProcess, pszLibFileRemote, (PVOID)modulePath, modulePathSize * sizeof(wchar_t), NULL);

	// ��ȡLoadLibraryW����ʵ��ַ��
	PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");

	// ���������ڴ棬�Ѵ��ھ���Ž�ȥ����dll�� 
	HANDLE hMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(HHOOK), gameHwndMapViewName);
	HWND *pBuffer = (HWND *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	*pBuffer = hwnd;
	UnmapViewOfFile(pBuffer);


	// ����Զ���̵߳���LoadLibraryW(DLLPathname)
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, pfnThreadRtn, pszLibFileRemote, 0, NULL);

	// ���������ڴ棬����Դ·���Ž�ȥ
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
