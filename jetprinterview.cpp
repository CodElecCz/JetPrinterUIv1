#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QSignalMapper>
#include <QFileDialog>
#include <QMessageBox>
#include <QFontDatabase>
#include <QTextStream>
#include <QDebug>
#include "JetPrinterLib/JetPrinterException.h"
#include "mainwindow.h"
#include "jetprinterview.h"
#include "ui_jetprinterview.h"

#define JetPrinterStateSize 16

static const char* const JetPrinterNozzleState[JetPrinterStateSize] =
{
    "Tryska: -",
    "Tryska: Otevírá se",
    "Tryska: Otevřena",
    "Tryska: Zavírá se",
    "Tryska: Zavřena",
    "Tryska: Mezipoloha"
};

static const char* const JetPrinterMachineState[JetPrinterStateSize] =
{
    "",
    "Tiskárna: Vypnuta",
    "Tiskárna: Inicializace",
    "Tiskárna: Servisní panel",
    "Tiskárna: Připravena",             //Tryska: Zavřena
    "Tiskárna: Připravna k tisku",      //Tryska: Otevřena; Tisk Stop
    "Tiskárna: Tiskne"                  //Tryska: Otevřena; Tisk Start
};

static const char* const JetPrinterHeadState[JetPrinterStateSize] =
{
    "Kryt: Uzavřen",
    "Kryt: Otevřen"
};


JetPrinterView::JetPrinterView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JetPrinterView),
    jetPrinter(nullptr)
{
    ui->setupUi(this);
    this->readSettings();

    initialize();
    labelList();
}

JetPrinterView::~JetPrinterView()
{
    delete ui;
}

void JetPrinterView::initialize()
{
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->scale(3,3); //zoom

    int id1 = QFontDatabase::addApplicationFont(":/fonts/Code 5x5.ttf");
    int id2 = QFontDatabase::addApplicationFont(":/fonts/Code 7x5.ttf");

    family5x5 = QFontDatabase::applicationFontFamilies(id1).at(0);
    family7x5 = QFontDatabase::applicationFontFamilies(id2).at(0);

    buttons = new QButtonGroup(this);
    buttons->addButton(ui->pushButtonPwr, 0);
    buttons->addButton(ui->pushButtonPrintStart, 1);
    buttons->addButton(ui->pushButtonPrintStop, 2);
    buttons->addButton(ui->pushButtonNozzleOpen, 3);
    buttons->addButton(ui->pushButtonNozzleClose, 4);
    buttons->addButton(ui->pushButtonScript1, 5);
    buttons->addButton(ui->pushButtonScript2, 6);

    connect(buttons, SIGNAL(buttonClicked(int)), this, SLOT(printerCmd(int)));
}

bool JetPrinterView::initializeSer(QString ser, int baudRate)
{
    Serial::BaudRate serBaudRate = Serial::BR_115200;
    bool status = false;

    switch(baudRate)
    {
        case 9600:
            serBaudRate = Serial::BR_9600;
            break;
        case 57600:
            serBaudRate = Serial::BR_57600;
            break;
        default:
            serBaudRate = Serial::BR_115200;
            break;
    }

    try
    {
        UINT nozle, machine, error, headcover, speed;
        jetPrinter = new JetPrinterProtocol(ser.toLatin1().data(), serBaudRate);
        jetPrinter->QueryStatus(&nozle, &machine, &error, &headcover, &speed);

        status = true;
    }
    catch(JetPrinterException& e)
    {
        QMessageBox::warning(this, "Warning", QString(e.what()));
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(printeInfo()));
    timer->start(1000);

    return status;
}

