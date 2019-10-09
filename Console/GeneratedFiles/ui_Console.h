/********************************************************************************
** Form generated from reading UI file 'Console.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONSOLE_H
#define UI_CONSOLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConsoleClass
{
public:

    void setupUi(QWidget *ConsoleClass)
    {
        if (ConsoleClass->objectName().isEmpty())
            ConsoleClass->setObjectName(QStringLiteral("ConsoleClass"));
        ConsoleClass->resize(393, 192);

        retranslateUi(ConsoleClass);

        QMetaObject::connectSlotsByName(ConsoleClass);
    } // setupUi

    void retranslateUi(QWidget *ConsoleClass)
    {
        ConsoleClass->setWindowTitle(QApplication::translate("ConsoleClass", "Console", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ConsoleClass: public Ui_ConsoleClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONSOLE_H
