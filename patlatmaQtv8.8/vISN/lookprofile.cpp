#include "lookprofile.h"
#include "ui_lookprofile.h"
#include <QComboBox>

profileStruct tLookProfile[MaxProfile];

lookProfile::lookProfile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lookProfile)
{

    ui->setupUi(this);
    ui->tableWidget->setRowCount(21);
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setItem(0, 1, new QTableWidgetItem("Süre"));
    ui->tableWidget->setItem(0, 2, new QTableWidgetItem("Birim"));
    ui->tableWidget->setItem(0, 3, new QTableWidgetItem("Sıcaklık (°C)"));
    ui->tableWidget->setItem(0, 4, new QTableWidgetItem("Nem (%rh)"));
    ui->tableWidget->setItem(1, 0, new QTableWidgetItem("Başlangıç"));
    ui->tableWidget->setItem(2, 0, new QTableWidgetItem("Adım 1"));
    ui->tableWidget->setItem(3, 0, new QTableWidgetItem("Adım 2"));
    ui->tableWidget->setItem(4, 0, new QTableWidgetItem("Adım 3"));
    ui->tableWidget->setItem(5, 0, new QTableWidgetItem("Adım 4"));
    ui->tableWidget->setItem(6, 0, new QTableWidgetItem("Adım 5"));
    ui->tableWidget->setItem(7, 0, new QTableWidgetItem("Adım 6"));
    ui->tableWidget->setItem(8, 0, new QTableWidgetItem("Adım 7"));
    ui->tableWidget->setItem(9, 0, new QTableWidgetItem("Adım 8"));
    ui->tableWidget->setItem(10, 0, new QTableWidgetItem("Adım 9"));
    ui->tableWidget->setItem(11, 0, new QTableWidgetItem("Adım 10"));
    ui->tableWidget->setItem(12, 0, new QTableWidgetItem("Adım 11"));
    ui->tableWidget->setItem(13, 0, new QTableWidgetItem("Adım 12"));
    ui->tableWidget->setItem(14, 0, new QTableWidgetItem("Adım 13"));
    ui->tableWidget->setItem(15, 0, new QTableWidgetItem("Adım 14"));
    ui->tableWidget->setItem(16, 0, new QTableWidgetItem("Adım 15"));
    ui->tableWidget->setItem(17, 0, new QTableWidgetItem("Adım 16"));
    ui->tableWidget->setItem(18, 0, new QTableWidgetItem("Adım 17"));
    ui->tableWidget->setItem(19, 0, new QTableWidgetItem("Adım 18"));
    ui->tableWidget->setItem(20, 0, new QTableWidgetItem("Adım 19"));
/*
     QComboBox* myComboBox1 = new QComboBox();
    ui->tableWidget->setCellWidget(1,2,myComboBox1);
    myComboBox1->insertItem(0,"sn");
    myComboBox1->insertItem(1,"dk");
    myComboBox1->insertItem(2,"saat");
    myComboBox1->insertItem(3,"gün");
    QComboBox* myComboBox2 = new QComboBox();
    ui->tableWidget->setCellWidget(2,2,myComboBox2);
    myComboBox2->insertItem(0,"sn");
    myComboBox2->insertItem(1,"dk");
    myComboBox2->insertItem(2,"saat");
    myComboBox2->insertItem(3,"gün");
    QComboBox* myComboBox3 = new QComboBox();
    ui->tableWidget->setCellWidget(3,2,myComboBox3);
    myComboBox3->insertItem(0,"sn");
    myComboBox3->insertItem(1,"dk");
    myComboBox3->insertItem(2,"saat");
    myComboBox3->insertItem(3,"gün");
    QComboBox* myComboBox4 = new QComboBox();
    ui->tableWidget->setCellWidget(4,2,myComboBox4);
    myComboBox4->insertItem(0,"sn");
    myComboBox4->insertItem(1,"dk");
    myComboBox4->insertItem(2,"saat");
    myComboBox4->insertItem(3,"gün");
    QComboBox* myComboBox5 = new QComboBox();
    ui->tableWidget->setCellWidget(5,2,myComboBox5);
    myComboBox5->insertItem(0,"sn");
    myComboBox5->insertItem(1,"dk");
    myComboBox5->insertItem(2,"saat");
    myComboBox5->insertItem(3,"gün");
    QComboBox* myComboBox6 = new QComboBox();
    ui->tableWidget->setCellWidget(6,2,myComboBox6);
    myComboBox6->insertItem(0,"sn");
    myComboBox6->insertItem(1,"dk");
    myComboBox6->insertItem(2,"saat");
    myComboBox6->insertItem(3,"gün");
    QComboBox* myComboBox7 = new QComboBox();
    ui->tableWidget->setCellWidget(7,2,myComboBox7);
    myComboBox7->insertItem(0,"sn");
    myComboBox7->insertItem(1,"dk");
    myComboBox7->insertItem(2,"saat");
    myComboBox7->insertItem(3,"gün");
    QComboBox* myComboBox8 = new QComboBox();
    ui->tableWidget->setCellWidget(8,2,myComboBox8);
    myComboBox8->insertItem(0,"sn");
    myComboBox8->insertItem(1,"dk");
    myComboBox8->insertItem(2,"saat");
    myComboBox8->insertItem(3,"gün");
    QComboBox* myComboBox9 = new QComboBox();
    ui->tableWidget->setCellWidget(9,2,myComboBox9);
    myComboBox9->insertItem(0,"sn");
    myComboBox9->insertItem(1,"dk");
    myComboBox9->insertItem(2,"saat");
    myComboBox9->insertItem(3,"gün");
    QComboBox* myComboBox10 = new QComboBox();
    ui->tableWidget->setCellWidget(10,2,myComboBox10);
    myComboBox10->insertItem(0,"sn");
    myComboBox10->insertItem(1,"dk");
    myComboBox10->insertItem(2,"saat");
    myComboBox10->insertItem(3,"gün");
    */
}

