#include "mainwindow.h"
#include "ui_mainwindow.h"
#include  "lookprofile.h"
#include "keyboard/numpad.h"
#include <QTextStream>
#include <QFile>

char appName[] = {'i', 'n', 'D', 'e', 't', 'a', 'i', 'l', ' ', 'V', '1', '5', '0','0'};
profileStruct nProfileSave[MaxProfile];
profileStruct tProfileSave[MaxProfile];
profileStruct pProfileSave[MaxProfile];
profileStruct vProfileSave[MaxProfile];

profileStruct nProfileEdit[MaxProfile];
profileStruct tProfileEdit[MaxProfile];
profileStruct pProfileEdit[MaxProfile];
profileStruct vProfileEdit[MaxProfile];

profileStruct nProfileLoad[MaxProfile];
profileStruct tProfileLoad[MaxProfile];
profileStruct pProfileLoad[MaxProfile];
profileStruct vProfileLoad[MaxProfile];


PLC myPLC;

quint8 currentProfile = 0;
quint8 currentTStep = 0;
quint8 currentNStep = 0;
quint8 currentPStep = 0;
quint8 currentVStep = 0;

quint16 profileId = 0;

double oldTValue = 0;
double oldNValue = 0;
double oldPValue = 0;
double oldVValue = 0;

int tempPeriod;
int vibPeriod;
int pressurePeriod;
int pipeInfo;

double nKey = 0;
double tKey = 0;
double tKeyElapsed = 0;
double nKeyElapsed = 0;

double pKey = 0;
double vKey = 0;



float pipePressure1In;
float pipe1Pressure;
float liquid_temp;
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

 char activeChannel;
 char activeChannel1;// channel durum belirten degişken

bool commStatus;
quint32 nElapsedSeconds;
quint32 tElapsedSeconds;
quint32 pElapsedSeconds;
quint32 vElapsedSeconds;
quint8  tStep;
quint8  nStep;
quint8  pStep;
quint8  vStep;
quint32 pStepRepeat;
quint32 vStepRepeat;
quint16 tCycle;
quint16 nCycle;
quint16 pCycle;
quint16 vCycle;

quint32 totalTestDuration;

quint8 askCounter = 1;

