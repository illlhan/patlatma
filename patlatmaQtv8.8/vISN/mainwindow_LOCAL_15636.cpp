#include "mainwindow.h"
#include "ui_mainwindow.h"
#include  "lookprofile.h"
#include "keyboard/numpad.h"

char appName[] = {'i', 'n', 'D', 'e', 't', 'a', 'i', 'l', ' ', 'V', '1', '5', '0','0'};
profileStruct tProfileSave[MaxProfile];
profileStruct pProfileSave[MaxProfile];
profileStruct vProfileSave[MaxProfile];

profileStruct tProfileEdit[MaxProfile];
profileStruct pProfileEdit[MaxProfile];
profileStruct vProfileEdit[MaxProfile];

profileStruct tProfileLoad[MaxProfile];
profileStruct pProfileLoad[MaxProfile];
profileStruct vProfileLoad[MaxProfile];

PLC myPLC;

quint8 currentProfile = 0;
quint8 currentTStep = 0;
quint8 currentPStep = 0;
quint8 currentVStep = 0;

quint16 profileId = 0;

double oldTValue = 0;
double oldPValue = 0;
double oldVValue = 0;

int tempPeriod;
int vibPeriod;
int pressurePeriod;

double tKey = 0;
double tKeyElapsed = 0;
double pKey = 0;
double vKey = 0;


float pipePressure1In;
float pipe1Pressure;
float setPressure;
float pipe2Pressure;
float pipe3Pressure;
float pipe4Pressure;
float pipe5Pressure;
float pipe6Pressure;
float ExpansionTankLevel;
float CleanTankLevel;
float DirtyTankLevel;

float cabinBottomTemperatureIn;
float cabinTopTemperatureIn;
float cabinPIDTemperatureIn;
float cabinSetTemperatureIn;
float cabinBottomTemperatureValue;
float cabinTopTemperatureValue;
float cabinAverageTemp;
float waterTankTemperature;
bool  waterTankLiquidLevel;
float pipeVibrationFrequency;

bool commStatus;

quint32 tElapsedSeconds;
quint32 pElapsedSeconds;
quint32 vElapsedSeconds;
quint8  tStep;
quint8  pStep;
quint8  vStep;
quint32 pStepRepeat;
quint32 vStepRepeat;
quint16 tCycle;
quint16 pCycle;
quint16 vCycle;

quint32 totalTestDuration;

quint8 askCounter = 1;

QString testFolder;
QString dataFilePath;
bool startFromRecords;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(appName);

    //    QPixmap bkgnd("png\\background.jpg");
    //    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    //    QPalette palette;
    //    palette.setBrush(QPalette::Background, bkgnd);
    //    ui->tab_Main->setPalette(palette);
    //    ui->tab_Details->setPalette(palette);
    //    ui->tab_EditPro->setPalette(palette);
    //    ui->tab_NewPro->setPalette(palette);
    //    ui->tab_Manual->setPalette(palette);
    //    ui->tab_Maintenance->setPalette(palette);

    lineEditkeyboard = new Numpad();



#ifdef Q_OS_LINUX
    //linux code goes here
    if (!QDir("/home/pi/InDetail/profiles").exists())
    {
        QDir().mkdir("/home/pi/InDetail/profiles");
    }
    if (!QDir("/home/pi/InDetail/records").exists())
    {
        QDir().mkdir("/home/pi/InDetail/records");
    }
    if (!QDir("/home/pi/InDetail/screenshots").exists())
    {
        QDir().mkdir("/home/pi/InDetail/screenshots");
    }
#endif
#ifdef Q_OS_WIN
    // windows code goes here
    if (!QDir("profiles").exists())
    {
        QDir().mkdir("profiles");
    }
    if (!QDir("records").exists())
    {
        QDir().mkdir("records");
    }
    if (!QDir("screenshots").exists())
    {
        QDir().mkdir("screenshots");
    }
#endif

    serial = new mySerial(this);

    connect(serial, &mySerial::messageSerial, this, &MainWindow::serialMessage);

    serial->startSerial();
    proc = new SerialProcess(this);
    connect(proc, &SerialProcess::write,serial, &mySerial::writeSerial);
    connect(proc, &SerialProcess::profileReady, this, &MainWindow::profileSent);
    connect(proc, &SerialProcess::commStatus, this, &MainWindow::commInfo);

    setupTGraph();
    setupVGraph();
    setupPGraphs();
    setupPreviewGraphs();
    setupVisuals();

    setupComboBoxes();

    timer1000 = new QTimer(this);
    connect(timer1000, SIGNAL(timeout()), this, SLOT(getCurrentDateTime()));
    timer1000->start(1000);

    timer250 = new QTimer(this);
    connect(timer250, SIGNAL(timeout()), this, SLOT(askOtherStuff()));
    timer250->start(1000);

    timerTemp = new QTimer(this);
    connect(timerTemp, SIGNAL(timeout()), this, SLOT(updateTPlot()));

    timerScreen = new QTimer(this);
    connect(timerScreen, SIGNAL(timeout()), this, SLOT(on_bScreenshot_saved()));
    /*
    timerVib = new QTimer(this);
    connect(timerVib, SIGNAL(timeout()), this, SLOT(updateVPlot()));
*/
    timerPressure = new QTimer(this);
    connect(timerPressure, SIGNAL(timeout()), this, SLOT(updatePPlots()));

    //myPLC.cabinTemperatureStat = true;
    //myPLC.tankTemperatureStat = true;
    //myPLC.pipePressureStat = true;

    askSensorValues();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn;
    if ( myPLC.deviceState == char(0x02) )
    {
        resBtn = QMessageBox::question( this, appName,
                                        tr("Devam eden Test var! Uygulamadan çıkmak istiyor musunuz?"),
                                        QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                        QMessageBox::Yes);
    }
    else
    {
        resBtn = QMessageBox::question( this, appName,
                                        tr("Uygulamadan çıkmak istiyor musunuz?"),
                                        QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                        QMessageBox::Yes);

    }

    if (resBtn != QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::profileSent()
{
    /// Test Profili bilgisi PLC'ye gönderildi.
    ///
    /// Test profili bilgisi PLC'ye seri haberleşeme
    /// ile gönderildikten sonra PLC'den onay cevabı gelir
    /// ve Test başlatma prosedürü devam ettirilir.   



    ui->bStartTest->setEnabled(true);
    ui->bStartTestManual->setEnabled(true);
    ui->bSetTemperatureStart->setEnabled(true);
    ui->bStartTest1500h->setEnabled(true);

    proc->start();
}

void MainWindow::commInfo(bool status)
{
    /// Haberleşme durumunu ekranda göster.
    /// Eğer haberleşme açıksa ekranda yeşil OK yazısı yazılır.
    /// Değilse kırmızı NOK yazısı yazılır ve haberleşme değişkenleri temizlenir.

    if (status == true)
    {

        commStatus = true;
  //      ui->laCommErr->setStyleSheet("QLabel { color : green; }");
  //      ui->laCommErr->setText("OK");
    }
    else
    {
    //    ui->laCommErr->setStyleSheet("QLabel { color : red; }");
    //    ui->laCommErr->setText("NOK");
        proc->stop();
        proc->commandMessages.clear();
        proc->profileMessages.clear();
        commStatus = false;
    }

}

void MainWindow::setupTGraph()
{
    // make a time ticker
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");

    // make a vertical Ticker for temperature
    QSharedPointer<QCPAxisTicker> tTicker(new QCPAxisTicker);
    tTicker->setTickCount(15);

    QSharedPointer<QCPAxisTicker> pTicker(new QCPAxisTicker);
        pTicker->setTickCount(9);

    // make temperature plot general settings
    ui->tTestGraph->addGraph(ui->tTestGraph->xAxis, ui->tTestGraph->yAxis);
    QColor orangeColor(255,165,0);
    ui->tTestGraph->graph(0)->setPen(QPen(orangeColor,3));
    ui->tTestGraph->addGraph(ui->tTestGraph->xAxis,ui->tTestGraph->yAxis2);
    ui->tTestGraph->graph(1)->setPen(QPen(Qt::red,3));
    ui->tTestGraph->setInteractions(QCP::iRangeDrag | QCP::iSelectAxes |
                                    QCP::iSelectLegend| QCP::iRangeZoom | QCP::iSelectPlottables);
    //ui->tTestGraph->axisRect()->setRangeDrag(Qt::Horizontal);
    //ui->tTestGraph->axisRect()->setRangeZoom(Qt::Horizontal);

    // make Y axis temperature ticker
    ui->tTestGraph->yAxis->setTicker(tTicker);


    //make visible right axes for pressure

     ui->tTestGraph->yAxis2->setTicker(pTicker);
      ui->tTestGraph->yAxis2->setVisible(true);

    // make X axis as time ticker
    ui->tTestGraph->xAxis->setTicker(timeTicker);
    ui->tTestGraph->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignBottom|Qt::AlignRight);
    ui->tTestGraph->xAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->tTestGraph->xAxis->setLabel("Zaman (hh:mm:ss)");
    ui->tTestGraph->yAxis->setLabelFont(QFont(QFont().family(), 12));
    ui->tTestGraph->yAxis->setLabel("Kabin Sıcaklıgı (°C)");
        ui->tTestGraph->yAxis2->setLabelFont(QFont(QFont().family(), 12));
        ui->tTestGraph->yAxis2->setLabel("Boru Basıncı (bar)");
    ui->tTestGraph->xAxis->setLabelColor(Qt::white);
    ui->tTestGraph->yAxis->setLabelColor(Qt::white);
        ui->tTestGraph->yAxis2->setLabelColor(Qt::white);
    ui->tTestGraph->xAxis->setTickLabelFont(QFont(QFont().family(), 14));
    ui->tTestGraph->yAxis->setTickLabelFont(QFont(QFont().family(), 14));
        ui->tTestGraph->yAxis2->setTickLabelFont(QFont(QFont().family(), 14));
        ui->tTestGraph->yAxis2->setTickLabelColor(Qt::white);
    ui->tTestGraph->yAxis->setRange(0, 150.0);
        ui->tTestGraph->yAxis2->setRange(0, 6);

    //Background Colour brush pens
    ui->tTestGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->tTestGraph->yAxis->setBasePen(QPen(Qt::white, 1));
        ui->tTestGraph->yAxis2->setBasePen(QPen(Qt::white, 1));
    ui->tTestGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->tTestGraph->yAxis->setTickPen(QPen(Qt::white, 1));
        ui->tTestGraph->yAxis2->setTickPen(QPen(Qt::white, 1));
    ui->tTestGraph->xAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->tTestGraph->yAxis->setSubTickPen(QPen(Qt::white, 1));
        ui->tTestGraph->yAxis2->setSubTickPen(QPen(Qt::white, 1));
    ui->tTestGraph->xAxis->setTickLabelColor(Qt::white);
    ui->tTestGraph->yAxis->setTickLabelColor(Qt::white);
    ui->tTestGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->tTestGraph->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->tTestGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->tTestGraph->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->tTestGraph->xAxis->grid()->setSubGridVisible(true);
    ui->tTestGraph->yAxis->grid()->setSubGridVisible(true);
    ui->tTestGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->tTestGraph->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->tTestGraph->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->tTestGraph->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    ui->tTestGraph->setBackground(plotGradient);


    //   connect(ui->tTestGraph, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->tTestGraph, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->tTestGraph, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
 //   connect(ui->tTestGraph->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->tTestGraph->xAxis2, SLOT(setRange(QCPRange)));
 //   connect(ui->tTestGraph->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->tTestGraph->yAxis2, SLOT(setRange(QCPRange)));
if (startFromRecords){
    QVector<double> x(180000), y(180000);
        #ifdef Q_OS_WIN
                // windows code goes here
                QString filePath = dataFilePath;
        #endif

                QFile file(filePath);

                if(!file.open(QIODevice::ReadOnly))
                {

                }
                QTextStream xin (&file);
                int rowcount = 0;
                while (!xin.atEnd()) {
                    auto line =xin.readLine();
                    auto values =line.split(",");
                    x[rowcount] = values[0].toDouble();
                    y[rowcount]= values[1].toDouble();
                    rowcount++;
                }


           ui->tTestGraph->graph(0)->setData(x, y);
}




              // exponentially decaying cosine


          //  pKey = QTime::currentTime().msecsSinceStartOfDay()/1000;


            // make key axis range scroll with the data (at a constant range size of 30):
        //    ui->pTestGraph->xAxis->setRange(pKey, 60, Qt::AlignRight);


            // rescale key (horizontal) axis to fit the current data:
           // ui->pTestGraph->graph(0)->rescaleKeyAxis();


            // replot the graph with the setdata


}

void MainWindow::setupPGraphs()
{
    // make a time ticker
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");

    // make a vertical Ticker for pressure
    QSharedPointer<QCPAxisTicker> pTicker(new QCPAxisTicker);
    pTicker->setTickCount(10);

    // make pressure plot general settings
    ui->pTestGraph->addGraph();
    ui->pTestGraph->graph(0)->setPen(QPen(Qt::darkGreen));
    ui->pTestGraph->addGraph();
    ui->pTestGraph->graph(1)->setPen(QPen(Qt::darkBlue));
    ui->pTestGraph->addGraph();
    ui->pTestGraph->graph(2)->setPen(QPen(Qt::darkYellow));
    ui->pTestGraph->addGraph();
    ui->pTestGraph->graph(3)->setPen(QPen(Qt::darkMagenta));
    ui->pTestGraph->addGraph();
    ui->pTestGraph->graph(4)->setPen(QPen(Qt::darkRed));
    ui->pTestGraph->addGraph();
    ui->pTestGraph->graph(5)->setPen(QPen(Qt::darkCyan));

    ui->pTestGraph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->pTestGraph->axisRect()->setRangeDrag(Qt::Horizontal);
    ui->pTestGraph->axisRect()->setRangeZoom(Qt::Horizontal);

    // make Y axis pressure ticker
    ui->pTestGraph->yAxis->setTicker(pTicker);

    // make X axis as time ticker
    ui->pTestGraph->xAxis->setTicker(timeTicker);
    ui->pTestGraph->axisRect()->setupFullAxesBox();
    ui->pTestGraph->xAxis->setLabel("Time (hh:mm:ss)");
    ui->pTestGraph->yAxis->setLabel("Pipe Pressure (bar)");
    ui->pTestGraph->yAxis->setRange(0, 10.0);
    ui->pTestGraph->setBackground(Qt::white);
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->pTestGraph->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->pTestGraph->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->pTestGraph->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->pTestGraph->yAxis2, SLOT(setRange(QCPRange)));

}

void MainWindow::setupVGraph()
{
    // make a time ticker
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");

    // make a vertical Ticker for vibration
    QSharedPointer<QCPAxisTicker> vTicker(new QCPAxisTicker);
    vTicker->setTickCount(12);

    // make vibration plot general settings
    //ui->vTestGraph->addGraph();
    //ui->vTestGraph->graph(0)->setPen(QPen(Qt::yellow));
    //ui->vTestGraph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    //ui->vTestGraph->axisRect()->setRangeDrag(Qt::Horizontal);
    //ui->vTestGraph->axisRect()->setRangeZoom(Qt::Horizontal);

    // make Y axis vibration ticker
    //ui->vTestGraph->yAxis->setTicker(vTicker);

    // make X axis as time ticker
    //ui->vTestGraph->xAxis->setTicker(timeTicker);
    //ui->vTestGraph->axisRect()->setupFullAxesBox();
    //ui->vTestGraph->xAxis->setLabel("Time (hh:mm:ss)");
    //ui->vTestGraph->yAxis->setLabel("Vibration Frequency (Hz)");
    //ui->vTestGraph->yAxis->setRange(0, 60.0);
    //ui->vTestGraph->setBackground(Qt::white);
    // make left and bottom axes transfer their ranges to right and top axes:
    //connect(ui->vTestGraph->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->vTestGraph->xAxis2, SLOT(setRange(QCPRange)));
    //connect(ui->vTestGraph->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->vTestGraph->yAxis2, SLOT(setRange(QCPRange)));

}

void MainWindow::setupPreviewGraphs()
{
    /// Önizleme grafiklerini hazırla.
    /// Sıcaklık Profili oluşturulurken biri güncel adımı  diğeri
    /// profil bütününü görselleştirmek için 2 grafik için
    /// hazırlık parametreleri giriliyor.


    ///adım önizlemesi için tPreview alanına grafik ekle.
    ui->tPreview->addGraph();
    QPen tPen;
    tPen.setWidth(3);
    tPen.setColor(Qt::red);
    tPen.setStyle(Qt::SolidLine);
    ui->tPreview->graph(0)->setPen(tPen);
    ui->tPreview->xAxis2->setVisible(true);
    ui->tPreview->xAxis2->setTickLabels(false);
    ui->tPreview->yAxis2->setVisible(true);
    ui->tPreview->yAxis2->setTickLabels(false);
    ui->tPreview->yAxis->setLabel("Cabin Temperature (°C)");
    ui->tPreview->setBackground(Qt::white);
    ui->tPreview->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->tPreview_2->addGraph();
    QPen tPen_2;
    tPen_2.setWidth(3);
    tPen_2.setColor(Qt::red);
    tPen_2.setStyle(Qt::SolidLine);
    ui->tPreview_2->graph(0)->setPen(tPen);
    ui->tPreview_2->xAxis2->setVisible(true);
    ui->tPreview_2->xAxis2->setTickLabels(false);
    ui->tPreview_2->yAxis2->setVisible(true);
    ui->tPreview_2->yAxis2->setTickLabels(false);
    ui->tPreview_2->yAxis->setLabel("Cabin Temperature (°C)");
    ui->tPreview_2->setBackground(Qt::white);
    ui->tPreview_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    /*
    ui->pPreview->addGraph();
    QPen pPen;
    pPen.setWidth(3);
    pPen.setColor(Qt::green);
    pPen.setStyle(Qt::SolidLine);
    ui->pPreview->graph(0)->setPen(pPen);
    ui->pPreview->xAxis2->setVisible(true);
    ui->pPreview->xAxis2->setTickLabels(false);
    ui->pPreview->yAxis2->setVisible(true);
    ui->pPreview->yAxis2->setTickLabels(false);
    ui->pPreview->yAxis->setLabel("Pipe Pressure (bar)");
    ui->pPreview->setBackground(Qt::white);
    ui->pPreview->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->vPreview->addGraph();
    QPen vPen;
    vPen.setWidth(3);
    vPen.setColor(Qt::yellow);
    vPen.setStyle(Qt::SolidLine);
    ui->vPreview->graph(0)->setPen(vPen);
    ui->vPreview->xAxis2->setVisible(true);
    ui->vPreview->xAxis2->setTickLabels(false);
    ui->vPreview->yAxis2->setVisible(true);
    ui->vPreview->yAxis2->setTickLabels(false);
    ui->vPreview->yAxis->setLabel("Vibration Frequency (Hz)");
    ui->vPreview->setBackground(Qt::white);
    ui->vPreview->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

*/
}