lookProfile::~lookProfile()
{
    delete ui;
}

bool lookProfile::readProfiles(int index)
{
    if (index == 0)
    {

    }
    else
    {
        profileIndex = index;
        QByteArray rProfile;
        quint16 pos;

        #ifdef Q_OS_LINUX
                //linux code goes here
                QString filePath = "/home/pi/InDetail/profiles/Profile " + QString::number(index) + ".dat";
        #endif
        #ifdef Q_OS_WIN
                // windows code goes here
                QString filePath = "profiles\\Profile " + QString::number(index) + ".dat";
        #endif

        QFile file(filePath);

        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information( this, "Profil Okuma", "Profil okunmadı.");
        return(false);
        }
        else
        {
             rProfile = file.readAll();
             QString name;
             pos = 0;
     //   QMessageBox::information( this, "Profil Okuma", "Profil okundu.");
             if (rProfile[pos] == '/')
             {
                 ui->label->setText("isimsiz");
               //  ui->laSelectedProfileManual->setText("isimsiz");
               //  ui->laTestName->setText("isimsiz");
             }
             else
             {
                 while (rProfile[pos] != '/')
                 {
                     name.append(char(rProfile[pos]));
                     pos++;
                 }
                 ui->label->setText(name);
       //          ui->laSelectedProfileManual->setText(name);
       //          ui->laTestName->setText(name);
             }
             pos=pos+26;

             tLookProfile[index - 1].active = rProfile[pos];
             pos = pos + 2;

             tLookProfile[index - 1].startValue = qint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;

             ui->tableWidget->setItem(1, 4, new QTableWidgetItem(QString::number(tLookProfile[index - 1].startValue)));                                    //tableWigdet da 1 - 4 ün yerine değer yazan kod
             pos = pos + 3;

             tLookProfile[index - 1].totalStep = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));
             ui->spinBox->setValue(tLookProfile[index - 1].totalStep);
            for (int j=ui->spinBox->value()+2;j<20;j++)
            {
                ui->tableWidget->setItem(j, 1, new QTableWidgetItem(""));
                ui->tableWidget->item(j,1)->setFlags(!Qt::ItemIsEditable);
                ui->tableWidget->setItem(j, 2, new QTableWidgetItem(""));
                ui->tableWidget->item(j,2)->setFlags(!Qt::ItemIsEditable);
            }
             pos = pos + 3;

             for(int i=0; i<tLookProfile[index - 1].totalStep; i++)
             {
                 pos = pos + 7;

                 tLookProfile[index - 1].step[i].stepUnit = rProfile[pos];

                 pos = pos + 2;

                 tLookProfile[index - 1].step[i].stepType = rProfile[pos];
                 pos = pos + 2;

                 tLookProfile[index - 1].step[i].lDuration = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));


                 ui->tableWidget->setItem(i+2, 1, new QTableWidgetItem(QString::number(tLookProfile[index - 1].step[i].lDuration)));
                 if(tLookProfile[index-1].step[i].stepUnit == 1)
                 {
                     ui->tableWidget->setItem(i+2, 2, new QTableWidgetItem("sn"));
                 }
                 else if(tLookProfile[index-1].step[i].stepUnit == 2)
                 {
                     ui->tableWidget->setItem(i+2, 2, new QTableWidgetItem("dk"));
                 }
                 else if(tLookProfile[index-1].step[i].stepUnit == 3){
                      ui->tableWidget->setItem(i+2, 2, new QTableWidgetItem("saat"));
                 }
                 else if(tLookProfile[index-1].step[i].stepUnit == 4){
                      ui->tableWidget->setItem(i+2, 2, new QTableWidgetItem("gün"));
                 }


     //            ui->tableWidget->setItem(i+2, 2, new QTableWidgetItem(QString::number(tLookProfile[index - 1].step[i].stepUnit)));
                 pos = pos + 3;

                 tLookProfile[index - 1].step[i].lTarget = qint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                 ui->tableWidget->setItem(i+2, 3, new QTableWidgetItem(QString::number(tLookProfile[index - 1].step[i].lTarget)));
                 pos = pos + 3;

             }
             pos = pos + 22;

        }
    }
    return(true);
}