QString denemeTestFolder;
QString testFolder;
QString dataFilePath;
bool startFromRecords = false;



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

    }
    else
    {

        ui->tabWidget->setCurrentIndex(1);
        ui->detailsPages->setCurrentIndex(0);
        ui->detailsBottomPages->setCurrentIndex(4);
        ui->bResetFault->setVisible(true);
        ui->laFault47->setVisible(true);
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
    ui->tTestGraph->yAxis->setLabel("Basınç (Bar)");
        ui->tTestGraph->yAxis2->setLabelFont(QFont(QFont().family(), 12));
        ui->tTestGraph->yAxis2->setLabel("Kabin Sıcaklıgı (°C)");
    ui->tTestGraph->xAxis->setLabelColor(Qt::white);
    ui->tTestGraph->yAxis->setLabelColor(Qt::yellow);
        ui->tTestGraph->yAxis2->setLabelColor(Qt::red);
    ui->tTestGraph->xAxis->setTickLabelFont(QFont(QFont().family(), 14));
    ui->tTestGraph->yAxis->setTickLabelFont(QFont(QFont().family(), 14));
        ui->tTestGraph->yAxis2->setTickLabelFont(QFont(QFont().family(), 14));
        ui->tTestGraph->yAxis2->setTickLabelColor(Qt::white);
    ui->tTestGraph->yAxis->setRange(0, 200.0);
        ui->tTestGraph->yAxis2->setRange(0, 180.0);

    //Background Colour brush pens
    ui->tTestGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->tTestGraph->yAxis->setBasePen(QPen(Qt::yellow, 1));
        ui->tTestGraph->yAxis2->setBasePen(QPen(Qt::red, 1));
    ui->tTestGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->tTestGraph->yAxis->setTickPen(QPen(Qt::white, 1));
        ui->tTestGraph->yAxis2->setTickPen(QPen(Qt::white, 1));
    ui->tTestGraph->xAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->tTestGraph->yAxis->setSubTickPen(QPen(Qt::white, 1));
        ui->tTestGraph->yAxis2->setSubTickPen(QPen(Qt::white, 1));
    ui->tTestGraph->xAxis->setTickLabelColor(Qt::white);
    ui->tTestGraph->yAxis->setTickLabelColor(Qt::white);
        ui->tTestGraph->xAxis2->setTickLabelColor(Qt::white);
    ui->tTestGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->tTestGraph->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
        ui->tTestGraph->yAxis2->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->tTestGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->tTestGraph->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
        ui->tTestGraph->yAxis2->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->tTestGraph->xAxis->grid()->setSubGridVisible(true);
    ui->tTestGraph->yAxis->grid()->setSubGridVisible(true);
        ui->tTestGraph->yAxis2->grid()->setSubGridVisible(true);
    ui->tTestGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->tTestGraph->yAxis->grid()->setZeroLinePen(Qt::NoPen);
        ui->tTestGraph->yAxis2->grid()->setZeroLinePen(Qt::NoPen);
    ui->tTestGraph->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->tTestGraph->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
        ui->tTestGraph->yAxis2->setUpperEnding(QCPLineEnding::esSpikeArrow);
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
/*if (startFromRecords){
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

*/


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
    ui->pTestGraph->xAxis->setLabel("Zaman (hh:mm:ss)");
    ui->pTestGraph->yAxis->setLabel("Kanal basınçı (bar)");
    ui->pTestGraph->yAxis->setRange(0, 200.0);
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


    ui->nPreview->addGraph();
    QPen nPen;
    nPen.setWidth(3);
    nPen.setColor(Qt::red);
    nPen.setStyle(Qt::SolidLine);
    ui->nPreview->graph(0)->setPen(nPen);
    ui->nPreview->xAxis2->setVisible(true);
    ui->nPreview->xAxis2->setTickLabels(false);
    ui->nPreview->yAxis2->setVisible(true);
    ui->nPreview->yAxis2->setTickLabels(false);
    ui->nPreview->yAxis->setLabel("Cabin Temperature (°C)");
    ui->nPreview->setBackground(Qt::white);
    ui->nPreview->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->tPreview_2->addGraph();
    ui->tPreview_2->addGraph();

    QPen tPen_2;
    QPen nPen_2;
    tPen_2.setWidth(3);
    nPen_2.setWidth(3);
    tPen_2.setColor(Qt::red);
    nPen_2.setColor(Qt::black);
    tPen_2.setStyle(Qt::SolidLine);
    nPen_2.setStyle(Qt::SolidLine);
    ui->tPreview_2->graph(0)->setPen(tPen);
    ui->tPreview_2->graph(1)->setPen(nPen);
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

    ui->bEditTablo->setEnabled(false);
    //zaman
    connect(ui->leTime_1,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_2,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_3,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_4,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_5,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_6,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_7,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_8,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_9,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_10,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_11,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_12,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_13,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_14,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTime_15,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    //
   connect(ui->leTempInput_0,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_1,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_2,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_3,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_4,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_5,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_6,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_7,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_8,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_9,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_10,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_11,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_12,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_13,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_14,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leTempInput_15,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    //
   connect(ui->leHumidity_0,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_1,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_2,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_3,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_4,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_5,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_6,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_7,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_8,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_9,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_10,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_11,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_12,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_13,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_14,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   connect(ui->leHumidity_15,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
   //
    connect(ui->leSaveProfileName,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leSaveParcaAdi,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));


    connect(ui->leProfileName,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTTotalCycle,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leSetPressure1500h,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leSetTemperature1500h,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTotalTestDuration1500h,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leLiquidChangePeriod1500h,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leFixTempValue,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leFixHumidtyValue,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leFixLiquidTempValue,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leFixPressureVelocity,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    connect(ui->leCalCleanTankLevelErr ,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leCalCleanTankCoeff ,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    connect(ui->leCalDirtyTankErr  ,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leCalDirtyTankCoeff ,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    connect(ui->leCalExpansionTankErr ,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leCalExpansionTankCoeff ,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    connect(ui->leCalCabinTopTempErr,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leCalCabinTopTempCoeff,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    connect(ui->leCalCabinBottomTempErr,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leCalCabinBottomTempCoeff,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    connect(ui->leCalPipePressure1Err,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leCalPipePressure1Coeff,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    connect(ui->leLiquidChangeTemp1500h,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leLiquidCirculationTime1500h,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    connect(ui->lineEdit,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leFixLiquitTempValue,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    connect(ui->leCalLiquidTempErr,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leCalLiquidTempCoeff,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    connect(ui->leCalLiquidTankLevelErr,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leCalLiquidTankLevelCoeff,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    connect(ui->lePartName2_1,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lePartName2_2,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lePartName2_3,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lePartName2_4,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lePartName2_5,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    connect(ui->lePartName1_1,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lePartName1_2,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lePartName1_3,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lePartName1_4,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lePartName1_5,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

   //  connect(ui->leCalLiquidTankLevelCoeff,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));


    labelEnable(1);

    ui->tTestGraph->setVisible(true);
    ui->pTestGraph->setVisible(true);
    ui->rectRecipe->setVisible(true);
    ui->rectFix->setVisible(false);
    ui->rect1500h->setVisible(false);
    //fix mod
    ui->cbChooseProfileTypeNew->setVisible(false);
    ui->leFixHumidtyValue->setVisible(false);

    ui->label_151->setVisible(false);
    ui->leFixPressureVelocity_2->setVisible(false);
    ui->label_152->setVisible(false);//ana ssayfa patlatma recete

    ui->leStartValue_2->setEnabled(false);
    ui->leStartValue->setEnabled(false);     
    ui->bTemperatureSet->setEnabled(true);
    ui->leFixTempValue->setEnabled(true);
    ui->leFixHumidtyValue->setEnabled(true);

    ui->cbFixMode->setEnabled(false);




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

    ui->control_hortum_1->setVisible(false);
    ui->control_hortum_2->setVisible(false);
    ui->control_hortum_3->setVisible(false);
    ui->control_hortum_4->setVisible(false);
    ui->control_hortum_5->setVisible(false);

         ui->b1500hMode->setVisible(false);  //kullanılmayan 1500h butonu

         ui->btnDetailsPipes->setVisible(false);  //kullanılmayan hata sayfasında hortum kontrol sayfası

         ui->btnDetailsTanklevel->setVisible(false);  //kullanılmayan hata sayfasınd tank ılme sayfaası


         ui->stackedWidget_2->setVisible(false);  //kullanılmayan izleme sayfasında 1500 h sayfası

         ui->sW_0->setVisible(false);  //kullanılmayan 1500h durum ızleme sayfası

         //ui->label_14->setVisible(false);  //kullanılmayan bar labeli grafik sayfası

         ui->dsbPipe1Pressure->setVisible(false);  //kullanılmayan bar göstergesi grafik sayfası

         ui-> pb_testProgress_2->setVisible(false);  //kullanılmayan bar göstergesi test takip sayfası

         ui-> label_27->setVisible(false);  //barın acıklaması test takip

         ui-> label_72->setVisible(false);  //kaliprasyon sayfası tank texti
         ui-> label_75->setVisible(false);  //kaliprasyon sayfası tank texti
         ui-> label_76->setVisible(false);  //kaliprasyon sayfası tank texti

         ui-> cB_sogutma->setVisible(false);  //test takip sayfası  sogutma izleme
         ui-> cB_nem_alma->setVisible(false);  //test takip sayfası  nem_alma izleme

         ui-> dsbSetHumidty->setVisible(false);  //TEST TAKİP NEM
         ui-> dsbHumidty->setVisible(false);  //NEM  TEST TAKİP
         //patlatma için gizlendi
         ui->btnIsnHumidty_2->setVisible(false);  //kullanılmayan nem sayfası
         ui->btnIsnHumidty->setVisible(false);  //kullanılmayan sogutma sayfası
         ui->cbSelectGraph->setVisible(false);  //kullanılmayan sogutma sayfası
         ui->leTTotalCycle->setVisible(false);//kullannılmayan test tekrar
         //izleme sayfası
         ui->cB_nem->setVisible(false);
         ui->cB_Kondenser->setVisible(false);
         ui->cB_kompresor->setVisible(false);
          ui->cB_sogutma_yapiliyor->setVisible(false);
         ui->cB_nem_cihaz->setVisible(false);
         ui->cB_gaz_toplama->setVisible(false);
         ui->cB_nem_istnen_degerde->setVisible(false);
         ui->cB_nemlendirme->setVisible(false);
         ui->cB_nem_aliniyor->setVisible(false);
         //izleme



        //kaliprasyon sayfası tank degerlerı
         ui->dsbCalTankDirtyInput->setVisible(false);
         ui->leCalExpansionTankCoeff->setVisible(false);
         ui->leCalDirtyTankErr->setVisible(false);
         ui->leCalDirtyTankCoeff->setVisible(false);
         ui->leCalCleanTankLevelErr->setVisible(false);
         ui->leCalCleanTankCoeff->setVisible(false);
         ui->dsbCalTankCleanInput->setVisible(false);
         ui->dsbCalTankCleanOut->setVisible(false);
         ui->dsbCalTankDirtyInput->setVisible(false);
         ui->dsbCalTankDirtyOut->setVisible(false);
         ui->dsbCalTankPressureInput->setVisible(false);
         ui->dsbCalTankPressureOut->setVisible(false);
         ui->leCalExpansionTankErr->setVisible(false);

        //kaliprasyon sayfası tank degerlerı
          //  nem ve sogutma hatalarının ılk etapta false edılmesı
           ui->laFault51->setVisible(false);
           ui->laFault52->setVisible(false);
           ui->laFault53->setVisible(false);
           ui->laFault54->setVisible(false);
           ui->laFault61->setVisible(false);
           ui->laFault62->setVisible(false);
           ui->laFault63->setVisible(false);
           ui->laFault64->setVisible(false);
           ui->laFault65->setVisible(false);

        //  nem ve sogutma hatalarının ılk etapta false edılmesı

           ui->cbFixMode->setVisible(false);

    loadValueExhaustValve();
    loadValueTopTempSensorCalibration();
    loadValueBottomTempSensorCalibration();
    loadValuePressureSensor1Calibration();

    loadValueCleanTankLevelCalibration();
    loadValueDirtyTankLevelCalibration();
    loadValueExpansionTankLevelCalibration();

    loadValueLiquidTankLevelCalibration();

    loadValueLiquidTempCalibration();


   resetFaultVisuals();




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

    case 0x0F:

        updateInfo(4,data);

        break;

    case 0x32:

        double pipePressure1Coeff ;
        double pipePressure1Err;
        float inpLiquidTankLevel;
        float inpExpansionTankLevel;
        float inpCleanTankLevel;
        float inpDirtyTankLevel;
        float CalCleanTankCoeff;
        float CalCleanTankLevelErr;
        float CalDirtyTankCoeff ;
        float CalDirtyTankErr ;
        float CalExpansionTankCoeff ;
        float CalExpansionTankErr;
        float vCleanTankLevel;
        float vExpansionTankLevel;
        float vDirtyTankLevel;
        float CalLiquidTankLevelCoeff ;
        float CalLiquidTankLevelErr;
        float vLiquidTankLevel;        
        float inpLiquidTemp;
        float dsbCalLiquidTempOut;
        float CalLiquidTempCoeff;
        float CalLiquidTempErr;

        float liquid_level;




        float dsbCalLiquidTankLevelOut;


        pipePressure1In = qint16(((data[1] & 0xff) << 8) | (data[0] & 0xff)) ;
        inpLiquidTemp = qint16(((data[3] & 0xff) << 8) | (data[2] & 0xff));

        //inpExpansionTankLevel = qint16(((data[3] & 0xff) << 8) | (data[2] & 0xff));//dikkat patlatmada sıvı sıcaklıgı
        //inpCleanTankLevel = qint16(((data[5] & 0xff) << 8) | (data[4] & 0xff)); patlatmada seviye
        inpLiquidTankLevel = qint16(((data[5] & 0xff) << 8) | (data[4] & 0xff));
        inpDirtyTankLevel = qint16(((data[7] & 0xff) << 8) | (data[6] & 0xff));
        setPressure = qint16(((data[9] & 0xff) << 8) | (data[8] & 0xff));

        //ui->dsbLiquidTemp_2->setValue(inpExpansionTankLevel);

        liquid_level = ((ui->leCalLiquidTankLevelCoeff->text().toDouble() * inpLiquidTankLevel) + ui->leCalLiquidTankLevelErr->text().toDouble() );  //patlatma sıvı seviye
        ui->dsbCalLiquidTankLevelInput->setValue(inpLiquidTankLevel);
        ui->pbLiquidTankLevel->setValue(liquid_level);
        ui->pbLiquidTankLevel1->setValue(liquid_level);
        ui->dsbCalLiquidTankLevelOut->setValue(liquid_level);


        CalCleanTankCoeff = ui->leCalCleanTankCoeff->text().toDouble();
        CalCleanTankLevelErr = ui->leCalCleanTankLevelErr->text().toDouble();
        vCleanTankLevel = ((CalCleanTankCoeff * inpCleanTankLevel) + CalCleanTankLevelErr );
        ui->pbCleanTankLevel->setMaximum(32000);
        ui->pbCleanTankLevel->setMinimum(0);
        ui->pbCleanTankLevel->setValue(vCleanTankLevel);



        CalExpansionTankCoeff = ui->leCalExpansionTankCoeff->text().toDouble();
        CalExpansionTankErr = ui->leCalExpansionTankErr->text().toDouble();
        vExpansionTankLevel = ((CalExpansionTankCoeff * inpExpansionTankLevel) + CalExpansionTankErr );
        ui->pbExpansionTankLevel->setMaximum(15000);
        ui->pbExpansionTankLevel->setMinimum(0);
        ui->pbExpansionTankLevel->setValue(vExpansionTankLevel);


        CalLiquidTankLevelCoeff = ui->leCalLiquidTankLevelCoeff->text().toDouble();
        CalLiquidTankLevelErr = ui->leCalLiquidTankLevelErr->text().toDouble();
        //vLiquidTankLevel = ((CalLiquidTankLevelCoeff * inpLiquidTankLevel) + CalLiquidTankLevelErr );
        ui->pbLiquidTankLevel->setMaximum(32000);
        ui->pbLiquidTankLevel->setMinimum(0);
        ui->pbLiquidTankLevel->setValue(liquid_level);
        ui->pbLiquidTankLevel1->setMaximum(32000);
        ui->pbLiquidTankLevel1->setMinimum(0);
        ui->pbLiquidTankLevel1->setValue(liquid_level);


        CalDirtyTankCoeff = ui->leCalDirtyTankCoeff->text().toDouble();
        CalDirtyTankErr = ui->leCalDirtyTankErr->text().toDouble();
        vDirtyTankLevel = ((CalDirtyTankCoeff * inpDirtyTankLevel) + CalDirtyTankErr);


        ui->pbDirtyTankLevel->setMaximum(32000);
        ui->pbDirtyTankLevel->setMinimum(0);
        ui->pbDirtyTankLevel->setValue(vDirtyTankLevel);

        ui->dsbSetPressure_2->setValue(setPressure/10);
        //ui->dsbCabinSetTemp_2->setValue(setPressure/10);
        ui->dsbCabinSetpressure->setValue(setPressure/10);
        pipePressure1Coeff = ui->leCalPipePressure1Coeff->text().toDouble();
        pipePressure1Err = ui->leCalPipePressure1Err->text().toDouble();


        pipe1Pressure = ((pipePressure1Coeff * pipePressure1In) + pipePressure1Err);
        ui->dsbPipe1Pressure->setValue(pipe1Pressure);
        ui->dsbPipe1Pressure_2->setValue(pipe1Pressure);
        ui->dsbPipe1Pressure_4->setValue(pipe1Pressure);
        ui->dsbSetHumidtyCabinAvrHumidty->setValue(pipe1Pressure);
        ui->dsbPipe1PressureMaintenance->setValue(pipe1Pressure);
        ui->dsbCalPipePressure1Input->setValue(pipePressure1In);
        ui->dsbCalPipePressure1Out->setValue(pipe1Pressure);

        //ui->dsbCalLiquidTankLevelOut->setValue(vDirtyTankLevel);
        CalLiquidTempCoeff = ui->leCalLiquidTempCoeff->text().toDouble();
        CalLiquidTempErr = ui->leCalLiquidTempErr->text().toDouble();

        ui->dsbCalLiquidTempInput->setValue(inpLiquidTemp); // sıvı sıcaklıgı veri ataması
        liquid_temp = (( CalLiquidTempCoeff * inpLiquidTemp) + CalLiquidTempErr );  //sıvı sıcaklıgı kalibre
        ui->dsbLiquidTemp_2->setValue(liquid_temp); //sıvı sıcaklıgı test takip sayfası
        ui->dsbFixLiquidTempValue->setValue(liquid_temp);
        ui->dsbCalLiquidTempOut->setValue(liquid_temp);
        ui->dsbLiquidTemp->setValue(liquid_temp);  // sıvı sıcaklıgı grafik


        break;

    case 0x33:


        cabinTopTemperatureIn       = qint16(((data[1] & 0xff) << 8) | (data[0] & 0xff));
        cabinBottomTemperatureIn    = qint16(((data[3] & 0xff) << 8) | (data[2] & 0xff));
        cabinPIDTemperatureIn       = qint16(((data[5] & 0xff) << 8) | (data[4] & 0xff))/10;
        cabinSetTemperatureIn       = qint16(((data[7] & 0xff) << 8) | (data[6] & 0xff))/10;

        double  calCabinTopTempCoeff    = ui->leCalCabinTopTempCoeff->text().toDouble();
        double  calCabinTopTempErr      = ui->leCalCabinTopTempErr->text().toDouble();
        double  calCabinBottomTempCoeff = ui->leCalCabinBottomTempCoeff->text().toDouble();
        double  calCabinBottomTempErr   = ui->leCalCabinBottomTempErr->text().toDouble();

        cabinBottomTemperatureValue = (cabinBottomTemperatureIn * calCabinBottomTempCoeff) + calCabinBottomTempErr ;
        cabinTopTemperatureValue = (cabinTopTemperatureIn * calCabinTopTempCoeff) + calCabinTopTempErr;

        cabinAverageTemp = (cabinTopTemperatureValue + cabinBottomTemperatureValue)/2;


        ui->dsbCabinTopTemp->setValue(cabinAverageTemp);
        ui->dsbCabinTopTemp_2->setValue(cabinAverageTemp);
        ui->dsbCabinTopTempMaintenance->setValue(cabinAverageTemp);
        ui->dsbSetTempCabinAvrTemp->setValue(cabinAverageTemp);
        ui->dsbCabinSetTemp->setValue(cabinSetTemperatureIn);



        ui->dsbCabinPIDTemp_2->setValue(cabinSetTemperatureIn);
        ui->dsbCabinSetTemp->setValue(cabinSetTemperatureIn);

        ui->dsbCalCabinBottomInput->setValue(cabinBottomTemperatureIn);
        ui->dsbCalCabinTopTempInput->setValue(cabinTopTemperatureIn);

        ui->dsbCalCabinBottomTempOut->setValue(cabinBottomTemperatureValue);
        ui->dsbCalCabinTopTempOut->setValue(cabinTopTemperatureValue);

        break;

    }
}

void MainWindow::prepareTestTimers()
{
    tempPeriod = 300;
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

}

void MainWindow::updateInfo(quint8 index, QByteArray data)
{
    if (index == 3)
    {
        if (profileId != data[10])
        {
            profileId = quint16(data[10]);
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
                    ui->leTTotalCycle->setEnabled(true);
                    ui->bSetTemperatureStop->setEnabled(false);


                    ui->cbSelectProfileManual->setCurrentIndex(0);
                    ui->cbSelectProfileManual->setEnabled(true);

                    ui->bStartTestManual->setEnabled(true);
                    ui->bStopTestManual->setEnabled(false);
                    ui->bPauseTestManual->setEnabled(false);

                    ui->sbTTotalCycleManual->setEnabled(true);
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

                    ui->bManualPressureLinesStart->setEnabled(true);
                    ui->bManualEvacLines->setEnabled(true);
                    ui->bManualPrepareLines->setEnabled(true);

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
                    ui->bManualPressureLinesStart->setEnabled(true);
                    ui->bManualEvacLines->setEnabled(true);
                    ui->bManualPrepareLines->setEnabled(true);

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
                    ui->bManualPressureLinesStart->setEnabled(false);
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
                    ui->bPauseTest1500h->setEnabled(true);
                    ui->bStopTest1500h->setEnabled(true);
                    ui->bSetTemperatureStart->setEnabled(false);
                    ui->bSetTemperatureStop->setEnabled(true);
                    ui->bStartTest->setEnabled(false);
                    ui->bStopTest->setEnabled(true);
                    ui->bPauseTest->setEnabled(true);
                    ui->bResetFault->setVisible(false);

                    ui->bManualPressureLinesStart->setEnabled(false);
                    ui->bManualPressureLinesStop->setEnabled(false);
                    ui->bManualEvacLines->setEnabled(false);
                    ui->bManualPrepareLines->setEnabled(false);
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
        else if (data[0] == char(0x0D))
        {
            if (myPLC.deviceState == (data[0]))
            {

            }
            else
            {
                myPLC.deviceState = data[0];
                if (myPLC.deviceState)
                {
                    ui->bManualPressureLinesStart->setEnabled(true);
                    ui->bManualPrepareLines->setEnabled(true);
                    ui->bManualPressureLinesStop->setEnabled(true);
                    ui->bManualEvacLines->setEnabled(true);
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
        else if (data[0] == char(0x33))   // hata 51 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(6);
                    ui->bResetFault->setVisible(true);
                    ui->laFault51->setVisible(true);
                }
            }
        }    
        else if (data[0] == char(0x34))   // hata 52 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(6);
                    ui->bResetFault->setVisible(true);
                    ui->laFault52->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x35))   // hata 53 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(6);
                    ui->bResetFault->setVisible(true);
                    ui->laFault53->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x36))   // hata 54 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(6);
                    ui->bResetFault->setVisible(true);
                    ui->laFault54->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x3D))   // hata 61 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(7);
                    ui->bResetFault->setVisible(true);
                    ui->laFault61->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x3E))   // hata 62 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(7);
                    ui->bResetFault->setVisible(true);
                    ui->laFault62->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x3F))   // hata 63 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(7);
                    ui->bResetFault->setVisible(true);
                    ui->laFault63->setVisible(true);
                }
            }
        }


        else if (data[0] == char(0x40))   // hata 64 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(7);
                    ui->bResetFault->setVisible(true);
                    ui->laFault64->setVisible(true);
                }
            }
        }


        else if (data[0] == char(0x41))   // hata 65 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(7);
                    ui->bResetFault->setVisible(true);
                    ui->laFault65->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x47))   // hata 71 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(8);
                    ui->bResetFault->setVisible(true);
                    ui->laFault71->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x48))   // hata 72 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(8);
                    ui->bResetFault->setVisible(true);
                    ui->laFault72->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x49))   // hata 73 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(8);
                    ui->bResetFault->setVisible(true);
                    ui->laFault73->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x4A))   // hata 74 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(8);
                    ui->bResetFault->setVisible(true);
                    ui->laFault74->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x4B))   // hata 75 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(8);
                    ui->bResetFault->setVisible(true);
                    ui->laFault75->setVisible(true);
                }
            }
        }
        else if (data[0] == char(0x4C))   // hata 76 ekranda gosterılmesi
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
                    ui->detailsBottomPages->setCurrentIndex(8);
                    ui->bResetFault->setVisible(true);
                    ui->laFault76->setVisible(true);
                }
            }
        }
        if (myPLC.resistancesActive == (data[1] & 0b00000001))
        {

        }
        else
        {
            myPLC.resistancesActive = (data[1] & 0b00000001);

            if (myPLC.resistancesActive)
            {
               // ui->sW_0->setCurrentIndex(0);
                ui->cB_rezistans->setEnabled(true);


            }
            else
            {
                ui->cB_rezistans->setEnabled(false);

            }
        }

        if (myPLC.fansActive == (data[1] & 0b00000010) >> 1)
        {

        }
        else
        {
            myPLC.fansActive = (data[1] & 0b00000010) >> 1;

            if (myPLC.fansActive)
            {

                ui->cB_fan->setEnabled(true);

            }
            else
            {

                ui->cB_fan->setEnabled(false);


            }
        }

        if (myPLC.liquidChangeActive == (data[1] & 0b00000100) >> 2)
        {

        }
        else
        {
            myPLC.liquidChangeActive = (data[1] & 0b00000100) >> 2;

            if (myPLC.liquidChangeActive)
            {
                writeToLogTable("Sıvı degişimi yapılıyor.");
                ui->cB_tte_22->setEnabled(true);
            }
            else
            {
                ui->cB_tte_22->setEnabled(false);
                writeToLogTable("sıvı degişimi tamamlandı.");

            }
        }

        if (myPLC.liquidChangeCoolingActive == (data[1] & 0b00001000) >> 3)
        {

        }
        else
        {
            myPLC.liquidChangeCoolingActive = (data[1] & 0b00001000) >> 3;

            if (myPLC.liquidChangeCoolingActive)
            {
                ui->cB_tte_23->setCheckState(Qt::CheckState(true));
                writeToLogTable("sıcaklık düşürülüyor.");
            }
            else
            {
               ui->cB_tte_23->setCheckState(Qt::CheckState(true));
               writeToLogTable("sıcaklık düşürme tamamlandı.");
            }
        }

        // mainwindow.cpp de 1868. satırdan itibaren yapılan değişiklik
        if (myPLC.temperatureTestActive == (data[1] & 0b00010000) >> 4)
        {

        }
        else
        {
            myPLC.temperatureTestActive = (data[1] & 0b00010000) >> 4;

            if (myPLC.temperatureTestActive)
            {
                ui->cB_sicaklik->setEnabled(true);
                ui->cB_tte_33->setCheckState(Qt::CheckState(true));
                writeToLogTable("sıcaklık kontrol aktif.");
                myPLC.temperatureTestActive = true;
                  prepareTestTimers();
                 ui->cB_isit_aktif->setEnabled(true);

            }
            else
            {
                ui->cB_sicaklik->setEnabled(false);
                ui->cB_tte_33->setCheckState(Qt::CheckState(false));
                myPLC.temperatureTestActive = false;
                writeToLogTable("sıcaklık kontrol kapalı.");
                ui->cB_isit_aktif->setEnabled(false);
            }
        }

        if (myPLC.temperaturePrepActive == (data[1] & 0b00100000) >> 5)
        {

        }
        else
        {
            myPLC.temperaturePrepActive = (data[1] & 0b00100000) >> 5;
            if (myPLC.temperaturePrepActive)
            {
                ui->cB_test_hazirlik->setEnabled(true);
                writeToLogTable("sıcaklık ayarlanıyor.");
            }
            else
            {
                ui->cB_test_hazirlik->setEnabled(false);
                writeToLogTable("sıcaklık ayarlama yapıldı.");
            }
        }

        if (myPLC.temperatureFixing == (data[1] & 0b01000000) >> 6)
        {

        }
        else
        {
            myPLC.temperatureFixing = (data[1] & 0b01000000) >> 6;

            if (myPLC.temperatureFixing)
            {
                ui->cB_sicaklik->setEnabled(true);
            }
            else
            {
                ui->cB_sicaklik->setEnabled(false);
            }
        }

        if (myPLC.temperatureFixed == (data[1] & 0b10000000) >> 7)
        {

        }
        else
        {
            myPLC.temperatureFixed = (data[1] & 0b10000000) >> 7;

            if (myPLC.temperatureFixed)
            {
                ui->cB_sicalik_set_degerde->setEnabled(true);
            }
            else
            {
                ui->cB_sicalik_set_degerde->setEnabled(false);
            }
        }

        if (myPLC.pipe1Control == (data[2] & 0b00000001) )  //pipe1
        {

        }
        else
        {
            myPLC.pipe1Control = (data[2] & 0b00000001) ;

            if (myPLC.pipe1Control)
            {
                /*writeToLogTable("hortum 1 kontrol.");
                ui->control_hortum_1->setVisible(true);*/

                pipeInfo=1;

            }
            else
            {
               /*writeToLogTable("hortum1 kontrol edildi.");
               ui->control_hortum_1->setVisible(false);*/
                pipeInfo=0;
            }


        }

        if (myPLC.pipe2Control == (data[2] & 0b00000010) >> 1)
        {

        }
        else
        {
            myPLC.pipe2Control = (data[2] & 0b00000010) >> 1 ;

            if (myPLC.pipe2Control)
            {/*
                writeToLogTable("hortum 2 kontrol.");
                ui->control_hortum_2->setVisible(true);*/
                pipeInfo=2;
            }
            else
            {
/*
                    writeToLogTable("hortum 2 kontrol edildi.");
                    ui->control_hortum_2->setVisible(false);
*/  pipeInfo=0;

            }
        }

        if (myPLC.pipe3Control == (data[2] & 0b00000100) >> 2 )
        {

        }
        else
        {
            myPLC.pipe3Control = (data[2] & 0b00000100) >> 2 ;

            if (myPLC.pipe3Control)
            {/*
                writeToLogTable("hortum 3 kontrol.");
                ui->control_hortum_3->setVisible(true);*/ pipeInfo=3;
            }
            else
            {
/*
                    writeToLogTable("hortum 3 kontrol edildi.");
                    ui->control_hortum_3->setVisible(false);
*/ pipeInfo=0;
            }
        }

        if (myPLC.pipe4Control == (data[2] & 0b00001000) >> 3 )
        {

        }
        else
        {
            myPLC.pipe4Control = (data[2] & 0b00001000) >> 3 ;

            if (myPLC.pipe4Control)
            { /*
                writeToLogTable("hortum 4 kontrol.");
                ui->control_hortum_4->setVisible(true);*/ pipeInfo=4;
            }
            else
            {
                    /*ui->control_hortum_4->setVisible(false);
                    writeToLogTable("hortum 4 kontrol edildi.");*/ pipeInfo=0;


            }
        }

        if (myPLC.pipe5Control == (data[2] & 0b00010000) >> 4 )
        {

        }
        else
        {
            myPLC.pipe5Control = (data[2] & 0b00010000) >> 4 ;

            if (myPLC.pipe5Control)
            {  /*
                writeToLogTable("hortum 5 kontrol.");
                ui->control_hortum_5->setVisible(true);*/ pipeInfo=5;
            }
            else
            {
                /*writeToLogTable("hortum 5 kontrol edildi.");
                ui->control_hortum_5->setVisible(false);*/ pipeInfo=0;
            }
        }

        if (myPLC.pipePrepareActive == (data[2] & 0b00100000) >> 5 )
        {

        }
        else
        {
            myPLC.pipePrepareActive = (data[2] & 0b00100000) >> 5 ;

            if (myPLC.pipePrepareActive)
            {
                writeToLogTable("hortum hava alma kontrol.");
            }
            else
            {

                    writeToLogTable("hortum hava alma kontrol edildi.");

            }
        }

        if (myPLC.pipePressureStat == (data[2] & 0b01000000) >> 6 )
        {

        }
        else
        {
            myPLC.pipePressureStat = (data[2] & 0b01000000) >> 6 ;

            if (myPLC.pipePressureStat)
            {
                writeToLogTable("hortum kontrol");
                ui->tabWidget->setCurrentIndex(1);
                ui->detailsBottomPages->setCurrentIndex(2);               
                ui->laFault4A_2->setVisible(true);
                ui->bResetFault->setVisible(true);
            }
            else
            {

                    writeToLogTable("hortum kontrol edildi.");
                    ui->laFault4A_2->setVisible(false);
            }
        }

        if (myPLC.pipePressureStat == (data[2] & 0b10000000) >> 7 )
        {

        }
        else
        {
            myPLC.manualPressureLine = (data[2] & 0b10000000) >> 7 ;

            if (myPLC.manualPressureLine)
            {
                writeToLogTable("hortum manuel basınçlandırılıyor");
                ui->detailsBottomPages->setCurrentIndex(2);
                ui->bManualPressureLinesStop->setEnabled(true);

            }
            else
            {

                    writeToLogTable("hortum manuel basınçlandırma tamamlandı.");

            }
        }

        if (myPLC.expansion_tank_exhaust_to_dirty_tank_active == (data[3] & 0b00000001)  )
        {

        }
        else
        {
            myPLC.expansion_tank_exhaust_to_dirty_tank_active = (data[3] & 0b00000001) ;

            if (myPLC.expansion_tank_exhaust_to_dirty_tank_active)
            {
                writeToLogTable("temiz tanktan basınc tankına sıvı aktarılıyor .");
            }
            else
            {
                writeToLogTable("temiz tanktan basınc tankına sıvı aktarıldı .");

            }
        }

        if (myPLC.workPumpCleanToExpansion == (data[3] & 0b00000010) >> 1 )
        {

        }
        else
        {
            myPLC.workPumpCleanToExpansion = (data[3] & 0b00000010) >> 1 ;

            if (myPLC.workPumpCleanToExpansion)
            {
                writeToLogTable("temiz tanktan kirli tankına sıvı aktarılıyor.");
            }
            else
            {
                writeToLogTable("temiz tanktan kirli tankına sıvı aktarılıyor.");
            }
        }

        if (myPLC.clean_tank_exhaust_to_dirty_tank_active == (data[3] & 0b00000100) >> 2 )
        {

        }
        else
        {
            myPLC.clean_tank_exhaust_to_dirty_tank_active = (data[3] & 0b00000100) >> 2 ;

            if (myPLC.clean_tank_exhaust_to_dirty_tank_active)
            {
                writeToLogTable("hatlardaki sıvı boşaltılıyor");
            }
            else
            {
                writeToLogTable("hatlardaki sıvı boşaltıldı.");
            }
        }

        if (myPLC.circulationPumpActive == (data[3] & 0b00001000) >> 3 )
        {

        }
        else
        {
            myPLC.circulationPumpActive = (data[3] & 0b00001000) >> 3 ;

            if (myPLC.circulationPumpActive)
            {
                writeToLogTable("Sirkülasyon Pompası Çalışıyor");
            }
            else
            {
               writeToLogTable("Sirkülasyon Pompası durdu. ");
            }
        }

        if (myPLC.cleanTankLow == (data[3] & 0b00010000) >> 4 )
        {

        }
        else
        {
            myPLC.cleanTankLow = (data[3] & 0b00010000) >> 4 ;

            if (myPLC.cleanTankLow)
            {
                writeToLogTable("Temiz tank seviyesi çok düşük.");
            }
            else
            {

                    writeToLogTable("Temiz tank seviyesi normal");

            }
        }

        if (myPLC.expansionTankHigh == (data[3] & 0b00100000) >> 5 )
        {

        }
        else
        {
            myPLC.expansionTankHigh = (data[3] & 0b00100000) >> 5 ;

            if (myPLC.expansionTankHigh)
            {
                writeToLogTable("basınç tankı sıvı seviyesi yüksek");
            }
            else
            {
               writeToLogTable("basınç tankı sıvı seviyesi normale döndü.");
            }
        }

        if (myPLC.cabinDoorClosed == (data[3] & 0b01000000) >> 6 )
        {

        }
        else
        {
            myPLC.cabinDoorClosed = (data[3] & 0b01000000) >> 6 ;

            if (myPLC.cabinDoorClosed)
            {
                writeToLogTable("Kapak kapalı");
                ui->bMainDoorInfo->setEnabled(true);
                ui->bMainDoorInfo->setChecked(true);
            }
            else
            {
               writeToLogTable("Kapak açık");
               ui->bMainDoorInfo->setChecked(false);
               ui->bMainDoorInfo->setEnabled(false);
            }
        }

        if (myPLC.openDoorRequest == (data[3] & 0b10000000) >> 7 )
        {

        }
        else
        {
            myPLC.openDoorRequest = (data[3] & 0b10000000) >> 7 ;

            if (myPLC.openDoorRequest)
            {
                writeToLogTable("Kapak kilidi aktif");
                ui->bMainDoorInfo->setEnabled(true);
            }
            else
            {
               writeToLogTable("Kapak kilidi kullanım dışı");
               ui->bMainDoorInfo->setEnabled(false);
            }
        }


        if (myPLC.pipe1LeakageDetected == (data[4] & 0b00000001)  )
        {

        }
        else
        {
            myPLC.pipe1LeakageDetected = (data[4] & 0b00000001) ;

            if (myPLC.pipe1LeakageDetected)
            {
                writeToLogTable("1. hatta sızıntı tespit edildi.");
                ui->laFault21->setVisible(true);
                ui->Hortum1_2->setChecked(false);
            }
            else
            {
                ui->Hortum1_2->setChecked(true);


            }
        }

        if (myPLC.pipe2LeakageDetected == (data[4] & 0b00000010) >> 1 )
        {

        }
        else
        {
            myPLC.pipe2LeakageDetected = (data[4] & 0b00000010) >> 1 ;

            if (myPLC.pipe2LeakageDetected)
            {
                writeToLogTable("2. hatta sızıntı tespit edildi.");
                ui->laFault22->setVisible(true);
                ui->Hortum2_2->setChecked(false);
            }
            else
            {
                ui->Hortum2_2->setChecked(true);

            }
        }

        if (myPLC.pipe3LeakageDetected == (data[4] & 0b00000100) >> 2 )
        {

        }
        else
        {
            myPLC.pipe3LeakageDetected = (data[4] & 0b00000100) >> 2 ;

            if (myPLC.pipe3LeakageDetected)
            {
                writeToLogTable("3.hatta sızıntı tespit edildi.");
                ui->laFault23->setVisible(true);
                ui->Hortum3_2->setChecked(false);
            }
            else
            {
                ui->Hortum3_2->setChecked(true);
            }
        }

        if (myPLC.pipe4LeakageDetected== (data[4] & 0b00001000) >> 3 )
        {

        }
        else
        {
            myPLC.pipe4LeakageDetected = (data[4] & 0b00001000) >> 3 ;

            if (myPLC.pipe4LeakageDetected)
            {
                writeToLogTable("4.hatta sızıntı tespit edildi.");
                ui->laFault24->setVisible(true);
                ui->Hortum4_2->setChecked(false);
            }
            else
            {
                ui->Hortum4_2->setChecked(true);
            }
        }

        if (myPLC.pipe5LeakageDetected == (data[4] & 0b00010000) >> 4 )
        {

        }
        else
        {
            myPLC.pipe5LeakageDetected = (data[4] & 0b00010000) >> 4 ;

            if (myPLC.pipe5LeakageDetected)
            {
                writeToLogTable("5.hatta sızıntı tespit edildi.");
                ui->laFault25->setVisible(true);
                ui->Hortum5_2->setChecked(false);
            }
            else
            {
                ui->Hortum5_2->setChecked(true);
            }
        }

        //nemlendirme test
        myPLC.nemlendirmetest = (data[5] & 0b00000001) ;
        if (myPLC.nemlendirmetest)
        {
             ui-> cB_nem->setEnabled(true);
        }
        else
        {
             ui->cB_nem->setEnabled(false);
        }
        //nemlendirme test
        //nem alma test
        myPLC.nemlamatest = (data[5] & 0b00000010) >>1 ;
        if (myPLC.nemlamatest)
        {
             ui-> cB_nem_alma->setEnabled(true);
        }
        else
        {
             ui->cB_nem_alma->setEnabled(false);
        }
        //nem alma  test
        //condencer
        myPLC.condencer = (data[5] & 0b00000100) >>2 ;
        if (myPLC.condencer)
        {
            ui->cB_Kondenser->setEnabled(true);
        }
        else
        {
            ui->cB_Kondenser->setEnabled(false);
        }
        //condencer
        //nem cihaz
        myPLC.nemcihaz = (data[5] & 0b00001000) >>3 ;
        if (myPLC.nemcihaz)
        {
            ui->cB_nem_cihaz->setEnabled(true);


        }
        else
        {
            ui->cB_nem_cihaz->setEnabled(false);

        }
        //nem cihaz
        //nem alınıyor
        myPLC.nemaliniyor = (data[5] & 0b00010000) >>4 ;
        if (myPLC.nemaliniyor)
        {
        ui->cB_nem_aliniyor->setEnabled(true);


        }
        else
        {
        ui->cB_nem_aliniyor->setEnabled(false);

        }
        //nem calınıyor
        //nem istenen degerde
        myPLC.nemistenende = (data[5] & 0b00100000) >>5 ;
        if (myPLC.nemistenende)
        {
        ui->cB_nem_istnen_degerde->setEnabled(true);


        }
        else
        {
        ui->cB_nem_istnen_degerde->setEnabled(false);

        }
        //nem istenen degerde
        //nemlendirme yapılıyor
        myPLC.nemlendirmeyapiliyor = (data[5] & 0b01000000) >>6 ;
        if (myPLC.nemlendirmeyapiliyor)
        {
          ui->cB_nemlendirme->setEnabled(true);
        }
        else
        {
          ui->cB_nemlendirme->setEnabled(false);
        }
        //nemlendirme yapılıyor

        //compresor aktive
        myPLC.compressorAktive = (data[6] & 0b00100000) >> 5 ;
        if (myPLC.compressorAktive)
        {
        ui->cB_kompresor->setEnabled(true);


        }
        else
        {
        ui->cB_kompresor->setEnabled(false);

        }
        //compresor aktive
        //gaz toplama  aktive
        myPLC.gazToplamaAktive = (data[6] & 0b00010000) >> 4 ;
        if (myPLC.gazToplamaAktive)
        {
             ui->cB_gaz_toplama->setEnabled(true);
        }
        else
        {
             ui->cB_gaz_toplama->setEnabled(false);
        }

        //gaz toplama  aktive
        //sogutm yapılıyor aktive
        myPLC.sogutmaYapiliyor = (data[6] & 0b00001000) >> 3 ;
        if (myPLC.sogutmaYapiliyor)
        {
           ui->cB_sogutma_yapiliyor->setEnabled(true);
        }
        else
        {
        ui->cB_sogutma_yapiliyor->setEnabled(false);

        }
        //sogutma yapılıyor  aktive
        //test hazırlık
        myPLC.TestHazirlik = (data[6] & 0b00000001)   ;
        if (myPLC.TestHazirlik)
        {
            ui->cB_test_hazirlik->setEnabled(true);
        }
        else
        {
          ui->cB_test_hazirlik->setEnabled(false);

        }
        //test hazırlık
        //test
        myPLC.Test = (data[6] & 0b00000010) >> 1    ;
        if (myPLC.Test)
        {

        ui->cB_test->setEnabled(true);


        }
        else
        {
            ui->cB_test->setEnabled(false);
        }
        //test
        //test  tamamlandı
        myPLC.TestTamamlandi = (data[6] & 0b00000100) >> 2    ;
        if (myPLC.TestTamamlandi)
        {
           ui->cB_test_tamamlandi->setEnabled(true);
        }
        else
        {
           ui->cB_test_tamamlandi->setEnabled(false);
        }
        //test  tamamlandı
        //sogutma test
        myPLC.sogutmatest = (data[6] & 0b01000000) >> 6    ;
        if (myPLC.sogutmatest)
        {
            ui->cB_sogutma->setEnabled(true);
        }
        else
        {
           ui->cB_sogutma->setEnabled(false);

        }
        //sogutma test
        //buzzer acık
        myPLC.buzzeracik = (data[8] & 0b00000001)     ;
        if (myPLC.buzzeracik)
        {
           ui->bResetFaultBuzzer->setEnabled(true);
        }
        else
        {
           ui->bResetFaultBuzzer->setEnabled(false);
        }
        //buzzer acık



       /*
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
*/



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

        ui->laCurrentTStepMain->setText(QString::number(tStep));


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

    }
    else if(index == 4){
        if (tCycle != quint16(((data[1] & 0xff) << 8) | (data[0] & 0xff)))
        {
            tCycle = quint16(((data[1] & 0xff) << 8) | (data[0] & 0xff));
          //           ui->laTTotalCycleMain->setText(QString::number(tCycle));
         /*   if (tCycle != 0)
            {
                tKey = 0;
                ui->tTestGraph->clearPlottables();
                setupTGraph();
            }*/
        }
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

void MainWindow::on_cbSelectGraph_currentIndexChanged(int index)                      // Grafik sayfası "Grafik secim acılan kutusu" secim yapıldı.
{

    if (index == 0)
    {
        ui->tTestGraph->setVisible(true);
        ui->pTestGraph->setVisible(true);
        ui->tTestGraph->setVisible(true);

    }
    else if (index == 1)
    {
        ui->pTestGraph->setVisible(true);
        ui->tTestGraph->setVisible(true);
        ui->pTestGraph->setVisible(true);

    }
}

void MainWindow::on_bEditPro_clicked()                                                // Yeni Recete sayfasında "Yeni Recete" butonuna basıldı.
{   connect(ui->leStartValue_2,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leStartValue,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leTLDuration,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->leNLDuration,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
     connect(ui->leNLTarget,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
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


    ui->cbTSelectSUnit->setEnabled(true);
     ui->cbNSelectSUnit->setEnabled(true);
    ui->leStartValue_2->setEnabled(true);
    ui->leStartValue->setEnabled(true);
    ui->leProfileName->setEnabled(true);
    ui->bSavePro->setEnabled(false);
}

void MainWindow::on_bClearPro_clicked()                                               // Yeni Recete sayfasında "Temizle" butonuna basıldı.
{    disconnect(ui->leStartValue_2,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    disconnect(ui->leStartValue,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    disconnect(ui->leTLDuration,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
      disconnect(ui->leNLDuration,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    disconnect(ui->leTLTarget,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    clearProfileSlot('s', 't', currentProfile);
    clearProfileSlot('s', 'p', currentProfile);
    clearProfileSlot('s', 'v', currentProfile);

    ui->cbSelectProfile->setCurrentIndex(0);
    ui->cbSelectProfile->setEnabled(true);

     ui->cbChooseProfileSlot->setCurrentIndex(0);
      ui->cbChooseProfileSlot->setEnabled(true);

     ui->cbChooseProfileType->setCurrentIndex(0);

    ui->leProfileName->setText("");
    ui->leProfileName->setEnabled(false);

    ui->leStartValue->setText("0");
    ui->leStartValue->setEnabled(false);

    ui->leStartValue_2->setText("0");
    ui->leStartValue_2->setEnabled(false);

    ui->laTTotalStep->setText("0");

    ui->tWidget->setCurrentIndex(0);

    ui->cbTSelectSUnit->setEnabled(false);

    ui->tPreview->clearPlottables();

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

void MainWindow::on_cbSelectProfile_currentIndexChanged(int index)                    // Yeni Recete sayfasından Recete Slotu secme.
{
    if (index == 0)
    {
        ui->bEditPro->setEnabled(false);
        ui->bEditProLook->setEnabled(false);
        ui->bSavePro->setEnabled(false);
        ui->bClearPro->setEnabled(false);
        ui->bNewTStep->setEnabled(false);

    }
    else
    {
        ui->bEditPro->setEnabled(true);
        ui->bEditProLook->setEnabled(true);
        currentProfile = index - 1;
        profileIndexMain = currentProfile;

    }
}

void MainWindow::on_cbChooseProfileType_currentIndexChanged(int index)                // Yeni Recete sayfasından sıcaklık veya nem recetesi olusturma secimi.
{
    if (index == 0){

       ui->tWidget->setCurrentIndex(0);
    }
    else if (index == 1){
        ui->tWidget->setCurrentIndex(3);
    }
}

void MainWindow::on_cbTSelectSUnit_currentIndexChanged(int index)                     // Yeni Recete sayfasından yeni sıcaklık adımını aktif etme
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

void MainWindow::on_cbNSelectSUnit_currentIndexChanged(int index)                     // Yeni Recete sayfasından yeni nem adımını aktif etme
{

        if (index =! 0)
        {

            ui->bNewNStep->setEnabled(true);
        }
        else
        {

             ui->bNewNStep->setEnabled(false);
        }

}

void MainWindow::on_bNewTStep_clicked()                                               // Yeni Recete sayfasında "Sıcaklık Admı" butonuna basıldı.
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

void MainWindow::on_bNewNStep_clicked()                                               // Yeni Recete sayfasında "Nem Adımı" butonuna basıldı.
{    if (tProfileSave[currentProfile].totalStep == 0)
    {
       ///Eger kullanıcı baslangic adimindaysa


       // oldNValue = ui->dsbNStartValue->value();    /// OldNValue eski degerin tutulması icin baslangic degeri olarak kaydedilir.
        oldNValue = ui->leStartValue_2->text().toDouble();
        ui->laOldNValue->setText(QString::number(oldNValue));   ///OldNValue ekranda gösterilir.
    }

    if (  ( ui->cbNSelectSUnit->currentIndex() == 0 ) )
    {

        //nothing selected for neither step type nor step time unit. Going forward should be banned.
    }
    else
    {
        ui->bNewNStep->setEnabled(true);
        nProfileSave[currentProfile].startValue = ui->leStartValue_2->text().toDouble();
        if ( ui->cbNSelectSUnit->currentIndex() == 1 )
        {
            //second selected for step time unit, do what you gotta do.

            nProfileSave[currentProfile].active = 1;
            nProfileSave[currentProfile].step[currentNStep].stepUnit = 1;
          //  tProfileSave[currentProfile].startValue = ui->dsbTStartValue->value();


            ui->laNLinDurationSave->setText("s.");
        }
        else if ( ui->cbNSelectSUnit->currentIndex() == 2 )
        {
            //minute selected for step time unit, do what you gotta do.

            nProfileSave[currentProfile].active = 1;
            nProfileSave[currentProfile].step[currentNStep].stepUnit = 2;
       //     tProfileSave[currentProfile].startValue = ui->dsbTStartValue->value();

            ui->laNLinDurationSave->setText("m.");
        }
        else if ( ui->cbNSelectSUnit->currentIndex() == 3 )
        {
            //hour selected for step time unit, do what you gotta do.

            nProfileSave[currentProfile].active = 1;
            nProfileSave[currentProfile].step[currentNStep].stepUnit = 3;
       //     tProfileSave[currentProfile].startValue = ui->dsbTStartValue->value();

            ui->laNLinDurationSave->setText("h.");
        }
        else if ( ui->cbNSelectSUnit->currentIndex() == 4 )
        {
            //day selected for step time unit, do what you gotta do.

            nProfileSave[currentProfile].active = 1;
            nProfileSave[currentProfile].step[currentNStep].stepUnit = 4;
       //     tProfileSave[currentProfile].startValue = ui->dsbTStartValue->value();

            ui->laNLinDurationSave->setText("d.");
        }
        nProfileSave[currentProfile].step[currentNStep].stepType = 1; /// Sıcaklık adımı tipi linner olarak belirtildi.
        ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() + 1); ///bir sonraki pencereye geçildi
    }

}

void MainWindow::on_bTForward2_clicked()                                             // Yeni Recete sayfasında "İleri" butonuna basıldı.
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

void MainWindow::on_bNForward2_clicked()                                             // Yeni Recete sayfasında "İleri" butonuna basıldı.
{
    float delta1;

   // tProfileSave[currentProfile].step[currentTStep].lDuration = ui->sbTLDuration->value();
   // tProfileSave[currentProfile].step[currentTStep].lTarget = ui->dsbTLTarget->value();

    nProfileSave[currentProfile].step[currentNStep].lDuration = ui->leNLDuration->text().toDouble();
    nProfileSave[currentProfile].step[currentNStep].lTarget = ui->leNLTarget->text().toDouble();

    float startValue = nProfileSave[currentProfile].startValue;
    float duration = nProfileSave[currentProfile].step[currentNStep].lDuration;
    float oldTarget = nProfileSave[currentProfile].step[currentNStep-1].lTarget;
    float target = nProfileSave[currentProfile].step[currentNStep].lTarget;

    if ( currentNStep == 0 )
    {
        delta1 = qFabs( target - startValue );
    }
    else
    {
        delta1 = qFabs( target - oldTarget );
    }

    if ( ui->cbNSelectSUnit->currentIndex() == 1 )
    {
        if ( ( delta1 * 60 / 5 ) <= ( duration * 1 ) )
        {
            ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() + 1);
            updateNPreview();
        }
    }
    else if ( ui->cbNSelectSUnit->currentIndex() == 2 )
    {
        if ( ( delta1 * 60 / 5 ) <= ( duration * 60 ) )
        {
            ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() + 1);
            updateNPreview();
        }
    }
    else if ( ui->cbNSelectSUnit->currentIndex() == 3 )
    {
        if ( ( delta1 * 60 / 5 ) <= ( duration * 60 * 60 ) )
        {
            ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() + 1);
            updateNPreview();
        }
    }
    else if ( ui->cbNSelectSUnit->currentIndex() == 4 )
    {
        if ( ( delta1 * 60 / 5 ) <= ( duration * 60 * 60 * 24 ) )
        {
            ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() + 1);
            updateNPreview();
        }
    }

}

void MainWindow::on_bTBack2_clicked()                                                // Yeni Recete sayfasında "Geri" butonuna basıldı.
{
    ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() - 1);
}

void MainWindow::on_bTBack2_2_clicked()                                              // Yeni Recete sayfasında "Geri" butonuna basıldı.
{
    ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() - 1);
}

void MainWindow::on_bTBack3_clicked()                                                // Yeni Recete sayfasında "Geri" butonuna basıldı.
{
    ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() - 1);
}

void MainWindow::on_bNBack3_clicked()                                                // Yeni Recete sayfasında "Geri" butonuna basıldı.
{
    ui->tWidget->setCurrentIndex(ui->tWidget->currentIndex() - 1);
}

void MainWindow::on_bTSaveStep_clicked()                                              // Yeni Recete sayfasında "Adım Onayla" butonuna basıldı. (Sıcaklık için)
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

void MainWindow::on_bNSaveStep_clicked()                                               // Yeni Recete sayfasında "Adım Onayla" butonuna basıldı. (Nem için)
{
    disconnect(ui->leStartValue,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));

    ui->leStartValue->setEnabled(false);

    nProfileSave[currentProfile].totalStep++;
    currentNStep++;
    ui->laNTotalStep->setText(QString::number(currentNStep));

    oldNValue = nProfileSave[currentProfile].step[currentNStep - 1].lTarget;
    ui->laOldNValue->setText(QString::number(oldNValue));

    ui->leNLDuration->setText("1");
    ui->leNLTarget->setText("0");

    //ui->cbTSelectSType->setCurrentIndex(0);
    ui->cbNSelectSUnit->setCurrentIndex(3);

    ui->tWidget->setCurrentIndex(3);
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

void MainWindow::updateNPreview()
{
    //ui->nPreview->clearPlottables();
    ui->nPreview->xAxis->setRange(0, ui->leNLTarget->text().toDouble());
    ui->nPreview->yAxis->setRange(0, 100);

    if (nProfileSave[currentProfile].step[currentNStep].stepUnit == 1)
    {
        ui->nPreview->xAxis->setLabel("Time (seconds)");
       ui->tPreview_2->xAxis->setLabel("Time (seconds)");
    }
    else if (nProfileSave[currentProfile].step[currentNStep].stepUnit == 2)
    {
        ui->nPreview->xAxis->setLabel("Time (minutes)");
        ui->tPreview_2->xAxis->setLabel("Time (minutes)");
    }
    else if (nProfileSave[currentProfile].step[currentNStep].stepUnit == 3)
    {
        ui->nPreview->xAxis->setLabel("Time (hours)");
        ui->tPreview_2->xAxis->setLabel("Time (hours)");
    }
    else if (nProfileSave[currentProfile].step[currentNStep].stepUnit == 4)
    {
        ui->nPreview->xAxis->setLabel("Time (days)");
        ui->tPreview_2->xAxis->setLabel("Time (days)");
    }


    float delta2 = 0;
    float duration = nProfileSave[currentProfile].step[currentNStep].lDuration;
    float startValue = nProfileSave[currentProfile].startValue;
    float oldTarget = nProfileSave[currentProfile].step[currentNStep-1].lTarget;
    float target = nProfileSave[currentProfile].step[currentNStep].lTarget;
    float oldDuration= nProfileSave[currentProfile].step[currentNStep-1].lDuration;

    QVector<double> nPreviewX( ( duration * 10 ) + 1 ), nPreviewY( ( duration * 10 ) + 1 );
    QVector<double> tPreviewX_2(21), tPreviewY_2( 21 );

    if (currentNStep == 0)
    {
       nPreviewY[0] = startValue;
        nPreviewX[0] = 0;
        delta2 = target - startValue;
        tPreviewY_2[1] = target;
        tPreviewX_2[1] = duration;
        tPreviewY_2[0] = startValue;
        tPreviewX_2[0] = 0.1;
    }
    else
    {
        nPreviewY[0] = oldTarget;
        nPreviewX[0] = 0;
        delta2 = target - oldTarget;
        if (currentNStep == 1)
        {
            tPreviewY_2[0] = startValue;
            tPreviewX_2[0] = 1;
            tPreviewY_2[1] = oldTarget;
            tPreviewX_2[1] = oldDuration;
            tPreviewY_2[2] = target;
            tPreviewX_2[2] = tPreviewX_2[1] + duration;
        }
        else if (currentNStep == 2)
        {
            tPreviewY_2[0] = startValue;
            tPreviewX_2[0] = 1;
            tPreviewY_2[1] = nProfileSave[currentProfile].step[currentNStep-2].lTarget;
            tPreviewX_2[1] = nProfileSave[currentProfile].step[currentNStep-2].lDuration;
            tPreviewY_2[2] = nProfileSave[currentProfile].step[currentNStep-1].lTarget;
            tPreviewX_2[2] = tPreviewX_2[1] + nProfileSave[currentProfile].step[currentNStep-1].lDuration;
            tPreviewY_2[3] = target;
            tPreviewX_2[3] = tPreviewX_2[2] + duration ;
        }
        else if (currentNStep == 3)
        {
            tPreviewY_2[0] = startValue;
            tPreviewX_2[0] = 1;
            tPreviewY_2[1] = nProfileSave[currentProfile].step[currentNStep-3].lTarget;
            tPreviewX_2[1] = nProfileSave[currentProfile].step[currentNStep-3].lDuration;
            tPreviewY_2[2] = nProfileSave[currentProfile].step[currentNStep-2].lTarget;
            tPreviewX_2[2] = tPreviewX_2[currentNStep - 2] + nProfileSave[currentProfile].step[currentNStep-2].lDuration;
            tPreviewY_2[3] = nProfileSave[currentProfile].step[currentNStep-1].lTarget;
            tPreviewX_2[3] = tPreviewX_2[currentNStep - 1] + nProfileSave[currentProfile].step[currentNStep-1].lDuration;
            tPreviewY_2[4] = target;
            tPreviewX_2[4] = tPreviewX_2[currentNStep] + duration ;
        }
        else if (currentNStep >= 4)
        {
            tPreviewY_2[0] = startValue;
            tPreviewX_2[0] = 1;
            for(int j=1; j < (currentNStep + 1 ); j++)
            {
                tPreviewY_2[j] = nProfileSave[currentProfile].step[j-1].lTarget;
                tPreviewX_2[j] = tPreviewX_2[j-1] + nProfileSave[currentProfile].step[j-1].lDuration;
            }
            tPreviewY_2[currentNStep + 1] = target;
            tPreviewX_2[currentNStep + 1] = tPreviewX_2[currentNStep] + duration ;
        }
    }

    for(int i=1; i < ( duration * 10) + 1; i++)
    {
        nPreviewY[i] = nPreviewY[i-1] + ( delta2 / ( duration * 10 ) );
        nPreviewX[i] = float(i) / 10;
    }

    ui->nPreview->graph(0)->setData(nPreviewX, nPreviewY);
    ui->nPreview->graph(0)->rescaleAxes();
    ui->nPreview->replot();

    connect(ui->tPreview_2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->tPreview_2->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->tPreview_2->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->tPreview_2->yAxis2, SLOT(setRange(QCPRange)));


    ui->tPreview_2->graph(0)->setData(tPreviewX_2, tPreviewY_2);
    ui->tPreview_2->graph(0)->rescaleAxes();
    ui->tPreview_2->replot();


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
                ui->cbChooseProfileSlot->addItem(QString::number(index) + " - bos slot");
                ui->cbChooseProfileSlot->setItemData(index, QColor( Qt::gray ), Qt::TextColorRole );

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
                    ui->cbChooseProfileSlot->addItem(QString::number(index) + " - " + "isimsiz");
                    ui->cbChooseProfileSlot->setItemData(index, QColor( Qt::black ), Qt::TextColorRole );
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
                    ui->cbChooseProfileSlot->addItem(QString::number(index) + " - " + name);
                    ui->cbChooseProfileSlot->setItemData(index, QColor( Qt::black ), Qt::TextColorRole );
                }

            }
        }
    }
    return true;
}

