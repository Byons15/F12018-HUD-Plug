#include "PlugConsole.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	PlugConsole w;
	w.show();
	return a.exec();
}