void JetPrinterView::printerCmd(int index)
{
    if(jetPrinter!=nullptr)
    {
        try
        {
            switch(index)
            {
            case 0: //pushButtonPwr
                if(printerStatus==1)
                    jetPrinter->PowerOn();
                else
                    jetPrinter->PowerOff();
                break;
            case 1: //pushButtonPrintStart
                jetPrinter->StartPrintReady();
                break;
            case 2: //pushButtonPrintStop
                jetPrinter->StopPrintReady();
                break;
            case 3: //pushButtonNozzleOpen
                jetPrinter->NozzleOpen();
                break;
            case 4: //pushButtonNozzleClose
                jetPrinter->NozzleClose();
                break;
            case 5: //pushButtonScript1
                if(ui->listWidget->currentRow()>=0)
                {
                    QString jobPath = "\\FFSDISK\\JOBS\\" + ui->listWidget->currentItem()->text();
                    jetPrinter->LoadJob(jobPath.toLatin1().data());

                    refreshLoadedJob = true;
                }
                break;
            case 6: //pushButtonScript2
				if (ui->listWidget->currentRow() >= 0)
				{
					QString script = "";
					scriptRead(ui->listWidget->currentItem()->text(), &script);
					jetPrinter->ScriptProcessing(script.toLatin1().data());

                    refreshLoadedJob = true;
				}
                break;
            }
        }
        catch(JetPrinterException& e)
        {
            QMessageBox::warning(this, "Warning", QString(e.what()));
        }
    }
}

void JetPrinterView::printeInfo()
{
    static clock_t t_1 = clock();
    static clock_t t_2 = clock();
    static int boot = 0;

    UINT nozzle=0, machine=0, error=0, headcover=0, speed=0;
    UINT productCnt = 0, stopCnt = 0, totalCnt = 0;

    try
    {
        jetPrinter->QueryStatus(&nozzle, &machine, &error, &headcover, &speed);
        nozzleStatus = nozzle;
        printerStatus = machine;

        if(machine==1) //standby
        {
            QPalette palette;// = ui->labelPwrStatus->palette();
            palette.setColor(QPalette::Window, Qt::red);
            ui->labelPwrStatus->setPalette(palette);
        }
        else
        {
            QPalette palette;// = ui->labelPwrStatus->palette();
            palette.setColor(QPalette::Window, Qt::green);
            ui->labelPwrStatus->setPalette(palette);
        }

        if(machine<JetPrinterStateSize)
        {
            ui->labelInfo1->setText(JetPrinterMachineState[machine]);

            QStringList stateList = QString(JetPrinterMachineState[machine]).split(": ");

            if(stateList.length()>0)
            {
                emit printerState(stateList.at(1));
            }
        }
        if(nozzle<JetPrinterStateSize)
            ui->labelInfo2->setText(JetPrinterNozzleState[nozzle]);
        if(headcover<JetPrinterStateSize)
            ui->labelInfo3->setText(JetPrinterHeadState[headcover]);

        if(error==0)
            ui->labelInfo4->setText("Chyba: -");
        else
            ui->labelInfo4->setText("Chyba: " + QString::number(error));

        ui->lineEditSpeed->setText(QString::number((double)speed/10.0, '.', 1));

        if(machine>1)
        {
            //Counters
            if((clock() - t_1)>5000)
            {
                jetPrinter->QueryCounter(&productCnt, &stopCnt, &totalCnt);

                ui->lcdNumber1->display(QString::number(productCnt));
                ui->lcdNumber2->display(QString::number(totalCnt));

                t_1 = clock();
            }

            //job
            if((clock() - t_2)>10000 || refreshLoadedJob)
            {
                char job[256] = "";
                jetPrinter->QueryLoadedJob(job, sizeof(job)-1);
                ui->lineEditJob->setText(job);

                t_2 = clock();
                refreshLoadedJob = false;
            }
        }
        else
        {
            t_1 = clock();
            t_2 = clock();
        }

        if(machine>3)
        {
            if(!ui->pushButtonPrintStart->isEnabled())
            {
                ui->pushButtonNozzleOpen->setEnabled(true);
                ui->pushButtonNozzleClose->setEnabled(true);

                ui->pushButtonScript1->setEnabled(true);
                ui->pushButtonScript2->setEnabled(true);
            }
        }
        else
        {
            ui->pushButtonNozzleOpen->setEnabled(false);
            ui->pushButtonNozzleClose->setEnabled(false);

            ui->pushButtonScript1->setEnabled(false);
            ui->pushButtonScript2->setEnabled(false);
        }

        if(nozzle==2)
        {
            if(!ui->pushButtonPrintStart->isEnabled())
            {
                ui->pushButtonPrintStart->setEnabled(true);
                ui->pushButtonPrintStop->setEnabled(true);
            }
        }
        else
        {
            ui->pushButtonPrintStart->setEnabled(false);
            ui->pushButtonPrintStop->setEnabled(false);
        }

        //boot sequence
        switch(boot)
        {
            case 0: //stanby
                if(machine==1)
                {
                    jetPrinter->PowerOn();
                }
                boot++;
                break;
            case 1: //ready
                if(machine==4)
                {
                    jetPrinter->NozzleOpen();
                    boot++;
                }
                else if(machine>4)
                    boot++;

                break;
            case 2: //ready to print
                if(machine==5)
                {
                    jetPrinter->StartPrintReady();
                    boot++;
                }
                break;
            case 3: //print
                ;
                break;
        }

        emit statusPrinter("Tiskárna: připojeno");
    }
    catch(JetPrinterException& e)
    {
        qDebug()<<e.what();

        emit statusPrinter("Tiskárna: chyba");
    }
}

