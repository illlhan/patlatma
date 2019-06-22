/********************************************************************************
** Form generated from reading UI file 'keyboard.ui'
**
** Created: Wed Feb 29 21:59:51 2012
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NUMPAD_H
#define UI_NUMPAD_H

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

QT_BEGIN_NAMESPACE

class Ui_Numpad
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit;
    QPushButton *backButton;
    QPushButton *clear;
    QGridLayout *gridLayout;

    QPushButton *Button0;
    QPushButton *Button1;
    QPushButton *Button2;
    QPushButton *Button3;
    QPushButton *Button4;
    QPushButton *Button5;
    QPushButton *Button6;
    QPushButton *Button7;
    QPushButton *Button8;
    QPushButton *Button9;

    QPushButton *enterButton;
       QPushButton *commaButton;



    void setupUi(QWidget *Keyboard)
    {
        if (Keyboard->objectName().isEmpty())
            Keyboard->setObjectName(QString::fromUtf8("Keyboard"));
        Keyboard->setWindowModality(Qt::ApplicationModal );
        Keyboard->resize(600, 315);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Keyboard->sizePolicy().hasHeightForWidth());
        Keyboard->setSizePolicy(sizePolicy);
        Keyboard->setWindowTitle(QString::fromUtf8("Keyboard"));
        Keyboard->setStyleSheet(QString::fromUtf8(  "Keyboard{\n"
                                                    "color: rgb(0, 0, 0);\n"
                                                    "background-color: rgb(255, 255, 255);\n"
                                                    "}\n"
                                                    "QPushButton {\ncolor: rgb(255, 255, 255);\n"
                                                    "border: 2px solid rgb(255, 85, 0);\n"
                                                    "\nborder-radius: 5px;\n"
                                                    "padding: 5px;\n"
                                                    "background: qradialgradient(cx: 0.3, cy: -0.6,\nfx: 0.3, fy: -0.4,\nradius: 1.35, stop: 0 rgb(0, 0, 0), stop: 1 rgb(30, 30, 30));\n"
                                                    "min-width: 80px;\n}"
                                                    "\nQPushButton:hover {\n"
                                                    "background: qradialgradient(cx: 0.3, cy: -0.4,\nfx: 0.3, fy: -0.4,\nradius: 1.35, stop: 0 rgb(80, 80, 80), stop: 1 rgb(50, 50, 50));\n}\n"
                                                    "QPushButton:pressed {\n"
                                                    "background: qradialgradient(cx: 0.4, cy: -0.1,\nfx: 0.4, fy: -0.1,\nradius: 1.35, stop: 0 #65BAAB, stop: 1 #ddd);\n}\n"
                                                    "QPushButton:disabled {\n"
                                                    "color: #888;\nborder: 2px solid rgb(189, 189, 189);\n"
                                                    "background: qradialgradient(cx: 0.4, cy: -0.1,\nfx: 0.4, fy: -0.1,\nradius: 1.35, stop: 0 rgb(104, 104, 104), stop: 1 rgb(104, 104,104));\n}"));
        verticalLayout = new QVBoxLayout(Keyboard);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineEdit = new QLineEdit(Keyboard);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);\n"
                                                  "background-color: rgb(255, 255, 255);\n"
                                                  "font: 16pt;\n"
                                                  "QPushButton {\ncolor: rgb(255, 255, 255);\n"
                                                  "border: 2px solid rgb(255, 85, 0);\n"
                                                  "\nborder-radius: 5px;\n"
                                                  "padding: 5px;\n"
                                                  "background: qradialgradient(cx: 0.3, cy: -0.6,\nfx: 0.3, fy: -0.4,\nradius: 1.35, stop: 0 rgb(0, 0, 0), stop: 1 rgb(30, 30, 30));\n"
                                                  "min-width: 80px;\n}"
                                                  "\nQPushButton:hover {\n"
                                                  "background: qradialgradient(cx: 0.3, cy: -0.4,\nfx: 0.3, fy: -0.4,\nradius: 1.35, stop: 0 rgb(80, 80, 80), stop: 1 rgb(50, 50, 50));\n}\n"
                                                  "QPushButton:pressed {\n"
                                                  "background: qradialgradient(cx: 0.4, cy: -0.1,\nfx: 0.4, fy: -0.1,\nradius: 1.35, stop: 0 #65BAAB, stop: 1 #ddd);\n}\n"
                                                  "QPushButton:disabled {\n"
                                                  "color: #888;\nborder: 2px solid rgb(189, 189, 189);\n"
                                                  "background: qradialgradient(cx: 0.4, cy: -0.1,\nfx: 0.4, fy: -0.1,\nradius: 1.35, stop: 0 rgb(104, 104, 104), stop: 1 rgb(104, 104,104));\n}"));


        horizontalLayout->addWidget(lineEdit);

        backButton = new QPushButton(Keyboard);
        backButton->setObjectName(QString::fromUtf8("backButton"));

        horizontalLayout->addWidget(backButton);

        clear = new QPushButton(Keyboard);
        clear->setObjectName(QString::fromUtf8("clear"));

        horizontalLayout->addWidget(clear);


        verticalLayout->addLayout(horizontalLayout);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));


        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);



        Button0 = new QPushButton(Keyboard);
        Button0->setObjectName(QString::fromUtf8("Button0"));
        sizePolicy1.setHeightForWidth(Button0->sizePolicy().hasHeightForWidth());
        Button0->setSizePolicy(sizePolicy1);
        Button0->setMinimumSize(QSize(20, 0));

        gridLayout->addWidget(Button0, 3, 1, 1, 1);

        Button1 = new QPushButton(Keyboard);
        Button1->setObjectName(QString::fromUtf8("Button1"));
        sizePolicy1.setHeightForWidth(Button1->sizePolicy().hasHeightForWidth());
        Button1->setSizePolicy(sizePolicy1);
        Button1->setMinimumSize(QSize(20, 0));

        gridLayout->addWidget(Button1, 2, 0, 1, 1);

        Button2 = new QPushButton(Keyboard);
        Button2->setObjectName(QString::fromUtf8("Button2"));
        sizePolicy1.setHeightForWidth(Button2->sizePolicy().hasHeightForWidth());
        Button2->setSizePolicy(sizePolicy1);
        Button2->setMinimumSize(QSize(20, 0));

        gridLayout->addWidget(Button2, 2, 1, 1, 1);

        Button3 = new QPushButton(Keyboard);
        Button3->setObjectName(QString::fromUtf8("Button3"));
        sizePolicy1.setHeightForWidth(Button3->sizePolicy().hasHeightForWidth());
        Button3->setSizePolicy(sizePolicy1);
        Button3->setMinimumSize(QSize(20, 0));

        gridLayout->addWidget(Button3, 2, 2, 1, 1);

        Button4 = new QPushButton(Keyboard);
        Button4->setObjectName(QString::fromUtf8("Button4"));
        sizePolicy1.setHeightForWidth(Button4->sizePolicy().hasHeightForWidth());
        Button4->setSizePolicy(sizePolicy1);
        Button4->setMinimumSize(QSize(20, 0));

        gridLayout->addWidget(Button4, 1, 0, 1, 1);

        Button5 = new QPushButton(Keyboard);
        Button5->setObjectName(QString::fromUtf8("Button5"));
        sizePolicy1.setHeightForWidth(Button5->sizePolicy().hasHeightForWidth());
        Button5->setSizePolicy(sizePolicy1);
        Button5->setMinimumSize(QSize(20, 0));

        gridLayout->addWidget(Button5, 1, 1, 1, 1);

        Button6 = new QPushButton(Keyboard);
        Button6->setObjectName(QString::fromUtf8("Button6"));
        sizePolicy1.setHeightForWidth(Button6->sizePolicy().hasHeightForWidth());
        Button6->setSizePolicy(sizePolicy1);
        Button6->setMinimumSize(QSize(20, 0));

        gridLayout->addWidget(Button6, 1, 2, 1, 1);

        Button7 = new QPushButton(Keyboard);
        Button7->setObjectName(QString::fromUtf8("Button7"));
        sizePolicy1.setHeightForWidth(Button7->sizePolicy().hasHeightForWidth());
        Button7->setSizePolicy(sizePolicy1);
        Button7->setMinimumSize(QSize(20, 0));

        gridLayout->addWidget(Button7, 0, 0, 1, 1);

        Button8 = new QPushButton(Keyboard);
        Button8->setObjectName(QString::fromUtf8("Button8"));
        sizePolicy1.setHeightForWidth(Button8->sizePolicy().hasHeightForWidth());
        Button8->setSizePolicy(sizePolicy1);
        Button8->setMinimumSize(QSize(20, 0));

        gridLayout->addWidget(Button8, 0, 1, 1, 1);

        Button9 = new QPushButton(Keyboard);
        Button9->setObjectName(QString::fromUtf8("Button9"));
        sizePolicy1.setHeightForWidth(Button9->sizePolicy().hasHeightForWidth());
        Button9->setSizePolicy(sizePolicy1);
        Button9->setMinimumSize(QSize(20, 0));

        gridLayout->addWidget(Button9, 0, 2, 1, 1);

        enterButton = new QPushButton(Keyboard);
        enterButton->setObjectName(QString::fromUtf8("enterButton"));
        sizePolicy1.setHeightForWidth(enterButton->sizePolicy().hasHeightForWidth());
        enterButton->setSizePolicy(sizePolicy1);
        enterButton->setMinimumSize(QSize(20, 0));

        gridLayout->addWidget(enterButton, 3, 2, 1, 1);

        commaButton = new QPushButton(Keyboard);
        commaButton->setObjectName(QString::fromUtf8("commaButton"));
        sizePolicy1.setHeightForWidth(commaButton->sizePolicy().hasHeightForWidth());
        commaButton->setSizePolicy(sizePolicy1);
        commaButton->setMinimumSize(QSize(20, 0));

        gridLayout->addWidget(commaButton, 3, 0, 1, 1);

        verticalLayout->addLayout(gridLayout);


        retranslateUi(Keyboard);

        QMetaObject::connectSlotsByName(Keyboard);
    } // setupUi

    void retranslateUi(QWidget *Keyboard)
    {
        backButton->setText(QApplication::translate("Keyboard", "Back", 0));
        clear->setText(QApplication::translate("Keyboard", "Clear", 0));
        Button0->setText(QApplication::translate("Keyboard", "0", 0));
        Button1->setText(QApplication::translate("Keyboard", "1", 0));
        Button2->setText(QApplication::translate("Keyboard", "2", 0));
        Button3->setText(QApplication::translate("Keyboard", "3", 0));
        Button4->setText(QApplication::translate("Keyboard", "4", 0));
        Button5->setText(QApplication::translate("Keyboard", "5", 0));
        Button6->setText(QApplication::translate("Keyboard", "6", 0));
        Button7->setText(QApplication::translate("Keyboard", "7", 0));
        Button8->setText(QApplication::translate("Keyboard", "8", 0));
        Button9->setText(QApplication::translate("Keyboard", "9", 0));    
        enterButton->setText(QApplication::translate("Keyboard", "Enter", 0));
        commaButton->setText(QApplication::translate("Keyboard", ".", 0));
        Q_UNUSED(Keyboard);
    } // retranslateUi

};

namespace Ui {
    class Numpad: public Ui_Numpad {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NUMPAD_H
