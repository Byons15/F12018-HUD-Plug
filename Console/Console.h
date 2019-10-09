#pragma once

#include <QtWidgets/QWidget>
#include "ui_Console.h"
#include <Windows.h>

class Receiver;
class QThread;

class Console : public QWidget
{
	Q_OBJECT

public:
	Console(QWidget *parent = Q_NULLPTR);
	void Initialize(HWND hwnd);
	~Console();
public slots:
	void SetupHUDPlug();

private:
	Ui::ConsoleClass ui;
	QThread *workingThread;
	Receiver *receiver;
};
