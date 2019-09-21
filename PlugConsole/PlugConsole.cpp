#include "PlugConsole.h"
#include <QMessagebox>
#include <Windows.h>
#include "HUD.h"

#pragma comment(lib, "HUDPlug.lib")

PlugConsole::PlugConsole(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	HWND hwnd = FindWindowA("DXDEMO1", "DXDemo1");

	SetHook(hwnd);
}