void MainWindow::setupVisuals()
{
    connect(ui->leProfileName,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTTotalCycle,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leSetPressure1500h,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leSetTemperature1500h,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTotalTestDuration1500h,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leLiquidChangePeriod1500h,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leFixTempValue,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    ui->tTestGraph->setVisible(false);
    ui->pTestGraph->setVisible(false);
    ui->rectRecipe->setVisible(true);
    ui->rectFix->setVisible(false);
    ui->rect1500h->setVisible(false);

    ui->leStartValue->setEnabled(false);     
    ui->bTemperatureSet->setEnabled(true);
    ui->leFixTempValue->setEnabled(true);

    ui->tWidget->setCurrentIndex(0);
    ui->mainPage->setCurrentIndex(0);

    ui->dsbCabinTopTemp->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->dsbPipe1Pressure->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->dsbCabinTopTempMaintenance->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->dsbPipe1PressureMaintenance->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->sbTTotalCycleManual->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->dsbTTimeSetManual->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->sbTStepSetManual->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->sbTStepRepeatSetManual->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->sbTCycleSetManual->setButtonSymbols(QAbstractSpinBox::NoButtons);

    loadValueExhaustValve();
    loadValueTopTempSensorCalibration();
    loadValueBottomTempSensorCalibration();
    loadValuePressureSensor1Calibration();
    on_bResetFault_clicked();
}

void MainWindow::serialMessage(uint command, QByteArray data)
{

    qDebug() <<"RCV: "<<QTime::currentTime().toString("hh.mm.ss.zzz")<< "Command: " << QString::number(command, 16) << " Data: " << QString(data.toHex());

    proc->PLCReply(command);
    switch (command)
    {
    case 0x0A:

        proc->start();

        break;

    case 0x0C:

        updateInfo(1,data);

        break;

    case 0x0D:

        updateInfo(2,data);

        break;

    case 0x0E:

        updateInfo(3,data);

        break;

    case 0x32:
        /*
        pipe1Pressure = quint8(data[0]) / 10.0;
        pipe2Pressure = quint8(data[1]) / 10.0;
        pipe3Pressure = quint8(data[2]) / 10.0;
        pipe4Pressure = quint8(data[3]) / 10.0;
        pipe5Pressure = quint8(data[4]) / 10.0;
        pipe6Pressure = quint8(data[5]) / 10.0;
*/
        double pipePressure1Coeff ;
        double pipePressure1Err;
        float inpExpansionTankLevel;
        float inpCleanTankLevel;
        float inpDirtyTankLevel;


        pipePressure1In = qint16(((data[1] & 0xff) << 8) | (data[0] & 0xff)) ;
        /*      pipe2Pressure = qint16(((data[3] & 0xff) << 8) | (data[2] & 0xff)) / 100.0;
        pipe3Pressure = qint16(((data[5] & 0xff) << 8) | (data[4] & 0xff)) / 100.0;
        pipe4Pressure = qint16(((data[7] & 0xff) << 8) | (data[6] & 0xff)) / 100.0;
        pipe5Pressure = qint16(((data[9] & 0xff) << 8) | (data[8] & 0xff)) / 100.0;
        pipe6Pressure = qint16(((data[11] & 0xff) << 8) | (data[10] & 0xff)) / 100.0;
*/
        inpExpansionTankLevel = qint16(((data[3] & 0xff) << 8) | (data[2] & 0xff));
        inpCleanTankLevel = qint16(((data[5] & 0xff) << 8) | (data[4] & 0xff));
        inpDirtyTankLevel = qint16(((data[7] & 0xff) << 8) | (data[6] & 0xff));
        setPressure = qint16(((data[9] & 0xff) << 8) | (data[8] & 0xff));

        ui->pbCleanTankLevel->setMaximum(32000);
        ui->pbCleanTankLevel->setMinimum(0);
        ui->pbCleanTankLevel->setValue(inpCleanTankLevel);

        ui->pbExpansionTankLevel->setMaximum(15000);
        ui->pbExpansionTankLevel->setMinimum(0);
        ui->pbExpansionTankLevel->setValue(inpExpansionTankLevel);

        ui->pbDirtyTankLevel->setMaximum(32000);
        ui->pbDirtyTankLevel->setMinimum(0);
        ui->pbDirtyTankLevel->setValue(inpDirtyTankLevel);

        ui->dsbSetPressure_2->setValue(setPressure/10);

        pipePressure1Coeff = ui->dsbCalPipePressure1Coeff->value();
        pipePressure1Err = ui->dsbCalPipePressure1Err->value();

        pipe1Pressure = ((pipePressure1Coeff * pipePressure1In) + pipePressure1Err);
        ui->dsbPipe1Pressure->setValue(pipe1Pressure);
        ui->dsbPipe1Pressure_2->setValue(pipe1Pressure);
        ui->dsbPipe1PressureMaintenance->setValue(pipe1Pressure);
        ui->dsbCalPipePressure1Input->setValue(pipePressure1In);
        ui->dsbCalPipePressure1Out->setValue(pipe1Pressure);
        /*
         *
         *
        ui->dsbPipe2Pressure->setValue(pipe2Pressure);
        ui->dsbPipe2PressureMaintenance->setValue(pipe2Pressure);
        ui->dsbPipe3Pressure->setValue(pipe3Pressure);
        ui->dsbPipe3PressureMaintenance->setValue(pipe3Pressure);
        ui->dsbPipe4Pressure->setValue(pipe4Pressure);
        ui->dsbPipe4PressureMaintenance->setValue(pipe4Pressure);
        ui->dsbPipe5Pressure->setValue(pipe5Pressure);
        ui->dsbPipe5PressureMaintenance->setValue(pipe5Pressure);
        ui->dsbPipe6Pressure->setValue(pipe6Pressure);
        ui->dsbPipe6PressureMaintenance->setValue(pipe6Pressure);
*/
        break;

    case 0x33:

        //       waterTankLiquidLevel = quint8(data[0]);
        //       waterTankTemperature = qint16(((data[2] & 0xff) << 8) | (data[1] & 0xff)) / 10.0;
        cabinTopTemperatureIn = qint16(((data[1] & 0xff) << 8) | (data[0] & 0xff));
        cabinBottomTemperatureIn = qint16(((data[3] & 0xff) << 8) | (data[2] & 0xff));
        cabinPIDTemperatureIn = qint16(((data[5] & 0xff) << 8) | (data[4] & 0xff))/10;
        cabinSetTemperatureIn = qint16(((data[7] & 0xff) << 8) | (data[6] & 0xff))/10;
        //       pipeVibrationFrequency = quint16(((data[8] & 0xff) << 8) | (data[7] & 0xff)) / 10.0;
        double calCabinTopTempCoeff = ui->dsbCalCabinTopTempCoeff->value();
        double calCabinTopTempErr   = ui->dsbCalCabinTopTempErr->value();
        double  calCabinBottomTempCoeff = ui->dsbCalCabinBottomTempCoeff->value();
        double  calCabinBottomTempErr       =  ui->dsbCalCabinBottomTempErr->value();

        cabinBottomTemperatureValue = (cabinBottomTemperatureIn * calCabinBottomTempCoeff) + calCabinBottomTempErr ;
        cabinTopTemperatureValue = (cabinTopTemperatureIn * calCabinTopTempCoeff) + calCabinTopTempErr;

        cabinAverageTemp = (cabinTopTemperatureValue + cabinBottomTemperatureValue)/2;
        //ui->dsbTankTemp->setValue(waterTankTemperature);
        //ui->dsbTankTempMaintenance->setValue(waterTankTemperature);

        ui->dsbCabinTopTemp->setValue(cabinAverageTemp);
        ui->dsbCabinTopTemp_2->setValue(cabinAverageTemp);
        ui->dsbCabinTopTempMaintenance->setValue(cabinAverageTemp);
        ui->dsbSetTempCabinAvrTemp->setValue(cabinAverageTemp);
        ui->dsbCabinSetTemp->setValue(cabinSetTemperatureIn);
 //       ui->dsbCabinPIDTemp->setValue(cabinPIDTemperatureIn);
        ui->dsbCabinPIDTemp_2->setValue(cabinPIDTemperatureIn);

        ui->dsbCalCabinBottomInput->setValue(cabinBottomTemperatureIn);
        ui->dsbCalCabinTopTempInput->setValue(cabinTopTemperatureIn);

        ui->dsbCalCabinBottomTempOut->setValue(cabinBottomTemperatureValue);
        ui->dsbCalCabinTopTempOut->setValue(cabinTopTemperatureValue);
        //ui->dsbPipeVibration->setValue(pipeVibrationFrequency);
        //ui->dsbPipeVibrationMaintenance->setValue(pipeVibrationFrequency);

        break;

    }
}

void MainWindow::prepareTestTimers()
{
    tempPeriod = 30000;
    vibPeriod = 1000;
    pressurePeriod = 30000;

    if (myPLC.temperatureTestActive)
    {
        timerTemp->start(tempPeriod);
        timerScreen->start(300000);
    }

    if (myPLC.temperatureTestActive)
    {
        timerPressure->start(pressurePeriod);
    }
    /*
    if (myPLC.vibrationTestActive)
    {
       timerVib->start(vibPeriod);
    }
*/
}

void MainWindow::updateInfo(quint8 index, QByteArray data)
{
    if (index == 3)
    {
        if (profileId != data[5])
        {
            profileId = quint16(data[5]);
            ui->cbSelectProfileMain->setCurrentIndex(profileId);
        }
        if (data[0] == char(0x01))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = (data[0]);

                if (myPLC.deviceState)
                {
                    timerTemp->stop();
                    /*    timerVib->stop(); */
                    timerPressure->stop();

                    writeToLogTable("Cihaz Boşta");

                    ui->cbSelectProfileMain->setCurrentIndex(0);
                    ui->cbSelectProfileMain->setEnabled(true);

                    ui->bStartTest->setEnabled(true);
                    ui->bStopTest->setEnabled(false);
                    ui->bPauseTest->setEnabled(false);

                    ui->bStartTest1500h->setText("Başla");
                    ui->bStartTest1500h->setEnabled (true);
                    ui->bStopTest1500h->setEnabled (false);
                    ui->bPauseTest1500h->setEnabled (false);

 //                   ui->sbTTotalCycle->setEnabled(true);
                    ui->leTTotalCycle->setEnabled(true);
                    


                   ui->bSetTemperatureStop->setEnabled(false);


                    ui->cbSelectProfileManual->setCurrentIndex(0);
                    ui->cbSelectProfileManual->setEnabled(true);

                    ui->bStartTestManual->setEnabled(true);
                    ui->bStopTestManual->setEnabled(false);
                    ui->bPauseTestManual->setEnabled(false);

                    ui->sbTTotalCycleManual->setEnabled(true);
                    //     ui->sbPTotalCycleManual->setEnabled(true);
                    //     ui->sbVTotalCycleManual->setEnabled(true);
                    //     ui->dsbTankTempSetManual->setEnabled(true);
                    //     ui->chbEllipticalVibrationSetManual->setEnabled(true);
                }
            }
        }
        else if (data[0] == char(0x02))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = (data[0]);

                if (myPLC.deviceState)
                {
                    testFolder = ui->laSelectedProfileMain->text() + "-" +
                            QDate::currentDate().toString("yy.MM.dd") + "-" +
                            QTime::currentTime().toString("hh.mm.ss");
#ifdef Q_OS_LINUX
                    //linux code goes here
                    if (!QDir("/home/pi/InDetail/records/" + testFolder).exists())
                    {
                        QDir().mkdir("/home/pi/InDetail/records/" + testFolder);
                    }
#endif
#ifdef Q_OS_WIN
                    // windows code goes here
                    if (!QDir("records\\" + testFolder).exists())
                    {
                        QDir().mkdir("records\\" + testFolder);
                    }
#endif

                    writeToLogTable("Test Başladı.");

                    ui->cbSelectProfileMain->setEnabled(false);
                    ui->bStartTest->setEnabled(false);
                    ui->bStopTest->setEnabled(true);
                    ui->bPauseTest->setEnabled(true);

             //       ui->sbTTotalCycle->setEnabled(false);
                    ui->leTTotalCycle->setEnabled(false);

                    ui->cbSelectProfileManual->setEnabled(false);
                    ui->bStartTestManual->setEnabled(false);
                    ui->bStopTestManual->setEnabled(true);
                    ui->bPauseTestManual->setEnabled(true);

                    ui->bStartTest1500h->setEnabled(false);

                    ui->bResetFault->setVisible(false);
                    ui->sbTTotalCycleManual->setEnabled(false);
                    //    ui->sbPTotalCycleManual->setEnabled(false);
                    //    ui->sbVTotalCycleManual->setEnabled(false);
                    //    ui->dsbTankTempSetManual->setEnabled(false);
                    //    ui->chbEllipticalVibrationSetManual->setEnabled(false);
                }
            }
        }
        else if (data[0] == char(0x03))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    timerTemp->stop();
                    /*  timerVib->stop();*/
                    timerPressure->stop();

                    writeToLogTable("Test beklemede.");

                    ui->bPauseTestManual->setEnabled(false);
                    ui->bPauseTest->setEnabled(false);

                    ui->bStartTest->setEnabled(true);
                    ui->bStartTestManual->setEnabled(true);

                    ui->bStartTest1500h->setText("Devam");
                    ui->bStartTest1500h->setEnabled(true);
                    ui->bPauseTest1500h->setEnabled(false);




                }
            }
        }
        else if (data[0] == char(0x04))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    writeToLogTable("Bakım modu aktif");

                }
            }
        }
        else if (data[0] == char(0x05))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    writeToLogTable("Sıcaklık sabitleme modu aktif");
                    ui->bSetTemperatureStart->setEnabled(false);
                    ui->bSetTemperatureStop->setEnabled(true);
                    ui->bStartTest->setEnabled(false);
                    ui->bStopTest->setEnabled(true);
                    ui->bPauseTest->setEnabled(false);
                    ui->bResetFault->setVisible(false);
                }
            }
        }
        else if (data[0] == char(0x06))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    testFolder =
                            QDate::currentDate().toString("dd.MM.yy") ;
#ifdef Q_OS_LINUX
                    //linux code goes here
                    if (!QDir("/home/pi/InDetail/records/" + testFolder).exists())
                    {
                        QDir().mkdir("/home/pi/InDetail/records/" + testFolder);
                    }
#endif
#ifdef Q_OS_WIN
                    // windows code goes here
                    if (!QDir("records\\" + testFolder).exists())
                    {
                        QDir().mkdir("records\\" + testFolder);
                    }
#endif
                    writeToLogTable("1500h modu aktif");
                    ui->bStartTest1500h->setEnabled(false);
                    ui->bStopTest1500h->setEnabled(true);
                    ui->bSetTemperatureStart->setEnabled(false);
                    ui->bSetTemperatureStop->setEnabled(true);
                    ui->bStartTest->setEnabled(false);
                    ui->bStopTest->setEnabled(true);
                    ui->bPauseTest->setEnabled(true);
                    ui->bResetFault->setVisible(false);
                }
            }
        }
        else if (data[0] == char(0x0B))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(5);
                    ui->bResetFault->setVisible(true);
                    ui->laFault11->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x0C))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(5);
                    ui->bResetFault->setVisible(true);
                    ui->laFault12->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x15))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(2);
                    ui->bResetFault->setVisible(true);
                    ui->laFault21->setVisible(true);
                    ui->Hortum1->setChecked(false);
                }
            }
        }
        else if (data[0] == char(0x16))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(2);
                    ui->bResetFault->setVisible(true);
                    ui->laFault22->setVisible(true);
                    ui->Hortum2->setChecked(false);
                }
            }
        }
        else if (data[0] == char(0x17))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(2);
                    ui->bResetFault->setVisible(true);
                    ui->laFault23->setVisible(true);
                    ui->Hortum3->setChecked(false);
                }
            }
        }
        else if (data[0] == char(0x18))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(2);
                    ui->bResetFault->setVisible(true);
                    ui->laFault24->setVisible(true);
                    ui->Hortum4->setChecked(false);
                }
            }
        }
        else if (data[0] == char(0x19))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(2);
                    ui->bResetFault->setVisible(true);
                    ui->laFault25->setVisible(true);
                    ui->Hortum5->setChecked(false);
                }
            }
        }
        else if (data[0] == char(0x1F))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(1);
                    ui->bResetFault->setVisible(true);
                    ui->laFault31->setVisible(true);
                    ui->gb_CleanTankLevel->setStyleSheet("background-color: rgb(255, 0, 0);");
                }
            }
        }
        else if (data[0] == char(0x20))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(1);
                    ui->bResetFault->setVisible(true);
                    ui->laFault32->setVisible(true);
                   ui->gb_KirliTankLevel->setStyleSheet("background-color: rgb(255, 0, 0);");
                }
            }
        }
        else if (data[0] == char(0x21))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(1);
                    ui->bResetFault->setVisible(true);
                    ui->laFault33->setVisible(true);
                    ui->gb_basincTankLevel->setStyleSheet("background-color: rgb(255, 0, 0);");
                }
            }
        }
        else if (data[0] == char(0x22))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(1);
                    ui->bResetFault->setVisible(true);
                    ui->laFault34->setVisible(true);
                    ui->gb_basincTankLevel->setStyleSheet("background-color: rgb(255, 0, 0);");
                }
            }
        }
        else if (data[0] == char(0x29))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(4);
                    ui->bResetFault->setVisible(true);
                    ui->laFault41->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x2A))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(3);
                    ui->bResetFault->setVisible(true);
                    ui->laFault42->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x2B))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(3);
                    ui->bResetFault->setVisible(true);
                    ui->laFault43->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x2C))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(3);
                    ui->bResetFault->setVisible(true);
                    ui->laFault44->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x2D))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(4);
                    ui->bResetFault->setVisible(true);
                    ui->laFault45->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x2E))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(3);
                    ui->bResetFault->setVisible(true);
                    ui->laFault46->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x2F))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(4);
                    ui->bResetFault->setVisible(true);
                    ui->laFault47->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x30))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(4);
                    ui->bResetFault->setVisible(true);
                    ui->laFault48->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x31))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(4);
                    ui->bResetFault->setVisible(true);
                    ui->laFault49->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x32))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->tabWidget->setCurrentIndex(1);
                    ui->detailsPages->setCurrentIndex(0);
                    ui->detailsBottomPages->setCurrentIndex(4);
                    ui->bResetFault->setVisible(true);
                    ui->laFault4A->setVisible(true);
                }
            }
        }
        if (myPLC.pressurePrepActive == (data[1] & 0b00001000) >> 3)
        {

        }
        else
        {
            myPLC.pressurePrepActive = (data[1] & 0b00001000) >> 3;

            if (myPLC.pressurePrepActive)
            {
                writeToLogTable("Basınç Hazırlık Adımı Başladı.");
            }
            else
            {
                writeToLogTable("Basınç Hazırlık Adımı Tamamlandı.");
            }
        }

        if (myPLC.pressureTestCompleted == (data[1] & 0b00100000) >> 5)
        {

        }
        else
        {
            myPLC.pressureTestCompleted = (data[1] & 0b00100000) >> 5;

            if (myPLC.pressureTestCompleted)
            {
                writeToLogTable("Basınç testi tamamlandı.");
            }
            else
            {

            }
        }

        if (myPLC.pressureTestActive == (data[1] & 0b00010000) >> 4)
        {

        }
        else
        {
            myPLC.pressureTestActive = (data[1] & 0b00010000) >> 4;

            if (myPLC.pressureTestActive)
            {
                writeToLogTable("Basıç testi aktif.");
            }
            else
            {
                if (myPLC.pressureTestCompleted)
                {

                }
                else
                {
                    writeToLogTable("Basınç testi iptal edildi");
                }
            }
        }

        if (myPLC.vibrationPrepActive == (data[1] & 0b01000000) >> 6)
        {

        }
        else
        {
            myPLC.vibrationPrepActive = (data[1] & 0b01000000) >> 6;

            if (myPLC.vibrationPrepActive)
            {
                writeToLogTable("Vibration prep. started.");
            }
            else
            {
                writeToLogTable("Vibration prep. completed.");
            }
        }

        if (myPLC.vibrationTestCompleted == (data[2] & 0b00000001))
        {

        }
        else
        {
            myPLC.vibrationTestCompleted = (data[2] & 0b00000001);

            if (myPLC.vibrationTestCompleted)
            {
                writeToLogTable("Vibration test completed.");
            }
            else
            {

            }
        }

        if (myPLC.vibrationTestActive == (data[1] & 0b10000000) >> 7)
        {

        }
        else
        {
            myPLC.vibrationTestActive = (data[1] & 0b10000000) >> 7;

            if (myPLC.vibrationTestActive)
            {
                writeToLogTable("Vibration test started.");
            }
            else
            {
                if (myPLC.vibrationTestCompleted)
                {

                }
                else
                {
                    writeToLogTable("Vibration test canceled.");
                }
            }
        }

        if (myPLC.temperaturePrepActive == (data[1] & 0b00000001))
        {

        }
        else
        {
            myPLC.temperaturePrepActive = (data[1] & 0b00000001);

            if (myPLC.temperaturePrepActive)
            {
                writeToLogTable("Sıcaklık hazırlığı başladı.");
            }
            else
            {
                writeToLogTable("Sicaklik hazirligi tamamlandi.");
            }
        }

        if (myPLC.temperatureTestCompleted == (data[1] & 0b00000100) >> 2)
        {

        }
        else
        {
            myPLC.temperatureTestCompleted = (data[1] & 0b00000100) >> 2;

            if (myPLC.temperatureTestCompleted)
            {
                writeToLogTable("sicaklik testi tamamlandi.");
            }
            else
            {

            }
        }

        if (myPLC.temperatureTestActive == (data[1] & 0b00000010) >> 1)
        {

        }
        else
        {
            myPLC.temperatureTestActive = (data[1] & 0b00000010) >> 1;

            if (myPLC.temperatureTestActive)
            {
                writeToLogTable("Sicaklik testi basladi.");
            }
            else
            {
                if (myPLC.temperatureTestCompleted)
                {

                }
                else
                {
                    writeToLogTable("Sicaklik testi iptal edildi.");
                }
            }
        }

        if (myPLC.profileActive == (data[2] & 0b00000010) >> 1)
        {

        }
        else
        {
            myPLC.profileActive = (data[2] & 0b00000010) >> 1;

            if (myPLC.profileActive)
            {
                writeToLogTable("Profil basladi.");
                prepareTestTimers();
            }
            else
            {

            }
        }

        if (myPLC.cabinTemperatureStat == (data[2] & 0b00000100) >> 2)
        {

        }
        else
        {
            myPLC.cabinTemperatureStat = (data[2] & 0b00000100) >> 2;


        }

        if (myPLC.tankTemperatureStat == (data[2] & 0b00001000) >> 3)
        {

        }
        else
        {
            myPLC.tankTemperatureStat = (data[2] & 0b00001000) >> 3;


        }

        if (myPLC.pipePressureStat == (data[2] & 0b00010000) >> 4)
        {

        }
        else
        {
            myPLC.pipePressureStat = (data[2] & 0b00010000) >> 4;


        }

        if (myPLC.vibrationMotor1Stat == (data[2] & 0b00100000) >> 5)
        {

        }
        else
        {
            myPLC.vibrationMotor1Stat = (data[2] & 0b00100000) >> 5;


        }

        if (myPLC.vibrationMotor2Stat == (data[2] & 0b01000000) >> 6)
        {

        }
        else
        {

        }

        if (tCycle != quint16(((data[4] & 0xff) << 8) | (data[3] & 0xff)))
        {
            tCycle = quint16(((data[4] & 0xff) << 8) | (data[3] & 0xff));
          //           ui->laTTotalCycleMain->setText(QString::number(tCycle));
         /*   if (tCycle != 0)
            {
                tKey = 0;
                ui->tTestGraph->clearPlottables();
                setupTGraph();
            }*/
        }
        if (profileId != data[5])
        {
            profileId = quint16(data[5]);
            ui->cbSelectProfileMain->setCurrentIndex(profileId);
        }

        /*     if ( pCycle != quint16(((data[6] & 0xff) << 8) | (data[5] & 0xff)) )
        {

            pCycle = quint16(((data[6] & 0xff) << 8) | (data[5] & 0xff));

            if (pCycle != 0)
            {
                pKey = 0;
                ui->pTestGraph->clearPlottables();
                setupPGraphs();
            }
        }
        if ( vCycle != quint16(((data[8] & 0xff) << 8) | (data[7] & 0xff)) )
        {
            vCycle = quint16(((data[8] & 0xff) << 8) | (data[7] & 0xff));

            if (vCycle != 0)
            {
                vKey = 0;
                //ui->vTestGraph->clearPlottables();
                //setupVGraph();
            }
        }
*/
        if ( myPLC.deviceState == char(0x02) )
        {
#ifdef Q_OS_LINUX
            //linux code goes here
            QString filePath = "/home/pi/InDetail/records/" + testFolder + "/" + "testProgress.txt";
#endif
#ifdef Q_OS_WIN
            // windows code goes here
            QString filePath = "records\\" + testFolder + "\\" + "testProgress.txt";
#endif

            QFile file(filePath);

            if (file.open(QFile::WriteOnly|QFile::Truncate))
            {
                QTextStream stream(&file);
                stream << "Temperature Elapsed Seconds: " << tElapsedSeconds << "\n"
                       << "Pressure Elapsed Seconds: " << pElapsedSeconds << "\n"
                       << "Vibration Elapsed Seconds: " << vElapsedSeconds << "\n"
                       << "Temperature Step: " << tStep << "\n"
                       << "Pressure Step: " << pStep << "\t" << "Pressure Step Repeat: " << pStepRepeat << "\n"
                       << "Vibration Step: " << vStep << "\t" << "Vibration Step Repeat: " << vStepRepeat << "\n"
                       << "Temperature Cycle: " << tCycle << "\n"
                       << "Pressure Cycle: " << pCycle << "\n"
                       << "Vibration Cycle: " << vCycle << "\n";
                file.close();
            }
        }
    }
    else if (index == 2)
    {
        tStep = data[0];
        pStep = data[1];
        vStep = data[2];
        pStepRepeat = (data[3] & 0xFF) | ((data[4] & 0xFF) <<  8) |
                ((data[5] & 0xFF) << 16);
        vStepRepeat = (data[6] & 0xFF) | ((data[7] & 0xFF) <<  8) |
                ((data[8] & 0xFF) << 16);

        //   ui->laCurrentTStepMain->setText(QString::number(tStep));


    }
    else if (index == 1)
    {
        tElapsedSeconds = (data[0] & 0xFF) | ((data[1] & 0xFF) <<  8) |
                ((data[2] & 0xFF) << 16);
        pElapsedSeconds = (data[3] & 0xFF) | ((data[4] & 0xFF) <<  8) |
                ((data[5] & 0xFF) << 16);
        vElapsedSeconds = (data[6] & 0xFF) | ((data[7] & 0xFF) <<  8) |
                ((data[8] & 0xFF) << 16);
 //       ui->laTTestElepsedSecond->setText(QString::number(tElapsedSeconds));
        ui->progressBar_2->setValue(tElapsedSeconds);
    }
}

