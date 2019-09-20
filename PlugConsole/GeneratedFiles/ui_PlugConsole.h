/********************************************************************************
** Form generated from reading UI file 'PlugConsole.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLUGCONSOLE_H
#define UI_PLUGCONSOLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlugConsoleClass
{
public:
    QPushButton *pushButton;

    void setupUi(QWidget *PlugConsoleClass)
    {
        if (PlugConsoleClass->objectName().isEmpty())
            PlugConsoleClass->setObjectName(QStringLiteral("PlugConsoleClass"));
        PlugConsoleClass->resize(600, 400);
        pushButton = new QPushButton(PlugConsoleClass);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(250, 170, 75, 23));

        retranslateUi(PlugConsoleClass);
        QObject::connect(pushButton, SIGNAL(clicked()), PlugConsoleClass, SLOT(buttonDown()));

        QMetaObject::connectSlotsByName(PlugConsoleClass);
    } // setupUi

    void retranslateUi(QWidget *PlugConsoleClass)
    {
        PlugConsoleClass->setWindowTitle(QApplication::translate("PlugConsoleClass", "PlugConsole", Q_NULLPTR));
        pushButton->setText(QApplication::translate("PlugConsoleClass", "PushButton", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class PlugConsoleClass: public Ui_PlugConsoleClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLUGCONSOLE_H
