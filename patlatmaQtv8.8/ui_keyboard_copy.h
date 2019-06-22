/********************************************************************************
** Form generated from reading UI file 'keyboard_copy.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KEYBOARD_COPY_H
#define UI_KEYBOARD_COPY_H

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
    QPushButton *Buttonq;
    QPushButton *Buttonw;
    QPushButton *Buttone;
    QPushButton *Buttonr;
    QPushButton *Buttont;
    QPushButton *Buttony;
    QPushButton *Buttonu;
    QPushButton *Buttoni;
    QPushButton *Buttono;
    QPushButton *Buttons;
    QPushButton *Buttond;
    QPushButton *Buttonf;
    QPushButton *Buttong;
    QPushButton *Buttonh;
    QPushButton *Buttonj;
    QPushButton *Buttonl;
    QPushButton *Buttonz;
    QPushButton *Buttonx;
    QPushButton *Buttonv;
    QPushButton *Buttonb;
    QPushButton *Buttonn;
    QPushButton *Buttonm;
    QPushButton *char_2;
    QPushButton *space;
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
    QPushButton *Buttonp;
    QPushButton *enterButton;
    QPushButton *Buttona;
    QPushButton *shift;
    QPushButton *Buttonc;
    QPushButton *Buttonk;

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
        Keyboard->setStyleSheet(QLatin1String("\n"
"QPushButton {\n"
"color: rgb(255, 255, 255);\n"
"font-family: \"Gill Sans MT\";\n"
"font-size: 21px;\n"
"border: 2px solid rgb(255, 85, 0);\n"
"/*border: 2px solid rgb(0, 115, 255);*/\n"
"border-radius: 5px;\n"
"padding: 5px;\n"
"background: qradialgradient(cx: 0.3, cy: -0.6,\n"
"fx: 0.3, fy: -0.4,\n"
"radius: 1.35, stop: 0 rgb(0, 0, 0), stop: 1 rgb(30, 30, 30));\n"
"min-width: 10px;\n"
"}\n"
"QPushButton:hover {\n"
"background: qradialgradient(cx: 0.3, cy: -0.4,\n"
"fx: 0.3, fy: -0.4,\n"
"radius: 1.35, stop: 0 rgb(80, 80, 80), stop: 1 rgb(50, 50, 50));\n"
"}\n"
"QPushButton:pressed {\n"
"background: qradialgradient(cx: 0.4, cy: -0.1,\n"
"fx: 0.4, fy: -0.1,\n"
"radius: 1.35, stop: 0 #65BAAB, stop: 1 #ddd);\n"
"}\n"
"QPushButton:disabled {\n"
"color: #888;\n"
"border: 2px solid rgb(189, 189, 189);\n"
"background: qradialgradient(cx: 0.4, cy: -0.1,\n"
"fx: 0.4, fy: -0.1,\n"
"radius: 1.35, stop: 0 rgb(104, 104, 104), stop: 1 rgb(104, 104,104));\n"
"}"));
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

        horizontalLayout->addWidget(backButton);

        clear = new QPushButton(Keyboard);
        clear->setObjectName(QStringLiteral("clear"));

        horizontalLayout->addWidget(clear);


        verticalLayout->addLayout(horizontalLayout);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(2);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        Buttonq = new QPushButton(Keyboard);
        Buttonq->setObjectName(QStringLiteral("Buttonq"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(Buttonq->sizePolicy().hasHeightForWidth());
        Buttonq->setSizePolicy(sizePolicy1);
        Buttonq->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonq, 1, 0, 1, 1);

        Buttonw = new QPushButton(Keyboard);
        Buttonw->setObjectName(QStringLiteral("Buttonw"));
        sizePolicy1.setHeightForWidth(Buttonw->sizePolicy().hasHeightForWidth());
        Buttonw->setSizePolicy(sizePolicy1);
        Buttonw->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonw, 1, 1, 1, 1);

        Buttone = new QPushButton(Keyboard);
        Buttone->setObjectName(QStringLiteral("Buttone"));
        sizePolicy1.setHeightForWidth(Buttone->sizePolicy().hasHeightForWidth());
        Buttone->setSizePolicy(sizePolicy1);
        Buttone->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttone, 1, 2, 1, 1);

        Buttonr = new QPushButton(Keyboard);
        Buttonr->setObjectName(QStringLiteral("Buttonr"));
        sizePolicy1.setHeightForWidth(Buttonr->sizePolicy().hasHeightForWidth());
        Buttonr->setSizePolicy(sizePolicy1);
        Buttonr->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonr, 1, 3, 1, 1);

        Buttont = new QPushButton(Keyboard);
        Buttont->setObjectName(QStringLiteral("Buttont"));
        sizePolicy1.setHeightForWidth(Buttont->sizePolicy().hasHeightForWidth());
        Buttont->setSizePolicy(sizePolicy1);
        Buttont->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttont, 1, 4, 1, 1);

        Buttony = new QPushButton(Keyboard);
        Buttony->setObjectName(QStringLiteral("Buttony"));
        sizePolicy1.setHeightForWidth(Buttony->sizePolicy().hasHeightForWidth());
        Buttony->setSizePolicy(sizePolicy1);
        Buttony->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttony, 1, 5, 1, 1);

        Buttonu = new QPushButton(Keyboard);
        Buttonu->setObjectName(QStringLiteral("Buttonu"));
        sizePolicy1.setHeightForWidth(Buttonu->sizePolicy().hasHeightForWidth());
        Buttonu->setSizePolicy(sizePolicy1);
        Buttonu->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonu, 1, 6, 1, 1);

        Buttoni = new QPushButton(Keyboard);
        Buttoni->setObjectName(QStringLiteral("Buttoni"));
        sizePolicy1.setHeightForWidth(Buttoni->sizePolicy().hasHeightForWidth());
        Buttoni->setSizePolicy(sizePolicy1);
        Buttoni->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttoni, 1, 7, 1, 1);

        Buttono = new QPushButton(Keyboard);
        Buttono->setObjectName(QStringLiteral("Buttono"));
        sizePolicy1.setHeightForWidth(Buttono->sizePolicy().hasHeightForWidth());
        Buttono->setSizePolicy(sizePolicy1);
        Buttono->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttono, 1, 8, 1, 1);

        Buttons = new QPushButton(Keyboard);
        Buttons->setObjectName(QStringLiteral("Buttons"));
        sizePolicy1.setHeightForWidth(Buttons->sizePolicy().hasHeightForWidth());
        Buttons->setSizePolicy(sizePolicy1);
        Buttons->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttons, 3, 1, 1, 1);

        Buttond = new QPushButton(Keyboard);
        Buttond->setObjectName(QStringLiteral("Buttond"));
        sizePolicy1.setHeightForWidth(Buttond->sizePolicy().hasHeightForWidth());
        Buttond->setSizePolicy(sizePolicy1);
        Buttond->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttond, 3, 2, 1, 1);

        Buttonf = new QPushButton(Keyboard);
        Buttonf->setObjectName(QStringLiteral("Buttonf"));
        sizePolicy1.setHeightForWidth(Buttonf->sizePolicy().hasHeightForWidth());
        Buttonf->setSizePolicy(sizePolicy1);
        Buttonf->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonf, 3, 3, 1, 1);

        Buttong = new QPushButton(Keyboard);
        Buttong->setObjectName(QStringLiteral("Buttong"));
        sizePolicy1.setHeightForWidth(Buttong->sizePolicy().hasHeightForWidth());
        Buttong->setSizePolicy(sizePolicy1);
        Buttong->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttong, 3, 4, 1, 1);

        Buttonh = new QPushButton(Keyboard);
        Buttonh->setObjectName(QStringLiteral("Buttonh"));
        sizePolicy1.setHeightForWidth(Buttonh->sizePolicy().hasHeightForWidth());
        Buttonh->setSizePolicy(sizePolicy1);
        Buttonh->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonh, 3, 5, 1, 1);

        Buttonj = new QPushButton(Keyboard);
        Buttonj->setObjectName(QStringLiteral("Buttonj"));
        sizePolicy1.setHeightForWidth(Buttonj->sizePolicy().hasHeightForWidth());
        Buttonj->setSizePolicy(sizePolicy1);
        Buttonj->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonj, 3, 6, 1, 1);

        Buttonl = new QPushButton(Keyboard);
        Buttonl->setObjectName(QStringLiteral("Buttonl"));
        sizePolicy1.setHeightForWidth(Buttonl->sizePolicy().hasHeightForWidth());
        Buttonl->setSizePolicy(sizePolicy1);
        Buttonl->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonl, 3, 8, 1, 1);

        Buttonz = new QPushButton(Keyboard);
        Buttonz->setObjectName(QStringLiteral("Buttonz"));
        sizePolicy1.setHeightForWidth(Buttonz->sizePolicy().hasHeightForWidth());
        Buttonz->setSizePolicy(sizePolicy1);
        Buttonz->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonz, 4, 1, 1, 1);

        Buttonx = new QPushButton(Keyboard);
        Buttonx->setObjectName(QStringLiteral("Buttonx"));
        sizePolicy1.setHeightForWidth(Buttonx->sizePolicy().hasHeightForWidth());
        Buttonx->setSizePolicy(sizePolicy1);
        Buttonx->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonx, 4, 2, 1, 1);

        Buttonv = new QPushButton(Keyboard);
        Buttonv->setObjectName(QStringLiteral("Buttonv"));
        sizePolicy1.setHeightForWidth(Buttonv->sizePolicy().hasHeightForWidth());
        Buttonv->setSizePolicy(sizePolicy1);
        Buttonv->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonv, 4, 4, 1, 1);

        Buttonb = new QPushButton(Keyboard);
        Buttonb->setObjectName(QStringLiteral("Buttonb"));
        sizePolicy1.setHeightForWidth(Buttonb->sizePolicy().hasHeightForWidth());
        Buttonb->setSizePolicy(sizePolicy1);
        Buttonb->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonb, 4, 5, 1, 1);

        Buttonn = new QPushButton(Keyboard);
        Buttonn->setObjectName(QStringLiteral("Buttonn"));
        sizePolicy1.setHeightForWidth(Buttonn->sizePolicy().hasHeightForWidth());
        Buttonn->setSizePolicy(sizePolicy1);
        Buttonn->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonn, 4, 6, 1, 1);

        Buttonm = new QPushButton(Keyboard);
        Buttonm->setObjectName(QStringLiteral("Buttonm"));
        sizePolicy1.setHeightForWidth(Buttonm->sizePolicy().hasHeightForWidth());
        Buttonm->setSizePolicy(sizePolicy1);
        Buttonm->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonm, 4, 7, 1, 1);

        char_2 = new QPushButton(Keyboard);
        char_2->setObjectName(QStringLiteral("char_2"));
        sizePolicy1.setHeightForWidth(char_2->sizePolicy().hasHeightForWidth());
        char_2->setSizePolicy(sizePolicy1);
        char_2->setMinimumSize(QSize(24, 0));
        char_2->setCheckable(true);
        char_2->setChecked(false);

        gridLayout->addWidget(char_2, 4, 8, 2, 1);

        space = new QPushButton(Keyboard);
        space->setObjectName(QStringLiteral("space"));
        space->setMinimumSize(QSize(24, 50));

        gridLayout->addWidget(space, 5, 1, 1, 7);

        Button0 = new QPushButton(Keyboard);
        Button0->setObjectName(QStringLiteral("Button0"));
        sizePolicy1.setHeightForWidth(Button0->sizePolicy().hasHeightForWidth());
        Button0->setSizePolicy(sizePolicy1);
        Button0->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Button0, 0, 0, 1, 1);

        Button1 = new QPushButton(Keyboard);
        Button1->setObjectName(QStringLiteral("Button1"));
        sizePolicy1.setHeightForWidth(Button1->sizePolicy().hasHeightForWidth());
        Button1->setSizePolicy(sizePolicy1);
        Button1->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Button1, 0, 1, 1, 1);

        Button2 = new QPushButton(Keyboard);
        Button2->setObjectName(QStringLiteral("Button2"));
        sizePolicy1.setHeightForWidth(Button2->sizePolicy().hasHeightForWidth());
        Button2->setSizePolicy(sizePolicy1);
        Button2->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Button2, 0, 2, 1, 1);

        Button3 = new QPushButton(Keyboard);
        Button3->setObjectName(QStringLiteral("Button3"));
        sizePolicy1.setHeightForWidth(Button3->sizePolicy().hasHeightForWidth());
        Button3->setSizePolicy(sizePolicy1);
        Button3->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Button3, 0, 3, 1, 1);

        Button4 = new QPushButton(Keyboard);
        Button4->setObjectName(QStringLiteral("Button4"));
        sizePolicy1.setHeightForWidth(Button4->sizePolicy().hasHeightForWidth());
        Button4->setSizePolicy(sizePolicy1);
        Button4->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Button4, 0, 4, 1, 1);

        Button5 = new QPushButton(Keyboard);
        Button5->setObjectName(QStringLiteral("Button5"));
        sizePolicy1.setHeightForWidth(Button5->sizePolicy().hasHeightForWidth());
        Button5->setSizePolicy(sizePolicy1);
        Button5->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Button5, 0, 5, 1, 1);

        Button6 = new QPushButton(Keyboard);
        Button6->setObjectName(QStringLiteral("Button6"));
        sizePolicy1.setHeightForWidth(Button6->sizePolicy().hasHeightForWidth());
        Button6->setSizePolicy(sizePolicy1);
        Button6->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Button6, 0, 6, 1, 1);

        Button7 = new QPushButton(Keyboard);
        Button7->setObjectName(QStringLiteral("Button7"));
        sizePolicy1.setHeightForWidth(Button7->sizePolicy().hasHeightForWidth());
        Button7->setSizePolicy(sizePolicy1);
        Button7->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Button7, 0, 7, 1, 1);

        Button8 = new QPushButton(Keyboard);
        Button8->setObjectName(QStringLiteral("Button8"));
        sizePolicy1.setHeightForWidth(Button8->sizePolicy().hasHeightForWidth());
        Button8->setSizePolicy(sizePolicy1);
        Button8->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Button8, 0, 8, 1, 1);

        Button9 = new QPushButton(Keyboard);
        Button9->setObjectName(QStringLiteral("Button9"));
        sizePolicy1.setHeightForWidth(Button9->sizePolicy().hasHeightForWidth());
        Button9->setSizePolicy(sizePolicy1);
        Button9->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Button9, 0, 9, 1, 1);

        Buttonp = new QPushButton(Keyboard);
        Buttonp->setObjectName(QStringLiteral("Buttonp"));
        sizePolicy1.setHeightForWidth(Buttonp->sizePolicy().hasHeightForWidth());
        Buttonp->setSizePolicy(sizePolicy1);
        Buttonp->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonp, 1, 9, 1, 1);

        enterButton = new QPushButton(Keyboard);
        enterButton->setObjectName(QStringLiteral("enterButton"));
        sizePolicy1.setHeightForWidth(enterButton->sizePolicy().hasHeightForWidth());
        enterButton->setSizePolicy(sizePolicy1);
        enterButton->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(enterButton, 3, 9, 3, 1);

        Buttona = new QPushButton(Keyboard);
        Buttona->setObjectName(QStringLiteral("Buttona"));
        sizePolicy1.setHeightForWidth(Buttona->sizePolicy().hasHeightForWidth());
        Buttona->setSizePolicy(sizePolicy1);
        Buttona->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttona, 3, 0, 1, 1);

        shift = new QPushButton(Keyboard);
        shift->setObjectName(QStringLiteral("shift"));
        sizePolicy1.setHeightForWidth(shift->sizePolicy().hasHeightForWidth());
        shift->setSizePolicy(sizePolicy1);
        shift->setMinimumSize(QSize(24, 0));
        shift->setStyleSheet(QStringLiteral(""));
        shift->setCheckable(false);

        gridLayout->addWidget(shift, 4, 0, 2, 1);

        Buttonc = new QPushButton(Keyboard);
        Buttonc->setObjectName(QStringLiteral("Buttonc"));
        sizePolicy1.setHeightForWidth(Buttonc->sizePolicy().hasHeightForWidth());
        Buttonc->setSizePolicy(sizePolicy1);
        Buttonc->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonc, 4, 3, 1, 1);

        Buttonk = new QPushButton(Keyboard);
        Buttonk->setObjectName(QStringLiteral("Buttonk"));
        sizePolicy1.setHeightForWidth(Buttonk->sizePolicy().hasHeightForWidth());
        Buttonk->setSizePolicy(sizePolicy1);
        Buttonk->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(Buttonk, 3, 7, 1, 1);


        verticalLayout->addLayout(gridLayout);


        retranslateUi(Keyboard);

        QMetaObject::connectSlotsByName(Keyboard);
    } // setupUi

    void retranslateUi(QWidget *Keyboard)
    {
        backButton->setText(QApplication::translate("Keyboard", "<<", Q_NULLPTR));
        clear->setText(QApplication::translate("Keyboard", "Temizle", Q_NULLPTR));
        Buttonq->setText(QApplication::translate("Keyboard", "q", Q_NULLPTR));
        Buttonw->setText(QApplication::translate("Keyboard", "w", Q_NULLPTR));
        Buttone->setText(QApplication::translate("Keyboard", "e", Q_NULLPTR));
        Buttonr->setText(QApplication::translate("Keyboard", "r", Q_NULLPTR));
        Buttont->setText(QApplication::translate("Keyboard", "t", Q_NULLPTR));
        Buttony->setText(QApplication::translate("Keyboard", "y", Q_NULLPTR));
        Buttonu->setText(QApplication::translate("Keyboard", "u", Q_NULLPTR));
        Buttoni->setText(QApplication::translate("Keyboard", "i", Q_NULLPTR));
        Buttono->setText(QApplication::translate("Keyboard", "o", Q_NULLPTR));
        Buttons->setText(QApplication::translate("Keyboard", "s", Q_NULLPTR));
        Buttond->setText(QApplication::translate("Keyboard", "d", Q_NULLPTR));
        Buttonf->setText(QApplication::translate("Keyboard", "f", Q_NULLPTR));
        Buttong->setText(QApplication::translate("Keyboard", "g", Q_NULLPTR));
        Buttonh->setText(QApplication::translate("Keyboard", "h", Q_NULLPTR));
        Buttonj->setText(QApplication::translate("Keyboard", "j", Q_NULLPTR));
        Buttonl->setText(QApplication::translate("Keyboard", "l", Q_NULLPTR));
        Buttonz->setText(QApplication::translate("Keyboard", "z", Q_NULLPTR));
        Buttonx->setText(QApplication::translate("Keyboard", "x", Q_NULLPTR));
        Buttonv->setText(QApplication::translate("Keyboard", "v", Q_NULLPTR));
        Buttonb->setText(QApplication::translate("Keyboard", "b", Q_NULLPTR));
        Buttonn->setText(QApplication::translate("Keyboard", "n", Q_NULLPTR));
        Buttonm->setText(QApplication::translate("Keyboard", "m", Q_NULLPTR));
        char_2->setText(QApplication::translate("Keyboard", "Char", Q_NULLPTR));
        space->setText(QApplication::translate("Keyboard", "bosluk", Q_NULLPTR));
        Button0->setText(QApplication::translate("Keyboard", "0", Q_NULLPTR));
        Button1->setText(QApplication::translate("Keyboard", "1", Q_NULLPTR));
        Button2->setText(QApplication::translate("Keyboard", "2", Q_NULLPTR));
        Button3->setText(QApplication::translate("Keyboard", "3", Q_NULLPTR));
        Button4->setText(QApplication::translate("Keyboard", "4", Q_NULLPTR));
        Button5->setText(QApplication::translate("Keyboard", "5", Q_NULLPTR));
        Button6->setText(QApplication::translate("Keyboard", "6", Q_NULLPTR));
        Button7->setText(QApplication::translate("Keyboard", "7", Q_NULLPTR));
        Button8->setText(QApplication::translate("Keyboard", "8", Q_NULLPTR));
        Button9->setText(QApplication::translate("Keyboard", "9", Q_NULLPTR));
        Buttonp->setText(QApplication::translate("Keyboard", "p", Q_NULLPTR));
        enterButton->setText(QApplication::translate("Keyboard", "Enter", Q_NULLPTR));
        Buttona->setText(QApplication::translate("Keyboard", "a", Q_NULLPTR));
        shift->setText(QApplication::translate("Keyboard", "Shift", Q_NULLPTR));
        Buttonc->setText(QApplication::translate("Keyboard", "c", Q_NULLPTR));
        Buttonk->setText(QApplication::translate("Keyboard", "k", Q_NULLPTR));
        Q_UNUSED(Keyboard);
    } // retranslateUi

};

namespace Ui {
    class Keyboard: public Ui_Keyboard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KEYBOARD_COPY_H