void MainWindow::writeToLogTable(QString info)
{
   ui->warningTable->insertRow(0);
    ui->warningTable->setItem(0, 0, new QTableWidgetItem(info));
    ui->warningTable->setItem(0, 1, new QTableWidgetItem(
                                  QDate::currentDate().toString(Qt::SystemLocaleShortDate) + "/" +
                                  QTime::currentTime().toString("hh.mm.ss")));
    if ( myPLC.deviceState == char(0x02) )
    {
#ifdef Q_OS_LINUX
        //linux code goes here
        QString filePath = "/home/pi/InDetail/records/" + testFolder + "/" + "testLogs.txt";
#endif
#ifdef Q_OS_WIN
        // windows code goes here
        QString filePath = "records\\" + testFolder + "\\" + "testLogs.txt";
#endif

        QFile file(filePath);

        if (file.open(QFile::WriteOnly|QFile::Append))
        {
            QTextStream stream(&file);
            stream << "Date: " << QDate::currentDate().toString(Qt::SystemLocaleShortDate)
                   << "\t" << "Time: " << QTime::currentTime().toString("hh.mm.ss")
                   << "\t" << "Info: " << info << "\n";
            file.close();
        }
    }
}

void MainWindow::on_cbSelectGraph_currentIndexChanged(int index)
{
 /*  if (index == 0)
    {
        ui->tTestGraph->setVisible(false);
        ui->pTestGraph->setVisible(false);
        //ui->vTestGraph->setVisible(false);
        ui->laCycleCounterDetails->setVisible(false);
        ui->laStepCounterDetails->setVisible(false);
        ui->laRepeatCounterDetails->setVisible(false);
        ui->laTCycleCounterDetails->setVisible(false);
        ui->laTStepCounterDetails->setVisible(false);
        ui->laPCycleCounterDetails->setVisible(false);
        ui->laPStepCounterDetails->setVisible(false);
        ui->laPRepeatCounterDetails->setVisible(false);
        ui->laVCycleCounterDetails->setVisible(false);
        ui->laVStepCounterDetails->setVisible(false);
        ui->laVRepeatCounterDetails->setVisible(false);
    }
    */
    if (index == 0)
    {
        ui->tTestGraph->setVisible(false);
        ui->pTestGraph->setVisible(false);
        //ui->vTestGraph->setVisible(false);
        ui->tTestGraph->setVisible(true);

    }
    else if (index == 1)
    {
        ui->pTestGraph->setVisible(false);
        ui->tTestGraph->setVisible(false);
        //ui->vTestGraph->setVisible(false);
        ui->pTestGraph->setVisible(true);

    }
}

void MainWindow::on_bEditPro_clicked()
{
    connect(ui->leStartValue,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTLDuration,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTLTarget,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leProfileName,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    clearProfileSlot('s', 't', currentProfile);
    clearProfileSlot('s', 'p', currentProfile);
    clearProfileSlot('s', 'v', currentProfile);

    currentTStep = 0;
    currentPStep = 0;
    currentVStep = 0;

    ui->tabWidget->setTabEnabled(0, false);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(3, false);
    ui->tabWidget->setTabEnabled(5, false);
    ui->tabWidget->setTabEnabled(6, false);

    ui->cbSelectProfile->setEnabled(false);

    ui->bSavePro->setEnabled(true);
    ui->bClearPro->setEnabled(true);
    ui->bEditPro->setEnabled(false);
    ui->bEditProLook->setEnabled(false);

    ui->bNewTStep->setEnabled(false);
    //  ui->bNewPStep->setEnabled(true);
    //  ui->bNewVStep->setEnabled(true);

    ui->cbTSelectSUnit->setEnabled(true);
    //  ui->cbPSelectSUnit->setEnabled(true);
    // ui->cbVSelectSUnit->setEnabled(true);
    //  ui->cbTSelectSType->setEnabled(true);
    //  ui->cbPSelectSType->setEnabled(true);
    //  ui->cbVSelectSType->setEnabled(true);

    ui->leStartValue->setEnabled(true);
    //  ui->dsbPStartValue->setEnabled(true);
    //  ui->dsbVStartValue->setEnabled(true);

    ui->leProfileName->setEnabled(true);
    ui->bSavePro->setEnabled(false);
}

void MainWindow::on_bClearPro_clicked()
{
    disconnect(ui->leStartValue,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    disconnect(ui->leTLDuration,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    disconnect(ui->leTLTarget,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    clearProfileSlot('s', 't', currentProfile);
    clearProfileSlot('s', 'p', currentProfile);
    clearProfileSlot('s', 'v', currentProfile);

    ui->cbSelectProfile->setCurrentIndex(0);
    ui->cbSelectProfile->setEnabled(true);

    ui->leProfileName->setText("");
    ui->leProfileName->setEnabled(false);

    ui->leStartValue->setText("0");
    ui->leStartValue->setEnabled(false);
    //  ui->dsbPStartValue->setValue(0);
    //  ui->dsbPStartValue->setEnabled(false);
    //  ui->dsbVStartValue->setValue(0);
    //  ui->dsbVStartValue->setEnabled(false);

    ui->laTTotalStep->setText("0");
    //  ui->laPTotalStep->setText("0");
    //  ui->laVTotalStep->setText("0");

    ui->tWidget->setCurrentIndex(0);
    //  ui->pWidget->setCurrentIndex(0);
    //  ui->vWidget->setCurrentIndex(0);

    ui->cbTSelectSUnit->setEnabled(false);
    //  ui->cbPSelectSUnit->setEnabled(false);
    //  ui->cbVSelectSUnit->setEnabled(false);
    //  ui->cbTSelectSType->setEnabled(false);
    //  ui->cbPSelectSType->setEnabled(false);
    //  ui->cbVSelectSType->setEnabled(false);

    ui->tPreview->clearPlottables();
    //  ui->pPreview->clearPlottables();
    //  ui->vPreview->clearPlottables();

    currentTStep = 0;
    currentPStep = 0;
    currentVStep = 0;

    ui->tabWidget->setTabEnabled(0, true);
    ui->tabWidget->setTabEnabled(1, true);
    ui->tabWidget->setTabEnabled(2, true);
    ui->tabWidget->setTabEnabled(3, true);
    ui->tabWidget->setTabEnabled(5, true);
    ui->tabWidget->setTabEnabled(6, true);

    ui->tPreview_2->clearPlottables();   
    ui->tPreview_2->replot();

}

void MainWindow::on_cbSelectProfile_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->bEditPro->setEnabled(false);
        ui->bEditProLook->setEnabled(false);
        ui->bSavePro->setEnabled(false);
        ui->bClearPro->setEnabled(false);
        ui->bNewTStep->setEnabled(false);
        //      ui->bNewPStep->setEnabled(false);
        //      ui->bNewVStep->setEnabled(false);
    }
    else
    {
        ui->bEditPro->setEnabled(true);
        ui->bEditProLook->setEnabled(true);
        currentProfile = index - 1;
        profileIndexMain = currentProfile;

    }
}

void MainWindow::on_bNewTStep_clicked()
{
    if (tProfileSave[currentProfile].totalStep == 0)
    {
       ///Eger kullanıcı baslangic adimindaysa


       // oldTValue = ui->dsbTStartValue->value();    /// OldTValue eski degerin tutulması icin baslangic degeri olarak kaydedilir.
        oldTValue = ui->leStartValue->text().toDouble();
        ui->laOldTValue->setText(QString::number(oldTValue));   ///OldTValue ekranda gösterilir.
    }


    if (  ( ui->cbTSelectSUnit->currentIndex() == 0 ) )
    {

        //nothing selected for neither step type nor step time unit. Going forward should be banned.
    }
    else
    {
        ui->bNewTStep->setEnabled(true);
        tProfileSave[currentProfile].startValue = ui->leStartValue->text().toDouble();
        if ( ui->cbTSelectSUnit->currentIndex() == 1 )
        {
            //second selected for step time unit, do what you gotta do.

            tProfileSave[currentProfile].active = 1;
            tProfileSave[currentProfile].step[currentTStep].stepUnit = 1;
          //  tProfileSave[currentProfile].startValue = ui->dsbTStartValue->value();


            ui->laTLinDurationSave->setText("s.");
        }
        else if ( ui->cbTSelectSUnit->currentIndex() == 2 )
        {
            //minute selected for step time unit, do what you gotta do.

            tProfileSave[currentProfile].active = 1;
            tProfileSave[currentProfile].step[currentTStep].stepUnit = 2;
       //     tProfileSave[currentProfile].startValue = ui->dsbTStartValue->value();

            ui->laTLinDurationSave->setText("m.");
        }
        else if ( ui->cbTSelectSUnit->currentIndex() == 3 )
        {
            //hour selected for step time unit, do what you gotta do.

            tProfileSave[currentProfile].active = 1;
            tProfileSave[currentProfile].step[currentTStep].stepUnit = 3;
       //     tProfileSave[currentProfile].startValue = ui->dsbTStartValue->value();

            ui->laTLinDurationSave->setText("h.");
        }
        else if ( ui->cbTSelectSUnit->currentIndex() == 4 )
        {
            //day selected for step time unit, do what you gotta do.

            tProfileSave[currentProfile].active = 1;
            tProfileSave[currentProfile].step[currentTStep].stepUnit = 4;
       //     tProfileSave[currentProfile].startValue = ui->dsbTStartValue->value();

            ui->laTLinDurationSave->setText("d.");
        }
        tProfileSave[currentProfile].step[currentTStep].stepType = 1; /// Sıcaklık adımı tipi linner olarak belirtildi.
        ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() + 1); ///bir sonraki pencereye geçildi
    }
}

void MainWindow::on_bTForward2_clicked()
{
    float delta;

   // tProfileSave[currentProfile].step[currentTStep].lDuration = ui->sbTLDuration->value();
   // tProfileSave[currentProfile].step[currentTStep].lTarget = ui->dsbTLTarget->value();

    tProfileSave[currentProfile].step[currentTStep].lDuration = ui->leTLDuration->text().toDouble();
    tProfileSave[currentProfile].step[currentTStep].lTarget = ui->leTLTarget->text().toDouble();

    float startValue = tProfileSave[currentProfile].startValue;
    float duration = tProfileSave[currentProfile].step[currentTStep].lDuration;
    float oldTarget = tProfileSave[currentProfile].step[currentTStep-1].lTarget;
    float target = tProfileSave[currentProfile].step[currentTStep].lTarget;

    if ( currentTStep == 0 )
    {
        delta = qFabs( target - startValue );
    }
    else
    {
        delta = qFabs( target - oldTarget );
    }

    if ( ui->cbTSelectSUnit->currentIndex() == 1 )
    {
        if ( ( delta * 60 / 5 ) <= ( duration * 1 ) )
        {
            ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() + 1);
            updateTPreview();
        }
    }
    else if ( ui->cbTSelectSUnit->currentIndex() == 2 )
    {
        if ( ( delta * 60 / 5 ) <= ( duration * 60 ) )
        {
            ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() + 1);
            updateTPreview();
        }
    }
    else if ( ui->cbTSelectSUnit->currentIndex() == 3 )
    {
        if ( ( delta * 60 / 5 ) <= ( duration * 60 * 60 ) )
        {
            ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() + 1);
            updateTPreview();
        }
    }
    else if ( ui->cbTSelectSUnit->currentIndex() == 4 )
    {
        if ( ( delta * 60 / 5 ) <= ( duration * 60 * 60 * 24 ) )
        {
            ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() + 1);
            updateTPreview();
        }
    }
}

void MainWindow::on_bTBack2_clicked()
{
    ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() - 1);
}

void MainWindow::on_bTBack3_clicked()
{
    ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() - 1);
}

