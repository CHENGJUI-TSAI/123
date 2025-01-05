/********************************************************************************
** Form generated from reading UI file 'Qtproject.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTPROJECT_H
#define UI_QTPROJECT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Qtproject
{
public:
    QWidget *centralwidget;
    QPushButton *pushButton_1;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QStatusBar *statusbar;
    QMenuBar *menubar;

    void setupUi(QMainWindow *Qtproject)
    {
        if (Qtproject->objectName().isEmpty())
            Qtproject->setObjectName("Qtproject");
        Qtproject->resize(381, 677);
        centralwidget = new QWidget(Qtproject);
        centralwidget->setObjectName("centralwidget");
        pushButton_1 = new QPushButton(centralwidget);
        pushButton_1->setObjectName("pushButton_1");
        pushButton_1->setGeometry(QRect(120, 280, 141, 61));
        pushButton_1->setStyleSheet(QString::fromUtf8(""));
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(120, 370, 141, 61));
        pushButton_3 = new QPushButton(centralwidget);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setGeometry(QRect(120, 460, 141, 61));
        Qtproject->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(Qtproject);
        statusbar->setObjectName("statusbar");
        Qtproject->setStatusBar(statusbar);
        menubar = new QMenuBar(Qtproject);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 381, 20));
        Qtproject->setMenuBar(menubar);

        retranslateUi(Qtproject);

        QMetaObject::connectSlotsByName(Qtproject);
    } // setupUi

    void retranslateUi(QMainWindow *Qtproject)
    {
        Qtproject->setWindowTitle(QCoreApplication::translate("Qtproject", "Qtproject", nullptr));
        pushButton_1->setText(QString());
        pushButton_2->setText(QString());
        pushButton_3->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Qtproject: public Ui_Qtproject {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTPROJECT_H