void MainWindow::on_bSavePro_clicked()                                          // Yeni Recete sayfasında "Receteyi Kaydet" butonuna basıldı.
{   disconnect(ui->leNLDuration,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    disconnect(ui->leTLDuration,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    disconnect(ui->leTLTarget,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    disconnect(ui->leNLTarget,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
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
    ui->leStartValue_2->setText("0");
    ui->leStartValue_2->setEnabled(false);
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

bool MainWindow::sendProfileOverSerial(QString mode, int index)
{
    proc->stop();

    QByteArray cantTouchThis;           // bu satırda artık mıcıtmıyorum
    float tStart = tProfileLoad[index].startValue*10.0;
    float nStart = nProfileLoad[index].startValue*10.0;
    float sTarget = tProfileLoad[index].sTarget*10.0; // sıvı sıcaklık set
    float tTotalStep = tProfileLoad[index].totalStep;
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

    cantTouchThis.append(tProfileLoad[index].active);
    cantTouchThis.append(nProfileLoad[index].active);
    cantTouchThis.append(qint16(tStart) & 0x00FF);
    cantTouchThis.append(qint16(tStart) >> 8);
    cantTouchThis.append(qint16(nStart) & 0x00FF);
    cantTouchThis.append(qint16(nStart) >> 8);
    cantTouchThis.append(quint16(tTotalStep) & 0x00FF);
    cantTouchThis.append(quint16(tTotalStep) >> 8);
    cantTouchThis.append(quint16(tTotalCycle) & 0x00FF);
    cantTouchThis.append(quint16(tTotalCycle) >> 8);
    cantTouchThis.append(quint16(sTarget) & 0x00FF);
    cantTouchThis.append(quint16(sTarget) >> 8);
    cantTouchThis.append(index);
/*    cantTouchThis.append(quint16(tWaterTank) & 0x00FF);
    cantTouchThis.append(quint16(tWaterTank) >> 8);
*/
    proc->insertProfileMessage(mySerial::makeMessage(0x64,cantTouchThis));

    for(int i=0; i<tProfileLoad[index].totalStep; i++)
    {
        cantTouchThis.clear();

        quint8 tStepUnit = tProfileLoad[index].step[i].stepUnit;
        quint8 tStepType = tProfileLoad[index].step[i].stepType;
        float tLDuration = tProfileLoad[index].step[i].lDuration;
        float tLTarget = tProfileLoad[index].step[i].lTarget*10.0;
        float nLTarget = nProfileLoad[index].step[i].lTarget*10.0;

        cantTouchThis.append(i+1);
        cantTouchThis.append(tStepUnit);
        cantTouchThis.append(tStepType);
        cantTouchThis.append(quint16(tLDuration) & 0x00FF);
        cantTouchThis.append(quint16(tLDuration) >> 8);
        cantTouchThis.append(qint16(tLTarget) & 0x00FF);
        cantTouchThis.append(qint16(tLTarget) >> 8);
        cantTouchThis.append(qint16(nLTarget) & 0x00FF);
        cantTouchThis.append(qint16(nLTarget) >> 8);

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
    ui->bFixMode ->setEnabled(false);
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
    ui->bFixMode ->setEnabled(true);
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

    ui->laTime_4->setText(QTime::currentTime().toString());
    ui->laDate_4->setText(QDate::currentDate().toString(Qt::SystemLocaleShortDate));

    ui->laTime_5->setText(QTime::currentTime().toString());
    ui->laDate_5->setText(QDate::currentDate().toString(Qt::SystemLocaleShortDate));
}

void MainWindow::updateTPlot()
{
    int tKeyHour;
    tKeyElapsed = tElapsedSeconds + (double(tempPeriod)/1000.0) ;
   // tKey = tElapsedSeconds + (double(tempPeriod)/1000.0) ;
    //tKey = QTime::currentTime().msecsSinceStartOfDay()/1000;
    tKey = tKey + (double(tempPeriod)/1000.0);
    // add data to lines:
    ui->tTestGraph->graph(0)->addData(tKey,pipe1Pressure);
    ui->tTestGraph->graph(1)->addData(tKey,cabinAverageTemp);

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

    filePath = "records\\" + testFolder + "\\" + "allPipePressure.csv";
    }
    QString filePathLast ="lastTestName.txt";
#endif

    QFile file(filePath);

    if (file.open(QFile::WriteOnly|QFile::Append))
    {
        QTextStream stream(&file);
        stream << QDate::currentDate().toString(Qt::SystemLocaleShortDate)<<";"
               << QTime::currentTime().toString()<< ";"
               << tKeyElapsed << ";" << QString::number(pipe1Pressure) <<  ";"
               << QString::number(liquid_temp) <<  ";"
               << QString::number(cabinAverageTemp) << "\n";
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
{   int pKey ;

    pKey = pKey + (double(pressurePeriod)/1000.0);

  //  pKey = QTime::currentTime().msecsSinceStartOfDay()/1000;
    ui->pTestGraph->graph(0)->addData(pKey, pipe1Pressure);
    ui->pTestGraph->graph(1)->addData(pKey, pipe1Pressure);
    ui->pTestGraph->graph(2)->addData(pKey, pipe1Pressure);
    ui->pTestGraph->graph(3)->addData(pKey, pipe1Pressure);
    ui->pTestGraph->graph(4)->addData(pKey, pipe1Pressure);
    ui->pTestGraph->graph(5)->addData(pKey, pipe1Pressure);

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
    QString filePath;
    if (startFromRecords){
     filePath = dataFilePath;
    }
    else{
        switch (pipeInfo) {
        case 1:

            filePath = "records\\" + testFolder + "\\" + "pipe1.csv";
            break;
        case 2:

            filePath = "records\\" + testFolder + "\\" + "pipe2.csv";
            break;
        case 3:

            filePath = "records\\" + testFolder + "\\" + "pipe3.csv";
            break;
        case 4:

            filePath = "records\\" + testFolder + "\\" + "pipe4.csv";
            break;
        case 5:

            filePath = "records\\" + testFolder + "\\" + "pipe5.csv";
            break;

        default:
            break;
        }
    //filePath = "records\\" + testFolder + "\\" + "pressure.csv";
    }
    QString filePathLast ="lastTestName.txt";
#endif

    QFile file(filePath);


    if (file.open(QFile::WriteOnly|QFile::Append))
    {
        QTextStream stream(&file);
        stream << QDate::currentDate().toString(Qt::SystemLocaleShortDate)<<";"
               << QTime::currentTime().toString()<< ";"
               << tKeyElapsed << ";" << QString::number(cabinAverageTemp) << ";"
               << QString::number(pipe1Pressure) << "\n";
        file.close();

    }

    QFile file2(filePathLast);

    if (file2.open(QFile::WriteOnly|QFile::Text)){
         QTextStream stream2(&file2);

         stream2<<QString::number(currentProfile)<< "\n"<<QString::number(tKey);
         file2.close();
    }

}



void MainWindow::on_bScreenshot_saved()
{
    QScreen *screensave = QGuiApplication::primaryScreen();


    QPixmap pixmap = screensave->grabWindow(0);
//    QString fileDir = "/home/pi/InDetail/screenshots/";
    QString fileDir = "screenshots\\";
    QString fileName = QDate::currentDate().toString("dd.MM.yy") + "-" +
            QTime::currentTime().toString("hh.mm.ss");
    QString fileExtention = ".png";
    QFile file(fileDir + fileName + fileExtention);
    if (file.open(QIODevice::WriteOnly))
    {

        pixmap.save(&file, "PNG");

    }
}

void MainWindow::on_bScreenshot_clicked()                                      // Grafik sayfası dikey zoom + butonuna basıldı
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

void MainWindow::on_bStartMaintenance_clicked()                                // Bakım sayfasında "Bakım Moduna Gir" butonuna basıldı.
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
        ui->chbHumidityDevice->setEnabled(true);


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
        ui->chbHumidityDevice->setEnabled(false);

        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x04);
        proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
    }
}

void MainWindow::on_bRes_clicked()                                             // Bakım sayfasında "Isıtıcı" butonuna basıldı.
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

void MainWindow::on_bFan_clicked()                                             // Bakım sayfasında "Fan" butonuna basıldı.
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
        ui->tTestGraph->setVisible(true);
        ui->pTestGraph->setVisible(true);
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

    if (ui->pTestGraph->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->pTestGraph->axisRect()->setRangeDrag(ui->pTestGraph->xAxis->orientation());
    else if (ui->pTestGraph->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->pTestGraph->axisRect()->setRangeDrag(ui->pTestGraph->yAxis->orientation());
    else
        ui->pTestGraph->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
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
    if (ui->tTestGraph->yAxis2->range().upper > 200 || ui->pTestGraph->yAxis2->range().upper < 0 )
    {

         ui->tTestGraph->yAxis2->setRange(0,200);
         ui->tTestGraph->yAxis2->range().bounded(0,200);
         ui->tTestGraph->replot();
    }
    //ayırım
    if (ui->pTestGraph->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
        ui->pTestGraph->axisRect()->setRangeZoom(ui->pTestGraph->xAxis->orientation());

    }
    else if (ui->pTestGraph->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
        ui->pTestGraph->axisRect()->setRangeZoom(ui->pTestGraph->yAxis->orientation());

    }
    else {
        ui->pTestGraph->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);

    }
    if (ui->pTestGraph->yAxis2->range().upper > 200 || ui->pTestGraph->yAxis2->range().upper < 0 )
    {

         ui->pTestGraph->yAxis2->setRange(0,200);
         ui->pTestGraph->yAxis2->range().bounded(0,200);
         ui->pTestGraph->replot();
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

void MainWindow::on_ZoomInHor_clicked()                               // Grafik sayfası dikey zoom + butonuna basıldı
{

    ui->tTestGraph->xAxis->scaleRange(.85, ui->tTestGraph->xAxis->range().center());
    ui->tTestGraph->replot();


}

void MainWindow::on_ZoomOutHor_clicked()                              // Grafik sayfası dikey zoom - butonuna basıldı
{
    ui->tTestGraph->xAxis->scaleRange(1.15, ui->tTestGraph->xAxis->range().center());
    ui->tTestGraph->replot();
}

void MainWindow::on_ZoomInVer_clicked()                               // Grafik sayfası yatay zoom + butonuna basıldı
{
    ui->tTestGraph->yAxis->scaleRange(.85, ui->tTestGraph->yAxis->range().center());
    ui->tTestGraph->yAxis->range().bounded(-5,220);




    ui->tTestGraph->yAxis2->scaleRange(.85, ui->tTestGraph->yAxis2->range().center());
    ui->tTestGraph->yAxis2->setRangeLower(0);
    ui->tTestGraph->replot();
}

void MainWindow::on_ZoomOutVer_clicked()                              // Grafik sayfası yatay zoom - butonuna basıldı
{
    ui->tTestGraph->yAxis->scaleRange(1.15, ui->tTestGraph->yAxis->range().center());
    ui->tTestGraph->replot();
    if (ui->tTestGraph->yAxis->range().upper > 220 || ui->tTestGraph->yAxis->range().upper < -50 )
    {
         ui->tTestGraph->yAxis->setRange(-50,220);
         ui->tTestGraph->replot();
    }



    ui->tTestGraph->yAxis2->scaleRange(1.15, ui->tTestGraph->yAxis2->range().center());
    ui->tTestGraph->yAxis2->setRangeLower(0);
    if (ui->tTestGraph->yAxis2->range().upper > 200 || ui->tTestGraph->yAxis2->range().upper < 0 )
    {
         ui->tTestGraph->yAxis2->setRange(0,200);

         ui->tTestGraph->replot();
    }
}

bool MainWindow::on_bTemperatureSet_clicked()
{




    proc->stop();

    QByteArray cantTouchThis;           // bu satırda artık mıcıtmıyorum
    float tFix;
    float hFix;
    float sFix;
    float pvFix;
    int index;

/*
    if(ui->cbFixMode->currentIndex()==0){
        index = 51;
        tFix= (ui->leFixTempValue->text().toFloat() * 10);
        hFix= 0;
        cantTouchThis.append(qint16(tFix) & 0x00FF);
        cantTouchThis.append(qint16(tFix) >> 8);
        cantTouchThis.append(qint16(hFix) & 0x00FF);
        cantTouchThis.append(qint16(hFix) >> 8);

    }
    else if (ui->cbFixMode->currentIndex()==1) {*/
        index = 52;
        tFix= (ui->leFixTempValue->text().toFloat() * 10);
        hFix= (ui->leFixHumidtyValue->text().toFloat() * 10);
        sFix= (ui->leFixLiquidTempValue->text().toFloat() * 10);
        pvFix= (ui->leFixPressureVelocity->text().toFloat() * 10);

        cantTouchThis.append(qint16(tFix) & 0x00FF);
        cantTouchThis.append(qint16(tFix) >> 8);
        cantTouchThis.append(qint16(hFix) & 0x00FF);
        cantTouchThis.append(qint16(hFix) >> 8);
        cantTouchThis.append(qint16(sFix) & 0x00FF);
        cantTouchThis.append(qint16(sFix) >> 8);
        cantTouchThis.append(qint16(pvFix) & 0x00FF);
        cantTouchThis.append(qint16(pvFix) >> 8);

   // }

    cantTouchThis.append(index);

    proc->insertProfileMessage(mySerial::makeMessage(0x68,cantTouchThis));

    cantTouchThis.clear();

    proc->setProfile();

    return true;

}

void MainWindow::on_bSetTemperatureStart_clicked()
{        
       ui->dsbCabinSetpressure->setValue(ui->leFixPressureVelocity->text().toDouble());
       ui->dsbCabinSetTemp->setValue(ui->leFixTempValue->text().toDouble());
       ui->dsbLiquidSetTemp->setValue(ui->leFixLiquidTempValue->text().toDouble());

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
        ui->bRecipeMode ->setEnabled(false);
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
    ui->bRecipeMode ->setEnabled(true);
}

void MainWindow::on_ZoomCenter_clicked()
{


    ui->tTestGraph->graph(0)->rescaleKeyAxis();
    ui->tTestGraph->yAxis->setRange(0,200);
    ui->tTestGraph->yAxis->range().bounded(0,200);

    ui->tTestGraph->yAxis2->setRange(0,180);
    ui->tTestGraph->yAxis2->range().bounded(0,180);
    ui->tTestGraph->replot();
}

void MainWindow::on_bSetExhaustValve_clicked()                          // Bakım sayfası "Set" butonuna basıldı
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

void MainWindow::on_bSaveCalibrationValues_clicked()                    // Kalibrasyon sayfası "Kalibre Et" butonuna basıldı
{
    saveValueTopTempSensorCalibration();
    saveValueBottomTempSensorCalibration();
    saveValuePressureSensor1Calibration();
    saveValueCleanTankLevelCalibration();
    saveValueDirtyTankLevelCalibration();
    saveValueExpansionTankLevelCalibration();
    saveValueLiquidTankLevelCalibration();
    saveValueLiquidTempCalibration();

}
void MainWindow::saveValueLiquidTempCalibration()
{
    #ifdef Q_OS_LINUX
        //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationLiquidTemp.txt";
    #endif

    #ifdef Q_OS_WIN
        // windows code goes here
         QString filePath = "Settings\\calLiquidTemp.txt";
    #endif

    QFile file(filePath);

    if (file.open(QFile::WriteOnly|QFile::Truncate))
    {
        QTextStream stream(&file);

        stream <<  ui->leCalLiquidTempErr->text().toDouble() << ","
                <<  ui->leCalLiquidTempCoeff->text().toDouble() ;
        file.close();
    }

}
void MainWindow::saveValueLiquidTankLevelCalibration()
{
    #ifdef Q_OS_LINUX
        //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationLiquidTankLevel.txt";
    #endif

    #ifdef Q_OS_WIN
        // windows code goes here
         QString filePath = "Settings\\calLiquidTankLevel.txt";
    #endif

    QFile file(filePath);

    if (file.open(QFile::WriteOnly|QFile::Truncate))
    {
        QTextStream stream(&file);

        stream <<  ui->leCalLiquidTankLevelErr->text().toDouble() << ","
                <<  ui->leCalLiquidTankLevelCoeff->text().toDouble() ;
        file.close();
    }

}
void MainWindow::saveValueExpansionTankLevelCalibration()
{
    #ifdef Q_OS_LINUX
        //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationTopTemp.txt";
    #endif

    #ifdef Q_OS_WIN
        // windows code goes here
         QString filePath = "Settings\\calExpansionTankLevel.txt";
    #endif

    QFile file(filePath);

    if (file.open(QFile::WriteOnly|QFile::Truncate))
    {
        QTextStream stream(&file);

        stream <<  ui->leCalExpansionTankErr->text().toDouble() << ","
                <<  ui->leCalExpansionTankCoeff->text().toDouble() ;
        file.close();
    }

}

void MainWindow::saveValueDirtyTankLevelCalibration()
{
    #ifdef Q_OS_LINUX
        //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationTopTemp.txt";
    #endif

    #ifdef Q_OS_WIN
        // windows code goes here
         QString filePath = "Settings\\calDirtyTankLevel.txt";
    #endif

    QFile file(filePath);

    if (file.open(QFile::WriteOnly|QFile::Truncate))
    {
        QTextStream stream(&file);

        stream <<  ui->leCalDirtyTankErr ->text().toDouble() << ","
                <<  ui->leCalDirtyTankCoeff->text().toDouble() ;
        file.close();
    }

}

void MainWindow::saveValueCleanTankLevelCalibration()
{
    #ifdef Q_OS_LINUX
        //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationTopTemp.txt";
    #endif

    #ifdef Q_OS_WIN
        // windows code goes here
         QString filePath = "Settings\\calCleanTankLevel.txt";
    #endif

    QFile file(filePath);

    if (file.open(QFile::WriteOnly|QFile::Truncate))
    {
        QTextStream stream(&file);

        stream <<  ui->leCalCleanTankLevelErr->text().toDouble() << ","
                <<  ui->leCalCleanTankCoeff->text().toDouble() ;
        file.close();
    }

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

        stream <<  ui->leCalCabinTopTempErr->text().toDouble() << ","
                <<  ui->leCalCabinTopTempCoeff->text().toDouble() ;
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

        stream <<  ui->leCalCabinBottomTempErr->text().toDouble() << ","
                <<  ui->leCalCabinBottomTempCoeff->text().toDouble() ;
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
        ui->leCalCabinTopTempErr->setText(QString::number(v));
        ui->leCalCabinTopTempCoeff->setText(QString::number(y));
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
        ui->leCalCabinBottomTempErr->setText(QString::number(v));
        ui->leCalCabinBottomTempCoeff->setText(QString::number(y));
        file.close();
   //     ui->dsbCalCabinTopTempErr->setValue(dList[1]);
    //    ui->dsbCalCabinTopTempCoeff->setValue(dList[2]);
    }

 }
void MainWindow::loadValueLiquidTankLevelCalibration()
{
    #ifdef Q_OS_LINUX
    //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationBottomTemp.txt";
    #endif

    #ifdef Q_OS_WIN
    // windows code goes here
        QString filePath = "settings\\calLiquidTankLevel.txt";
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
        ui->leCalLiquidTankLevelErr->setText(QString::number(v));
        ui->leCalLiquidTankLevelCoeff->setText(QString::number(y));
        file.close();
   //     ui->dsbCalCabinTopTempErr->setValue(dList[1]);
    //    ui->dsbCalCabinTopTempCoeff->setValue(dList[2]);
    }

 }
void MainWindow::loadValueLiquidTempCalibration()
{
    #ifdef Q_OS_LINUX
    //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationBottomTemp.txt";
    #endif

    #ifdef Q_OS_WIN
    // windows code goes here
        QString filePath = "settings\\calLiquidTemp.txt";
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
        ui->leCalLiquidTempErr->setText(QString::number(v));
        ui->leCalLiquidTempCoeff->setText(QString::number(y));
        file.close();
   //     ui->dsbCalCabinTopTempErr->setValue(dList[1]);
    //    ui->dsbCalCabinTopTempCoeff->setValue(dList[2]);
    }

 }
void MainWindow::loadValueCleanTankLevelCalibration()
{
    #ifdef Q_OS_LINUX
    //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationBottomTemp.txt";
    #endif

    #ifdef Q_OS_WIN
    // windows code goes here
        QString filePath = "settings\\calCleanTankLevel.txt";
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
        ui->leCalCleanTankLevelErr->setText(QString::number(v));
        ui->leCalCleanTankCoeff->setText(QString::number(y));
        file.close();
   //     ui->dsbCalCabinTopTempErr->setValue(dList[1]);
    //    ui->dsbCalCabinTopTempCoeff->setValue(dList[2]);
    }

 }

void MainWindow::loadValueDirtyTankLevelCalibration()
{
    #ifdef Q_OS_LINUX
    //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationBottomTemp.txt";
    #endif

    #ifdef Q_OS_WIN
    // windows code goes here
        QString filePath = "settings\\calDirtyTankLevel.txt";
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
        ui->leCalDirtyTankErr->setText(QString::number(v));
        ui->leCalDirtyTankCoeff->setText(QString::number(y));
        file.close();
   //     ui->dsbCalCabinTopTempErr->setValue(dList[1]);
    //    ui->dsbCalCabinTopTempCoeff->setValue(dList[2]);
    }

 }

void MainWindow::loadValueExpansionTankLevelCalibration()
{
    #ifdef Q_OS_LINUX
    //linux code goes here
        QString filePath = "/home/pi/InDetail/settings/" +"calibrationBottomTemp.txt";
    #endif

    #ifdef Q_OS_WIN
    // windows code goes here
        QString filePath = "settings\\calExpansionTankLevel.txt";
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
        ui->leCalExpansionTankErr->setText(QString::number(v));
        ui->leCalExpansionTankCoeff->setText(QString::number(y));
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

        stream <<  ui->leCalPipePressure1Err->text().toDouble() << ","
                <<  ui->leCalPipePressure1Coeff->text().toDouble() ;
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
        ui->leCalPipePressure1Err->setText(QString::number(v));
        ui->leCalPipePressure1Coeff->setText(QString::number(y));
        file.close();
    }
}