void MainWindow::on_bTSaveStep_clicked()
{
    disconnect(ui->leStartValue,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    ui->leStartValue->setEnabled(false);

    tProfileSave[currentProfile].totalStep++;
    currentTStep++;
    ui->laTTotalStep->setText(QString::number(currentTStep));

    oldTValue = tProfileSave[currentProfile].step[currentTStep - 1].lTarget;
    ui->laOldTValue->setText(QString::number(oldTValue));

    ui->leTLDuration->setText("1");
    ui->leTLTarget->setText("0");

    //ui->cbTSelectSType->setCurrentIndex(0);
    ui->cbTSelectSUnit->setCurrentIndex(0);

    ui->tWidget->setCurrentIndex(0);
    ui->bSavePro->setEnabled(true);
}

void MainWindow::updateTPreview()
{
    //ui->tPreview->clearPlottables();
    ui->tPreview->xAxis->setRange(0, ui->leTLTarget->text().toDouble());
    ui->tPreview->yAxis->setRange(-40, 250);

    if (tProfileSave[currentProfile].step[currentTStep].stepUnit == 1)
    {
        ui->tPreview->xAxis->setLabel("Time (seconds)");
        ui->tPreview_2->xAxis->setLabel("Time (seconds)");
    }
    else if (tProfileSave[currentProfile].step[currentTStep].stepUnit == 2)
    {
        ui->tPreview->xAxis->setLabel("Time (minutes)");
        ui->tPreview_2->xAxis->setLabel("Time (minutes)");
    }
    else if (tProfileSave[currentProfile].step[currentTStep].stepUnit == 3)
    {
        ui->tPreview->xAxis->setLabel("Time (hours)");
        ui->tPreview_2->xAxis->setLabel("Time (hours)");
    }
    else if (tProfileSave[currentProfile].step[currentTStep].stepUnit == 4)
    {
        ui->tPreview->xAxis->setLabel("Time (days)");
        ui->tPreview_2->xAxis->setLabel("Time (days)");
    }


    float delta = 0;
    float duration = tProfileSave[currentProfile].step[currentTStep].lDuration;
    float startValue = tProfileSave[currentProfile].startValue;
    float oldTarget = tProfileSave[currentProfile].step[currentTStep-1].lTarget;
    float target = tProfileSave[currentProfile].step[currentTStep].lTarget;
    float oldDuration= tProfileSave[currentProfile].step[currentTStep-1].lDuration;

    QVector<double> tPreviewX( ( duration * 10 ) + 1 ), tPreviewY( ( duration * 10 ) + 1 );
    QVector<double> tPreviewX_2(21), tPreviewY_2( 21 );

    if (currentTStep == 0)
    {
       tPreviewY[0] = startValue;
        tPreviewX[0] = 0;
        delta = target - startValue;
        tPreviewY_2[1] = target;
        tPreviewX_2[1] = duration;
        tPreviewY_2[0] = startValue;
        tPreviewX_2[0] = 0.1;
    }
    else
    {
        tPreviewY[0] = oldTarget;
        tPreviewX[0] = 0;
        delta = target - oldTarget;
        if (currentTStep == 1)
        {
            tPreviewY_2[0] = startValue;
            tPreviewX_2[0] = 1;
            tPreviewY_2[1] = oldTarget;
            tPreviewX_2[1] = oldDuration;
            tPreviewY_2[2] = target;
            tPreviewX_2[2] = tPreviewX_2[1] + duration;
        }
        else if (currentTStep == 2)
        {
            tPreviewY_2[0] = startValue;
            tPreviewX_2[0] = 1;
            tPreviewY_2[1] = tProfileSave[currentProfile].step[currentTStep-2].lTarget;
            tPreviewX_2[1] = tProfileSave[currentProfile].step[currentTStep-2].lDuration;
            tPreviewY_2[2] = tProfileSave[currentProfile].step[currentTStep-1].lTarget;
            tPreviewX_2[2] = tPreviewX_2[1] + tProfileSave[currentProfile].step[currentTStep-1].lDuration;
            tPreviewY_2[3] = target;
            tPreviewX_2[3] = tPreviewX_2[2] + duration ;
        }
        else if (currentTStep == 3)
        {
            tPreviewY_2[0] = startValue;
            tPreviewX_2[0] = 1;
            tPreviewY_2[1] = tProfileSave[currentProfile].step[currentTStep-3].lTarget;
            tPreviewX_2[1] = tProfileSave[currentProfile].step[currentTStep-3].lDuration;
            tPreviewY_2[2] = tProfileSave[currentProfile].step[currentTStep-2].lTarget;
            tPreviewX_2[2] = tPreviewX_2[currentTStep - 2] + tProfileSave[currentProfile].step[currentTStep-2].lDuration;
            tPreviewY_2[3] = tProfileSave[currentProfile].step[currentTStep-1].lTarget;
            tPreviewX_2[3] = tPreviewX_2[currentTStep - 1] + tProfileSave[currentProfile].step[currentTStep-1].lDuration;
            tPreviewY_2[4] = target;
            tPreviewX_2[4] = tPreviewX_2[currentTStep] + duration ;
        }
        else if (currentTStep >= 4)
        {
            tPreviewY_2[0] = startValue;
            tPreviewX_2[0] = 1;
            for(int j=1; j < (currentTStep + 1 ); j++)
            {
                tPreviewY_2[j] = tProfileSave[currentProfile].step[j-1].lTarget;
                tPreviewX_2[j] = tPreviewX_2[j-1] + tProfileSave[currentProfile].step[j-1].lDuration;
            }
            tPreviewY_2[currentTStep + 1] = target;
            tPreviewX_2[currentTStep + 1] = tPreviewX_2[currentTStep] + duration ;
        }
    }

    for(int i=1; i < ( duration * 10) + 1; i++)
    {
        tPreviewY[i] = tPreviewY[i-1] + ( delta / ( duration * 10 ) );
        tPreviewX[i] = float(i) / 10;
    }

    ui->tPreview->graph(0)->setData(tPreviewX, tPreviewY);
    ui->tPreview->graph(0)->rescaleAxes();
    ui->tPreview->replot();

    connect(ui->tPreview_2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->tPreview_2->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->tPreview_2->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->tPreview_2->yAxis2, SLOT(setRange(QCPRange)));


    ui->tPreview_2->graph(0)->setData(tPreviewX_2, tPreviewY_2);
    ui->tPreview_2->graph(0)->rescaleAxes();
    ui->tPreview_2->replot();


}



void MainWindow::updateVPreview()
{
    //ui->vPreview->clearPlottables();
    //ui->vPreview->xAxis->setRange(0, ui->dsbVLTarget->value());
    //ui->vPreview->yAxis->setRange(1, 60);

    if (vProfileSave[currentProfile].step[currentVStep].stepUnit == 1)
    {
        ui->vPreview->xAxis->setLabel("Time (seconds)");
    }
    else if (vProfileSave[currentProfile].step[currentVStep].stepUnit == 2)
    {
        ui->vPreview->xAxis->setLabel("Time (minutes)");
    }
    else if (vProfileSave[currentProfile].step[currentVStep].stepUnit == 3)
    {
        ui->vPreview->xAxis->setLabel("Time (hours)");
    }
    else if (vProfileSave[currentProfile].step[currentVStep].stepUnit == 4)
    {
        ui->vPreview->xAxis->setLabel("Time (days)");
    }

    if (vProfileSave[currentProfile].step[currentVStep].stepType == 1)
    {
        float delta;
        float duration = vProfileSave[currentProfile].step[currentVStep].lDuration;
        float startValue = vProfileSave[currentProfile].startValue;
        float oldTarget = vProfileSave[currentProfile].step[currentVStep - 1].lTarget;
        float target = vProfileSave[currentProfile].step[currentVStep].lTarget;

        QVector<double> vPreviewX( ( duration * 10) + 1 ), vPreviewY( ( duration * 10) + 1 );

        if (currentVStep == 0)
        {
            vPreviewY[0] = startValue;
            vPreviewX[0] = 0;
            delta = target - startValue;
        }
        else
        {
            vPreviewY[0] = oldTarget;
            vPreviewX[0] = 0;
            delta = target - oldTarget;
        }

        for(int i=1; i < ( duration * 10) + 1; i++)
        {
            vPreviewY[i] = vPreviewY[i-1] + ( delta / ( duration * 10 ) );
            vPreviewX[i] = float(i) / 10;
        }

        ui->vPreview->graph(0)->setData(vPreviewX, vPreviewY);
        ui->vPreview->graph(0)->rescaleAxes();
        ui->vPreview->replot();
    }
    else if (vProfileSave[currentProfile].step[currentVStep].stepType == 4)
    {
        float rate = vProfileSave[currentProfile].step[currentVStep].logRate;
        float min = vProfileSave[currentProfile].step[currentVStep].logMin;
        float max = vProfileSave[currentProfile].step[currentVStep].logMax;
        double coeff = qPow(2, rate/600.0);
        quint16 duration=0;
        double buddy = min;
        while (buddy < max)
        {
            buddy = buddy * coeff;
            duration++;
        }

        duration = duration * 2;

        QVector<double> vPreviewX( ( duration) + 1 ), vPreviewY( ( duration) + 1 );

        vPreviewY[0] = min;
        vPreviewX[0] = 0;

        for(int i=1; i < ( duration / 2 ) + 1; i++)
        {
            vPreviewY[i] = vPreviewY[i-1] * coeff ;
            vPreviewX[i] = float(i) / 10;
        }
        for(int i=( duration / 2 ) + 1; i < (duration) + 1; i++)
        {
            vPreviewY[i] = vPreviewY[i-1] / coeff ;
            vPreviewX[i] = float(i) / 10;
        }

        ui->vPreview->graph(0)->setData(vPreviewX, vPreviewY);
        ui->vPreview->graph(0)->rescaleAxes();
        ui->vPreview->replot();

    }
}


bool MainWindow::readProfiles(char rType, int index)
{
    if (index == 0)
    {

    }
    else
    {
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
            if (rType != 'g')
            {
            QMessageBox::information(
                        this,
                        tr(appName),
                        tr("Could not open profile file to be read.") );
            return false;
            }
            else
            {
                ui->cbSelectProfileMain->addItem(QString::number(index) + " - bos slot");
                ui->cbSelectProfileMain->setItemData(index, QColor( Qt::gray ), Qt::TextColorRole );
                ui->cbSelectProfile->addItem(QString::number(index) + " - bos slot");
                ui->cbSelectProfile->setItemData(index, QColor( Qt::gray ), Qt::TextColorRole );
                ui->cbSelectProfileManual->addItem(QString::number(index) + " - bos slot");
                ui->cbSelectProfileManual->setItemData(index, QColor( Qt::gray ), Qt::TextColorRole );

            }
        }
        else
        {
            rProfile = file.readAll();

            QString name;

            if (rType == 'e')
            {
                pos = 0;

                if (rProfile[pos] == '/')
                {
               //   ui->leProfileNameEdit->setText("No Name Given");
                }
                else
                {
                    while (rProfile[pos] != '/')
                    {
                        name.append(char(rProfile[pos]));
                        pos++;
                    }
             //       ui->leProfileNameEdit->setText(name);
                }
                pos=pos+26;

                tProfileEdit[index - 1].active = rProfile[pos];
                pos = pos + 2;

                tProfileEdit[index - 1].startValue = qint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                pos = pos + 3;

                tProfileEdit[index - 1].totalStep = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));
                pos = pos + 3;

                for(int i=0; i<tProfileEdit[index - 1].totalStep; i++)
                {
                    pos = pos + 7;

                    tProfileEdit[index - 1].step[i].stepUnit = rProfile[pos];
                    pos = pos + 2;

                    tProfileEdit[index - 1].step[i].stepType = rProfile[pos];
                    pos = pos + 2;

                    tProfileEdit[index - 1].step[i].lDuration = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));
                    pos = pos + 3;

                    tProfileEdit[index - 1].step[i].lTarget = qint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;
                }

                pos = pos + 22;

                pProfileEdit[index - 1].active = rProfile[pos];
                pos = pos + 2;

                pProfileEdit[index - 1].startValue = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                pos = pos + 3;

                pProfileEdit[index - 1].totalStep = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));
                pos = pos + 3;

                for(int i=0; i<pProfileEdit[index - 1].totalStep; i++)
                {
                    pos = pos + 7;
                    pProfileEdit[index - 1].step[i].stepUnit = rProfile[pos];
                    pos = pos + 2;

                    pProfileEdit[index - 1].step[i].stepType = rProfile[pos];
                    pos = pos + 2;

                    pProfileEdit[index - 1].step[i].lDuration = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileEdit[index - 1].step[i].lTarget = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileEdit[index - 1].step[i].tRise = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileEdit[index - 1].step[i].tUp = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileEdit[index - 1].step[i].tFall = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileEdit[index - 1].step[i].tDown = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileEdit[index - 1].step[i].tLow = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileEdit[index - 1].step[i].tHigh = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileEdit[index - 1].step[i].sPeriod = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileEdit[index - 1].step[i].sMean = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileEdit[index - 1].step[i].sAmp = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileEdit[index - 1].step[i].repeatUnit = rProfile[pos];
                    pos = pos + 2;

                    pProfileEdit[index - 1].step[i].repeatDuration = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;
                }

                pos = pos + 23;

                //qDebug() << QByteArray::number(rProfile[pos],16);
                //qDebug() << QByteArray::number(rProfile[pos+1],16);

                vProfileEdit[index - 1].active = rProfile[pos];
                pos = pos + 2;

                vProfileEdit[index - 1].startValue = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                pos = pos + 3;

                vProfileEdit[index - 1].totalStep = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));
                pos = pos + 3;

                for(int i=0; i<vProfileEdit[index - 1].totalStep; i++)
                {
                    pos = pos + 7;

                    vProfileEdit[index - 1].step[i].stepUnit = rProfile[pos];
                    pos = pos + 2;

                    vProfileEdit[index - 1].step[i].stepType = rProfile[pos];
                    pos = pos + 2;

                    vProfileEdit[index - 1].step[i].lDuration = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));
                    pos = pos + 3;

                    vProfileEdit[index - 1].step[i].lTarget = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    vProfileEdit[index - 1].step[i].logRate = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 1000.0;
                    pos = pos + 3;

                    vProfileEdit[index - 1].step[i].logMin = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    vProfileEdit[index - 1].step[i].logMax = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    vProfileEdit[index - 1].step[i].repeatUnit = rProfile[pos];
                    pos = pos + 2;

                    vProfileEdit[index - 1].step[i].repeatDuration = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;
                }
            }
            else if (rType == 'l')
            {
                pos = 0;

                if (rProfile[pos] == '/')
                {
                    ui->laSelectedProfileMain->setText("isimsiz");
                    ui->laSelectedProfileManual->setText("isimsiz");
                    ui->laTestName->setText("isimsiz");
                }
                else
                {
                    while (rProfile[pos] != '/')
                    {
                        name.append(char(rProfile[pos]));
                        pos++;
                    }
                    ui->laSelectedProfileMain->setText(name);
                    ui->laSelectedProfileManual->setText(name);
                    ui->laTestName->setText(name);
                }
                pos=pos+26;

                tProfileLoad[index - 1].active = rProfile[pos];
                pos = pos + 2;

                tProfileLoad[index - 1].startValue = qint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                pos = pos + 3;

                tProfileLoad[index - 1].totalStep = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));
                pos = pos + 3;

                for(int i=0; i<tProfileLoad[index - 1].totalStep; i++)
                {
                    pos = pos + 7;

                    tProfileLoad[index - 1].step[i].stepUnit = rProfile[pos];
                    pos = pos + 2;

                    tProfileLoad[index - 1].step[i].stepType = rProfile[pos];
                    pos = pos + 2;

                    tProfileLoad[index - 1].step[i].lDuration = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));
                    pos = pos + 3;

                    tProfileLoad[index - 1].step[i].lTarget = qint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;
                }

                pos = pos + 22;

                pProfileLoad[index - 1].active = rProfile[pos];
                pos = pos + 2;

                pProfileLoad[index - 1].startValue = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                pos = pos + 3;

                pProfileLoad[index - 1].totalStep = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));
                pos = pos + 3;

                for(int i=0; i<pProfileLoad[index - 1].totalStep; i++)
                {
                    pos = pos + 7;

                    pProfileLoad[index - 1].step[i].stepUnit = rProfile[pos];
                    pos = pos + 2;

                    pProfileLoad[index - 1].step[i].stepType = rProfile[pos];
                    pos = pos + 2;

                    pProfileLoad[index - 1].step[i].lDuration = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileLoad[index - 1].step[i].lTarget = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileLoad[index - 1].step[i].tRise = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileLoad[index - 1].step[i].tUp = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileLoad[index - 1].step[i].tFall = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileLoad[index - 1].step[i].tDown = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileLoad[index - 1].step[i].tLow = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileLoad[index - 1].step[i].tHigh = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileLoad[index - 1].step[i].sPeriod = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileLoad[index - 1].step[i].sMean = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileLoad[index - 1].step[i].sAmp = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    pProfileLoad[index - 1].step[i].repeatUnit = rProfile[pos];
                    pos = pos + 2;

                    pProfileLoad[index - 1].step[i].repeatDuration = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;
                }

                pos = pos + 23;

                vProfileLoad[index - 1].active = rProfile[pos];
                pos = pos + 2;

                vProfileLoad[index - 1].startValue = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                pos = pos + 3;

                vProfileLoad[index - 1].totalStep = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));
                pos = pos + 3;

                for(int i=0; i<vProfileLoad[index - 1].totalStep; i++)
                {
                    pos = pos + 7;

                    vProfileLoad[index - 1].step[i].stepUnit = rProfile[pos];
                    pos = pos + 2;

                    vProfileLoad[index - 1].step[i].stepType = rProfile[pos];
                    pos = pos + 2;

                    vProfileLoad[index - 1].step[i].lDuration = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));
                    pos = pos + 3;

                    vProfileLoad[index - 1].step[i].lTarget = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    vProfileLoad[index - 1].step[i].logRate = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 1000.0;
                    pos = pos + 3;

                    vProfileLoad[index - 1].step[i].logMin = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    vProfileLoad[index - 1].step[i].logMax = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;

                    vProfileLoad[index - 1].step[i].repeatUnit = rProfile[pos];
                    pos = pos + 2;

                    vProfileLoad[index - 1].step[i].repeatDuration = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
                    pos = pos + 3;
                }
            }
            else if(rType == 'g')
            {
                pos = 0;

                if (rProfile[pos] == '/')
                {
                    ui->cbSelectProfileMain->addItem(QString::number(index) + " - " + "isimsiz");
                    ui->cbSelectProfile->addItem(QString::number(index) + " - " + "isimsiz");
                    ui->cbSelectProfileManual->addItem(QString::number(index) + " - " + "isimsiz");
                }
                else
                {
                    while (rProfile[pos] != '/')
                    {
                        name.append(char(rProfile[pos]));
                        pos++;
                    }
                    ui->cbSelectProfileMain->addItem( QString::number(index) + " - " + name);
                    ui->cbSelectProfile->addItem(QString::number(index) + " - " + name);
                    ui->cbSelectProfileManual->addItem(QString::number(index) + " - " + name);
                }

            }
        }
    }
    return true;
}

