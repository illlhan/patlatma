#ifndef LOOKPROFILE_H
#define LOOKPROFILE_H

#include <QDialog>
#include "profilestruct.h"
#include <QMessageBox>
#include <QComboBox>
namespace Ui {
class lookProfile;
}

class lookProfile : public QDialog
{
    Q_OBJECT

public:
    explicit lookProfile(QWidget *parent = 0);
    ~lookProfile();
    int profileIndex;


private:
    Ui::lookProfile *ui;
public slots:
    bool readProfiles(int index);
private slots:
    void on_btnLookEditProfile_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // LOOKPROFILE_H
