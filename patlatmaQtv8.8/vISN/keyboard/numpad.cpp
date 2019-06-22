/*
 * Author: LT Thomas <ltjr@ti.com>
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/


#include "numpad.h"
#include "ui_numpad.h"
#include <QtGui>

Numpad::Numpad(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Numpad)
{
    ui->setupUi(this);



    connect ( ui->Button0, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button1, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button2, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button3, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button4, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button5, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button6, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button7, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button8, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button9, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->commaButton, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );



    outputText = "";
    shift = false;
    ui->lineEdit->setFocus();

}

void Numpad::keyboardHandler()
{
    QPushButton *button = (QPushButton *)sender();
    //qDebug() << "pressed " << button->text();

    QString inputText = button->text();

    if (inputText == "Space")
    {
        outputText += " ";
    }
    else if(inputText == "&&")
    {
        outputText += "&";
    }
    else if(inputText == "\\")
    {
    }
    else
    {
        if(shift)
        {
            shift = false;
            outputText += inputText.toUpper();
        }
        else
        {
            outputText += inputText;
        }
    }


    ui->lineEdit->setText(outputText);

}

void Numpad::setLineEdit(QLineEdit * line)
{
    outputLineEdit = line;
}

Numpad::~Numpad()
{
    delete ui;
}


void Numpad::on_shift_clicked()
{
    shift = true;
}


void Numpad::on_char_2_toggled(bool checked)
{
    if(checked)
    {
        ui->Button0->setText("`");
        ui->Button1->setText("~");
        ui->Button2->setText("!");
        ui->Button3->setText("@");
        ui->Button4->setText("#");
        ui->Button5->setText("$");
        ui->Button6->setText("%");
        ui->Button7->setText("^");
        ui->Button8->setText("&&");// trUtf8("\u0040")
        ui->Button9->setText("*");




    }
    else
    {
        ui->Button0->setText("0");
        ui->Button1->setText("1");
        ui->Button2->setText("2");
        ui->Button3->setText("3");
        ui->Button4->setText("4");
        ui->Button5->setText("5");
        ui->Button6->setText("6");
        ui->Button7->setText("7");
        ui->Button8->setText("8");
        ui->Button9->setText("9");
        ui->commaButton->setText(".");

    }
}

void Numpad::on_clear_clicked()
{
    outputText="";
    ui->lineEdit->setText(outputText);
}



void Numpad::on_enterButton_clicked()
{
    //qDebug() << "enter";
    outputLineEdit->setText(outputText);
    outputText="";
    ui->lineEdit->setText(outputText);
    close();
}

void Numpad::on_lineEdit_textChanged(const QString &arg1)
{
    outputText = arg1;
    ui->lineEdit->setText(outputText);

}

void Numpad::on_lineEdit_returnPressed()
{
    //qDebug() << "enter";
    outputLineEdit->setText(outputText);
    outputText="";
    ui->lineEdit->setText(outputText);
    close();
}

void Numpad::on_backButton_clicked()
{
    outputText.remove(outputText.length()-1,outputText.length());
    ui->lineEdit->setText(outputText);
}