void MainWindow::on_bSavePro_clicked()
{
    disconnect(ui->leTLDuration,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    disconnect(ui->leTLTarget,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    QByteArray profile;
       ui->tPreview_2->graph(0)->data().clear();
       ui->tPreview_2->removeGraph(0);
       ui->tPreview_2->clearGraphs();
       ui->tPreview_2->clearItems();
       ui->tPreview_2->clearMask();
       ui->tPreview_2->clearPlottables();
       ui->tPreview_2->replot();

        setupPreviewGraphs();

#ifdef Q_OS_LINUX
    //linux code goes here
    QString filePath = "/home/pi/InDetail/profiles/Profile " + QString::number( ui->cbSelectProfile->currentIndex()) + ".dat";
#endif
#ifdef Q_OS_WIN
    // windows code goes here
    QString filePath = "profiles\\Profile " + QString::number(ui->cbSelectProfile->currentIndex()) + ".dat";
#endif

    QFile file(filePath);

    file.remove();

    profile.append(ui->leProfileName->text().toLocal8Bit());
    profile.append("/");

    profile.append("Temperature General Info");
    profile.append("/");
    profile.append(tProfileSave[currentProfile].active);
    profile.append("/");
    float tStart = tProfileSave[currentProfile].startValue*10.0;
    profile.append(qint16(tStart) & 0x00FF);
    profile.append(qint16(tStart) >> 8);
    profile.append("/");
    float tTotalStep = tProfileSave[currentProfile].totalStep;
    profile.append(quint16(tTotalStep) & 0x00FF);
    profile.append(quint16(tTotalStep) >> 8);

    for(int i=0; i<tProfileSave[currentProfile].totalStep; i++)
    {
        profile.append("/");
        profile.append("Step-");
        profile.append(i+1);
        profile.append("/");
        quint8 tStepUnit = tProfileSave[currentProfile].step[i].stepUnit;
        profile.append(tStepUnit);
        profile.append("/");
        quint8 tStepType = tProfileSave[currentProfile].step[i].stepType;
        profile.append(tStepType);
        profile.append("/");
        float tLDuration = tProfileSave[currentProfile].step[i].lDuration;
        profile.append(quint16(tLDuration) & 0x00FF);
        profile.append(quint16(tLDuration) >> 8);
        profile.append("/");
        float tLTarget = tProfileSave[currentProfile].step[i].lTarget*10.0;
        profile.append(qint16(tLTarget) & 0x00FF);
        profile.append(qint16(tLTarget) >> 8);
    }

    profile.append("/");
    profile.append("Pressure General Info");
    profile.append("/");
    profile.append(pProfileSave[currentProfile].active);
    profile.append("/");
    float pStart = pProfileSave[currentProfile].startValue*10.0;
    profile.append(quint16(pStart) & 0x00FF);
    profile.append(quint16(pStart) >> 8);
    profile.append("/");
    float pTotalStep = pProfileSave[currentProfile].totalStep;
    profile.append(quint16(pTotalStep) & 0x00FF);
    profile.append(quint16(pTotalStep) >> 8);

    for(int i=0; i<pProfileSave[currentProfile].totalStep; i++)
    {
        profile.append("/");
        profile.append("Step-");
        profile.append(i+1);
        profile.append("/");
        quint8 pStepUnit = pProfileSave[currentProfile].step[i].stepUnit;
        profile.append(pStepUnit);
        profile.append("/");
        quint8 pStepType = pProfileSave[currentProfile].step[i].stepType;
        profile.append(pStepType);
        profile.append("/");
        float pLDuration = pProfileSave[currentProfile].step[i].lDuration*10.0;
        profile.append(quint16(pLDuration) & 0x00FF);
        profile.append(quint16(pLDuration) >> 8);
        profile.append("/");
        float pLTarget = pProfileSave[currentProfile].step[i].lTarget*10.0;
        profile.append(quint16(pLTarget) & 0x00FF);
        profile.append(quint16(pLTarget) >> 8);
        profile.append("/");
        float pTRise = pProfileSave[currentProfile].step[i].tRise*10.0;
        profile.append(quint16(pTRise) & 0x00FF);
        profile.append(quint16(pTRise) >> 8);
        profile.append("/");
        float pTUp = pProfileSave[currentProfile].step[i].tUp*10.0;
        profile.append(quint16(pTUp) & 0x00FF);
        profile.append(quint16(pTUp) >> 8);
        profile.append("/");
        float pTFall = pProfileSave[currentProfile].step[i].tFall*10.0;
        profile.append(quint16(pTFall) & 0x00FF);
        profile.append(quint16(pTFall) >> 8);
        profile.append("/");
        float pTDown = pProfileSave[currentProfile].step[i].tDown*10.0;
        profile.append(quint16(pTDown) & 0x00FF);
        profile.append(quint16(pTDown) >> 8);
        profile.append("/");
        float ptLow = pProfileSave[currentProfile].step[i].tLow*10.0;
        profile.append(quint16(ptLow) & 0x00FF);
        profile.append(quint16(ptLow) >> 8);
        profile.append("/");
        float ptHigh = pProfileSave[currentProfile].step[i].tHigh*10.0;
        profile.append(quint16(ptHigh) & 0x00FF);
        profile.append(quint16(ptHigh) >> 8);
        profile.append("/");
        float pSPeriod = pProfileSave[currentProfile].step[i].sPeriod*10.0;
        profile.append(quint16(pSPeriod) & 0x00FF);
        profile.append(quint16(pSPeriod) >> 8);
        profile.append("/");
        float pSMean = pProfileSave[currentProfile].step[i].sMean*10.0;
        profile.append(quint16(pSMean) & 0x00FF);
        profile.append(quint16(pSMean) >> 8);
        profile.append("/");
        float pSAmp = pProfileSave[currentProfile].step[i].sAmp*10.0;
        profile.append(quint16(pSAmp) & 0x00FF);
        profile.append(quint16(pSAmp) >> 8);
        profile.append("/");
        quint8 pRepeatUnit = pProfileSave[currentProfile].step[i].repeatUnit;
        profile.append(pRepeatUnit);
        profile.append("/");
        float pRepeatDuration = pProfileSave[currentProfile].step[i].repeatDuration*10.0;
        profile.append(quint16(pRepeatDuration) & 0x00FF);
        profile.append(quint16(pRepeatDuration) >> 8);
    }

    profile.append("/");
    profile.append("Vibration General Info");
    profile.append("/");
    profile.append(vProfileSave[currentProfile].active);
    profile.append("/");
    float vStart = vProfileSave[currentProfile].startValue*10.0;
    profile.append(quint16(vStart) & 0x00FF);
    profile.append(quint16(vStart) >> 8);
    profile.append("/");
    float vTotalStep = vProfileSave[currentProfile].totalStep;
    profile.append(quint16(vTotalStep) & 0x00FF);
    profile.append(quint16(vTotalStep) >> 8);

    for(int i=0; i<vProfileSave[currentProfile].totalStep; i++)
    {
        profile.append("/");
        profile.append("Step-");
        profile.append(i+1);
        profile.append("/");
        quint8 vStepUnit = vProfileSave[currentProfile].step[i].stepUnit;
        profile.append(vStepUnit);
        profile.append("/");
        quint8 vStepType = vProfileSave[currentProfile].step[i].stepType;
        profile.append(vStepType);
        profile.append("/");
        float vLDuration = vProfileSave[currentProfile].step[i].lDuration;
        profile.append(quint16(vLDuration) & 0x00FF);
        profile.append(quint16(vLDuration) >> 8);
        profile.append("/");
        float vLTarget = vProfileSave[currentProfile].step[i].lTarget*10.0;
        profile.append(quint16(vLTarget) & 0x00FF);
        profile.append(quint16(vLTarget) >> 8);
        profile.append("/");
        float vLogRate = vProfileSave[currentProfile].step[i].logRate*1000;
        profile.append(quint16(vLogRate) & 0x00FF);
        profile.append(quint16(vLogRate) >> 8);
        profile.append("/");
        float vLogMin = vProfileSave[currentProfile].step[i].logMin*10.0;
        profile.append(quint16(vLogMin) & 0x00FF);
        profile.append(quint16(vLogMin) >> 8);
        profile.append("/");
        float vLogMax = vProfileSave[currentProfile].step[i].logMax*10.0;
        profile.append(quint16(vLogMax) & 0x00FF);
        profile.append(quint16(vLogMax) >> 8);
        profile.append("/");
        quint8 vRepeatUnit = vProfileSave[currentProfile].step[i].repeatUnit;
        profile.append(vRepeatUnit);
        profile.append("/");
        float vRepeatDuration = vProfileSave[currentProfile].step[i].repeatDuration*10.0;
        profile.append(quint16(vRepeatDuration) & 0x00FF);
        profile.append(quint16(vRepeatDuration) >> 8);
    }

    profile.append("/End of Profile.");



    QString name = QString::number(currentProfile + 1) +" - "+ ui->leProfileName->text();
    ui->cbSelectProfileMain->setItemText(currentProfile + 1, name);
    ui->cbSelectProfile->setItemText(currentProfile + 1, name);
    ui->cbSelectProfileManual->setItemText(currentProfile + 1, name);

    ui->cbSelectProfile->setCurrentIndex(0);
    ui->cbSelectProfile->setEnabled(true);

    ui->leProfileName->setText("");
    ui->leProfileName->setEnabled(false);

    ui->leStartValue->setText("0");
    ui->leStartValue->setEnabled(false);
    //  ui->dsbPStartValue->setValue(0);
    //  ui->dsbPStartValue->setEnabled(false);
    //  ui->dsbVStartValue->setValue(0);
    //  ui->dsbVStartValue->setEnabled(false);

    ui->laTTotalStep->setText("0");
    //  ui->laPTotalStep->setText("0");
    //  ui->laVTotalStep->setText("0");

    ui->tWidget->setCurrentIndex(0);
    //  ui->pWidget->setCurrentIndex(0);
    //  ui->vWidget->setCurrentIndex(0);

    ui->cbTSelectSUnit->setEnabled(false);
    //  ui->cbPSelectSUnit->setEnabled(false);
    //  ui->cbVSelectSUnit->setEnabled(false);
    //ui->cbTSelectSType->setEnabled(false);
    //  ui->cbPSelectSType->setEnabled(false);
    //  ui->cbVSelectSType->setEnabled(false);

    ui->tPreview->graph(0)->data().clear();
//    ui->tPreview->clearPlottables();
//    ui->tPreview->clearGraphs();
//    ui->tPreview->clearItems();
    ui->tPreview->replot();

    ui->tPreview_2->graph(0)->data().clear();
//    ui->tPreview_2->clearPlottables();
//    ui->tPreview_2->clearGraphs();
//    ui->tPreview_2->clearItems();
    ui->tPreview_2->replot();


    //  ui->pPreview->clearPlottables();
    //  ui->vPreview->clearPlottables();

    currentTStep = 0;
    currentPStep = 0;
    currentVStep = 0;

    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(
                    this,
                    tr(appName),
                    tr("Could not create profile file to be written.") );
    }
    else
    {
        if (file.write(profile))
        {
            file.flush();
            file.close();
            QMessageBox::information(
                        this,
                        tr(appName),
                        tr("Profile saved successfully.") );
        }
        else
        {
            QMessageBox::information(
                        this,
                        tr(appName),
                        tr("Error while writing profile info to profile file." ));
        }
    }

    ui->tabWidget->setTabEnabled(0, true);
    ui->tabWidget->setTabEnabled(1, true);
    ui->tabWidget->setTabEnabled(2, true);
    ui->tabWidget->setTabEnabled(4, true);
    ui->tabWidget->setTabEnabled(5, true);



}

void MainWindow::on_cbSelectProfileMain_currentIndexChanged(int index)
{
    //ui->laCurrentTCycleMain->setText("0");

    //ui->sbTTotalCycle->setValue(1);
    ui->leTTotalCycle->setText("1");
    ui->bStartTest->setEnabled(false);

    ui->cbSelectProfileManual->setCurrentIndex(0);



    if (index == 0)
    {
        ui->laSelectedProfileMain->setText("Recete secilmedi");
        ui->laSelectedProfileManual->setText("Recete secilmedi");
        ui->laTestName->setText("Recete secilmedi");
        ui->bSendProfileMain->setEnabled(false);
        totalTestDuration = 0;
    }
    else
    {
        quint8 index = ui->cbSelectProfileMain->currentIndex();
        if (readProfiles('l', index))
        {
            if (tProfileLoad[index-1].active == 0)
            {
      //          ui->sbTTotalCycle->setValue(0);
      //          ui->sbTTotalCycle->setEnabled(false);
                ui->leTTotalCycle->setText("0");
                ui->leTTotalCycle->setEnabled(false);
            }
            else
            {
       //         ui->sbTTotalCycle->setEnabled(true);
                ui->leTTotalCycle->setEnabled(true);
                for(int j=1; j <= (tProfileLoad[index-1].totalStep ); j++)
                {
                   if (tProfileLoad[index-1].step[j-1].stepUnit == 2 )
                   {
                    totalTestDuration = totalTestDuration + (tProfileLoad[index-1].step[j-1].lDuration * 60);
                   }
                }
      //          ui->laTotalTestSecond->setText(QString::number(totalTestDuration)) ;
                ui->progressBar_2->setValue(0);
                ui->progressBar_2->setMinimum(0);

                ui->progressBar_2->setMaximum(totalTestDuration);
            }
        }
    }

    ui->bSendProfileMain->setEnabled(true);

}

bool MainWindow::sendProfileOverSerial(QString mode, int index)
{
    proc->stop();

    QByteArray cantTouchThis;           // bu satırda artık mıcıtmıyorum
    float tStart = tProfileLoad[index-1].startValue*10.0;
    float tTotalStep = tProfileLoad[index-1].totalStep;
    quint16 tTotalCycle;
//    float tWaterTank;

    if (mode == "main")
    {
        //tTotalCycle = ui->sbTTotalCycle->value();
        tTotalCycle = ui->leTTotalCycle->text().toDouble();
//        ui->laTTotalCycleMain->setText(QString::number(tCycle));

    }
    else if (mode == "manual")
    {
        tTotalCycle = ui->sbTTotalCycleManual->value();
 //       ui->laTTotalCycleMain->setText(QString::number(tCycle));
        //       tWaterTank = ui->dsbTankTempSetManual->value()*10.0;
    }

    cantTouchThis.append(tProfileLoad[index-1].active);
    cantTouchThis.append(qint16(tStart) & 0x00FF);
    cantTouchThis.append(qint16(tStart) >> 8);
    cantTouchThis.append(quint16(tTotalStep) & 0x00FF);
    cantTouchThis.append(quint16(tTotalStep) >> 8);
    cantTouchThis.append(quint16(tTotalCycle) & 0x00FF);
    cantTouchThis.append(quint16(tTotalCycle) >> 8);
    cantTouchThis.append(index);
/*    cantTouchThis.append(quint16(tWaterTank) & 0x00FF);
    cantTouchThis.append(quint16(tWaterTank) >> 8);
*/
    proc->insertProfileMessage(mySerial::makeMessage(0x64,cantTouchThis));

    for(int i=0; i<tProfileLoad[index-1].totalStep; i++)
    {
        cantTouchThis.clear();

        quint8 tStepUnit = tProfileLoad[index-1].step[i].stepUnit;
        quint8 tStepType = tProfileLoad[index-1].step[i].stepType;
        float tLDuration = tProfileLoad[index-1].step[i].lDuration;
        float tLTarget = tProfileLoad[index-1].step[i].lTarget*10.0;

        cantTouchThis.append(i+1);
        cantTouchThis.append(tStepUnit);
        cantTouchThis.append(tStepType);
        cantTouchThis.append(quint16(tLDuration) & 0x00FF);
        cantTouchThis.append(quint16(tLDuration) >> 8);
        cantTouchThis.append(qint16(tLTarget) & 0x00FF);
        cantTouchThis.append(qint16(tLTarget) >> 8);

        proc->insertProfileMessage(mySerial::makeMessage(0x65,cantTouchThis));
    }

    cantTouchThis.clear();

    proc->insertProfileMessage(mySerial::makeMessage(0x66,cantTouchThis));

    cantTouchThis.clear();

    /*    float pStart = pProfileLoad[index-1].startValue*10.0;
    float pTotalStep = pProfileLoad[index-1].totalStep;
    quint16 pTotalCycle;



    cantTouchThis.append(pProfileLoad[index-1].active);
    cantTouchThis.append(quint16(pStart) & 0x00FF);
    cantTouchThis.append(quint16(pStart) >> 8);
    cantTouchThis.append(quint16(pTotalStep) & 0x00FF);
    cantTouchThis.append(quint16(pTotalStep) >> 8);
    cantTouchThis.append(quint16(pTotalCycle) & 0x00FF);
    cantTouchThis.append(quint16(pTotalCycle) >> 8);

    proc->insertProfileMessage(mySerial::makeMessage(0x67,cantTouchThis));

    for(int i=0; i<pProfileLoad[index-1].totalStep; i++)
    {
        if (pProfileLoad[index-1].step[i].stepType == 1)
        {
            cantTouchThis.clear();

            quint8 pStepUnit = pProfileLoad[index-1].step[i].stepUnit;
            quint8 pStepType = pProfileLoad[index-1].step[i].stepType;
            float pLDuration = pProfileLoad[index-1].step[i].lDuration*10.0;
            float pLTarget = pProfileLoad[index-1].step[i].lTarget*10.0;

            cantTouchThis.append(i+1);
            cantTouchThis.append(pStepUnit);
            cantTouchThis.append(pStepType);
            cantTouchThis.append(quint16(pLDuration) & 0x00FF);
            cantTouchThis.append(quint16(pLDuration) >> 8);
            cantTouchThis.append(quint16(pLTarget) & 0x00FF);
            cantTouchThis.append(quint16(pLTarget) >> 8);

            proc->insertProfileMessage(mySerial::makeMessage(0x68,cantTouchThis));
        }
        else if (pProfileLoad[index-1].step[i].stepType == 2)
        {
            cantTouchThis.clear();

            quint8 pStepUnit = pProfileLoad[index-1].step[i].stepUnit;
            quint8 pStepType = pProfileLoad[index-1].step[i].stepType;
            float pTRise = pProfileLoad[index-1].step[i].tRise*10.0;
            float pTUp = pProfileLoad[index-1].step[i].tUp*10.0;
            float pTFall = pProfileLoad[index-1].step[i].tFall*10.0;
            float pTDown = pProfileLoad[index-1].step[i].tDown*10.0;
            float ptLow = pProfileLoad[index-1].step[i].tLow*10.0;
            float ptHigh = pProfileLoad[index-1].step[i].tHigh*10.0;
            quint8 pRepeatUnit = pProfileLoad[index-1].step[i].repeatUnit;
            float pRepeatDuration = pProfileLoad[index-1].step[i].repeatDuration;

            cantTouchThis.append(i+1);
            cantTouchThis.append(pStepUnit);
            cantTouchThis.append(pStepType);
            cantTouchThis.append(quint16(pTRise) & 0x00FF);
            cantTouchThis.append(quint16(pTRise) >> 8);
            cantTouchThis.append(quint16(pTUp) & 0x00FF);
            cantTouchThis.append(quint16(pTUp) >> 8);
            cantTouchThis.append(quint16(pTFall) & 0x00FF);
            cantTouchThis.append(quint16(pTFall) >> 8);
            cantTouchThis.append(quint16(pTDown) & 0x00FF);
            cantTouchThis.append(quint16(pTDown) >> 8);
            cantTouchThis.append(quint16(ptLow) & 0x00FF);
            cantTouchThis.append(quint16(ptLow) >> 8);
            cantTouchThis.append(quint16(ptHigh) & 0x00FF);
            cantTouchThis.append(quint16(ptHigh) >> 8);
            cantTouchThis.append(pRepeatUnit);
            cantTouchThis.append(quint32(pRepeatDuration) & 0xFF);
            cantTouchThis.append((quint32(pRepeatDuration) >> 8)& 0xFF);
            cantTouchThis.append((quint32(pRepeatDuration) >> 16)& 0xFF);

            proc->insertProfileMessage(mySerial::makeMessage(0x68,cantTouchThis));
        }
        else if (pProfileLoad[index-1].step[i].stepType == 3)
        {
            cantTouchThis.clear();

            quint8 pStepUnit = pProfileLoad[index-1].step[i].stepUnit;
            quint8 pStepType = pProfileLoad[index-1].step[i].stepType;
            float pSPeriod = pProfileLoad[index-1].step[i].sPeriod*10.0;
            float pSMean = pProfileLoad[index-1].step[i].sMean*10.0;
            float pSAmp = pProfileLoad[index-1].step[i].sAmp*10.0;
            quint8 pRepeatUnit = pProfileLoad[index-1].step[i].repeatUnit;
            float pRepeatDuration = pProfileLoad[index-1].step[i].repeatDuration;

            cantTouchThis.append(i+1);
            cantTouchThis.append(pStepUnit);
            cantTouchThis.append(pStepType);
            cantTouchThis.append(quint16(pSPeriod) & 0x00FF);
            cantTouchThis.append(quint16(pSPeriod) >> 8);
            cantTouchThis.append(quint16(pSMean) & 0x00FF);
            cantTouchThis.append(quint16(pSMean) >> 8);
            cantTouchThis.append(quint16(pSAmp) & 0x00FF);
            cantTouchThis.append(quint16(pSAmp) >> 8);
            cantTouchThis.append(pRepeatUnit);
            cantTouchThis.append(quint16(pRepeatDuration) & 0x00FF);
            cantTouchThis.append(quint16(pRepeatDuration) >> 8);

            proc->insertProfileMessage(mySerial::makeMessage(0x68,cantTouchThis));
      }
  }
*/
    /*   cantTouchThis.clear();

    proc->insertProfileMessage(mySerial::makeMessage(0x69,cantTouchThis));

    cantTouchThis.clear();

    float vStart = vProfileLoad[index-1].startValue*10.0;
    float vTotalStep = vProfileLoad[index-1].totalStep;
    quint16 vTotalCycle;
    bool vEllipticalActive;



    cantTouchThis.append(vProfileLoad[index-1].active);
    cantTouchThis.append(quint16(vStart) & 0x00FF);
    cantTouchThis.append(quint16(vStart) >> 8);
    cantTouchThis.append(quint16(vTotalStep) & 0x00FF);
    cantTouchThis.append(quint16(vTotalStep) >> 8);
    cantTouchThis.append(quint16(vTotalCycle) & 0x00FF);
    cantTouchThis.append(quint16(vTotalCycle) >> 8);
    cantTouchThis.append(quint8(vEllipticalActive));

    proc->insertProfileMessage(mySerial::makeMessage(0x6A,cantTouchThis));

    for(int i=0; i<vProfileLoad[index-1].totalStep; i++)
    {
        if (vProfileLoad[index-1].step[i].stepType == 1)
        {
            cantTouchThis.clear();

            quint8 vStepUnit = vProfileLoad[index-1].step[i].stepUnit;
            quint8 vStepType = vProfileLoad[index-1].step[i].stepType;
            float vLDuration = vProfileLoad[index-1].step[i].lDuration;
            float vLTarget = vProfileLoad[index-1].step[i].lTarget*10.0;

            cantTouchThis.append(i+1);
            cantTouchThis.append(vStepUnit);
            cantTouchThis.append(vStepType);
            cantTouchThis.append(quint16(vLDuration) & 0x00FF);
            cantTouchThis.append(quint16(vLDuration) >> 8);
            cantTouchThis.append(quint16(vLTarget) & 0x00FF);
            cantTouchThis.append(quint16(vLTarget) >> 8);

            proc->insertProfileMessage(mySerial::makeMessage(0x6B,cantTouchThis));
        }
        else if (vProfileLoad[index-1].step[i].stepType == 4)
        {
            cantTouchThis.clear();

            quint8 vStepUnit = vProfileLoad[index-1].step[i].stepUnit;
            quint8 vStepType = vProfileLoad[index-1].step[i].stepType;
            float vLogRate = vProfileLoad[index-1].step[i].logRate*1000.0;
            float vLogMin = vProfileLoad[index-1].step[i].logMin*10.0;
            float vLogMax = vProfileLoad[index-1].step[i].logMax*10.0;
            quint8 vRepeatUnit = vProfileLoad[index-1].step[i].repeatUnit;
            float vRepeatDuration = vProfileLoad[index-1].step[i].repeatDuration;

            cantTouchThis.append(i+1);
            cantTouchThis.append(vStepUnit);
            cantTouchThis.append(vStepType);
            cantTouchThis.append(quint16(vLogRate) & 0x00FF);
            cantTouchThis.append(quint16(vLogRate) >> 8);
            cantTouchThis.append(quint16(vLogMin) & 0x00FF);
            cantTouchThis.append(quint16(vLogMin) >> 8);
            cantTouchThis.append(quint16(vLogMax) & 0x00FF);
            cantTouchThis.append(quint16(vLogMax) >> 8);
            cantTouchThis.append(vRepeatUnit);
            cantTouchThis.append(quint16(vRepeatDuration) & 0x00FF);
            cantTouchThis.append(quint16(vRepeatDuration) >> 8);

            proc->insertProfileMessage(mySerial::makeMessage(0x6B,cantTouchThis));
        }
    }
*/
    //    cantTouchThis.clear();

    //    proc->insertProfileMessage(mySerial::makeMessage(0x6C,cantTouchThis));

    if (mode == "main")
    {
        if (ui->cbSelectMethodManual->currentIndex() == 1)
        {
            cantTouchThis.clear();
            cantTouchThis.append(char(0x00));
            proc->insertProfileMessage(mySerial::makeMessage(0x6D,cantTouchThis));
        }
        else if (ui->cbSelectMethodManual->currentIndex() == 2)
        {
            cantTouchThis.clear();
            cantTouchThis.append(char(0x00));
            proc->insertProfileMessage(mySerial::makeMessage(0x6E,cantTouchThis));
        }

    }
    else if (mode == "manual")
    {
        quint16 tCycleToStart = ui->sbTCycleSetManual->value();
        //      quint16 pCycleToStart = ui->sbPCycleSetManual->value();
        //       quint16 vCycleToStart = ui->sbVCycleSetManual->value();

        if (ui->cbSelectMethodManual->currentIndex() == 1)
        {
            quint32 tSecondsToStart = ui->dsbTTimeSetManual->value();
            //         quint32 pSecondsToStart = ui->dsbPTimeSetManual->value();
            //         quint32 vSecondsToStart = ui->dsbVTimeSetManual->value();
            quint16 tCycleToStart = ui->sbTCycleSetManual->value();
            //         quint16 pCycleToStart = ui->sbPCycleSetManual->value();
            //        quint16 vCycleToStart = ui->sbVCycleSetManual->value();

            cantTouchThis.clear();
            cantTouchThis.append(0x01);

            cantTouchThis.append( (tSecondsToStart) & 0xFF );
            cantTouchThis.append( (tSecondsToStart >> 8) & 0xFF );
            cantTouchThis.append( (tSecondsToStart >> 16) & 0xFF );
            /*
            cantTouchThis.append( (pSecondsToStart) & 0xFF );
            cantTouchThis.append( (pSecondsToStart >> 8) & 0xFF );
            cantTouchThis.append( (pSecondsToStart >> 16) & 0xFF );

            cantTouchThis.append( (vSecondsToStart) & 0xFF );
            cantTouchThis.append( (vSecondsToStart >> 8) & 0xFF );
            cantTouchThis.append( (vSecondsToStart >> 16) & 0xFF );
*/
            cantTouchThis.append( (tCycleToStart) & 0x00FF);
            cantTouchThis.append( (tCycleToStart) >> 8);
            /*            cantTouchThis.append( (pCycleToStart) & 0x00FF);
            cantTouchThis.append( (pCycleToStart) >> 8);
            cantTouchThis.append( (vCycleToStart) & 0x00FF);
            cantTouchThis.append( (vCycleToStart) >> 8);
*/
            proc->insertProfileMessage(mySerial::makeMessage(0x6D,cantTouchThis));

            cantTouchThis.clear();
            cantTouchThis.append(char(0x00));
            proc->insertProfileMessage(mySerial::makeMessage(0x6E,cantTouchThis));
        }
        else if (ui->cbSelectMethodManual->currentIndex() == 2)
        {
            quint8 tStepToStart = ui->sbTStepSetManual->value();
            //            quint8 pStepToStart = ui->sbPStepSetManual->value();
            //           quint8 vStepToStart = ui->sbVStepSetManual->value();
            quint16 tStepSecondToStart = ui->sbTStepRepeatSetManual->value();
            //           quint16 pStepSecondToStart = ui->sbPStepRepeatSetManual->value();
            //           quint16 vStepSecondToStart = ui->sbVStepRepeatSetManual->value();

            cantTouchThis.clear();
            cantTouchThis.append(char(0x00));
            proc->insertProfileMessage(mySerial::makeMessage(0x6D,cantTouchThis));

            cantTouchThis.clear();
            cantTouchThis.append(0x01);
            cantTouchThis.append(tStepToStart);
            //            cantTouchThis.append(pStepToStart);
            //            cantTouchThis.append(vStepToStart);
            cantTouchThis.append( (tStepSecondToStart) & 0x00FF);
            cantTouchThis.append( (tStepSecondToStart) >> 8);
            /*          cantTouchThis.append( (pStepSecondToStart) & 0x00FF);
            cantTouchThis.append( (pStepSecondToStart) >> 8);
            cantTouchThis.append( (vStepSecondToStart) & 0x00FF);
            cantTouchThis.append( (vStepSecondToStart) >> 8);
 */         cantTouchThis.append( (tCycleToStart) & 0x00FF);
            cantTouchThis.append( (tCycleToStart) >> 8);
            /*           cantTouchThis.append( (pCycleToStart) & 0x00FF);
            cantTouchThis.append( (pCycleToStart) >> 8);
            cantTouchThis.append( (vCycleToStart) & 0x00FF);
            cantTouchThis.append( (vCycleToStart) >> 8);
*/
            proc->insertProfileMessage(mySerial::makeMessage(0x6E,cantTouchThis));
        }

    }

    proc->setProfile();

    return true;
}

void MainWindow::on_bStartTest_clicked()
{
    proc->stop();

    if (myPLC.deviceState == char(0x03))
    {

    }
    else
    {
        tKey = 0;
        pKey = 0;
        vKey = 0;

        //    ui->tTestGraph->graph(0)->data().clear();
        //    ui->pTestGraph->graph(0)->data().clear();
        //    ui->pTestGraph->graph(1)->data().clear();
        //    ui->pTestGraph->graph(2)->data().clear();
        //    ui->pTestGraph->graph(3)->data().clear();
        //    ui->pTestGraph->graph(4)->data().clear();
        //    ui->pTestGraph->graph(5)->data().clear();
        //    ui->vTestGraph->graph(0)->data().clear();
        //    ui->tTestGraph->replot();
        //    ui->pTestGraph->replot();
        //    ui->vTestGraph->replot();

        ui->tTestGraph->clearPlottables();
        ui->pTestGraph->clearPlottables();
        //ui->vTestGraph->clearPlottables();

        setupTGraph();
        setupPGraphs();
        setupVGraph();
    }

    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(0x02);
    proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));

}

