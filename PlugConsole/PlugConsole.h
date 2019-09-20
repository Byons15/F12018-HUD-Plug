#pragma once

#include <QtWidgets/QWidget>
#include "ui_PlugConsole.h"
#include <windows.h>

class PlugConsole : public QWidget
{
	Q_OBJECT

public:
	PlugConsole(QWidget *parent = Q_NULLPTR);

private:
	typedef void(__stdcall *SetHookFn)(DWORD threadId);

	Ui::PlugConsoleClass ui;
	SetHookFn setHook;
};
