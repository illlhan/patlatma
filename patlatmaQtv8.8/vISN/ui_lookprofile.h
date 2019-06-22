/********************************************************************************
** Form generated from reading UI file 'lookprofile.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOOKPROFILE_H
#define UI_LOOKPROFILE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_lookProfile
{
public:
    QTableWidget *tableWidget;
    QPushButton *btnLookEditProfile;
    QPushButton *btnLookCancelProfile;
    QLabel *label;
    QLabel *label_2;
    QSpinBox *spinBox;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QFrame *line;
    QFrame *line_2;

    void setupUi(QDialog *lookProfile)
    {
        if (lookProfile->objectName().isEmpty())
            lookProfile->setObjectName(QStringLiteral("lookProfile"));
        lookProfile->resize(630, 516);
        lookProfile->setStyleSheet(QLatin1String("lookProfile\n"
"{\n"
"color : rgb(255, 255, 255);\n"
"background-color: rgb(43, 43, 43);\n"
"}\n"
""));
        tableWidget = new QTableWidget(lookProfile);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setGeometry(QRect(20, 20, 411, 481));
        QFont font;
        font.setPointSize(16);
        tableWidget->setFont(font);
        tableWidget->setStyleSheet(QLatin1String("QTableView {\n"
"color: rgb(255, 85, 0);\n"
"background-color: rgb(44, 44, 44);\n"
"font: 16pt \"Segoe UI\";\n"
"border-style: outset;\n"
"border-width: 2px;\n"
"border-radius: 10px;\n"
"}\n"
"QHeaderView::section \n"
"{ \n"
"background-color:rgb(76, 76, 76);\n"
"font: 16pt \"Segoe UI\";\n"
"}\n"
""));
        tableWidget->horizontalHeader()->setVisible(false);
        tableWidget->horizontalHeader()->setStretchLastSection(false);
        tableWidget->verticalHeader()->setVisible(false);
        tableWidget->verticalHeader()->setDefaultSectionSize(30);
        tableWidget->verticalHeader()->setMinimumSectionSize(19);
        tableWidget->verticalHeader()->setStretchLastSection(false);
        btnLookEditProfile = new QPushButton(lookProfile);
        btnLookEditProfile->setObjectName(QStringLiteral("btnLookEditProfile"));
        btnLookEditProfile->setGeometry(QRect(440, 350, 181, 61));
        btnLookEditProfile->setStyleSheet(QLatin1String("color : rgb(255, 85, 0);\n"
"border-color: rgb(255, 85, 0);\n"
"background-color: rgb(0, 0, 0);\n"
"font: 16pt \"Segoe UI\";\n"
"border-style: outset;\n"
"border-width: 2px;\n"
"border-radius: 10px;"));
        btnLookCancelProfile = new QPushButton(lookProfile);
        btnLookCancelProfile->setObjectName(QStringLiteral("btnLookCancelProfile"));
        btnLookCancelProfile->setGeometry(QRect(440, 430, 181, 61));
        btnLookCancelProfile->setStyleSheet(QLatin1String("color : rgb(255, 85, 0);\n"
"border-color: rgb(255, 85, 0);\n"
"background-color: rgb(0, 0, 0);\n"
"font: 16pt \"Segoe UI\";\n"
"border-style: outset;\n"
"border-width: 2px;\n"
"border-radius: 10px;"));
        label = new QLabel(lookProfile);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(440, 30, 161, 21));
        QFont font1;
        font1.setFamily(QStringLiteral("Segoe UI"));
        font1.setPointSize(16);
        label->setFont(font1);
        label->setStyleSheet(QStringLiteral("color: rgb(255, 85, 0);"));
        label_2 = new QLabel(lookProfile);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(440, 80, 171, 31));
        label_2->setFont(font1);
        label_2->setStyleSheet(QStringLiteral("color: rgb(255, 85, 0);"));
        label_2->setAlignment(Qt::AlignCenter);
        spinBox = new QSpinBox(lookProfile);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setGeometry(QRect(500, 120, 61, 61));
        QFont font2;
        font2.setFamily(QStringLiteral("Segoe UI"));
        font2.setPointSize(20);
        font2.setBold(false);
        font2.setItalic(false);
        font2.setWeight(50);
        spinBox->setFont(font2);
        spinBox->setStyleSheet(QStringLiteral("font: 20pt \"Segoe UI\";"));
        spinBox->setAlignment(Qt::AlignCenter);
        spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        pushButton = new QPushButton(lookProfile);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(570, 120, 51, 61));
        pushButton->setStyleSheet(QLatin1String("color : rgb(255, 85, 0);\n"
"border-color: rgb(255, 85, 0);\n"
"background-color: rgb(0, 0, 0);\n"
"font: 16pt \"Segoe UI\";\n"
"border-style: outset;\n"
"border-width: 2px;\n"
"border-radius: 10px;"));
        pushButton_2 = new QPushButton(lookProfile);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(440, 120, 51, 61));
        pushButton_2->setStyleSheet(QLatin1String("color : rgb(255, 85, 0);\n"
"border-color: rgb(255, 85, 0);\n"
"background-color: rgb(0, 0, 0);\n"
"font: 16pt \"Segoe UI\";\n"
"border-style: outset;\n"
"border-width: 2px;\n"
"border-radius: 10px;"));
        line = new QFrame(lookProfile);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(440, 70, 181, 16));
        line->setStyleSheet(QStringLiteral("color: rgb(255, 85, 0);"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(lookProfile);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setGeometry(QRect(440, 190, 181, 16));
        line_2->setStyleSheet(QStringLiteral("color: rgb(255, 85, 0);"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        retranslateUi(lookProfile);

        QMetaObject::connectSlotsByName(lookProfile);
    } // setupUi

    void retranslateUi(QDialog *lookProfile)
    {
        lookProfile->setWindowTitle(QApplication::translate("lookProfile", "Dialog", Q_NULLPTR));
        btnLookEditProfile->setText(QApplication::translate("lookProfile", "Kaydet", Q_NULLPTR));
        btnLookCancelProfile->setText(QApplication::translate("lookProfile", "Geri al", Q_NULLPTR));
        label->setText(QApplication::translate("lookProfile", "TextLabel", Q_NULLPTR));
        label_2->setText(QApplication::translate("lookProfile", "Ad\304\261m Say\304\261s\304\261", Q_NULLPTR));
        pushButton->setText(QApplication::translate("lookProfile", "+", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("lookProfile", "-", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class lookProfile: public Ui_lookProfile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOOKPROFILE_H