void MainWindow::on_bStopTest_clicked()
{
    timerTemp->stop();
    //  timerVib->stop();
    timerPressure->stop();

    proc->stop();

    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(0x01);
    proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
}

void MainWindow::askSensorValues()
{
    QByteArray empty,cantTouchThis;

    cantTouchThis.append(mySerial::makeMessage(0x32,empty));
    //I will ask 0x33 with askOtherStuff function.
    //cantTouchThis.append(mySerial::makeMessage(0x33,empty));
    proc->setPeriodicMessage(cantTouchThis);
    proc->start();
}

void MainWindow::askOtherStuff()
{
    if (commStatus && (proc->state != proc->sProfileSend))
    {
        QByteArray empty, cantTouchThis;
        cantTouchThis.clear();
        if (askCounter == 1)
        {
            cantTouchThis.append(mySerial::makeMessage(0x0C,empty));
        }
        else if (askCounter == 2)
        {
            cantTouchThis.append(mySerial::makeMessage(0x0D,empty));
        }
        else if (askCounter == 3)
        {
            cantTouchThis.append(mySerial::makeMessage(0x0E,empty));
        }
        else if (askCounter == 4)
        {
            cantTouchThis.append(mySerial::makeMessage(0x33,empty));
        }
        askCounter++;
        if (askCounter == 5) askCounter = 1;
        proc->insertCommandMessage(cantTouchThis);
    }
}

void MainWindow::getCurrentDateTime()
{
    ui->laTime->setText(QTime::currentTime().toString());
    ui->laDate->setText(QDate::currentDate().toString(Qt::SystemLocaleShortDate));

    ui->laTime_2->setText(QTime::currentTime().toString());
    ui->laDate_2->setText(QDate::currentDate().toString(Qt::SystemLocaleShortDate));

    ui->laTime_3->setText(QTime::currentTime().toString());
    ui->laDate_3->setText(QDate::currentDate().toString(Qt::SystemLocaleShortDate));
}

void MainWindow::updateTPlot()
{
    int tKeyHour;
    tKeyElapsed = tElapsedSeconds + (double(tempPeriod)/1000.0) ;
   // tKey = tElapsedSeconds + (double(tempPeriod)/1000.0) ;
    //tKey = QTime::currentTime().msecsSinceStartOfDay()/1000;
    tKey = tKey + (double(tempPeriod)/1000.0);
    // add data to lines:
    ui->tTestGraph->graph(0)->addData(tKey, cabinAverageTemp);
    ui->tTestGraph->graph(1)->addData(tKey, pipe1Pressure);
    // rescale key (horizontal) axis to fit the current data:
      ui->tTestGraph->graph(0)->rescaleKeyAxis();
    // replot the graph with the added data
    ui->tTestGraph->replot();

    tKeyHour= tElapsedSeconds / 3600;
    ui->pb_testProgress->setValue(tKeyHour);

#ifdef Q_OS_LINUX
    //linux code goes here
    QString filePath = "/home/pi/InDetail/records/" + testFolder + "/" + "temperature.csv";
#endif
#ifdef Q_OS_WIN
    // windows code goes here
    QString filePath;
    if (startFromRecords){
     filePath = dataFilePath;
    }
    else{
    filePath = "records\\" + testFolder + "\\" + "temperature.csv";
    }
    QString filePathLast ="lastTestName.txt";
#endif

    QFile file(filePath);

    if (file.open(QFile::WriteOnly|QFile::Append))
    {
        QTextStream stream(&file);
        stream << tKeyElapsed << "," << cabinAverageTemp << "," << pipe1Pressure << "\n";
        file.close();

    }

    QFile file2(filePathLast);

    if (file2.open(QFile::WriteOnly|QFile::Text)){
         QTextStream stream2(&file2);

         stream2<<QString::number(currentProfile)<< "\n"<<QString::number(tKey);
         file2.close();
    }

}

void MainWindow::updatePPlots()
{
    pKey = pKey + (double(pressurePeriod)/1000.0);

  //  pKey = QTime::currentTime().msecsSinceStartOfDay()/1000;
    ui->pTestGraph->graph(0)->addData(pKey, pipe1Pressure);
    ui->pTestGraph->graph(1)->addData(pKey, pipe2Pressure);
    ui->pTestGraph->graph(2)->addData(pKey, pipe3Pressure);
    ui->pTestGraph->graph(3)->addData(pKey, pipe4Pressure);
    ui->pTestGraph->graph(4)->addData(pKey, pipe5Pressure);
    ui->pTestGraph->graph(5)->addData(pKey, pipe6Pressure);

    // make key axis range scroll with the data (at a constant range size of 30):
//    ui->pTestGraph->xAxis->setRange(pKey, 60, Qt::AlignRight);


    // rescale key (horizontal) axis to fit the current data:
   // ui->pTestGraph->graph(0)->rescaleKeyAxis();

    ui->pTestGraph->xAxis->setRangeUpper(pKey);
    ui->pTestGraph->xAxis->setRangeLower(pKey-30);
    // replot the graph with the added data
    ui->pTestGraph->replot();

#ifdef Q_OS_LINUX
    //linux code goes here
    QString filePath = "/home/pi/InDetail/records/" + testFolder + "/" + "pressure.csv";
#endif
#ifdef Q_OS_WIN
    // windows code goes here
    QString filePath = "records\\" + testFolder + "\\" + "pressure.csv";
#endif

    QFile file(filePath);

    if (file.open(QFile::WriteOnly|QFile::Append))
    {
        QTextStream stream(&file);
        stream << pKey << "," << pipe1Pressure << "," << pipe2Pressure << "," << pipe3Pressure << "," <<
                  pipe4Pressure << "," << pipe5Pressure << "," << pipe6Pressure << "\n";
        file.close();
    }

}

void MainWindow::on_bScreenshot_saved()
{
    QScreen *screensave = QGuiApplication::primaryScreen();

    QPixmap pixmap = screensave->grabWindow(0);
    QString fileDir = "screenshots\\";
    QString fileName = QDate::currentDate().toString("dd.MM.yy") + "-" +
            QTime::currentTime().toString("hh.mm.ss");
    QString fileExtention = ".png";
    QFile file(fileDir + fileName + fileExtention);
}

void MainWindow::on_bScreenshot_clicked()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QPixmap pixmap = screen->grabWindow(0);
    //    QString fileDir = "/home/pi/InDetail/screenshots/";
        QString fileDir = "screenshots\\";
        QString fileName = QDate::currentDate().toString("dd.MM.yy") + "-" +
                QTime::currentTime().toString("hh.mm.ss");
        QString fileExtention = ".png";
        QFile file(fileDir + fileName + fileExtention);
        if (file.open(QIODevice::WriteOnly))
        {

            pixmap.save(&file, "PNG");
            QMessageBox::information(
                    this,
                    tr(appName),
                    tr("Ekran görüntüsü kaydedildi.") );
        }
        else{
            QMessageBox::information(
                        this,
                        tr(appName),
                        tr("Dosya yolu hatalı.") );
        }
    }
    else
    {
        QMessageBox::information(
                    this,
                    tr(appName),
                    tr("Ekran görüntüsü kayıt alınamadı. ") );
    }

}

void MainWindow::on_bStartMaintenance_clicked()
{
    if (ui->bStartMaintenance->isChecked())
    {

        ui->tabWidget->setTabEnabled(0, false);
        ui->tabWidget->setTabEnabled(1, false);
        ui->tabWidget->setTabEnabled(2, false);
        ui->tabWidget->setTabEnabled(3, false);
        ui->tabWidget->setTabEnabled(4, false);

        //  ui->bStartCooler->setEnabled(true);
        //  ui->bStartVibration->setEnabled(true);
        //  ui->dsbVibrationMaintenance->setEnabled(true);
        //  ui->bStartPressure->setEnabled(true);
        //  ui->dsbPressureMaintenance->setEnabled(true);
        ui->bRes->setEnabled(true);
        ui->chbRes1->setEnabled(true);
        ui->chbRes2->setEnabled(true);
        ui->chbRes3->setEnabled(true);
        ui->chbResTank->setEnabled(true);

        ui->chbExhaustValve->setEnabled(true);
        ui->bFan->setEnabled(true);
        ui->chbFan1->setEnabled(true);
        ui->chbFan2->setEnabled(true);
        ui->chbFan3->setEnabled(true);

        ui->bExhaustValve->setEnabled(true);
        ui->bSetExhaustValve->setEnabled(true);
        ui->dsbExhaustValveAutoValue->setEnabled(true);

        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x04);
        proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
        /*
            bool ok;

            int i = QInputDialog::getInt(this, tr(appName),
                                tr("Enter Password:"), 0, 0, 9999, 1, &ok);
        if (ok)
        {
            if (i == 1881)
            {
                ui->tabWidget->setTabEnabled(0, false);
                ui->tabWidget->setTabEnabled(1, false);
                ui->tabWidget->setTabEnabled(2, false);
                ui->tabWidget->setTabEnabled(3, false);
                ui->tabWidget->setTabEnabled(4, false);

                //  ui->bStartCooler->setEnabled(true);
                //  ui->bStartVibration->setEnabled(true);
                //  ui->dsbVibrationMaintenance->setEnabled(true);
                //  ui->bStartPressure->setEnabled(true);
                //  ui->dsbPressureMaintenance->setEnabled(true);
                ui->bRes->setEnabled(true);
                ui->chbRes1->setEnabled(true);
                ui->chbRes2->setEnabled(true);
                ui->chbRes3->setEnabled(true);
                ui->chbResTank->setEnabled(true);

                ui->chbExhaustValve->setEnabled(true);
                ui->bFan->setEnabled(true);
                ui->chbFan1->setEnabled(true);
                ui->chbFan2->setEnabled(true);
                ui->chbFan3->setEnabled(true);

                ui->bExhaustValve->setEnabled(true);
                ui->bSetExhaustValve->setEnabled(true);
                ui->dsbExhaustValveAutoValue->setEnabled(true);

                QByteArray cantTouchThis;
                cantTouchThis.clear();
                cantTouchThis.append(0x04);
                proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
            }
            else
            {
                QMessageBox::information(
                            this,
                            tr(appName),
                            tr("Wrong password is entered. No action will be processed.") );
                ui->bStartMaintenance->setChecked(false);
            }
        }

        */
    }
    else
    {
        ui->tabWidget->setTabEnabled(0, true);
        ui->tabWidget->setTabEnabled(1, true);
        ui->tabWidget->setTabEnabled(2, true);
        ui->tabWidget->setTabEnabled(3, true);
        ui->tabWidget->setTabEnabled(4, true);

        //  ui->bStartCooler->setEnabled(false);
        // ui->bStartVibration->setEnabled(false);
        // ui->dsbVibrationMaintenance->setEnabled(false);
        // ui->bStartPressure->setEnabled(false);
        // ui->dsbPressureMaintenance->setEnabled(false);
        ui->bRes->setEnabled(false);
        ui->chbRes1->setEnabled(false);
        ui->chbRes2->setEnabled(false);
        ui->chbRes3->setEnabled(false);
        ui->chbResTank->setEnabled(false);
        ui->bFan->setEnabled(false);
        ui->chbFan1->setEnabled(false);
        ui->chbFan2->setEnabled(false);
        ui->chbFan3->setEnabled(false);

        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x04);
        proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
    }
}

void MainWindow::on_bRes_clicked()
{
    QByteArray cantTouchThis;
    cantTouchThis.clear();

    if (ui->chbRes1->isChecked())
    {
        cantTouchThis.append(0x01);
    }
    else
    {
        cantTouchThis.append(char(0x00));
    }

    if (ui->chbRes2->isChecked())
    {
        cantTouchThis.append(0x01);
    }
    else
    {
        cantTouchThis.append(char(0x00));
    }

    if (ui->chbRes3->isChecked())
    {
        cantTouchThis.append(0x01);
    }
    else
    {
        cantTouchThis.append(char(0x00));
    }

    if (ui->chbResTank->isChecked())
    {
        cantTouchThis.append(0x01);
    }
    else
    {
        cantTouchThis.append(char(0x00));
    }

    proc->insertCommandMessage(mySerial::makeMessage(0x97,cantTouchThis));
}

void MainWindow::on_bFan_clicked()
{
    QByteArray cantTouchThis;
    cantTouchThis.clear();

    if (ui->chbFan1->isChecked())
    {
        cantTouchThis.append(0x01);
    }
    else
    {
        cantTouchThis.append(char(0x00));
    }

    if (ui->chbFan2->isChecked())
    {
        cantTouchThis.append(0x01);
    }
    else
    {
        cantTouchThis.append(char(0x00));
    }

    if (ui->chbFan3->isChecked())
    {
        cantTouchThis.append(0x01);
    }
    else
    {
        cantTouchThis.append(char(0x00));
    }

    proc->insertCommandMessage(mySerial::makeMessage(0x9B,cantTouchThis));
}

void MainWindow::on_bLightsMain_clicked()
{
    if (ui->bLightsMain->isChecked())
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x01);
        proc->insertCommandMessage(mySerial::makeMessage(0x9C,cantTouchThis));
    }
    else
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(char(0x00));
        proc->insertCommandMessage(mySerial::makeMessage(0x9C,cantTouchThis));
    }
}


void MainWindow::on_bClearLogTable_clicked()
{
    // ui->warningTable->setRowCount(0);
}