void MainWindow::on_bEditProLook_clicked()                                  // Yeni Recete sayfasında "İncele" butonuna basıldı.
{

    lookProfile lookProfile;
    lookProfile.readProfiles(profileIndexMain+1);
    lookProfile.setModal(true);
    lookProfile.exec();
}

void MainWindow::on_bClearLogTable_2_clicked()                              // Sayfa kapatma butonuna basıldı.
{
    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::run_keyboard_lineEdit()
{
    QLineEdit *line = (QLineEdit *)sender();
    lineEditkeyboard->setLineEdit(line);

    lineEditkeyboard->show();
}

void MainWindow::on_bRecipeMode_clicked()                                     //  Ana Sayfada "Recete mod"u butonuna basıldı.
{
    ui->mainPage->setCurrentIndex(0);
    ui->rectRecipe->setVisible(true);
    ui->rectFix->setVisible(false);
    ui->rect1500h->setVisible(false);
}

void MainWindow::on_bFixMode_clicked()                                      // Ana Sayfada "Fix modu" butonuna basıldı.
{
    ui->mainPage->setCurrentIndex(1);
    ui->rectRecipe->setVisible(false);
    ui->rectFix->setVisible(true);
    ui->rect1500h->setVisible(false);
}

void MainWindow::on_b1500hMode_clicked()                                    // Ana Sayfada "1500H" butonuna basıldı.
{
    ui->mainPage->setCurrentIndex(2);
    ui->rectRecipe->setVisible(false);
    ui->rectFix->setVisible(false);
    ui->rect1500h->setVisible(true);
}

void MainWindow::on_bLightsMain_clicked()                                   //  Ana Sayfada "Aydınlatma" butonuna basıldı.
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

void MainWindow::on_bStartTest1500h_clicked()                               //  Ana Sayfada "Başlat" butonuna basıldı.
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

void MainWindow::on_bPauseTest_clicked()                                    //  Ana Sayfada "Beklet" butonuna basıldı.
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

void MainWindow::on_bStopTestManual_clicked()                               //  Ana Sayfada "İptal" butonuna basıldı.
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

void MainWindow::on_bSendProfileMain_clicked()                              //  Ana Sayfada "Receteyi Onayla" butonuna basıldı.
{

    quint8 index = ui->cbSelectProfileMain->currentIndex();
    if ((tProfileLoad[index].active == 0) && (pProfileLoad[index].active == 0) && (vProfileLoad[index].active == 0))
    {
        QMessageBox::information(
                    this,
                    tr(appName),
                    tr("This profile does not contain any step in temperature, pressure or vibration.") );
    }
    else
    {   ui->test_adi->setText(tProfileLoad[index].name);

        ui->test_adi_11->setText(tProfileLoad[index].name);



        ui->parca_adi->setText(tProfileLoad[index].partname);

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

void MainWindow::on_cbSelectProfileMain_currentIndexChanged(int index)      //  Ana Sayfada "Bir Test Recetesi Seciniz" açılan kutusu değişti.
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
    {   ui->laSelectedProfileMain->setText("");
        ui->laSelectedProfileManual->setText("");
        ui->laTestName->setText("");

        quint8 index = ui->cbSelectProfileMain->currentIndex();
        if (readProfile(index))
        {
            if (tProfileLoad[index].active == 0)
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
                for(int j=1; j <= (tProfileLoad[index].totalStep ); j++)
                {
                   if (tProfileLoad[index].step[j-1].stepUnit == 2 )
                   {
                    totalTestDuration = totalTestDuration + (tProfileLoad[index].step[j-1].lDuration * 60);
                   }
                }
      //          ui->laTotalTestSecond->setText(QString::number(totalTestDuration)) ;
                ui->progressBar_2->setValue(0);
                ui->progressBar_2->setMinimum(0);

                ui->progressBar_2->setMaximum(totalTestDuration);
            }
        }
    }

if (ui->bPatlatYesNo->isChecked()){
    ui->bSendProfileMain->setEnabled(true);
}

}

void MainWindow::on_btnBackDetails_clicked()                                // Test Takip sayfasında "Geri" butonuna basıldı.
{
    ui->detailsPages->setCurrentIndex(0);
}

void MainWindow::on_btnDetailsInfo_clicked()                                // Test Takip sayfasında "Detail bilgi" butonuna basıldı.
{
    ui->detailsPages->setCurrentIndex(0);
    ui->detailsBottomPages->setCurrentIndex(0);
}

void MainWindow::on_btnDetailsPipes_clicked()                                // Test Takip sayfasında "Detail hortumlar" butonuna basıldı.
{
    ui->detailsPages->setCurrentIndex(0);
    ui->detailsBottomPages->setCurrentIndex(2);
}

void MainWindow::on_btnDetailsTanklevel_clicked()                            // Test Takip sayfasında "Detail Tank Seviye" butonuna basıldı.
{
    ui->detailsPages->setCurrentIndex(0);
    ui->detailsBottomPages->setCurrentIndex(1);
}

void MainWindow::on_btnDetailsHeater_clicked()                               // Test Takip sayfasında "Detail ısıtıcı" butonuna basıldı.
{
    ui->detailsPages->setCurrentIndex(0);
    ui->detailsBottomPages->setCurrentIndex(3);
}

void MainWindow::on_btnDetailsPressure_clicked()                             // Test Takip sayfasında "Hata" butonuna basıldı.
{
    ui->detailsPages->setCurrentIndex(0);
    ui->detailsBottomPages->setCurrentIndex(4);

}

void MainWindow::on_bResetFault_clicked()                                    // Test Takip sayfasında "Hata" butonuna basıldı.
{

    proc->stop();

    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(0x03);
    proc->insertCommandMessage(mySerial::makeMessage(0x0A,cantTouchThis));
    ui->tabWidget->setCurrentIndex(0);

    if(!commStatus){
       serial->startSerial();
    }

    resetFaultVisuals();

}

void MainWindow::resetFaultVisuals(){

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
    ui->laFault4A_2->setVisible(false);
    ui->laFault71->setVisible(false);
    ui->laFault72->setVisible(false);
    ui->laFault73->setVisible(false);
    ui->laFault74->setVisible(false);
    ui->laFault75->setVisible(false);
    ui->laFault76->setVisible(false);

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

bool MainWindow::on_bSendProfile1500h_clicked()                              // Ana Sayfa "Secim Onayla" butonuna basıldı.
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
    quint16 LiquidCirculationtime1500h = ui->leLiquidCirculationTime1500h->text().toFloat();
    quint16 LiquidChangeTemp1500h = ui->leLiquidChangeTemp1500h->text().toFloat();
    cantTouchThis.append(1);

    cantTouchThis.append(qint16(setPressure1500h) & 0x00FF);
    cantTouchThis.append(qint16(setPressure1500h) >> 8);
    cantTouchThis.append(quint16(setTemp1500h) & 0x00FF);
    cantTouchThis.append(quint16(setTemp1500h) >> 8);
    cantTouchThis.append(quint16(totalDuration) & 0x00FF);
    cantTouchThis.append(quint16(totalDuration) >> 8);
    cantTouchThis.append(quint16(changePeriod) & 0x00FF);
    cantTouchThis.append(quint16(changePeriod) >> 8);
    cantTouchThis.append(quint16(LiquidCirculationtime1500h) & 0x00FF);
    cantTouchThis.append(quint16(LiquidCirculationtime1500h) >> 8);
    cantTouchThis.append(quint16(LiquidChangeTemp1500h) & 0x00FF);
    cantTouchThis.append(quint16(LiquidChangeTemp1500h) >> 8);

    cantTouchThis.append(activePipes);

    proc->insertProfileMessage(mySerial::makeMessage(0x67,cantTouchThis));

    cantTouchThis.clear();

    proc->setProfile();

    ui->pb_testProgress->setMaximum(totalDuration);
    ui->pb_testProgress->setMinimum(0);
    ui->pb_testProgress->setValue(0);


    return true;
}

bool MainWindow::on_checkBox_stateChanged()                                  // Ana Sayfa "Hortum 1" butonuna basıldı.
{
    if (ui->checkBox->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

bool MainWindow::on_checkBox_4_clicked()                                     // Ana Sayfa "Hortum 4" butonuna basıldı.
{
    if (ui->checkBox_4->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

bool MainWindow::on_checkBox_5_clicked()                                     // Ana Sayfa "Hortum 5" butonuna basıldı.
{
    if (ui->checkBox_5->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

bool MainWindow::on_checkBox_2_clicked()                                     // Ana Sayfa "Hortum 2" butonuna basıldı.
{
    if (ui->checkBox_2->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

bool MainWindow::on_checkBox_3_clicked()                                     // Ana Sayfa "Hortum 3" butonuna basıldı.
{
    if (ui->checkBox_3->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

void MainWindow::on_bMinimize_2_clicked()                                    // Test Takip sayfasında Sayfa kapatma butonuna basıldı.
{
     this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_bMinimize_3_clicked()                                     // Grafik sayfasında Sayfa kapatma butonuna basıldı.
{
    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_bStopTest1500h_clicked()                                 // Ana Sayfa "İptal" butonuna basıldı.
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

void MainWindow::on_bPauseTest1500h_clicked()                                // Ana Sayfa "beklet" butonuna basıldı.
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

void MainWindow::on_b_pipe_1_clicked()                                     // Test Kontrol sayfası "HAT 1" butonuna basıldı
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

void MainWindow::on_b_pipe_2_clicked()                                     // Test Kontrol sayfası "HAT 2" butonuna basıldı
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

void MainWindow::on_b_pipe_3_clicked()                                     // Test Kontrol sayfası "HAT 3" butonuna basıldı
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

void MainWindow::on_b_pipe_4_clicked()                                     // Test Kontrol sayfası "HAT 4" butonuna basıldı
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

void MainWindow::on_b_pipe_5_clicked()                                     // Test Kontrol sayfası "HAT 5" butonuna basıldı
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

void MainWindow::on_b_basinc_tank_doldur_clicked()                         // Test Kontrol sayfası "Basınç Tankına" butonuna basıldı
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

void MainWindow::on_b_temiz_tank_bosalt_clicked()                          // Test Kontrol sayfası "Kirli Tankına" butonuna basıldı.
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

void MainWindow::on_b_tum_tanklari_bosalt_clicked()                        // Test Kontrol sayfası "Hatları Boşalt" butonuna basıldı.
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

void MainWindow::on_bChooseData_clicked()                                  // Ana Sayfa "Kayıt Seç" butonuna basıldı.
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

void MainWindow::on_bDoorControlActive_clicked()                           // Bakım sayfası "Kapak Kilidi Kullanımda" butonuna basıldı
{

}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    if(ui->lineEdit->text() == "11"){
        writeToLogTable("Kapı kilidi şifresi onaylandı.");
        activateCabinDoorLock();
    }

}

void MainWindow::activateCabinDoorLock(){

       QByteArray cantTouchThis;
       cantTouchThis.clear();
       cantTouchThis.append(0x01);
       proc->insertCommandMessage(mySerial::makeMessage(0x99,cantTouchThis));
       ui->lineEdit->setText("");

}

void MainWindow::on_bDoorCOntrolDeactive_clicked()
{
    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(char(0x00));
    proc->insertCommandMessage(mySerial::makeMessage(0x99,cantTouchThis));
    ui->lineEdit->setText("");
}

bool MainWindow::on_Hortum1_stateChanged()                                        // Test Kontrol sayfası Hortum seçim kutucuğu hortum 1 değişti
{
    if (ui->Hortum1->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

bool MainWindow::on_Hortum2_stateChanged()                                        // Test Kontrol sayfası Hortum seçim kutucuğu hortum 2 değişti
{
    if (ui->Hortum2->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

bool MainWindow::on_Hortum3_stateChanged()                                        // Test Kontrol sayfası Hortum seçim kutucuğu hortum 3 değişti
{
    if (ui->Hortum3->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

bool MainWindow::on_Hortum4_stateChanged()                                        // Test Kontrol sayfası Hortum seçim kutucuğu hortum 4 değişti
{
    if (ui->Hortum4->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

bool MainWindow::on_Hortum5_stateChanged()                                        // Test Kontrol sayfası Hortum seçim kutucuğu hortum 5 değişti
{
    if (ui->Hortum5->isChecked()){
        return true;
    }
    else{
        return false;
    }
}

void MainWindow::on_bManualPressureLinesStart_clicked()                           // Test Kontrol sayfası "Hat Kontrol Başlat" butonu basıldı.
{
    char activePipes;

    if(on_Hortum1_stateChanged()){
        activePipes |= 1 << 0;
    }
    else
    {
        activePipes &= ~1;
    }

    if(on_Hortum2_stateChanged()){
        activePipes |= 1 << 1;
    }
    else
    {
        activePipes &= ~2;
    }
    if(on_Hortum3_stateChanged()){
        activePipes |= 1 << 2;
    }
    else
    {
        activePipes &= ~4;
    }

    if(on_Hortum4_stateChanged()){
        activePipes |= 1 << 3;
    }
    else
    {
        activePipes &= ~8;
    }

    if(on_Hortum5_stateChanged()){
        activePipes |= 1 << 4;
    }
    else
    {
        activePipes &= ~16;
    }
    QByteArray cantTouchThis;
    cantTouchThis.clear();
    writeToLogTable("Manual hortum kontrolu başlatıldı.");
    cantTouchThis.append(activePipes);
    cantTouchThis.append(char(0x01));
    cantTouchThis.append(char(0x00));
    cantTouchThis.append(char(0x00));

    proc->insertCommandMessage(mySerial::makeMessage(0xA1,cantTouchThis));
}

void MainWindow::on_bManualPressureLinesStop_clicked()                            // Test Kontrol sayfası "Hat Kontrol Durdur" butonu basıldı.
{
    char activePipes;

    if(on_Hortum1_stateChanged()){
        activePipes |= 1 << 0;
    }
    else
    {
        activePipes &= ~1;
    }

    if(on_Hortum2_stateChanged()){
        activePipes |= 1 << 1;
    }
    else
    {
        activePipes &= ~2;
    }
    if(on_Hortum3_stateChanged()){
        activePipes |= 1 << 2;
    }
    else
    {
        activePipes &= ~4;
    }

    if(on_Hortum4_stateChanged()){
        activePipes |= 1 << 3;
    }
    else
    {
        activePipes &= ~8;
    }

    if(on_Hortum5_stateChanged()){
        activePipes |= 1 << 4;
    }
    else
    {
        activePipes &= ~16;
    }
    QByteArray cantTouchThis;
    cantTouchThis.clear();

    writeToLogTable("Manual hortum kontrolleri durduruldu.");
    cantTouchThis.append(activePipes);
    cantTouchThis.append(char(0x00));
    cantTouchThis.append(char(0x00));
    cantTouchThis.append(char(0x00));

    proc->insertCommandMessage(mySerial::makeMessage(0xA1,cantTouchThis));
}

void MainWindow::on_bManualEvacLines_clicked()                                    // Test Kontrol sayfası "Hattı Boşalt" butonu basıldı.
{
    char activePipes;

    if(on_Hortum1_stateChanged()){
        activePipes |= 1 << 0;
    }
    else
    {
        activePipes &= ~1;
    }

    if(on_Hortum2_stateChanged()){
        activePipes |= 1 << 1;
    }
    else
    {
        activePipes &= ~2;
    }
    if(on_Hortum3_stateChanged()){
        activePipes |= 1 << 2;
    }
    else
    {
        activePipes &= ~4;
    }

    if(on_Hortum4_stateChanged()){
        activePipes |= 1 << 3;
    }
    else
    {
        activePipes &= ~8;
    }

    if(on_Hortum5_stateChanged()){
        activePipes |= 1 << 4;
    }
    else
    {
        activePipes &= ~16;
    }
    QByteArray cantTouchThis;
    cantTouchThis.clear();

    writeToLogTable("Manual hat boşaltma çalışıyor.");
    cantTouchThis.append(activePipes);
    cantTouchThis.append(char(0x00));
    cantTouchThis.append(char(0x01));
    cantTouchThis.append(char(0x00));

    proc->insertCommandMessage(mySerial::makeMessage(0xA1,cantTouchThis));
}

void MainWindow::on_bManualPrepareLines_clicked()                                 // Test Kontrol sayfası "Hava Alma Başlat" butonu basıldı.
{
    char activePipes;

    if(on_Hortum1_stateChanged()){
        activePipes |= 1 << 0;
    }
    else
    {
        activePipes &= ~1;
    }

    if(on_Hortum2_stateChanged()){
        activePipes |= 1 << 1;
    }
    else
    {
        activePipes &= ~2;
    }
    if(on_Hortum3_stateChanged()){
        activePipes |= 1 << 2;
    }
    else
    {
        activePipes &= ~4;
    }

    if(on_Hortum4_stateChanged()){
        activePipes |= 1 << 3;
    }
    else
    {
        activePipes &= ~8;
    }

    if(on_Hortum5_stateChanged()){
        activePipes |= 1 << 4;
    }
    else
    {
        activePipes &= ~16;
    }
    QByteArray cantTouchThis;
    cantTouchThis.clear();

    writeToLogTable("Manual hortum hava alma yapılıyor");
    cantTouchThis.append(activePipes);
    cantTouchThis.append(char(0x00));
    cantTouchThis.append(char(0x00));
    cantTouchThis.append(char(0x01));

    proc->insertCommandMessage(mySerial::makeMessage(0xA1,cantTouchThis));
}

void MainWindow::on_btnIsnHumidty_2_clicked()
{
    ui->detailsPages->setCurrentIndex(0);
    ui->detailsBottomPages->setCurrentIndex(6);
}

void MainWindow::on_btnIsnHumidty_clicked()
{
    ui->detailsPages->setCurrentIndex(0);
    ui->detailsBottomPages->setCurrentIndex(7);
}

void MainWindow::on_bMainDoorInfo_clicked()
{
    if (ui->bMainDoorInfo->isChecked())
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(0x01);
        proc->insertCommandMessage(mySerial::makeMessage(0x9A,cantTouchThis));
    }
    else
    {
        QByteArray cantTouchThis;
        cantTouchThis.clear();
        cantTouchThis.append(char(0x00));
        proc->insertCommandMessage(mySerial::makeMessage(0x9A,cantTouchThis));
        QMessageBox::information(
                    this,
                    tr(appName),
                    tr("Lütfen kapının kapanması için butona basınız.") );
    }


}

void MainWindow::on_bSaveProfromTabl_clicked()
{


    saveProfileInfoToStruct(ui->cbChooseProfileSlot->currentIndex());

    saveProfileInfoToFile(saveProfileInfoQByteArray(ui->cbChooseProfileSlot->currentIndex()));

    //setupComboBoxes();
}

void MainWindow::saveProfileInfoToStruct(quint8 currentProfile){


    if (ui->cbChooseProfileTypeNew->currentIndex() == 0)
    {
        tProfileSave[currentProfile].active = 1;
        nProfileSave[currentProfile].active = 0;
    }
    else if (ui->cbChooseProfileTypeNew->currentIndex() == 1)
    {
        tProfileSave[currentProfile].active = 1;
        nProfileSave[currentProfile].active = 1;
    }
        ui->bClearProfromTabl->setEnabled(false);


        ui->cbChooseProfileSlot->setItemText(currentProfile,ui->leSaveProfileName->text());
        ui->cbChooseProfileSlot->setItemData(currentProfile, QColor( Qt::black ), Qt::TextColorRole );
        ui->cbSelectProfileMain->setItemText(currentProfile,ui->leSaveProfileName->text());
        ui->cbSelectProfileMain->setItemData(currentProfile, QColor( Qt::black ), Qt::TextColorRole );

        tProfileSave[currentProfile].sTarget= ui->leFixLiquitTempValue->text().toDouble(); // dikkat


        tProfileSave[currentProfile].startValue = ui->leTempInput_0->text().toDouble();

        nProfileSave[currentProfile].startValue = ui->leHumidity_0->text().toDouble();

        tProfileSave[currentProfile].totalStep = ui->sbStepCount->value();
        nProfileSave[currentProfile].totalStep = ui->sbStepCount->value();

    if (  ( ui->cbChooseTimeCoeff->currentIndex() == 0 ) )
        {
        //nothing selected for neither step type nor step time unit. Going forward should be banned.

        ui->leChangetime->setText("");
        }
        else if ( ui->cbChooseTimeCoeff->currentIndex() == 1 )
        {
        tProfileSave[currentProfile].step[0].stepUnit  =1;
        //minute selected for step time unit, do what you gotta do.

        //   nProfileSave[currentProfile].active = 1;
        //   nProfileSave[currentProfile].step[currentNStep].stepUnit = 2;
        //     tProfileSave[currentProfile].startValue = ui->dsbTStartValue->value();

        ui->leChangetime->setText("(sa.)");
        }
        else if ( ui->cbChooseTimeCoeff->currentIndex() == 2 )
        {

        tProfileSave[currentProfile].step[0].stepUnit  =2;


        //hour selected for step time unit, do what you gotta do.

        //   nProfileSave[currentProfile].active = 1;
        //   nProfileSave[currentProfile].step[currentNStep].stepUnit = 3;
        //     tProfileSave[currentProfile].startValue = ui->dsbTStartValue->value();

        ui->leChangetime->setText("(dk.)");
        }
    else if ( ui->cbChooseTimeCoeff->currentIndex() == 3 )
    {

    tProfileSave[currentProfile].step[0].stepUnit  =3;


    //hour selected for step time unit, do what you gotta do.

    //   nProfileSave[currentProfile].active = 1;
    //   nProfileSave[currentProfile].step[currentNStep].stepUnit = 3;
    //     tProfileSave[currentProfile].startValue = ui->dsbTStartValue->value();

    ui->leChangetime->setText("(sn.)");
    }
    tProfileSave[currentProfile].step[0].lDuration = ui->leTime_1->text().toDouble();

    tProfileSave[currentProfile].step[0].lTarget   = ui->leTempInput_1->text().toDouble();
    nProfileSave[currentProfile].step[0].lTarget   = ui->leHumidity_1->text().toDouble();

    tProfileSave[currentProfile].step[1].lDuration = ui->leTime_2->text().toDouble();
    tProfileSave[currentProfile].step[1].lTarget   = ui->leTempInput_2->text().toDouble();
    nProfileSave[currentProfile].step[1].lTarget   = ui->leHumidity_2->text().toDouble();

    tProfileSave[currentProfile].step[2].lDuration = ui->leTime_3->text().toDouble();
    tProfileSave[currentProfile].step[2].lTarget   = ui->leTempInput_3->text().toDouble();
    nProfileSave[currentProfile].step[2].lTarget   = ui->leHumidity_3->text().toDouble();

    tProfileSave[currentProfile].step[3].lDuration = ui->leTime_4->text().toDouble();
    tProfileSave[currentProfile].step[3].lTarget   = ui->leTempInput_4->text().toDouble();
    nProfileSave[currentProfile].step[3].lTarget   = ui->leHumidity_4->text().toDouble();

    tProfileSave[currentProfile].step[4].lDuration = ui->leTime_5->text().toDouble();
    tProfileSave[currentProfile].step[4].lTarget   = ui->leTempInput_5->text().toDouble();
    nProfileSave[currentProfile].step[4].lTarget   = ui->leHumidity_5->text().toDouble();

    tProfileSave[currentProfile].step[5].lDuration = ui->leTime_6->text().toDouble();
    tProfileSave[currentProfile].step[5].lTarget   = ui->leTempInput_6->text().toDouble();
    nProfileSave[currentProfile].step[5].lTarget   = ui->leHumidity_6->text().toDouble();

    tProfileSave[currentProfile].step[6].lDuration = ui->leTime_7->text().toDouble();
    tProfileSave[currentProfile].step[6].lTarget   = ui->leTempInput_7->text().toDouble();
    nProfileSave[currentProfile].step[6].lTarget   = ui->leHumidity_7->text().toDouble();

    tProfileSave[currentProfile].step[7].lDuration = ui->leTime_8->text().toDouble();
    tProfileSave[currentProfile].step[7].lTarget   = ui->leTempInput_8->text().toDouble();
    nProfileSave[currentProfile].step[7].lTarget   = ui->leHumidity_8->text().toDouble();

    tProfileSave[currentProfile].step[8].lDuration = ui->leTime_9->text().toDouble();
    tProfileSave[currentProfile].step[8].lTarget   = ui->leTempInput_9->text().toDouble();
    nProfileSave[currentProfile].step[8].lTarget   = ui->leHumidity_9->text().toDouble();

    tProfileSave[currentProfile].step[9].lDuration = ui->leTime_10->text().toDouble();
    tProfileSave[currentProfile].step[9].lTarget   = ui->leTempInput_10->text().toDouble();
    nProfileSave[currentProfile].step[9].lTarget   = ui->leHumidity_10->text().toDouble();

    tProfileSave[currentProfile].step[10].lDuration = ui->leTime_11->text().toDouble();
    tProfileSave[currentProfile].step[10].lTarget   = ui->leTempInput_11->text().toDouble();
    nProfileSave[currentProfile].step[10].lTarget   = ui->leHumidity_11->text().toDouble();

    tProfileSave[currentProfile].step[11].lDuration = ui->leTime_12->text().toDouble();
    tProfileSave[currentProfile].step[11].lTarget   = ui->leTempInput_12->text().toDouble();
    nProfileSave[currentProfile].step[11].lTarget   = ui->leHumidity_12->text().toDouble();

    tProfileSave[currentProfile].step[12].lDuration = ui->leTime_13->text().toDouble();
    tProfileSave[currentProfile].step[12].lTarget   = ui->leTempInput_13->text().toDouble();
    nProfileSave[currentProfile].step[12].lTarget   = ui->leHumidity_13->text().toDouble();

    tProfileSave[currentProfile].step[13].lDuration = ui->leTime_14->text().toDouble();
    tProfileSave[currentProfile].step[13].lTarget   = ui->leTempInput_14->text().toDouble();
    nProfileSave[currentProfile].step[13].lTarget   = ui->leHumidity_14->text().toDouble();

    tProfileSave[currentProfile].step[14].lDuration = ui->leTime_15->text().toDouble();
    tProfileSave[currentProfile].step[14].lTarget   = ui->leTempInput_15->text().toDouble();
    nProfileSave[currentProfile].step[14].lTarget   = ui->leHumidity_15->text().toDouble();
    labelEnable(1);




}

QByteArray MainWindow::saveProfileInfoQByteArray(quint8 currentProfile){

    QByteArray profile;

    profile.append(ui->leSaveProfileName->text().toLocal8Bit());
       profile.append("/");
    profile.append("Temperature General Info");
    profile.append("/");
    profile.append(tProfileSave[currentProfile].active);
    profile.append("/");
    float tStart = tProfileSave[currentProfile].startValue*10.0;
    profile.append(qint16(tStart) & 0x00FF);
    profile.append(qint16(tStart) >> 8);
    profile.append("/");
    float sqTarget = tProfileSave[currentProfile].sTarget;
    profile.append(quint16(sqTarget) & 0x00FF);
    profile.append(quint16(sqTarget) >> 8);

    profile.append("/");
   // profile.append(ui->leSaveParcaAdi->text().toLocal8Bit());



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
    profile.append("/");
    profile.append("Humidity General Info");
    profile.append("/");
    profile.append(nProfileSave[currentProfile].active);
    profile.append("/");
    float nStart = nProfileSave[currentProfile].startValue*10.0;
    profile.append(qint16(nStart) & 0x00FF);
    profile.append(qint16(nStart) >> 8);
    profile.append("/");
    float nTotalStep = nProfileSave[currentProfile].totalStep;
    profile.append(quint16(nTotalStep) & 0x00FF);
    profile.append(quint16(nTotalStep) >> 8);

    for(int i=0; i<nProfileSave[currentProfile].totalStep; i++)
    {
        profile.append("/");
        profile.append("Step-");
        profile.append(i+1);
        profile.append("/");
        quint8 nStepUnit = nProfileSave[currentProfile].step[i].stepUnit;
        profile.append(nStepUnit);
        profile.append("/");
        quint8 nStepType = nProfileSave[currentProfile].step[i].stepType;
        profile.append(nStepType);
        profile.append("/");
        float nLDuration = nProfileSave[currentProfile].step[i].lDuration;
        profile.append(quint16(nLDuration) & 0x00FF);
        profile.append(quint16(nLDuration) >> 8);
        profile.append("/");
        float nLTarget = nProfileSave[currentProfile].step[i].lTarget*10.0;
        profile.append(qint16(nLTarget) & 0x00FF);
        profile.append(qint16(nLTarget) >> 8);
    }


    profile.append("/End of Profile.");

    return profile;

}

void MainWindow::saveProfileInfoToFile(QByteArray profile){


#ifdef Q_OS_LINUX
    //linux code goes here
    QString filePath = "/home/pi/InDetail/profiles/Profile " + QString::number( ui->cbChooseProfileSlot->currentIndex()) + ".dat";
#endif
#ifdef Q_OS_WIN
    // windows code goes here
    QString filePath = "profiles\\Profile " + QString::number(ui->cbChooseProfileSlot->currentIndex()) + ".dat";
#endif

    QFile file(filePath);

    file.remove();
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
}

void MainWindow::on_cbChooseProfileSlot_currentIndexChanged(int index)
{
    readProfile(index);



    plotRecipeGraph(index);
if(ui->cbChooseProfileSlot->currentIndex()==0){

    ui->bEditTablo->setEnabled(false);
    labelEnable(1);
    ui->bClearProfromTabl->setEnabled(false);
}
else{
    ui->bEditTablo->setEnabled(true);
    ui->bClearProfromTabl->setEnabled(false);
    labelEnable(1);
}
}

bool MainWindow::readProfile(int index){

   readProfileToStruct(readProfileFromFileToQbyteArray(index),index);

   readProfileToTable(index);

   return true;
}

QByteArray MainWindow::readProfileFromFileToQbyteArray(int index){

    QByteArray rProfile;
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
    {   if (ui->tabWidget->currentIndex() == 0)
        {
        QMessageBox::information( this, "Profil Okuma", "Profil okunmadı.");
    return(false);
        }
    }
    else
    {
         rProfile = file.readAll();
    }

    return rProfile;
}

void MainWindow::readProfileToStruct(QByteArray rProfile, int index){

     quint16 pos;
     QString name;
     QString partname;
     pos = 0;
     if (rProfile[pos] == '/')
     {
         ui->leSaveProfileName->setText("isimsiz");
     }
     else
     {
         while (rProfile[pos] != '/')
         {
             name.append(char(rProfile[pos]));
             pos++;
         }
         ui->leSaveProfileName->setText(name);
         tProfileLoad[index].name=name;
     }
     pos=pos+26;

     tProfileLoad[index].active = rProfile[pos];
     pos = pos + 2;

     tProfileLoad[index].startValue = qint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
     pos = pos + 3;
     tProfileLoad[index].sTarget = qint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) ;
     pos = pos + 3;


     tProfileLoad[index].totalStep = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));
     ui->sbStepCount->setValue(tProfileLoad[index].totalStep);



     pos = pos + 3;

     for(int i=0; i<tProfileLoad[index].totalStep; i++)
     {
         pos = pos + 7;

         tProfileLoad[index].step[i].stepUnit = rProfile[pos];

         pos = pos + 2;

         tProfileLoad[index].step[i].stepType = rProfile[pos];
         pos = pos + 2;

         tProfileLoad[index].step[i].lDuration = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));

         pos = pos + 3;

         tProfileLoad[index].step[i].lTarget = qint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;
         pos = pos + 3;

     }
     pos = pos + 22;

     pos = pos + 2;
     pos = pos + 3;
     pos = pos + 3;

     pos = pos + 23;
     pos = pos + 2;
     pos = pos + 3;
     pos = pos + 3;

     pos = pos + 22;

     nProfileLoad[index].active = rProfile[pos];
     pos = pos + 2;

     nProfileLoad[index].startValue = qint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;


     pos = pos + 3;

     nProfileLoad[index].totalStep = quint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff));
    // ui->spinBox->setValue(nProfileLoad[index - 1].totalStep);
     pos = pos + 3;
     /*
     if (rProfile[pos] == '/')
     {
         ui->leSaveParcaAdi->setText("isimsiz");
     }
     else
     {
         while (rProfile[pos] != '/')
         {
             partname.append(char(rProfile[pos]));
             pos++;
         }
         ui->leSaveParcaAdi->setText(partname);
        tProfileLoad[index].partname=partname;

     }*/



     for(int i=0; i<tProfileLoad[index].totalStep; i++)
     {
         pos = pos + 7;



         pos = pos + 2;


         pos = pos + 2;


         pos = pos + 3;

         nProfileLoad[index].step[i].lTarget = qint16(((rProfile[pos+1] & 0xff) << 8) | (rProfile[pos] & 0xff)) / 10.0;

         pos = pos + 3;/*

         if (rProfile[pos] == '/')
         {
             ui->leSaveParcaAdi->setText("isimsiz");
         }
         else
         {
             while (rProfile[pos] != '/')
             {
                 partname.append(char(rProfile[pos]));
                 pos++;
             }
             ui->leSaveParcaAdi->setText(partname);
            tProfileLoad[index].partname=partname;     }
*/

     }


}

void MainWindow::readProfileToTable(int index){

    if (  tProfileLoad[index].step[0].stepUnit  == 1)
        {
        ui->cbChooseTimeCoeff->setCurrentIndex(1);
        //nothing selected for neither step type nor step time unit. Going forward should be banned.


        }
        else if ( tProfileLoad[index].step[0].stepUnit  == 2 )
        {
       // tProfileSave[currentProfile].step[0].stepUnit  =1;
        //minute selected for step time unit, do what you gotta do.

        //   nProfileSave[currentProfile].active = 1;
        //   nProfileSave[currentProfile].step[currentNStep].stepUnit = 2;
        //     tProfileSave[currentProfile].startValue = ui->dsbTStartValue->value();

         ui->cbChooseTimeCoeff->setCurrentIndex(2);
        }
        else if ( tProfileLoad[index].step[0].stepUnit  == 0 )
        {

        ui->cbChooseTimeCoeff->setCurrentIndex(0);

        //hour selected for step time unit, do what you gotta do.

        //   nProfileSave[currentProfile].active = 1;
        //   nProfileSave[currentProfile].step[currentNStep].stepUnit = 3;
        //     tProfileSave[currentProfile].startValue = ui->dsbTStartValue->value();

       // ui->leTimTypeNew->setText("");
        }



    ui->leTime_1->setText(QString::number(tProfileLoad[index].step[0].lDuration,'g',3));
    ui->leTime_2->setText(QString::number(tProfileLoad[index].step[1].lDuration,'g',3));
    ui->leTime_3->setText(QString::number(tProfileLoad[index].step[2].lDuration,'g',3));
    ui->leTime_4->setText(QString::number(tProfileLoad[index].step[3].lDuration,'g',3));
    ui->leTime_5->setText(QString::number(tProfileLoad[index].step[4].lDuration,'g',3));
    ui->leTime_6->setText(QString::number(tProfileLoad[index].step[5].lDuration,'g',3));
    ui->leTime_7->setText(QString::number(tProfileLoad[index].step[6].lDuration,'g',3));
    ui->leTime_8->setText(QString::number(tProfileLoad[index].step[7].lDuration,'g',3));
    ui->leTime_9->setText(QString::number(tProfileLoad[index].step[8].lDuration,'g',3));
    ui->leTime_10->setText(QString::number(tProfileLoad[index].step[9].lDuration,'g',3));
    ui->leTime_11->setText(QString::number(tProfileLoad[index].step[10].lDuration,'g',3));
    ui->leTime_12->setText(QString::number(tProfileLoad[index].step[11].lDuration,'g',3));
    ui->leTime_13->setText(QString::number(tProfileLoad[index].step[12].lDuration,'g',3));
    ui->leTime_14->setText(QString::number(tProfileLoad[index].step[13].lDuration,'g',3));
    ui->leTime_15->setText(QString::number(tProfileLoad[index].step[14].lDuration,'g',3));


    ui->leTempInput_0->setText(QString::number(tProfileLoad[index].startValue,'g',3));
    ui->leTempInput_1->setText(QString::number(tProfileLoad[index].step[0].lTarget,'g',3));
    ui->leTempInput_2->setText(QString::number(tProfileLoad[index].step[1].lTarget,'g',3));
    ui->leTempInput_3->setText(QString::number(tProfileLoad[index].step[2].lTarget,'g',3));
    ui->leTempInput_4->setText(QString::number(tProfileLoad[index].step[3].lTarget,'g',3));
    ui->leTempInput_5->setText(QString::number(tProfileLoad[index].step[4].lTarget,'g',3));
    ui->leTempInput_6->setText(QString::number(tProfileLoad[index].step[5].lTarget,'g',3));
    ui->leTempInput_7->setText(QString::number(tProfileLoad[index].step[6].lTarget,'g',3));
    ui->leTempInput_8->setText(QString::number(tProfileLoad[index].step[7].lTarget,'g',3));
    ui->leTempInput_9->setText(QString::number(tProfileLoad[index].step[8].lTarget,'g',3));
    ui->leTempInput_10->setText(QString::number(tProfileLoad[index].step[9].lTarget,'g',3));
    ui->leTempInput_11->setText(QString::number(tProfileLoad[index].step[10].lTarget,'g',3));
    ui->leTempInput_12->setText(QString::number(tProfileLoad[index].step[11].lTarget,'g',3));
    ui->leTempInput_13->setText(QString::number(tProfileLoad[index].step[12].lTarget,'g',3));
    ui->leTempInput_14->setText(QString::number(tProfileLoad[index].step[13].lTarget,'g',3));
    ui->leTempInput_15->setText(QString::number(tProfileLoad[index].step[14].lTarget,'g',3));

    ui->leFixLiquitTempValue->setText(QString::number(tProfileLoad[index].sTarget,'g',3));

    ui->leHumidity_0->setText(QString::number(nProfileLoad[index].startValue,'g',3));
    ui->leHumidity_1->setText(QString::number(nProfileLoad[index].step[0].lTarget,'g',3));
    ui->leHumidity_2->setText(QString::number(nProfileLoad[index].step[1].lTarget,'g',3));
    ui->leHumidity_3->setText(QString::number(nProfileLoad[index].step[2].lTarget,'g',3));
    ui->leHumidity_4->setText(QString::number(nProfileLoad[index].step[3].lTarget,'g',3));
    ui->leHumidity_5->setText(QString::number(nProfileLoad[index].step[4].lTarget,'g',3));
    ui->leHumidity_6->setText(QString::number(nProfileLoad[index].step[5].lTarget,'g',3));
    ui->leHumidity_7->setText(QString::number(nProfileLoad[index].step[6].lTarget,'g',3));
    ui->leHumidity_8->setText(QString::number(nProfileLoad[index].step[7].lTarget,'g',3));
    ui->leHumidity_9->setText(QString::number(nProfileLoad[index].step[8].lTarget,'g',3));
    ui->leHumidity_10->setText(QString::number(nProfileLoad[index].step[9].lTarget,'g',3));
    ui->leHumidity_11->setText(QString::number(nProfileLoad[index].step[10].lTarget,'g',3));
    ui->leHumidity_12->setText(QString::number(nProfileLoad[index].step[11].lTarget,'g',3));
    ui->leHumidity_13->setText(QString::number(nProfileLoad[index].step[12].lTarget,'g',3));
    ui->leHumidity_14->setText(QString::number(nProfileLoad[index].step[13].lTarget,'g',3));
    ui->leHumidity_15->setText(QString::number(nProfileLoad[index].step[14].lTarget,'g',3));


/*
ui->leTempInput_0->setText(tProfileLoad[index].step[0].lTarget);
ui->leTempInput_1->setText(tProfileLoad[index].step[1].lTarget);
ui->leTempInput_2->setText(tProfileLoad[index].step[2].lTarget);
ui->leTempInput_3->setText(tProfileLoad[index].step[3].lTarget);
ui->leTempInput_4->setText(tProfileLoad[index].step[4].lTarget);
ui->leTempInput_5->setText(tProfileLoad[index].step[5].lTarget);
ui->leTempInput_6->setText(tProfileLoad[index].step[6].lTarget);
ui->leTempInput_7->setText(tProfileLoad[index].step[7].lTarget);
ui->leTempInput_8->setText(tProfileLoad[index].step[8].lTarget);
ui->leTempInput_9->setText(tProfileLoad[index].step[9].lTarget);
ui->leTempInput_10->setText(tProfileLoad[index].step[10].lTarget);
ui->leTempInput_11->setText(tProfileLoad[index].step[11].lTarget);
ui->leTempInput_12->setText(tProfileLoad[index].step[12].lTarget);
ui->leTempInput_13->setText(tProfileLoad[index].step[13].lTarget);
ui->leTempInput_14->setText(tProfileLoad[index].step[14].lTarget);
*/
}

void MainWindow::plotRecipeGraph(int index){
    int totalDuration=0;
    QVector<double> x(20), y(20), z(20);
    x[0]= 0;
    y[0] = 0;
    z[0] = 0;

    x[1]= 0;
    y[1] = tProfileLoad[index].startValue;
    z[1] = nProfileLoad[index].startValue;

    for(int i=0; i<tProfileLoad[index].totalStep; i++)
    {
        float tLDuration = tProfileLoad[index].step[i].lDuration;
        float tLTarget = tProfileLoad[index].step[i].lTarget;
        float nLTarget = nProfileLoad[index].step[i].lTarget;

        totalDuration = totalDuration + tLDuration;
        x[i+2] = totalDuration;

        y[i+2] = tLTarget;

        z[i+2] = nLTarget;


    }

    ui->tPreview_2->graph(0)->setData(x,y);
    ui->tPreview_2->graph(1)->setData(x,z);
    ui->tPreview_2->xAxis->setRange(0, totalDuration);

    ui->tPreview_2->yAxis->setRange(-40, 180);
    ui->tPreview_2->graph(0)->setPen(QPen(Qt::red));
    ui->tPreview_2->graph(1)->setPen(QPen(Qt::blue));



    ui->tPreview_2->replot();
}

void MainWindow::on_bResetFaultBuzzer_clicked()
{   if(myPLC.buzzeracik)
    {
    QByteArray cantTouchThis;
    cantTouchThis.clear();
    cantTouchThis.append(0x01);
    proc->insertCommandMessage(mySerial::makeMessage(0x89,cantTouchThis));
    }
    else
    {
        writeToLogTable("Siren aktif deyil yada beklemede");
    }

}

void MainWindow::on_bClearProfromTabl_clicked()
{


    //recete ismi temizleme
    ui->leSaveProfileName->setText("Bos Recete");
    ui->cbChooseProfileSlot->setItemText(currentProfile,ui->leSaveProfileName->text());
    ui->cbChooseProfileSlot->setItemData(currentProfile, QColor( Qt::black ), Qt::TextColorRole );
    //profil tipini temizleme
    ui->cbChooseProfileTypeNew->setCurrentIndex(0);
    //adım sayısı temizleme
    ui->sbStepCount->setValue(0);
    //zaman birimi temzileme
    ui->cbChooseTimeCoeff->setCurrentIndex(0);
    //sıvı set sıcaklıgı temizleme


    //zamanın ıcını sıfırlama
    ui->leTime_1->setText("0");
    ui->leTime_2->setText("0");
    ui->leTime_3->setText("0");
    ui->leTime_4->setText("0");
    ui->leTime_5->setText("0");
    ui->leTime_6->setText("0");
    ui->leTime_7->setText("0");
    ui->leTime_8->setText("0");
    ui->leTime_9->setText("0");
    ui->leTime_10->setText("0");
    ui->leTime_11->setText("0");
    ui->leTime_12->setText("0");
    ui->leTime_13->setText("0");
    ui->leTime_14->setText("0");
    ui->leTime_15->setText("0");

    //sıcaklıgın ıcını sıfırlama
    ui->leTempInput_0->setText("0");
    ui->leTempInput_1->setText("0");
    ui->leTempInput_2->setText("0");
    ui->leTempInput_3->setText("0");
    ui->leTempInput_4->setText("0");
    ui->leTempInput_5->setText("0");
    ui->leTempInput_6->setText("0");
    ui->leTempInput_7->setText("0");
    ui->leTempInput_8->setText("0");
    ui->leTempInput_9->setText("0");
    ui->leTempInput_10->setText("0");
    ui->leTempInput_11->setText("0");
    ui->leTempInput_12->setText("0");
    ui->leTempInput_13->setText("0");
    ui->leTempInput_14->setText("0");
    ui->leTempInput_15->setText("0");


    //nemin ıcını sıfırlama
    ui->leHumidity_0->setText("0");
    ui->leHumidity_1->setText("0");
    ui->leHumidity_2->setText("0");
    ui->leHumidity_3->setText("0");
    ui->leHumidity_4->setText("0");
    ui->leHumidity_5->setText("0");
    ui->leHumidity_6->setText("0");
    ui->leHumidity_7->setText("0");
    ui->leHumidity_8->setText("0");
    ui->leHumidity_9->setText("0");
    ui->leHumidity_10->setText("0");
    ui->leHumidity_11->setText("0");
    ui->leHumidity_12->setText("0");
    ui->leHumidity_13->setText("0");
    ui->leHumidity_14->setText("0");
    ui->leHumidity_15->setText("0");


}

void MainWindow::labelEnable(int index){
    if(index==1){
    ui->leTime_1->setEnabled(false);
    ui->leTime_2->setEnabled(false);
    ui->leTime_3->setEnabled(false);
    ui->leTime_4->setEnabled(false);
    ui->leTime_5->setEnabled(false);
    ui->leTime_6->setEnabled(false);
    ui->leTime_7->setEnabled(false);
    ui->leTime_8->setEnabled(false);
    ui->leTime_9->setEnabled(false);
    ui->leTime_10->setEnabled(false);
    ui->leTime_11->setEnabled(false);
    ui->leTime_12->setEnabled(false);
    ui->leTime_13->setEnabled(false);
    ui->leTime_14->setEnabled(false);
    ui->leTime_15->setEnabled(false);

    ui->leTempInput_0->setEnabled(false);
    ui->leTempInput_1->setEnabled(false);
    ui->leTempInput_2->setEnabled(false);
    ui->leTempInput_3->setEnabled(false);
    ui->leTempInput_4->setEnabled(false);
    ui->leTempInput_5->setEnabled(false);
    ui->leTempInput_6->setEnabled(false);
    ui->leTempInput_7->setEnabled(false);
    ui->leTempInput_8->setEnabled(false);
    ui->leTempInput_9->setEnabled(false);
    ui->leTempInput_10->setEnabled(false);
    ui->leTempInput_11->setEnabled(false);
    ui->leTempInput_12->setEnabled(false);
    ui->leTempInput_13->setEnabled(false);
    ui->leTempInput_14->setEnabled(false);
    ui->leTempInput_15->setEnabled(false);

    ui->leHumidity_0->setEnabled(false);
    ui->leHumidity_1->setEnabled(false);
    ui->leHumidity_2->setEnabled(false);
    ui->leHumidity_3->setEnabled(false);
    ui->leHumidity_4->setEnabled(false);
    ui->leHumidity_5->setEnabled(false);
    ui->leHumidity_6->setEnabled(false);
    ui->leHumidity_7->setEnabled(false);
    ui->leHumidity_8->setEnabled(false);
    ui->leHumidity_9->setEnabled(false);
    ui->leHumidity_10->setEnabled(false);
    ui->leHumidity_11->setEnabled(false);
    ui->leHumidity_12->setEnabled(false);
    ui->leHumidity_13->setEnabled(false);
    ui->leHumidity_14->setEnabled(false);
    ui->leHumidity_15->setEnabled(false);

    ui->leSaveProfileName->setEnabled(false);
    ui->leSaveParcaAdi->setEnabled(false);
    ui->sbStepCount->setEnabled(false);
    ui->cbChooseProfileTypeNew->setEnabled(false);
    ui->cbChooseTimeCoeff->setEnabled(false);
    ui->bSaveProfromTabl->setEnabled(false);
    ui->bClearProfromTabl->setEnabled(false);
    ui->leFixLiquitTempValue->setEnabled(false);



}
    else if(index==0) {
        ui->leTime_1->setEnabled(true);
        ui->leTime_2->setEnabled(true);
        ui->leTime_3->setEnabled(true);
        ui->leTime_4->setEnabled(true);
        ui->leTime_5->setEnabled(true);
        ui->leTime_6->setEnabled(true);
        ui->leTime_7->setEnabled(true);
        ui->leTime_8->setEnabled(true);
        ui->leTime_9->setEnabled(true);
        ui->leTime_10->setEnabled(true);
        ui->leTime_11->setEnabled(true);
        ui->leTime_12->setEnabled(true);
        ui->leTime_13->setEnabled(true);
        ui->leTime_14->setEnabled(true);
        ui->leTime_15->setEnabled(true);

        ui->leTempInput_0->setEnabled(true);
        ui->leTempInput_1->setEnabled(true);
        ui->leTempInput_2->setEnabled(true);
        ui->leTempInput_3->setEnabled(true);
        ui->leTempInput_4->setEnabled(true);
        ui->leTempInput_5->setEnabled(true);
        ui->leTempInput_6->setEnabled(true);
        ui->leTempInput_7->setEnabled(true);
        ui->leTempInput_8->setEnabled(true);
        ui->leTempInput_9->setEnabled(true);
        ui->leTempInput_10->setEnabled(true);
        ui->leTempInput_11->setEnabled(true);
        ui->leTempInput_12->setEnabled(true);
        ui->leTempInput_13->setEnabled(true);
        ui->leTempInput_14->setEnabled(true);
        ui->leTempInput_15->setEnabled(true);

        ui->leHumidity_0->setEnabled(true);
        ui->leHumidity_1->setEnabled(true);
        ui->leHumidity_2->setEnabled(true);
        ui->leHumidity_3->setEnabled(true);
        ui->leHumidity_4->setEnabled(true);
        ui->leHumidity_5->setEnabled(true);
        ui->leHumidity_6->setEnabled(true);
        ui->leHumidity_7->setEnabled(true);
        ui->leHumidity_8->setEnabled(true);
        ui->leHumidity_9->setEnabled(true);
        ui->leHumidity_10->setEnabled(true);
        ui->leHumidity_11->setEnabled(true);
        ui->leHumidity_12->setEnabled(true);
        ui->leHumidity_13->setEnabled(true);
        ui->leHumidity_14->setEnabled(true);
        ui->leHumidity_15->setEnabled(true);

        ui->bClearProfromTabl->setEnabled(true);
        ui->leSaveProfileName->setEnabled(true);
        ui->leSaveParcaAdi->setEnabled(true);
        ui->sbStepCount->setEnabled(true);
        ui->bSaveProfromTabl->setEnabled(true);
        ui->cbChooseProfileTypeNew->setEnabled(true);
        ui-> cbChooseTimeCoeff->setEnabled(true);
        ui->leFixLiquitTempValue->setEnabled(true);


    }
}

void MainWindow::on_bEditTablo_clicked()
{
    labelEnable(0);

}

void MainWindow::on_chbHumidityDevice_clicked()
{
    QByteArray cantTouchThis;
    cantTouchThis.clear();

    if (ui->chbHumidityDevice->isChecked())
    {
        cantTouchThis.append(char(0x01));
    }
    else
    {
        cantTouchThis.append(char(0x00));
    }

     proc->insertCommandMessage(mySerial::makeMessage(0x9C,cantTouchThis));
}

void MainWindow::on_btnPatlatmaIstasyon_clicked()
{
    ui->detailsPages->setCurrentIndex(0);
    ui->detailsBottomPages->setCurrentIndex(8);
}

void MainWindow::on_bMinimize_4_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_bMinimize_5_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_bKanal1_clicked()
{
            float ymax=0;


       QVector<double> x1(180000), y1(180000);
            #ifdef Q_OS_WIN
                    // windows code goes here
                    QString filePath =  "records\\" + testFolder + "\\" + "pipe1.csv";
            #endif


    QFile MyFile(filePath);
     MyFile.open(QIODevice::ReadOnly);
     QTextStream in (&MyFile);
             int rowcount = 0;
    while (!in.atEnd())
    {
        auto line = in.readLine();
        auto values =line.split(";");
        x1[rowcount] = values[2].toDouble();
        y1[rowcount]= values[3].toDouble();
        if(ymax<y1[rowcount]){

            ymax=y1[rowcount];

       }
        rowcount++;

    }
    MyFile.close();

    ui->olcum_max->setText(QString::number(ymax));
    ui->pTestGraph->graph(0)->setData(x1, y1);

            /*QTextStream xin (&file);
                        int rowcount = 0;
                        while (!xin.atEnd()) {
                            auto line =xin.readLine();
                            auto values =line.split(",");
                            x[rowcount] = values[0].toDouble();
                            y[rowcount]= values[1].toDouble();
                            rowcount++;*/




    ui->pTestGraph->graph(0)->setVisible(true);
    ui->pTestGraph->graph(1)->setVisible(false);
    ui->pTestGraph->graph(2)->setVisible(false);
    ui->pTestGraph->graph(3)->setVisible(false);
    ui->pTestGraph->graph(4)->setVisible(false);
    ui->pTestGraph->graph(5)->setVisible(false);


}

void MainWindow::on_bKanal2_clicked()
{
    denemeTestFolder="deneme";

 QVector<double> x2(180000), y2(180000);
      #ifdef Q_OS_WIN
              // windows code goes here
              QString filePath =  "records\\" + testFolder + "\\" + "pipe2.csv";
      #endif


QFile MyFile(filePath);
MyFile.open(QIODevice::ReadOnly);
QTextStream in (&MyFile);
       int rowcount = 0;
while (!in.atEnd())
{
  auto line = in.readLine();
  auto values =line.split(";");
  x2[rowcount] = values[2].toDouble();
  y2[rowcount]= values[3].toDouble();
  rowcount++;

}
MyFile.close();
ui->pTestGraph->graph(1)->setData(x2, y2);

      /*QTextStream xin (&file);
                  int rowcount = 0;
                  while (!xin.atEnd()) {
                      auto line =xin.readLine();
                      auto values =line.split(",");
                      x[rowcount] = values[0].toDouble();
                      y[rowcount]= values[1].toDouble();
                      rowcount++;*/
        ui->pTestGraph->graph(0)->setVisible(false);
        ui->pTestGraph->graph(1)->setVisible(true);
        ui->pTestGraph->graph(2)->setVisible(false);
        ui->pTestGraph->graph(3)->setVisible(false);
        ui->pTestGraph->graph(4)->setVisible(false);
        ui->pTestGraph->graph(5)->setVisible(false);
}

void MainWindow::on_bKanal3_clicked()
{

            QVector<double> x3(180000), y3(180000);
                 #ifdef Q_OS_WIN
                         // windows code goes here
                         QString filePath =  "records\\" + testFolder + "\\" + "pipe3.csv";
                 #endif


         QFile MyFile(filePath);
          MyFile.open(QIODevice::ReadOnly);
          QTextStream in (&MyFile);
                  int rowcount = 0;
         while (!in.atEnd())
         {
             auto line = in.readLine();
             auto values =line.split(";");
             x3[rowcount] = values[2].toDouble();
             y3[rowcount]= values[3].toDouble();
             rowcount++;

         }
         MyFile.close();
         ui->pTestGraph->graph(2)->setData(x3, y3);

                 /*QTextStream xin (&file);
                             int rowcount = 0;
                             while (!xin.atEnd()) {
                                 auto line =xin.readLine();
                                 auto values =line.split(",");
                                 x[rowcount] = values[0].toDouble();
                                 y[rowcount]= values[1].toDouble();
                                 rowcount++;*/
    ui->pTestGraph->graph(0)->setVisible(false);
     ui->pTestGraph->graph(1)->setVisible(false);
     ui->pTestGraph->graph(2)->setVisible(true);
     ui->pTestGraph->graph(3)->setVisible(false);
     ui->pTestGraph->graph(4)->setVisible(false);
     ui->pTestGraph->graph(5)->setVisible(false);
}

void MainWindow::on_bKanal4_clicked()
{           denemeTestFolder="deneme";

            QVector<double> x4(180000), y4(180000);
                 #ifdef Q_OS_WIN
                         // windows code goes here
                         QString filePath =  "records\\" + testFolder + "\\" + "pipe4.csv";
                 #endif


         QFile MyFile(filePath);
          MyFile.open(QIODevice::ReadOnly);
          QTextStream in (&MyFile);
                  int rowcount = 0;
         while (!in.atEnd())
         {
             auto line = in.readLine();
             auto values =line.split(";");
             x4[rowcount] = values[2].toDouble();
             y4[rowcount]= values[3].toDouble();
             rowcount++;

         }
         MyFile.close();
         ui->pTestGraph->graph(3)->setData(x4, y4);

                 /*QTextStream xin (&file);
                             int rowcount = 0;
                             while (!xin.atEnd()) {
                                 auto line =xin.readLine();
                                 auto values =line.split(",");
                                 x[rowcount] = values[0].toDouble();
                                 y[rowcount]= values[1].toDouble();
                                 rowcount++;*/
    ui->pTestGraph->graph(0)->setVisible(false);
     ui->pTestGraph->graph(1)->setVisible(false);
     ui->pTestGraph->graph(2)->setVisible(false);
     ui->pTestGraph->graph(3)->setVisible(true);
     ui->pTestGraph->graph(4)->setVisible(false);
     ui->pTestGraph->graph(5)->setVisible(false);
}

void MainWindow::on_bKanal5_clicked()
{

            QVector<double> x5(180000), y5(180000),z5(180000),d5(180000);
                 #ifdef Q_OS_WIN
                         // windows code goes here
                         QString filePath =  "records\\" + testFolder + "\\" + "pipe5.csv";
                 #endif


         QFile MyFile(filePath);
          MyFile.open(QIODevice::ReadOnly);
          QTextStream in (&MyFile);
                  int rowcount = 0;
         while (!in.atEnd())
         {
             auto line = in.readLine();
             auto values =line.split(";");

             x5[rowcount] = values[2].toDouble();
             y5[rowcount]= values[3].toDouble();
             rowcount++;

         }
         MyFile.close();
         ui->pTestGraph->graph(4)->setData(x5, y5);

                 /*QTextStream xin (&file);
                             int rowcount = 0;
                             while (!xin.atEnd()) {
                                 auto line =xin.readLine();
                                 auto values =line.split(";");
                                 x[rowcount] = values[0].toDouble();
                                 y[rowcount]= values[1].toDouble();
                                 rowcount++;*/
    ui->pTestGraph->graph(0)->setVisible(false);
     ui->pTestGraph->graph(1)->setVisible(false);
     ui->pTestGraph->graph(2)->setVisible(false);
     ui->pTestGraph->graph(3)->setVisible(false);
     ui->pTestGraph->graph(4)->setVisible(true);
     ui->pTestGraph->graph(5)->setVisible(false);
}

void MainWindow::on_bKanal5_2_clicked()
{
     on_bKanal1_clicked();
     on_bKanal2_clicked();
     on_bKanal3_clicked();
     on_bKanal4_clicked();
     on_bKanal5_clicked();

    ui->pTestGraph->graph(0)->setVisible(true);
     ui->pTestGraph->graph(1)->setVisible(true);
     ui->pTestGraph->graph(2)->setVisible(true);
     ui->pTestGraph->graph(3)->setVisible(true);
     ui->pTestGraph->graph(4)->setVisible(true);
     ui->pTestGraph->graph(5)->setVisible(false);
}





void MainWindow::on_ZoomCenter_2_clicked()
{
    ui->pTestGraph->xAxis->setRange(pKey, 60, Qt::AlignRight);
    ui->pTestGraph->yAxis->setRange(0,200);
    ui->pTestGraph->yAxis->range().bounded(0,20);
    ui->pTestGraph->replot();

}

void MainWindow::on_ZoomInVer_2_clicked()
{
    ui->pTestGraph->yAxis->scaleRange(.85, ui->pTestGraph->yAxis->range().center());
    ui->pTestGraph->yAxis->setRangeLower(0);
    ui->pTestGraph->replot();
}

void MainWindow::on_ZoomOutVer_2_clicked()
{
    ui->pTestGraph->yAxis->scaleRange(1.15, ui->pTestGraph->yAxis->range().center());
    ui->pTestGraph->yAxis->setRangeLower(0);
    ui->pTestGraph->replot();
    if (ui->pTestGraph->yAxis->range().upper > 200 || ui->pTestGraph->yAxis->range().upper < 0 )
    {
         ui->pTestGraph->yAxis->setRange(0,200);

         ui->pTestGraph->replot();
    }
}

void MainWindow::on_ZoomOutHor_2_clicked()
{
    ui->pTestGraph->xAxis->scaleRange(1.15, ui->pTestGraph->xAxis->range().center());
    ui->pTestGraph->replot();
}

void MainWindow::on_ZoomInHor_2_clicked()
{
    ui->pTestGraph->xAxis->scaleRange(.85, ui->pTestGraph->xAxis->range().center());
    ui->pTestGraph->replot();
}


void MainWindow::on_cBKanal1_clicked(bool checked)
{
         if(checked){
              activeChannel |= 1 << 0;
         ui->lePartName1_1->setVisible(true);
         ui->label_135->setVisible(true);

         }

    else{

            activeChannel &= ~1;
            ui->lePartName1_1->setVisible(false);
            ui->label_135->setVisible(false);
         }

}

void MainWindow::on_cBKanal2_clicked(bool checked)
{
    if(checked){
         activeChannel |= 1 << 1;
    ui->lePartName1_2->setVisible(true);
    ui->label_136->setVisible(true);

    }

else{

       activeChannel &= ~2;
       ui->lePartName1_2->setVisible(false);
       ui->label_136->setVisible(false);
    }

}

void MainWindow::on_cBKanal3_clicked(bool checked)
{
    if(checked){
         activeChannel |= 1 << 2;
    ui->lePartName1_3->setVisible(true);
    ui->label_137->setVisible(true);

    }

else{

       activeChannel &= ~4;
       ui->lePartName1_3->setVisible(false);
       ui->label_137->setVisible(false);
    }

}

void MainWindow::on_cBKanal4_clicked(bool checked)
{if(checked){
        activeChannel |= 1 << 3;
   ui->lePartName1_4->setVisible(true);
   ui->label_138->setVisible(true);

   }

else{

      activeChannel &= ~8;
      ui->lePartName1_4->setVisible(false);
      ui->label_138->setVisible(false);
   }


}

void MainWindow::on_cBKanal5_clicked(bool checked)
{
    if(checked){
            activeChannel |= 1 << 4;
       ui->lePartName1_5->setVisible(true);
       ui->label_139->setVisible(true);

       }

    else{

          activeChannel &= ~16;
          ui->lePartName1_5->setVisible(false);
          ui->label_139->setVisible(false);
       }
}

void MainWindow::on_cBKanal1_2_clicked(bool checked)
{
    if(checked){
         activeChannel1 |= 1 << 0;
    ui->lePartName2_1->setVisible(true);
    ui->label_144->setVisible(true);

    }

else{

       activeChannel1 &= ~1;
       ui->lePartName2_1->setVisible(false);
       ui->label_144->setVisible(false);
    }

}

void MainWindow::on_cBKanal2_2_clicked(bool checked)
{
    if(checked){
         activeChannel1 |= 1 << 1;
    ui->lePartName2_2->setVisible(true);
    ui->label_143->setVisible(true);

    }

else{

       activeChannel1 &= ~2;
       ui->lePartName2_2->setVisible(false);
       ui->label_143->setVisible(false);
    }

}

void MainWindow::on_cBKanal3_2_clicked(bool checked)
{
    if(checked){
         activeChannel1 |= 1 << 2;
    ui->lePartName2_3->setVisible(true);
    ui->label_149->setVisible(true);

    }

else{

       activeChannel1 &= ~4;
       ui->lePartName2_3->setVisible(false);
       ui->label_149->setVisible(false);
    }
}

void MainWindow::on_cBKanal4_2_clicked(bool checked)
{
    if(checked){
            activeChannel1 |= 1 << 3;
       ui->lePartName2_4->setVisible(true);
       ui->label_141->setVisible(true);

       }

    else{

          activeChannel1 &= ~8;
          ui->lePartName2_4->setVisible(false);
          ui->label_141->setVisible(false);
       }
}

void MainWindow::on_cBKanal5_2_clicked(bool checked)
{ if(checked){
        activeChannel1 |= 1 << 4;
   ui->lePartName2_5->setVisible(true);
   ui->label_148->setVisible(true);

   }

else{

      activeChannel1 &= ~16;
      ui->lePartName2_5->setVisible(false);
      ui->label_148->setVisible(false);
   }


}





void MainWindow::on_bPatlatYesNo_clicked(bool checked)
{
    if(checked){

       ui->leFixPressureVelocity_2->setVisible(true);
       ui->label_151->setVisible(true);
       ui->label_152->setVisible(true);
       ui->bPatlatYesNo->setStyleSheet("background-color: green");
       ui->bSendProfileMain->setEnabled(false);



       QMessageBox::information(
                   this,
                   tr(appName),
                   tr("Lütfen basınçlandırma hızını giriniz.") );}



    else{


          ui->leFixPressureVelocity_2->setVisible(false);
          ui->label_151->setVisible(false);
           ui->label_152->setVisible(false);
           ui->bPatlatYesNo->setStyleSheet("background-color: black");
       }

}


bool MainWindow::on_bLiquidCoolerControl_clicked(bool checked)
{
    if(checked){

       ui->bLiquidCoolerControl->setStyleSheet("background-color: blue");
       return true;
       }

    else{



           ui->bLiquidCoolerControl->setStyleSheet("background-color: black");
           return false;
       }
}

void MainWindow::on_leFixPressureVelocity_2_textChanged(const QString &arg1)
{
    if(ui->leFixPressureVelocity_2->text().toDouble() < 1 || ui->leFixPressureVelocity_2->text().toDouble() >= 101 )
    {
        ui->bSendProfileMain->setEnabled(false);
        QMessageBox::information(
                    this,
                    tr(appName),
                    tr("Lütfen basınçlandırma hızını 1-100 arasında bir deger giriniz.") );


}else {
 ui->bSendProfileMain->setEnabled(true);}
}

void MainWindow::on_cbChooseTimeCoeff_currentIndexChanged(int index)
{
    if(index==0){
        ui->leChangetime->setText("");
    }
    else if (index==1) {
        ui->leChangetime->setText("(sa.)");

    }
    else if (index==2) {
        ui->leChangetime->setText("(dk.)");

    }
    else if (index==3) {
        ui->leChangetime->setText("(sn.)");

    }
}