void lookProfile::on_btnLookEditProfile_clicked()
{
   QByteArray editProfile;
   editProfile.append( ui->label->text().toLocal8Bit());
   editProfile.append("/");
   editProfile.append("Temperature General Info");
   editProfile.append("/");
   editProfile.append(tLookProfile[profileIndex-1].active);
   editProfile.append("/");
   float tStart = ui->tableWidget->item(1,2)->data(Qt::DisplayRole).toFloat();
   editProfile.append(qint16(tStart*10) & 0x00FF);
   editProfile.append(qint16(tStart*10) >> 8);
   editProfile.append("/");
   float tTotalStep = ui->spinBox->value();
   editProfile.append(quint16(tTotalStep) & 0x00FF);
   editProfile.append(quint16(tTotalStep) >> 8);
   for(int i=0; i<ui->spinBox->value(); i++)
   {
       editProfile.append("/");
       editProfile.append("Step-");
       editProfile.append(i+1);
       editProfile.append("/");
       quint8 tStepUnit = tLookProfile[profileIndex-1].step[i].stepUnit;
       editProfile.append(tStepUnit);
       editProfile.append("/");
       quint8 tStepType = tLookProfile[profileIndex-1].step[i].stepType;
       editProfile.append(tStepType);
       editProfile.append("/");
       float tLDuration = ui->tableWidget->item(i+2,1)->data(Qt::DisplayRole).toFloat();
       editProfile.append(quint16(tLDuration) & 0x00FF);
       editProfile.append(quint16(tLDuration) >> 8);
       editProfile.append("/");
       float tLTarget = ui->tableWidget->item(i+2,3)->data(Qt::DisplayRole).toFloat();
       editProfile.append(qint16(tLTarget*10) & 0x00FF);
       editProfile.append(qint16(tLTarget*10) >> 8);
     }

   editProfile.append("/");
   editProfile.append("Pressure General Info");
   editProfile.append("/");
   editProfile.append(char(0x00));
   editProfile.append("/");
   editProfile.append(char(0x00));
   editProfile.append(char(0x00));
   editProfile.append("/");
   editProfile.append(char(0x00));
   editProfile.append(char(0x00));

   editProfile.append("/");
   editProfile.append("Vibration General Info");
   editProfile.append("/");
   editProfile.append(char(0x00));
   editProfile.append("/");
   editProfile.append(char(0x00));
   editProfile.append(char(0x00));
   editProfile.append("/");
   editProfile.append(char(0x00));
   editProfile.append(char(0x00));

   editProfile.append("/End of Profile.");

#ifdef Q_OS_LINUX
    //linux code goes here
    QString filePath = "/home/pi/InDetail/profiles/Profile " + QString::number(profileIndex) + ".dat";
#endif
#ifdef Q_OS_WIN
    // windows code goes here
    QString filePath = "profiles\\Profile " + QString::number(profileIndex) + ".dat";
#endif

    QFile file(filePath);

    file.remove();

   if(!file.open(QIODevice::WriteOnly))
   {
       QMessageBox::information(this,"",
                   tr("Could not create profile file to be written.") );
   }
   else
   {
       if (file.write(editProfile))
       {
           file.flush();
           file.close();
           QMessageBox::information(
            this,"",
                       tr("Profile saved successfully.") );
       }
       else
       {
           QMessageBox::information(
            this,"",
                       tr("Error while writing profile info to profile file." ));
       }
   }
}

void lookProfile::on_pushButton_clicked()
{
    ui->spinBox->setValue(ui->spinBox->value() + 1);
    ui->tableWidget->item(ui->spinBox->value()+1,1)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
    ui->tableWidget->item(ui->spinBox->value()+1,2)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
}

void lookProfile::on_pushButton_2_clicked()
{

    ui->tableWidget->item(ui->spinBox->value()+1,1)->setFlags(!Qt::ItemIsEditable);
    ui->tableWidget->item(ui->spinBox->value()+1,2)->setFlags(!Qt::ItemIsEditable);
    ui->spinBox->setValue(ui->spinBox->value() - 1);
}