void MainWindow::clearProfileSlot(char sType, char pType, quint8 index)
{
    if (sType == 's')
    {
        if (pType == 't')
        {
            tProfileSave[index].active = 0;
            tProfileSave[index].totalStep = 0;
            tProfileSave[index].totalTestCycle = 0;
            tProfileSave[index].startValue = 0;
            for (int i=0; i<MaxStep; i++)
            {
                tProfileSave[index].step[i].stepUnit = 0;
                tProfileSave[index].step[i].stepType = 0;
                tProfileSave[index].step[i].lDuration = 0;
                tProfileSave[index].step[i].lTarget = 0;
                tProfileSave[index].step[i].tRise = 0;
                tProfileSave[index].step[i].tUp = 0;
                tProfileSave[index].step[i].tFall = 0;
                tProfileSave[index].step[i].tDown = 0;
                tProfileSave[index].step[i].tLow = 0;
                tProfileSave[index].step[i].tHigh = 0;
                tProfileSave[index].step[i].sPeriod = 0;
                tProfileSave[index].step[i].sMean = 0;
                tProfileSave[index].step[i].sAmp = 0;
                tProfileSave[index].step[i].logRate = 0;
                tProfileSave[index].step[i].logMin = 0;
                tProfileSave[index].step[i].logMax = 0;
                tProfileSave[index].step[i].repeatUnit = 0;
                tProfileSave[index].step[i].repeatDuration = 0;
            }
            for (int i=0; i<MaxStep; i++)
                tProfileSave[index].stepDuration[i] = 0;

        }
        else if (pType == 'p')
        {
            pProfileSave[index].active = 0;
            pProfileSave[index].totalStep = 0;
            pProfileSave[index].totalTestCycle = 0;
            pProfileSave[index].startValue = 0;
            for (int i=0; i<MaxStep; i++)
            {
                pProfileSave[index].step[i].stepUnit = 0;
                pProfileSave[index].step[i].stepType = 0;
                pProfileSave[index].step[i].lDuration = 0;
                pProfileSave[index].step[i].lTarget = 0;
                pProfileSave[index].step[i].tRise = 0;
                pProfileSave[index].step[i].tUp = 0;
                pProfileSave[index].step[i].tFall = 0;
                pProfileSave[index].step[i].tDown = 0;
                pProfileSave[index].step[i].tLow = 0;
                pProfileSave[index].step[i].tHigh = 0;
                pProfileSave[index].step[i].sPeriod = 0;
                pProfileSave[index].step[i].sMean = 0;
                pProfileSave[index].step[i].sAmp = 0;
                pProfileSave[index].step[i].logRate = 0;
                pProfileSave[index].step[i].logMin = 0;
                pProfileSave[index].step[i].logMax = 0;
                pProfileSave[index].step[i].repeatUnit = 0;
                pProfileSave[index].step[i].repeatDuration = 0;
            }
            for (int i=0; i<MaxStep; i++)
                pProfileSave[index].stepDuration[i] = 0;
        }
        else if (pType == 'v')
        {
            vProfileSave[index].active = 0;
            vProfileSave[index].totalStep = 0;
            vProfileSave[index].totalTestCycle = 0;
            vProfileSave[index].startValue = 0;
            for (int i=0; i<MaxStep; i++)
            {
                vProfileSave[index].step[i].stepUnit = 0;
                vProfileSave[index].step[i].stepType = 0;
                vProfileSave[index].step[i].lDuration = 0;
                vProfileSave[index].step[i].lTarget = 0;
                vProfileSave[index].step[i].tRise = 0;
                vProfileSave[index].step[i].tUp = 0;
                vProfileSave[index].step[i].tFall = 0;
                vProfileSave[index].step[i].tDown = 0;
                vProfileSave[index].step[i].tLow = 0;
                vProfileSave[index].step[i].tHigh = 0;
                vProfileSave[index].step[i].sPeriod = 0;
                vProfileSave[index].step[i].sMean = 0;
                vProfileSave[index].step[i].sAmp = 0;
                vProfileSave[index].step[i].logRate = 0;
                vProfileSave[index].step[i].logMin = 0;
                vProfileSave[index].step[i].logMax = 0;
                vProfileSave[index].step[i].repeatUnit = 0;
                vProfileSave[index].step[i].repeatDuration = 0;
            }
            for (int i=0; i<MaxStep; i++)
                vProfileSave[index].stepDuration[i] = 0;
        }
    }
    else if (sType == 'e')
    {
        if (pType == 't')
        {
            tProfileEdit[index].active = 0;
            tProfileEdit[index].totalStep = 0;
            tProfileEdit[index].totalTestCycle = 0;
            tProfileEdit[index].startValue = 0;
            for (int i=0; i<MaxStep; i++)
            {
                tProfileEdit[index].step[i].stepUnit = 0;
                tProfileEdit[index].step[i].stepType = 0;
                tProfileEdit[index].step[i].lDuration = 0;
                tProfileEdit[index].step[i].lTarget = 0;
                tProfileEdit[index].step[i].tRise = 0;
                tProfileEdit[index].step[i].tUp = 0;
                tProfileEdit[index].step[i].tFall = 0;
                tProfileEdit[index].step[i].tDown = 0;
                tProfileEdit[index].step[i].tLow = 0;
                tProfileEdit[index].step[i].tHigh = 0;
                tProfileEdit[index].step[i].sPeriod = 0;
                tProfileEdit[index].step[i].sMean = 0;
                tProfileEdit[index].step[i].sAmp = 0;
                tProfileEdit[index].step[i].logRate = 0;
                tProfileEdit[index].step[i].logMin = 0;
                tProfileEdit[index].step[i].logMax = 0;
                tProfileEdit[index].step[i].repeatUnit = 0;
                tProfileEdit[index].step[i].repeatDuration = 0;
            }
            for (int i=0; i<MaxStep; i++)
                tProfileEdit[index].stepDuration[i] = 0;

        }
        else if (pType == 'p')
        {
            pProfileEdit[index].active = 0;
            pProfileEdit[index].totalStep = 0;
            pProfileEdit[index].totalTestCycle = 0;
            pProfileEdit[index].startValue = 0;
            for (int i=0; i<MaxStep; i++)
            {
                pProfileEdit[index].step[i].stepUnit = 0;
                pProfileEdit[index].step[i].stepType = 0;
                pProfileEdit[index].step[i].lDuration = 0;
                pProfileEdit[index].step[i].lTarget = 0;
                pProfileEdit[index].step[i].tRise = 0;
                pProfileEdit[index].step[i].tUp = 0;
                pProfileEdit[index].step[i].tFall = 0;
                pProfileEdit[index].step[i].tDown = 0;
                pProfileEdit[index].step[i].tLow = 0;
                pProfileEdit[index].step[i].tHigh = 0;
                pProfileEdit[index].step[i].sPeriod = 0;
                pProfileEdit[index].step[i].sMean = 0;
                pProfileEdit[index].step[i].sAmp = 0;
                pProfileEdit[index].step[i].logRate = 0;
                pProfileEdit[index].step[i].logMin = 0;
                pProfileEdit[index].step[i].logMax = 0;
                pProfileEdit[index].step[i].repeatUnit = 0;
                pProfileEdit[index].step[i].repeatDuration = 0;
            }
            for (int i=0; i<MaxStep; i++)
                pProfileEdit[index].stepDuration[i] = 0;
        }
        else if (pType == 'v')
        {
            vProfileEdit[index].active = 0;
            vProfileEdit[index].totalStep = 0;
            vProfileEdit[index].totalTestCycle = 0;
            vProfileEdit[index].startValue = 0;
            for (int i=0; i<MaxStep; i++)
            {
                vProfileEdit[index].step[i].stepUnit = 0;
                vProfileEdit[index].step[i].stepType = 0;
                vProfileEdit[index].step[i].lDuration = 0;
                vProfileEdit[index].step[i].lTarget = 0;
                vProfileEdit[index].step[i].tRise = 0;
                vProfileEdit[index].step[i].tUp = 0;
                vProfileEdit[index].step[i].tFall = 0;
                vProfileEdit[index].step[i].tDown = 0;
                vProfileEdit[index].step[i].tLow = 0;
                vProfileEdit[index].step[i].tHigh = 0;
                vProfileEdit[index].step[i].sPeriod = 0;
                vProfileEdit[index].step[i].sMean = 0;
                vProfileEdit[index].step[i].sAmp = 0;
                vProfileEdit[index].step[i].logRate = 0;
                vProfileEdit[index].step[i].logMin = 0;
                vProfileEdit[index].step[i].logMax = 0;
                vProfileEdit[index].step[i].repeatUnit = 0;
                vProfileEdit[index].step[i].repeatDuration = 0;
            }
            for (int i=0; i<MaxStep; i++)
                vProfileEdit[index].stepDuration[i] = 0;
        }
    }
}

void MainWindow::on_cbSelectMethodManual_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->dsbTTimeSetManual->setValue(0);
        //    ui->dsbPTimeSetManual->setValue(0);
        //    ui->dsbVTimeSetManual->setValue(0);
        ui->sbTStepSetManual->setValue(1);
        //    ui->sbPStepSetManual->setValue(1);
        //    ui->sbVStepSetManual->setValue(1);
        ui->sbTStepRepeatSetManual->setValue(0);
        //    ui->sbPStepRepeatSetManual->setValue(0);
        //    ui->sbVStepRepeatSetManual->setValue(0);
        ui->sbTCycleSetManual->setValue(0);
        //    ui->sbPCycleSetManual->setValue(0);
        //    ui->sbVCycleSetManual->setValue(0);

        ui->dsbTTimeSetManual->setEnabled(false);
        //    ui->dsbPTimeSetManual->setEnabled(false);
        //    ui->dsbVTimeSetManual->setEnabled(false);
        ui->sbTStepSetManual->setEnabled(false);
        //    ui->sbPStepSetManual->setEnabled(false);
        //    ui->sbVStepSetManual->setEnabled(false);
        ui->sbTStepRepeatSetManual->setEnabled(false);
        //    ui->sbPStepRepeatSetManual->setEnabled(false);
        //    ui->sbVStepRepeatSetManual->setEnabled(false);
        ui->sbTCycleSetManual->setEnabled(false);
        //    ui->sbPCycleSetManual->setEnabled(false);
        //    ui->sbVCycleSetManual->setEnabled(false);
    }
    else if (index == 1)
    {
        ui->dsbTTimeSetManual->setValue(0);
        //     ui->dsbPTimeSetManual->setValue(0);
        //     ui->dsbVTimeSetManual->setValue(0);
        ui->sbTStepSetManual->setValue(0);
        //     ui->sbPStepSetManual->setValue(0);
        //     ui->sbVStepSetManual->setValue(0);
        ui->sbTStepRepeatSetManual->setValue(1);
        //     ui->sbPStepRepeatSetManual->setValue(1);
        //     ui->sbVStepRepeatSetManual->setValue(1);
        ui->sbTCycleSetManual->setValue(0);
        //     ui->sbPCycleSetManual->setValue(0);
        //     ui->sbVCycleSetManual->setValue(0);
        ui->sbTStepSetManual->setEnabled(false);
        //     ui->sbPStepSetManual->setEnabled(false);
        //     ui->sbVStepSetManual->setEnabled(false);
        ui->sbTStepRepeatSetManual->setEnabled(false);
        //     ui->sbPStepRepeatSetManual->setEnabled(false);
        //     ui->sbVStepRepeatSetManual->setEnabled(false);

        quint8 profileNumber = ui->cbSelectProfileManual->currentIndex() - 1;

        if (tProfileLoad[profileNumber].active)
        {
            ui->dsbTTimeSetManual->setEnabled(true);
            ui->sbTCycleSetManual->setEnabled(true);
        }
        else
        {
            ui->dsbTTimeSetManual->setEnabled(false);
            ui->sbTCycleSetManual->setEnabled(false);
        }
        if (pProfileLoad[profileNumber].active)
        {
            //        ui->dsbPTimeSetManual->setEnabled(true);
            //        ui->sbPCycleSetManual->setEnabled(true);
        }
        else
        {
            //        ui->dsbPTimeSetManual->setEnabled(false);
            //        ui->sbPCycleSetManual->setEnabled(false);
        }
        /*    if (vProfileLoad[profileNumber].active)
        {
    //        ui->dsbVTimeSetManual->setEnabled(true);
    //        ui->sbVCycleSetManual->setEnabled(true);
        }
        else
        {
            ui->dsbVTimeSetManual->setEnabled(false);
            ui->sbVCycleSetManual->setEnabled(false);
        }
        */
        // there could be a code which checks the total profile seconds and limit the max value of
        // elapsed seconds spinbox
    }
    else if (index == 2)
    {
        ui->dsbTTimeSetManual->setValue(0);
        //  ui->dsbPTimeSetManual->setValue(0);
        //  ui->dsbVTimeSetManual->setValue(0);
        ui->sbTStepSetManual->setValue(0);
        //  ui->sbPStepSetManual->setValue(0);
        //  ui->sbVStepSetManual->setValue(0);
        ui->sbTStepRepeatSetManual->setValue(1);
        //  ui->sbPStepRepeatSetManual->setValue(1);
        //  ui->sbVStepRepeatSetManual->setValue(1);
        ui->sbTCycleSetManual->setValue(0);
        //  ui->sbPCycleSetManual->setValue(0);
        //  ui->sbVCycleSetManual->setValue(0);

        ui->dsbTTimeSetManual->setEnabled(false);
        //  ui->dsbPTimeSetManual->setEnabled(false);
        //  ui->dsbVTimeSetManual->setEnabled(false);

        quint8 profileNumber = ui->cbSelectProfileManual->currentIndex() - 1;
        if (tProfileLoad[profileNumber].active)
        {
            ui->sbTStepSetManual->setEnabled(true);
            ui->sbTStepRepeatSetManual->setEnabled(true);
            ui->sbTCycleSetManual->setEnabled(true);
        }
        else
        {
            ui->sbTStepSetManual->setEnabled(false);
            ui->sbTStepRepeatSetManual->setEnabled(false);
            ui->sbTCycleSetManual->setEnabled(false);
        }
        /*   if (pProfileLoad[profileNumber].active)
        {
            ui->sbPStepSetManual->setEnabled(true);
            ui->sbPStepRepeatSetManual->setEnabled(true);
            ui->sbPCycleSetManual->setEnabled(true);
        }
        else
        {
            ui->sbPStepSetManual->setEnabled(false);
            ui->sbPStepRepeatSetManual->setEnabled(false);
            ui->sbPCycleSetManual->setEnabled(false);
        }
        */
        /*
        if (vProfileLoad[profileNumber].active)
        {
            ui->sbVStepSetManual->setEnabled(true);
            ui->sbVStepRepeatSetManual->setEnabled(true);
            ui->sbVCycleSetManual->setEnabled(true);
        }
        else
        {
            ui->sbVStepSetManual->setEnabled(false);
            ui->sbVStepRepeatSetManual->setEnabled(false);
            ui->sbVCycleSetManual->setEnabled(false);
        }
        */
    }
}

void MainWindow::on_cbSelectProfileManual_currentIndexChanged(int index)
{
    ui->bStartTestManual->setEnabled(false);
    ui->cbSelectProfileMain->setCurrentIndex(0);
    ui->sbTTotalCycleManual->setValue(1);
    //  ui->sbPTotalCycleManual->setValue(1);
    //  ui->sbVTotalCycleManual->setValue(1);
    // ui->laTTotalCycleMain->setText(QString::number(1));


    if (index == 0)
    {
        ui->laSelectedProfileMain->setText("Recete secilmedi");
        ui->laSelectedProfileManual->setText("Recete secilmedi");
        ui->laTestName->setText("Recete secilmedi");
        ui->bSendProfileManual->setEnabled(false);
        ui->cbSelectMethodManual->setCurrentIndex(0);
        ui->cbSelectMethodManual->setEnabled(false);
    //    ui->laTTotalCycleMain->setText(QString::number(1));

    }
    else
    {
        ui->cbSelectMethodManual->setCurrentIndex(0);
        ui->cbSelectMethodManual->setEnabled(true);

        quint8 index = ui->cbSelectProfileManual->currentIndex();
        if (readProfiles('l', index))
        {
            if (tProfileLoad[index-1].active == 0)
            {
                ui->sbTTotalCycleManual->setValue(0);
                ui->sbTTotalCycleManual->setEnabled(false);
            }
            else
            {
                ui->sbTTotalCycleManual->setEnabled(true);
            }
            /*
            if (pProfileLoad[index-1].active == 0)
            {
                ui->sbPTotalCycleManual->setValue(0);
                ui->sbPTotalCycleManual->setEnabled(false);
            }
            else
            {
                ui->sbPTotalCycleManual->setEnabled(true);
            }
*/
            /*
            if (vProfileLoad[index-1].active == 0)
            {
                ui->sbVTotalCycleManual->setValue(0);
                ui->sbVTotalCycleManual->setEnabled(false);
                ui->chbEllipticalVibrationSetManual->setChecked(false);
                ui->chbEllipticalVibrationSetManual->setEnabled(false);
            }
            else
            {
                ui->sbVTotalCycleManual->setEnabled(true);
                ui->chbEllipticalVibrationSetManual->setChecked(false);
                ui->chbEllipticalVibrationSetManual->setEnabled(true);
            }
            */
        }

        ui->bSendProfileManual->setEnabled(true);
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 0)
    {

    }
    else if (index == 1)
    {


    }
    else if (index == 2)
    {
        ui->cbSelectGraph->setCurrentIndex(0);
        ui->tTestGraph->setVisible(false);
        ui->pTestGraph->setVisible(false);
        //ui->vTestGraph->setVisible(false);
        ui->tTestGraph->setVisible(true);
        qApp->processEvents();
    }
    else if (index == 3)
    {

    }
    else if (index == 4)
    {

    }
    else if (index == 5)
    {

    }
}

void MainWindow::on_bSendProfileMain_clicked()
{
    quint8 index = ui->cbSelectProfileMain->currentIndex();
    if ((tProfileLoad[index-1].active == 0) && (pProfileLoad[index-1].active == 0) && (vProfileLoad[index-1].active == 0))
    {
        QMessageBox::information(
                    this,
                    tr(appName),
                    tr("This profile does not contain any step in temperature, pressure or vibration.") );
    }
    else
    {

        if (sendProfileOverSerial("main",index))
        {
            QString filePathLast ="lastTestName.txt";
            QFile file2(filePathLast);
            if (file2.open(QFile::WriteOnly|QFile::Append))
            {
                 QTextStream stream2(&file2);
                 stream2<<currentProfile<< "\n"<<tKey;
                 file2.close();
            }
        }
    }
}

void MainWindow::on_bStartTestManual_clicked()
{
    proc->stop();

    if (myPLC.deviceState == char(0x03))
    {

    }
    else
    {
        tKey = 0;
        pKey = 0;
        vKey = 0;

        //    ui->tTestGraph->graph(0)->data().clear();
        //    ui->pTestGraph->graph(0)->data().clear();
        //    ui->pTestGraph->graph(1)->data().clear();
        //    ui->pTestGraph->graph(2)->data().clear();
        //    ui->pTestGraph->graph(3)->data().clear();
        //    ui->pTestGraph->graph(4)->data().clear();
        //    ui->pTestGraph->graph(5)->data().clear();
        //    ui->vTestGraph->graph(0)->data().clear();
        //    ui->tTestGraph->replot();
        //    ui->pTestGraph->replot();
        //    ui->vTestGraph->replot();

        ui->tTestGraph->clearPlottables();
        ui->pTestGraph->clearPlottables();
        //ui->vTestGraph->clearPlottables();

        setupTGraph();
        setupPGraphs();
        setupVGraph();
    }

    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(0x02);
    proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
}

void MainWindow::on_bStopTestManual_clicked()
{
    timerTemp->stop();
    //    timerVib->stop();
    timerPressure->stop();

    proc->stop();

    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(0x01);
    proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
}

void MainWindow::on_bSendProfileManual_clicked()
{
    quint8 index = ui->cbSelectProfileManual->currentIndex();
    if ((tProfileLoad[index-1].active == 0) && (pProfileLoad[index-1].active == 0) && (vProfileLoad[index-1].active == 0))
    {
        QMessageBox::information(
                    this,
                    tr(appName),
                    tr("This profile does not contain any step in temperature, pressure or vibration.") );
    }
    else
    {
        sendProfileOverSerial("manual",index);
    }
}

void MainWindow::on_bPauseTestManual_clicked()
{
    timerTemp->stop();
    //   timerVib->stop();
    timerPressure->stop();

    proc->stop();

    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(0x03);
    proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
}

void MainWindow::on_bPauseTest_clicked()
{
    timerTemp->stop();
    //   timerVib->stop();
    timerPressure->stop();

    proc->stop();

    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(0x03);
    proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
}

void MainWindow::mousePress()
{
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if (ui->tTestGraph->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->tTestGraph->axisRect()->setRangeDrag(ui->tTestGraph->xAxis->orientation());
    else if (ui->tTestGraph->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->tTestGraph->axisRect()->setRangeDrag(ui->tTestGraph->yAxis->orientation());
    else
        ui->tTestGraph->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::mouseWheel()
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (ui->tTestGraph->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
        ui->tTestGraph->axisRect()->setRangeZoom(ui->tTestGraph->xAxis->orientation());

    }
    else if (ui->tTestGraph->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
        ui->tTestGraph->axisRect()->setRangeZoom(ui->tTestGraph->yAxis->orientation());

    }
    else {
        ui->tTestGraph->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);

    }
    if (ui->tTestGraph->yAxis2->range().upper > 10 || ui->pTestGraph->yAxis2->range().upper < 0 )
    {

         ui->tTestGraph->yAxis2->setRange(0,10);
         ui->tTestGraph->yAxis2->range().bounded(0,10);
         ui->tTestGraph->replot();
    }

}

void MainWindow::setupComboBoxes()
{
    for (int i = 0; i <= 20; i++)
    {
       if( readProfiles('g', i))
       {

       }
    }
}

void MainWindow::on_ZoomInHor_clicked()
{

    ui->tTestGraph->xAxis->scaleRange(.85, ui->tTestGraph->xAxis->range().center());
    ui->tTestGraph->replot();


}

void MainWindow::on_ZoomOutHor_clicked()
{
    ui->tTestGraph->xAxis->scaleRange(1.15, ui->tTestGraph->xAxis->range().center());
    ui->tTestGraph->replot();
}

void MainWindow::on_ZoomInVer_clicked()
{
    ui->tTestGraph->yAxis->scaleRange(.85, ui->tTestGraph->yAxis->range().center());
    ui->tTestGraph->yAxis->range().bounded(-50,220);


    ui->pTestGraph->yAxis->scaleRange(.85, ui->pTestGraph->yAxis->range().center());
    ui->pTestGraph->yAxis->setRangeLower(0);
    ui->pTestGraph->replot();

    ui->tTestGraph->yAxis2->scaleRange(.85, ui->tTestGraph->yAxis2->range().center());
    ui->tTestGraph->yAxis2->setRangeLower(0);
    ui->tTestGraph->replot();
}

void MainWindow::on_ZoomOutVer_clicked()
{
    ui->tTestGraph->yAxis->scaleRange(1.15, ui->tTestGraph->yAxis->range().center());
    ui->tTestGraph->replot();
    if (ui->tTestGraph->yAxis->range().upper > 220 || ui->tTestGraph->yAxis->range().upper < -50 )
    {
         ui->tTestGraph->yAxis->setRange(-50,220);
         ui->tTestGraph->replot();
    }

    ui->pTestGraph->yAxis->scaleRange(1.15, ui->pTestGraph->yAxis->range().center());
    ui->pTestGraph->yAxis->setRangeLower(0);
    ui->pTestGraph->replot();
    if (ui->pTestGraph->yAxis->range().upper > 10 || ui->pTestGraph->yAxis->range().upper < 0 )
    {
         ui->pTestGraph->yAxis->setRange(0,10);

         ui->pTestGraph->replot();
    }

    ui->tTestGraph->yAxis2->scaleRange(1.15, ui->tTestGraph->yAxis2->range().center());
    ui->tTestGraph->yAxis2->setRangeLower(0);
    if (ui->tTestGraph->yAxis2->range().upper > 10 || ui->tTestGraph->yAxis2->range().upper < 0 )
    {
         ui->tTestGraph->yAxis2->setRange(0,10);

         ui->tTestGraph->replot();
    }
}

bool MainWindow::on_bTemperatureSet_clicked()
{
    proc->stop();

    QByteArray cantTouchThis;           // bu satırda artık mıcıtmıyorum
    float tStart;
    float tTotalStep = 0;
    quint16 tTotalCycle = 0;
    int index = 51;
    tStart = (ui->leFixTempValue->text().toFloat() * 10);
    cantTouchThis.append(tProfileLoad[index-1].active);
    cantTouchThis.append(qint16(tStart) & 0x00FF);
    cantTouchThis.append(qint16(tStart) >> 8);
    cantTouchThis.append(quint16(tTotalStep) & 0x00FF);
    cantTouchThis.append(quint16(tTotalStep) >> 8);
    cantTouchThis.append(quint16(tTotalCycle) & 0x00FF);
    cantTouchThis.append(quint16(tTotalCycle) >> 8);
    cantTouchThis.append(index);

    proc->insertProfileMessage(mySerial::makeMessage(0x64,cantTouchThis));

    cantTouchThis.clear();

    proc->setProfile();

    return true;

}

void MainWindow::on_bSetTemperatureStart_clicked()
{
        proc->stop();

        if (myPLC.deviceState == char(0x03))
        {

        }
        else
        {
            tKey = 0;
            pKey = 0;
            vKey = 0;

            //    ui->tTestGraph->graph(0)->data().clear();
            //    ui->pTestGraph->graph(0)->data().clear();
            //    ui->pTestGraph->graph(1)->data().clear();
            //    ui->pTestGraph->graph(2)->data().clear();
            //    ui->pTestGraph->graph(3)->data().clear();
            //    ui->pTestGraph->graph(4)->data().clear();
            //    ui->pTestGraph->graph(5)->data().clear();
            //    ui->vTestGraph->graph(0)->data().clear();
            //    ui->tTestGraph->replot();
            //    ui->pTestGraph->replot();
            //    ui->vTestGraph->replot();

            ui->tTestGraph->clearPlottables();
            ui->pTestGraph->clearPlottables();
            //ui->vTestGraph->clearPlottables();

            setupTGraph();
            setupPGraphs();
            setupVGraph();
        }

        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x05);
        proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
}

