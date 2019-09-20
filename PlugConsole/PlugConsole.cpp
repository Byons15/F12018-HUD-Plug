#include "PlugConsole.h"

PlugConsole::PlugConsole(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	
	auto lib = LoadLibraryA("HUDPlug.dll");
	setHook = (SetHookFn)GetProcAddress(lib, "SetHook");
}