void JetPrinterView::jobList(QStringList* jobList)
{
    for(int i=0; i<ui->listWidget->count(); i++)
    {
        QListWidgetItem *item = ui->listWidget->item(i);

        jobList->append(item->text());
    }
}

void JetPrinterView::labelList()
{
    this->jobsFilePath = QCoreApplication::applicationDirPath() + "/LJ3/Jobs/";
    QDir dir(jobsFilePath);

    if(!dir.exists())
    {
        QString msg = "Printer files not found: " + jobsFilePath;
        QMessageBox::question(this, "Warning", msg, QMessageBox::Ok);
    }

    QStringList jobsFile = dir.entryList(QStringList() << "*.job", QDir::Files);
    foreach(QString jobFile, jobsFile)
    {
        QListWidgetItem *item = new QListWidgetItem(QIcon(), jobFile);
        ui->listWidget->addItem(item);
    }

    connect(ui->listWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(labelActivated(QString)));

    //select first item form list
    if(ui->listWidget->count()>0)
    {
        QListWidgetItem *item = ui->listWidget->item(0);
        ui->listWidget->setItemSelected(item, true);
        labelActivated(item->text());
    }
}

void JetPrinterView::scriptRead(QString jobFileName, QString* script)
{
    QFile jobFile(this->jobsFilePath + jobFileName);
    if(!jobFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "error", jobFile.errorString());
    }

    QTextStream jobFileText(&jobFile);
    while(!jobFileText.atEnd())
    {
        QString line = jobFileText.read(4096);

        script->append(line);
    }

    jobFile.close();
}

void JetPrinterView::labelActivated(QString jobFileName)
{
    labelPreviewDemo();

    QFile jobFile(this->jobsFilePath + jobFileName);
    if(!jobFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "error", jobFile.errorString());
    }

    QTextStream jobFileText(&jobFile);
    QStringList script;
    while(!jobFileText.atEnd())
    {
        QString line = jobFileText.readLine();

        script.append(line);
    }

    jobFile.close();

    labelPreview(script);
}