void MainWindow::on_bSetTemperatureStop_clicked()
{
    timerTemp->stop();
    //    timerVib->stop();
    timerPressure->stop();

    proc->stop();

    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(0x01);
    proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
}

void MainWindow::on_cbTSelectSUnit_currentIndexChanged(int index)
{
    if (index =! 0)
    {
        ui->bNewTStep->setEnabled(true);
    }
    else
    {
        ui->bNewTStep->setEnabled(false);
    }
}

void MainWindow::on_ZoomCenter_clicked()
{
    ui->pTestGraph->xAxis->setRange(pKey, 60, Qt::AlignRight);
    ui->pTestGraph->yAxis->setRange(0,10);
    ui->pTestGraph->yAxis->range().bounded(0,10);
    ui->pTestGraph->replot();

    ui->tTestGraph->graph(0)->rescaleKeyAxis();
    ui->tTestGraph->yAxis->setRange(0,150);
    ui->tTestGraph->yAxis->range().bounded(0,150);
    ui->tTestGraph->yAxis2->setRange(0,6);
    ui->tTestGraph->yAxis2->range().bounded(0,6);
    ui->tTestGraph->replot();
}

void MainWindow::on_bSetExhaustValve_clicked()
{
    int tExhaustValveSetTemp;
    QByteArray cantTouchThis;

    cantTouchThis.clear();
    tExhaustValveSetTemp = ui->dsbExhaustValveAutoValue->value() * 10;

    if (ui->chbExhaustValve->isChecked())
    {
        cantTouchThis.append(char(0x01));
    }
    else
    {
        cantTouchThis.append(char(0x00));
    }
     cantTouchThis.append(qint16(tExhaustValveSetTemp) & 0x00FF);
     cantTouchThis.append(qint16(tExhaustValveSetTemp) >> 8);
     proc->insertCommandMessage(mySerial::makeMessage(0x6F,cantTouchThis));

     saveValueExhaustValve(tExhaustValveSetTemp);
}

void MainWindow::saveValueExhaustValve(int value)
{
    #ifdef Q_OS_LINUX
        //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"ExhaustValveSet.txt";
    #endif

    #ifdef Q_OS_WIN
        // windows code goes here
        QString filePath = "Settings\\ExhaustValveSet.txt";
    #endif

        QFile file(filePath);

        if (file.open(QFile::WriteOnly|QFile::Truncate))
        {
            QTextStream stream(&file);

            stream <<  value ;
            file.close();
        }
}

void MainWindow::loadValueExhaustValve()
{
#ifdef Q_OS_LINUX
    //linux code goes here
    QString filePath = "/home/pi/InDetail/settings/" +"ExhaustValveSet.txt";
#endif

#ifdef Q_OS_WIN
    // windows code goes here
    QString filePath = "Settings\\ExhaustValveSet.txt";
#endif

    QFile file(filePath);

    if (file.open(QFile::ReadOnly))
    {
    QTextStream stream(&file);
         while (!stream.atEnd())
         {
          QString line = stream.readLine();
            ui->dsbExhaustValveAutoValue->setValue(line.toInt());
         }
    }
}

void MainWindow::on_bSaveCalibrationValues_clicked()
{
    saveValueTopTempSensorCalibration();
    saveValueBottomTempSensorCalibration();
    saveValuePressureSensor1Calibration();
}

void MainWindow::saveValueTopTempSensorCalibration()
{
    #ifdef Q_OS_LINUX
        //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationTopTemp.txt";
    #endif

    #ifdef Q_OS_WIN
        // windows code goes here
         QString filePath = "Settings\\calibrationTopTemp.txt";
    #endif

    QFile file(filePath);

    if (file.open(QFile::WriteOnly|QFile::Truncate))
    {
        QTextStream stream(&file);

        stream <<  ui->dsbCalCabinTopTempErr->value() << ","
                <<  ui->dsbCalCabinTopTempCoeff->value() ;
        file.close();
    }

}

void MainWindow::saveValueBottomTempSensorCalibration()
{
    #ifdef Q_OS_LINUX
        //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationBottomTemp.txt";
    #endif

    #ifdef Q_OS_WIN
        // windows code goes here
         QString filePath = "Settings\\calibrationBottomTemp.txt";
    #endif

    QFile file(filePath);

    if (file.open(QFile::WriteOnly|QFile::Truncate))
    {
        QTextStream stream(&file);

        stream <<  ui->dsbCalCabinBottomTempErr->value() << ","
                <<  ui->dsbCalCabinBottomTempCoeff->value() ;
        file.close();
    }
}

void MainWindow::loadValueTopTempSensorCalibration()
{
    #ifdef Q_OS_LINUX
    //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationTopTemp.txt";
    #endif

    #ifdef Q_OS_WIN
    // windows code goes here
        QString filePath = "Settings\\calibrationTopTemp.txt";
    #endif

    QStringList wordList;
//     QList<double> dList;
//     QByteArray dList;
    QFile file(filePath);
    QString line;
    if (file.open(QFile::ReadOnly))
    {
        while (!file.atEnd())
        {
   //      dList = file.readLine();
         line = file.readLine();

        }
       double v = line.split(",")[0].toDouble();
       double y = line.split(",")[1].toDouble();
        ui->dsbCalCabinTopTempErr->setValue(v);
        ui->dsbCalCabinTopTempCoeff->setValue(y);
        file.close();
   //     ui->dsbCalCabinTopTempErr->setValue(dList[1]);
    //    ui->dsbCalCabinTopTempCoeff->setValue(dList[2]);
    }
}

void MainWindow::loadValueBottomTempSensorCalibration()
{
    #ifdef Q_OS_LINUX
    //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationBottomTemp.txt";
    #endif

    #ifdef Q_OS_WIN
    // windows code goes here
        QString filePath = "Settings\\calibrationBottomTemp.txt";
    #endif

    QStringList wordList;
//     QList<double> dList;
//     QByteArray dList;
     QFile file(filePath);
    QString line;
    if (file.open(QFile::ReadOnly))
    {

        while (!file.atEnd())
        {
   //      dList = file.readLine();
         line = file.readLine();

        }
       double v = line.split(",")[0].toDouble();
       double y = line.split(",")[1].toDouble();
        ui->dsbCalCabinBottomTempErr->setValue(v);
        ui->dsbCalCabinBottomTempCoeff->setValue(y);
        file.close();
   //     ui->dsbCalCabinTopTempErr->setValue(dList[1]);
    //    ui->dsbCalCabinTopTempCoeff->setValue(dList[2]);
    }

 }

void MainWindow::on_bCabinDoor_clicked()
{
   if (myPLC.deviceState == 0 )
   {
    QMessageBox::information( this, "Cihazı Kapat", "Cihazı şimdi kapatabilirsiniz.");
   }
   else if(myPLC.deviceState == 1)
   {
     QMessageBox::information( this, "Cihazı Kapat", "kapatma prosedürü işletiliyor. lütfen bekleyiniz.");
   }
   else
   {
       timerTemp->stop();
       //    timerVib->stop();
       timerPressure->stop();

       proc->stop();

       QByteArray cantTouchThis;
       cantTouchThis.clear();
       cantTouchThis.append(0x01);
       proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
   }
}

void MainWindow::saveValuePressureSensor1Calibration()
{
    #ifdef Q_OS_LINUX
        //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationPressure1.txt";
    #endif

    #ifdef Q_OS_WIN
        // windows code goes here
         QString filePath = "Settings\\calibrationPressure1.txt";
    #endif

    QFile file(filePath);

    if (file.open(QFile::WriteOnly|QFile::Truncate))
    {
        QTextStream stream(&file);

        stream <<  ui->dsbCalPipePressure1Err->value() << ","
                <<  ui->dsbCalPipePressure1Coeff->value() ;
        file.close();
    }
}

void MainWindow::loadValuePressureSensor1Calibration()
{
    #ifdef Q_OS_LINUX
    //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationPressure1.txt";
    #endif

    #ifdef Q_OS_WIN
    // windows code goes here
        QString filePath = "Settings\\calibrationPressure1.txt";
    #endif

    //     QList<double> dList;
    //     QByteArray dList;
    QFile file(filePath);
    QString line;
    if (file.open(QFile::ReadOnly))
    {
        while (!file.atEnd())
        {
             line = file.readLine();
        }
       double v = line.split(",")[0].toDouble();
       double y = line.split(",")[1].toDouble();
        ui->dsbCalPipePressure1Err->setValue(v);
        ui->dsbCalPipePressure1Coeff->setValue(y);
        file.close();
    }
}

void MainWindow::on_bEditProLook_clicked()
{

    lookProfile lookProfile;
    lookProfile.readProfiles(profileIndexMain+1);
    lookProfile.setModal(true);
    lookProfile.exec();
}

void MainWindow::on_bClearLogTable_2_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::run_keyboard_lineEdit()
{
    QLineEdit *line = (QLineEdit *)sender();
    lineEditkeyboard->setLineEdit(line);

    lineEditkeyboard->show();
}

void MainWindow::on_bRecipeMode_clicked()
{
    ui->mainPage->setCurrentIndex(0);
    ui->rectRecipe->setVisible(true);
    ui->rectFix->setVisible(false);
    ui->rect1500h->setVisible(false);
}

void MainWindow::on_bFixMode_clicked()
{
    ui->mainPage->setCurrentIndex(1);
    ui->rectRecipe->setVisible(false);
    ui->rectFix->setVisible(true);
    ui->rect1500h->setVisible(false);
}

void MainWindow::on_b1500hMode_clicked()
{
    ui->mainPage->setCurrentIndex(2);
    ui->rectRecipe->setVisible(false);
    ui->rectFix->setVisible(false);
    ui->rect1500h->setVisible(true);
}

void MainWindow::on_btnBackDetails_clicked()
{
    ui->detailsPages->setCurrentIndex(0);
}

void MainWindow::on_btnDetailsInfo_clicked()
{
    ui->detailsPages->setCurrentIndex(0);
    ui->detailsBottomPages->setCurrentIndex(0);
}

void MainWindow::on_btnDetailsTanklevel_clicked()
{
    ui->detailsPages->setCurrentIndex(0);
    ui->detailsBottomPages->setCurrentIndex(1);
}

void MainWindow::on_btnDetailsHeater_clicked()
{
    ui->detailsPages->setCurrentIndex(0);
    ui->detailsBottomPages->setCurrentIndex(3);
}

void MainWindow::on_btnDetailsPressure_clicked()
{
    ui->detailsPages->setCurrentIndex(1);

}

void MainWindow::on_bStartTest1500h_clicked()
{
    proc->stop();

    if (myPLC.deviceState == char(0x03))
    {

    }
    else
    {
        tKey = 0;
        pKey = 0;
        vKey = 0;



        ui->tTestGraph->clearPlottables();
        ui->pTestGraph->clearPlottables();

        setupTGraph();
        setupPGraphs();
        setupVGraph();
    }

    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(0x06);
    proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));

}

bool MainWindow::on_bSendProfile1500h_clicked()
{
    char activePipes;

    if(on_checkBox_stateChanged()){
        activePipes |= 1 << 0;
    }
    else
    {
        activePipes &= ~1;
    }

    if(on_checkBox_4_clicked()){
        activePipes |= 1 << 1;
    }
    else
    {
        activePipes &= ~2;
    }
    if(on_checkBox_5_clicked()){
        activePipes |= 1 << 2;
    }
    else
    {
        activePipes &= ~4;
    }

    if(on_checkBox_2_clicked()){
        activePipes |= 1 << 3;
    }
    else
    {
        activePipes &= ~8;
    }

    if(on_checkBox_3_clicked()){
        activePipes |= 1 << 4;
    }
    else
    {
        activePipes &= ~16;
    }

    proc->stop();

    QByteArray cantTouchThis;
    float setTemp1500h = ui->leSetTemperature1500h->text().toFloat()*10;
    float setPressure1500h = ui->leSetPressure1500h->text().toFloat()*10;
    quint16 totalDuration = ui->leTotalTestDuration1500h->text().toFloat();
    quint16 changePeriod = ui->leLiquidChangePeriod1500h->text().toFloat();


    cantTouchThis.append(1);

    cantTouchThis.append(qint16(setPressure1500h) & 0x00FF);
    cantTouchThis.append(qint16(setPressure1500h) >> 8);
    cantTouchThis.append(quint16(setTemp1500h) & 0x00FF);
    cantTouchThis.append(quint16(setTemp1500h) >> 8);
    cantTouchThis.append(quint16(totalDuration) & 0x00FF);
    cantTouchThis.append(quint16(totalDuration) >> 8);
    cantTouchThis.append(quint16(changePeriod) & 0x00FF);
    cantTouchThis.append(quint16(changePeriod) >> 8);

    cantTouchThis.append(activePipes);

    proc->insertProfileMessage(mySerial::makeMessage(0x67,cantTouchThis));

    cantTouchThis.clear();

    proc->setProfile();

ui->pb_testProgress->setMaximum(totalDuration);
ui->pb_testProgress->setMinimum(0);
ui->pb_testProgress->setValue(0);


    return true;
}

bool MainWindow::on_checkBox_stateChanged()
{
    if (ui->checkBox->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

bool MainWindow::on_checkBox_4_clicked()
{
    if (ui->checkBox_4->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

bool MainWindow::on_checkBox_5_clicked()
{
    if (ui->checkBox_5->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

bool MainWindow::on_checkBox_2_clicked()
{
    if (ui->checkBox_2->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

bool MainWindow::on_checkBox_3_clicked()
{
    if (ui->checkBox_3->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

void MainWindow::on_bMinimize_2_clicked()
{
     this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_bMinimize_3_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_bStopTest1500h_clicked()
{
    timerTemp->stop();
    //  timerVib->stop();
    timerPressure->stop();

    proc->stop();

    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(0x01);
    proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
    startFromRecords = false;
}

void MainWindow::on_bPauseTest1500h_clicked()
{
    timerTemp->stop();
    //   timerVib->stop();
    timerPressure->stop();

    proc->stop();

    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(0x03);
    proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
}

void MainWindow::on_b_pipe_1_clicked()
{
    if (ui->b_pipe_1->isChecked())
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x01);
        proc->insertCommandMessage(mySerial::makeMessage(0x91,cantTouchThis));
    }
    else
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(char(0x00));
        proc->insertCommandMessage(mySerial::makeMessage(0x91,cantTouchThis));
    }
}

void MainWindow::on_b_pipe_2_clicked()
{
    if (ui->b_pipe_2->isChecked())
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x01);
        proc->insertCommandMessage(mySerial::makeMessage(0x91,cantTouchThis));
    }
    else
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(char(0x00));
        proc->insertCommandMessage(mySerial::makeMessage(0x91,cantTouchThis));
    }
}

void MainWindow::on_b_pipe_3_clicked()
{
    if (ui->b_pipe_3->isChecked())
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x01);
        proc->insertCommandMessage(mySerial::makeMessage(0x91,cantTouchThis));
    }
    else
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(char(0x00));
        proc->insertCommandMessage(mySerial::makeMessage(0x91,cantTouchThis));
    }
}

void MainWindow::on_b_pipe_4_clicked()
{
    if (ui->b_pipe_4->isChecked())
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x01);
        proc->insertCommandMessage(mySerial::makeMessage(0x91,cantTouchThis));
    }
    else
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(char(0x00));
        proc->insertCommandMessage(mySerial::makeMessage(0x91,cantTouchThis));
    }
}

void MainWindow::on_b_pipe_5_clicked()
{
    if (ui->b_pipe_5->isChecked())
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x01);
        proc->insertCommandMessage(mySerial::makeMessage(0x91,cantTouchThis));
    }
    else
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(char(0x00));
        proc->insertCommandMessage(mySerial::makeMessage(0x91,cantTouchThis));
    }
}

void MainWindow::on_b_basinc_tank_doldur_clicked()
{
    if (ui->b_basinc_tank_doldur->isChecked())
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x01);
        proc->insertCommandMessage(mySerial::makeMessage(0x9E,cantTouchThis));
    }
    else
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(char(0x00));
        proc->insertCommandMessage(mySerial::makeMessage(0x9E,cantTouchThis));
    }
}

void MainWindow::on_b_temiz_tank_bosalt_clicked()
{
    if (ui->b_temiz_tank_bosalt->isChecked())
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x01);
        proc->insertCommandMessage(mySerial::makeMessage(0x9D,cantTouchThis));
    }
    else
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(char(0x00));
        proc->insertCommandMessage(mySerial::makeMessage(0x9D,cantTouchThis));
    }

}

void MainWindow::on_b_tum_tanklari_bosalt_clicked()
{
    if (ui->b_tum_tanklari_bosalt->isChecked())
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x01);
        proc->insertCommandMessage(mySerial::makeMessage(0x9F,cantTouchThis));
    }
    else
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(char(0x00));
        proc->insertCommandMessage(mySerial::makeMessage(0x9F,cantTouchThis));
    }
}

void MainWindow::on_bResetFault_clicked()
{

    proc->stop();

    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(0x03);
    proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
    ui->tabWidget->setCurrentIndex(0);


    // yazı kısmının resetlenmesi
    ui->laFault11->setVisible(false);
    ui->laFault12->setVisible(false);
    ui->laFault21->setVisible(false);
    ui->laFault22->setVisible(false);
    ui->laFault23->setVisible(false);
    ui->laFault24->setVisible(false);
    ui->laFault25->setVisible(false);
    ui->laFault31->setVisible(false);
    ui->laFault32->setVisible(false);
    ui->laFault33->setVisible(false);
    ui->laFault34->setVisible(false);
    ui->laFault41->setVisible(false);
    ui->laFault42->setVisible(false);
    ui->laFault43->setVisible(false);
    ui->laFault44->setVisible(false);
    ui->laFault45->setVisible(false);
    ui->laFault46->setVisible(false);
    ui->laFault47->setVisible(false);
    ui->laFault48->setVisible(false);
    ui->laFault49->setVisible(false);
    ui->laFault4A->setVisible(false);




    // tank kısmını resetleme
     ui->gb_basincTankLevel->setStyleSheet(" ");
     ui->gb_CleanTankLevel->setStyleSheet(" ");
     ui->gb_KirliTankLevel->setStyleSheet(" ");
     // hortum kısmının resetlenmesı
     ui->Hortum1->setChecked(true);
     ui->Hortum2->setChecked(true);
     ui->Hortum3->setChecked(true);
     ui->Hortum4->setChecked(true);
     ui->Hortum5->setChecked(true);


}

void MainWindow::on_bChooseData_clicked()
{

    dataFilePath = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                "records\\",
                "CSV file (*.csv);;All files (*.*)"
                );
    if (!dataFilePath.isNull()){
          startFromRecords = true;
          setupTGraph();
          ui->tabWidget->setCurrentIndex(2);
    }
    else{
       startFromRecords = false;
    }

}

void MainWindow::on_btnDetailsPipes_clicked()
{
    ui->detailsPages->setCurrentIndex(0);
    ui->detailsBottomPages->setCurrentIndex(2);
}

void MainWindow::on_btnDetailsPressure_2_clicked()
{
    ui->detailsPages->setCurrentIndex(0);
    ui->detailsBottomPages->setCurrentIndex(4);
}


