#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QValidator>
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>
#include <QValidator>
#include <QtSerialPort/QSerialPort>
#include <QMouseEvent>
#include <QMessageBox>
#include <QTime>
#include <QPixmap>
#include <QCloseEvent>
#include <QApplication>
#include <QFileDialog>
#include "profilestruct.h"
#include "serialprocess.h"
#include "myserial.h"
#include "keyboard/ui_numpad.h"
#include "keyboard/numpad.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int profileIndexMain;

public slots:



private slots:

    void on_cbSelectGraph_currentIndexChanged(int index);
    void serialMessage(uint command, QByteArray data);
    void on_cbSelectProfile_currentIndexChanged(int index);
    void on_bEditPro_clicked();
    void on_bClearPro_clicked();
    void on_bSavePro_clicked();
    void updateTPreview();
    void updateNPreview();
    void on_bNewTStep_clicked();
    void on_bTForward2_clicked();
    void on_bTBack2_clicked();
    void on_bTBack3_clicked();
    void on_bTSaveStep_clicked();

    bool readProfiles(char rType, int index);
 //   void on_cbSelectProfileEdit_currentIndexChanged(int index);
 //   void on_cbSelectStepEdit_currentIndexChanged(int index);
 //   void on_cbSelectPTypeEdit_currentIndexChanged(int index);
    void on_cbSelectProfileMain_currentIndexChanged(int index);
    bool sendProfileOverSerial(QString mode, int index);
    void on_bStartTest_clicked();
    void on_bStopTest_clicked();
    void askSensorValues();
    void askOtherStuff();
    void getCurrentDateTime();
    void prepareTestTimers();
    void updateInfo(quint8 index, QByteArray data);
    void writeToLogTable(QString info);
    void commInfo(bool status);

    void updateTPlot();
    void updatePPlots();
 //   void updateVPlot();

    void profileSent();

    void setupTGraph();
    void setupPGraphs();
    void setupVGraph();

    void setupPreviewGraphs();

    void setupVisuals();
    void on_bScreenshot_clicked();
    void on_bScreenshot_saved();

  //  void on_bStartVibration_clicked();
  //  void on_bStartPressure_clicked();
    void on_bRes_clicked();
    void on_bFan_clicked();
    void on_bLightsMain_clicked();


    void closeEvent(QCloseEvent *event);
 //   void on_cbVSelectSType_currentIndexChanged(int index);
    void on_bClearLogTable_clicked();
    void clearProfileSlot(char sType, char pType, quint8 index);
    void on_cbSelectMethodManual_currentIndexChanged(int index);
    void on_cbSelectProfileManual_currentIndexChanged(int index);
    void on_tabWidget_currentChanged(int index);
 //   void on_cbSelectSTypeEdit_currentIndexChanged(int index);
 //   void on_cbSelectSUnitEdit_currentIndexChanged(int index);
    void on_bSendProfileMain_clicked();
    void on_bStartTestManual_clicked();
    void on_bStopTestManual_clicked();
    void on_bSendProfileManual_clicked();
    void on_bPauseTestManual_clicked();
    void on_bPauseTest_clicked();
    void on_bStartMaintenance_clicked();
    void mousePress();
    void mouseWheel();
    void setupComboBoxes();

  //  void on_cbSelectProfileMain_2_currentIndexChanged(int index);

    void on_ZoomInHor_clicked();

    void on_ZoomOutHor_clicked();

    void on_ZoomInVer_clicked();

    void on_ZoomOutVer_clicked();

    bool on_bTemperatureSet_clicked();

    void on_bSetTemperatureStart_clicked();

    void on_bSetTemperatureStop_clicked();

    void on_cbTSelectSUnit_currentIndexChanged(int index);

    void on_ZoomCenter_clicked();

    void on_bSetExhaustValve_clicked();

    void saveValueExhaustValve(int value);

    void loadValueExhaustValve();

    void saveValueTopTempSensorCalibration();

    void saveValueBottomTempSensorCalibration();

    void saveValuePressureSensor1Calibration();

    void saveValueCleanTankLevelCalibration();

    void saveValueDirtyTankLevelCalibration();

    void saveValueExpansionTankLevelCalibration();

    void saveValueLiquidTankLevelCalibration();

    void saveValueLiquidTempCalibration();

    void loadValueLiquidTankLevelCalibration();

    void loadValueLiquidTempCalibration();

    void loadValueTopTempSensorCalibration();

    void loadValueBottomTempSensorCalibration();

    void loadValuePressureSensor1Calibration();

    void loadValueCleanTankLevelCalibration();

    void loadValueDirtyTankLevelCalibration();

    void loadValueExpansionTankLevelCalibration();

    void on_bSaveCalibrationValues_clicked();

    void on_bCabinDoor_clicked();

    void on_bEditProLook_clicked();

    void on_bClearLogTable_2_clicked();

    void run_keyboard_lineEdit();

    void on_bRecipeMode_clicked();

    void on_bFixMode_clicked();

    void on_b1500hMode_clicked();

    void on_btnBackDetails_clicked();

    void on_btnDetailsInfo_clicked();

    void on_btnDetailsTanklevel_clicked();

    void on_btnDetailsHeater_clicked();

    void on_btnDetailsPressure_clicked();

    void on_bStartTest1500h_clicked();

    bool on_bSendProfile1500h_clicked();

    bool on_checkBox_stateChanged();

    bool on_checkBox_4_clicked();

    bool on_checkBox_5_clicked();

    bool on_checkBox_2_clicked();

    bool on_checkBox_3_clicked();

    void on_bMinimize_2_clicked();

    void on_bMinimize_3_clicked();

    void on_bStopTest1500h_clicked();

    void on_bPauseTest1500h_clicked();

    void on_b_pipe_1_clicked();

    void on_b_pipe_2_clicked();

    void on_b_pipe_3_clicked();

    void on_b_pipe_4_clicked();

    void on_b_pipe_5_clicked();

    void on_b_basinc_tank_doldur_clicked();

    void on_b_temiz_tank_bosalt_clicked();

    void on_b_tum_tanklari_bosalt_clicked();

    void on_bResetFault_clicked();

    void resetFaultVisuals();

    void on_bChooseData_clicked();

    void on_btnDetailsPipes_clicked();

 //   void on_btnDetailsPressure_2_clicked();

    void on_bDoorControlActive_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void activateCabinDoorLock();

    void on_bDoorCOntrolDeactive_clicked();



    bool on_Hortum1_stateChanged();

    bool on_Hortum2_stateChanged();

    bool on_Hortum3_stateChanged();

    bool on_Hortum4_stateChanged();

    bool on_Hortum5_stateChanged();

    void on_bManualPressureLinesStart_clicked();

    void on_bManualPressureLinesStop_clicked();

    void on_bManualEvacLines_clicked();

    void on_bManualPrepareLines_clicked();

    void on_cbChooseProfileType_currentIndexChanged(int index);

    void on_cbNSelectSUnit_currentIndexChanged(int index);

    void on_bNewNStep_clicked();

    void on_bNForward2_clicked();

    void on_bNSaveStep_clicked();

    void on_bTBack2_2_clicked();

    void on_bNBack3_clicked();

    void on_btnIsnHumidty_2_clicked();

    void on_btnIsnHumidty_clicked();

    void on_bMainDoorInfo_clicked();

    void on_bSaveProfromTabl_clicked();

    void saveProfileInfoToStruct(quint8 currentProfile);

    QByteArray saveProfileInfoQByteArray(quint8 currentProfile);

    void saveProfileInfoToFile(QByteArray profile);

    void on_cbChooseProfileSlot_currentIndexChanged(int index);

    QByteArray readProfileFromFileToQbyteArray(int index);

    void readProfileToStruct (QByteArray rProfile,int index);

    void readProfileToTable(int index);

   bool readProfile(int index);
   void plotRecipeGraph(int index);





   void on_bResetFaultBuzzer_clicked();

   //void on_cbChooseProfileTypeNew_currentIndexChanged(int index);

   void on_bClearProfromTabl_clicked();



   void on_bEditTablo_clicked();
   void labelEnable(int index);

   void on_chbHumidityDevice_clicked();

   void on_btnPatlatmaIstasyon_clicked();

   void on_bMinimize_4_clicked();

   void on_bMinimize_5_clicked();



   void on_bKanal1_clicked();

   void on_bKanal2_clicked();

   void on_bKanal3_clicked();

   void on_bKanal4_clicked();

   void on_bKanal5_clicked();

   void on_bKanal5_2_clicked();

  // void on_bKanal5_3_clicked();

   void on_ZoomCenter_2_clicked();

   void on_ZoomInVer_2_clicked();

   void on_ZoomOutVer_2_clicked();

   void on_ZoomOutHor_2_clicked();

   void on_ZoomInHor_2_clicked();



   void on_cBKanal1_clicked(bool checked);

   void on_cBKanal2_clicked(bool checked);

   void on_cBKanal3_clicked(bool checked);

   void on_cBKanal4_clicked(bool checked);

   void on_cBKanal5_clicked(bool checked);

   void on_cBKanal1_2_clicked(bool checked);

   void on_cBKanal2_2_clicked(bool checked);

   void on_cBKanal3_2_clicked(bool checked);

   void on_cBKanal4_2_clicked(bool checked);

   void on_cBKanal5_2_clicked(bool checked);







   void on_bPatlatYesNo_clicked(bool checked);



   bool on_bLiquidCoolerControl_clicked(bool checked);

   void on_leFixPressureVelocity_2_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;

    mySerial *serial;
    QTimer *timer1000, *timer250, *timerTemp, *timerVib, *timerPressure,*timerScreen;
    SerialProcess *proc;
    Numpad *lineEditkeyboard;
};

#endif // MAINWINDOW_H