void JetPrinterView::labelPreview(QStringList script)
{
    const int y_offset = 50;
    QFont code5x5(family5x5);
    QFont code7x5(family7x5);

    code7x5.setStretch(100);
    code7x5.setPixelSize(11);

    code5x5.setStretch(100);
    code5x5.setPixelSize(8);

    //label size
    ui->graphicsView->setSceneRect(0,0,200,65);
    scene->clear();
    scene->addRect(0,0,200,65);

    QStringList split;
    QString slineText;
    QString scmd;
    QString sdata;
    QString stext;
    QString sfont;
    for(int i = 0; i<script.count(); i++)
    {
        slineText = script.at(i);

        //OBJ [4 0 3 0 (ISO1_5X5)  (CZECH REPUBLIC) 1 0 0 0 0 1 0 0 0 0 0 0 ()  () 0 0 () ]
        split = slineText.split(" [");

        scmd = split.at(0);
        sdata = split.at(1);
        if(scmd.contains("OBJ"))
        {
            split = sdata.split(" ");

            sdata = split.at(1);
            int x = sdata.toInt();

            sdata = split.at(2);
            int y = sdata.toInt();

            sfont = split.at(4);
            stext = split.at(6);

            //handle text with spaces
            int ispace = 7;
            while(!stext.contains(")"))
            {
                stext += " " + split.at(ispace++);

                if(ispace>=split.count())
                    break;
            }

            stext.replace("(", "");
            stext.replace(")", "");

            if(sfont.contains("ISO1_7X5"))
            {
                QGraphicsTextItem *text = scene->addText(stext, code7x5);
                text->setPos(x, y_offset - y);
            }
            else if(sfont.contains("ISO1_5X5") || sfont.contains("ISO1_5x5_OU"))
            {
                QGraphicsTextItem *text = scene->addText(stext, code5x5);
                text->setPos(x, y_offset - y);
            }
        }
    }
}

void JetPrinterView::labelPreviewDemo()
{
    QFont code5x5(family5x5);
    QFont code7x5(family7x5);

    code7x5.setStretch(100);
    code7x5.setPixelSize(7);

    code5x5.setStretch(100);
    code5x5.setPixelSize(5);

    //label size
    ui->graphicsView->setSceneRect(0,0,200,65);
    scene->addRect(0,0,200,65);

    QGraphicsTextItem *text;

    text = scene->addText("VOLVO", code7x5);
    text->setPos(0, 53 - 25);

    text = scene->addText("31651561", code5x5);
    text->setPos(0, 53 - 18);

    text = scene->addText("GGRFA  AA", code5x5);
    text->setPos(0, 53 - 10);

    text = scene->addText("CZECH REPUBLIC", code5x5);
    text->setPos(0, 53 - 3);

    text = scene->addText("0612P170178", code5x5);
    text->setPos(119, 53 - 10);

    text = scene->addText("{YY-j}", code5x5);
    text->setPos(81, 53 - 18);

    text = scene->addText("{j}", code5x5);
    text->setPos(97, 53 - 10);
}

void JetPrinterView::writeSettings()
{

}

void JetPrinterView::readSettings()
{

}

void JetPrinterView::printerProgram(QString job)
{
    int i;
    for(i=0; i<ui->listWidget->count(); i++)
    {
        QString jobItemName = ui->listWidget->item(i)->text();

        QString jobItem = jobItemName.split(".")[0];

        if(jobItem.compare(job)==0)
            break;
    }

    if(i==ui->listWidget->count())
    {
        QString msg = "Tisková úloha: '" + job + "' nenalezena!";
        QMessageBox::warning(this, "Warning", msg);
    }

    //setup print job
    if(jetPrinter)
    {
        QString jobPath = "\\FFSDISK\\JOBS\\" + job + ".job";
        jetPrinter->LoadJob(jobPath.toLatin1().data());
        refreshLoadedJob = true;
    }
}

void JetPrinterView::printerExText(QString text)
{
    if(jetPrinter)
    {
        jetPrinter->ExternalText(text.toLatin1().data());
    }
}

void JetPrinterView::printerCounterSet(int val)
{
    if(jetPrinter)
    {
        jetPrinter->SetCounters(val);
    }
}
