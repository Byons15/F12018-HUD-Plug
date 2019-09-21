#pragma once

#include <QtWidgets/QWidget>
#include "ui_PlugConsole.h"

class PlugConsole : public QWidget
{
	Q_OBJECT

public:
	PlugConsole(QWidget *parent = Q_NULLPTR);

private:
	Ui::PlugConsoleClass ui;
};
