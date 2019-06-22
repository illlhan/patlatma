/********************************************************************************
** Form generated from reading UI file 'numpad.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NUMPAD_H
#define UI_NUMPAD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Keyboard
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit;
    QPushButton *backButton;
    QPushButton *clear;
    QGridLayout *gridLayout;
    QPushButton *Button9;
    QPushButton *Button1;
    QPushButton *Button7;
    QPushButton *Button4;
    QPushButton *Button6;
    QPushButton *Button5;
    QPushButton *Button8;
    QPushButton *Button2;
    QPushButton *Button3;
    QPushButton *enterButton;
    QPushButton *Button0;
    QPushButton *commaButton;

    void setupUi(QWidget *Keyboard)
    {
        if (Keyboard->objectName().isEmpty())
            Keyboard->setObjectName(QStringLiteral("Keyboard"));
        Keyboard->setWindowModality(Qt::NonModal);
        Keyboard->resize(600, 315);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Keyboard->sizePolicy().hasHeightForWidth());
        Keyboard->setSizePolicy(sizePolicy);
        Keyboard->setWindowTitle(QStringLiteral("Keyboard"));
        Keyboard->setStyleSheet(QStringLiteral(""));
        verticalLayout = new QVBoxLayout(Keyboard);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        lineEdit = new QLineEdit(Keyboard);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setStyleSheet(QLatin1String("color: rgb(0, 0, 0);\n"
"background-color: rgb(255, 255, 255);"));

        horizontalLayout->addWidget(lineEdit);

        backButton = new QPushButton(Keyboard);
        backButton->setObjectName(QStringLiteral("backButton"));
        backButton->setStyleSheet(QStringLiteral("<"));

        horizontalLayout->addWidget(backButton);

        clear = new QPushButton(Keyboard);
        clear->setObjectName(QStringLiteral("clear"));

        horizontalLayout->addWidget(clear);


        verticalLayout->addLayout(horizontalLayout);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(2);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        Button9 = new QPushButton(Keyboard);
        Button9->setObjectName(QStringLiteral("Button9"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(Button9->sizePolicy().hasHeightForWidth());
        Button9->setSizePolicy(sizePolicy1);
        Button9->setMinimumSize(QSize(94, 0));

        gridLayout->addWidget(Button9, 0, 2, 1, 1);

        Button1 = new QPushButton(Keyboard);
        Button1->setObjectName(QStringLiteral("Button1"));
        sizePolicy1.setHeightForWidth(Button1->sizePolicy().hasHeightForWidth());
        Button1->setSizePolicy(sizePolicy1);
        Button1->setMinimumSize(QSize(94, 0));

        gridLayout->addWidget(Button1, 3, 0, 1, 1);

        Button7 = new QPushButton(Keyboard);
        Button7->setObjectName(QStringLiteral("Button7"));
        sizePolicy1.setHeightForWidth(Button7->sizePolicy().hasHeightForWidth());
        Button7->setSizePolicy(sizePolicy1);
        Button7->setMinimumSize(QSize(94, 0));

        gridLayout->addWidget(Button7, 0, 0, 1, 1);

        Button4 = new QPushButton(Keyboard);
        Button4->setObjectName(QStringLiteral("Button4"));
        sizePolicy1.setHeightForWidth(Button4->sizePolicy().hasHeightForWidth());
        Button4->setSizePolicy(sizePolicy1);
        Button4->setMinimumSize(QSize(94, 0));

        gridLayout->addWidget(Button4, 1, 0, 1, 1);

        Button6 = new QPushButton(Keyboard);
        Button6->setObjectName(QStringLiteral("Button6"));
        sizePolicy1.setHeightForWidth(Button6->sizePolicy().hasHeightForWidth());
        Button6->setSizePolicy(sizePolicy1);
        Button6->setMinimumSize(QSize(94, 0));

        gridLayout->addWidget(Button6, 1, 2, 1, 1);

        Button5 = new QPushButton(Keyboard);
        Button5->setObjectName(QStringLiteral("Button5"));
        sizePolicy1.setHeightForWidth(Button5->sizePolicy().hasHeightForWidth());
        Button5->setSizePolicy(sizePolicy1);
        Button5->setMinimumSize(QSize(94, 0));

        gridLayout->addWidget(Button5, 1, 1, 1, 1);

        Button8 = new QPushButton(Keyboard);
        Button8->setObjectName(QStringLiteral("Button8"));
        sizePolicy1.setHeightForWidth(Button8->sizePolicy().hasHeightForWidth());
        Button8->setSizePolicy(sizePolicy1);
        Button8->setMinimumSize(QSize(94, 0));

        gridLayout->addWidget(Button8, 0, 1, 1, 1);

        Button2 = new QPushButton(Keyboard);
        Button2->setObjectName(QStringLiteral("Button2"));
        sizePolicy1.setHeightForWidth(Button2->sizePolicy().hasHeightForWidth());
        Button2->setSizePolicy(sizePolicy1);
        Button2->setMinimumSize(QSize(94, 0));

        gridLayout->addWidget(Button2, 3, 1, 1, 1);

        Button3 = new QPushButton(Keyboard);
        Button3->setObjectName(QStringLiteral("Button3"));
        sizePolicy1.setHeightForWidth(Button3->sizePolicy().hasHeightForWidth());
        Button3->setSizePolicy(sizePolicy1);
        Button3->setMinimumSize(QSize(94, 0));

        gridLayout->addWidget(Button3, 3, 2, 1, 1);

        enterButton = new QPushButton(Keyboard);
        enterButton->setObjectName(QStringLiteral("enterButton"));
        sizePolicy1.setHeightForWidth(enterButton->sizePolicy().hasHeightForWidth());
        enterButton->setSizePolicy(sizePolicy1);
        enterButton->setMinimumSize(QSize(94, 0));

        gridLayout->addWidget(enterButton, 4, 2, 1, 1);

        Button0 = new QPushButton(Keyboard);
        Button0->setObjectName(QStringLiteral("Button0"));
        sizePolicy1.setHeightForWidth(Button0->sizePolicy().hasHeightForWidth());
        Button0->setSizePolicy(sizePolicy1);
        Button0->setMinimumSize(QSize(94, 0));

        gridLayout->addWidget(Button0, 4, 1, 1, 1);

        commaButton = new QPushButton(Keyboard);
        commaButton->setObjectName(QStringLiteral("commaButton"));
        sizePolicy1.setHeightForWidth(commaButton->sizePolicy().hasHeightForWidth());
        commaButton->setSizePolicy(sizePolicy1);
        commaButton->setMinimumSize(QSize(94, 0));

        gridLayout->addWidget(commaButton, 4, 0, 1, 1);


        verticalLayout->addLayout(gridLayout);


        retranslateUi(Keyboard);

        QMetaObject::connectSlotsByName(Keyboard);
    } // setupUi

    void retranslateUi(QWidget *Keyboard)
    {
        backButton->setText(QApplication::translate("Keyboard", "<<", Q_NULLPTR));
        clear->setText(QApplication::translate("Keyboard", "Temizle", Q_NULLPTR));
        Button9->setText(QApplication::translate("Keyboard", "9", Q_NULLPTR));
        Button1->setText(QApplication::translate("Keyboard", "1", Q_NULLPTR));
        Button7->setText(QApplication::translate("Keyboard", "7", Q_NULLPTR));
        Button4->setText(QApplication::translate("Keyboard", "4", Q_NULLPTR));
        Button6->setText(QApplication::translate("Keyboard", "6", Q_NULLPTR));
        Button5->setText(QApplication::translate("Keyboard", "5", Q_NULLPTR));
        Button8->setText(QApplication::translate("Keyboard", "8", Q_NULLPTR));
        Button2->setText(QApplication::translate("Keyboard", "2", Q_NULLPTR));
        Button3->setText(QApplication::translate("Keyboard", "3", Q_NULLPTR));
        enterButton->setText(QApplication::translate("Keyboard", "Enter", Q_NULLPTR));
        Button0->setText(QApplication::translate("Keyboard", "0", Q_NULLPTR));
        commaButton->setText(QApplication::translate("Keyboard", ",", Q_NULLPTR));
        Q_UNUSED(Keyboard);
    } // retranslateUi

};

namespace Ui {
    class Keyboard: public Ui_Keyboard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NUMPAD_H
