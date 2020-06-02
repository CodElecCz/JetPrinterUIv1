#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QProxyStyle>
#include <QDateTime>
#include <QMainWindow>
#include <QLayout>
#include <QMessageBox>
#include <QSqlError>
#include <QTcpSocket>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loadview.h"

class CustomTabStyle : public QProxyStyle
{
public:
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &size, const QWidget *widget) const
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == QStyle::CT_TabBarTab)
            s.transpose();
        return s;
    }

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if (element == CE_TabBarTabLabel)
        {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option))
            {
                QStyleOptionTab opt(*tab);
                opt.shape = QTabBar::RoundedNorth;
                QProxyStyle::drawControl(element, &opt, painter, widget);
                return;
            }
        }
        QProxyStyle::drawControl(element, option, painter, widget);
    }
};

void MainWindow::initialize()
{   
    LoadView *info = new LoadView(0);
    info->show();

    //config.ini
    QFile configFile(this->configFilePath);
    if(!configFile.exists())
    {
        QString msg = "Config file not found: " + this->configFilePath;
        QMessageBox::question(this, "Warning", msg, QMessageBox::Ok);
    }
    QSettings *config = new QSettings(this->configFilePath, QSettings::IniFormat);
    config->setIniCodec("UTF-8");

    //aplication
    info->setText("Nahravám: Konfiguraci");
    initializeAplication(config);

    //setings.ini
    info->setText("Nahravám: Nastavení");
    QString settingsFileName = config->value(QString("Settings/File"), QVariant("settings.ini")).toString();
    this->settingsFilePath = QCoreApplication::applicationDirPath() + "/" + settingsFileName;
    QFile settingsFile(this->settingsFilePath);
    if(!settingsFile.exists())
    {
        QString msg = "Setting file not found: " + this->settingsFilePath;
        QMessageBox::question(this, "Warning", msg, QMessageBox::Ok);
    }
    this->settings = new QSettings(this->settingsFilePath, QSettings::IniFormat);
    this->settings->setIniCodec("UTF-8");
    initializePrograms();

    //widgets
    info->setText("Nahravám: Záložky");
    ui->tabWidgets->tabBar()->setStyle(new CustomTabStyle);
    setCentralWidget(ui->tabWidgets);

    QStringList groups = config->childGroups();
    QString swidget;
    QString sname;
    QString srights;
    QString sicon;
    settingsview = nullptr;
    plcview = nullptr;
    printerview = nullptr;
    reportview = nullptr;
    for(int i = 0; i<groups.count(); i++)
    {
        QString group = groups.at(i);
        config->beginGroup(group);

        if(group.contains(QString("Widget")))
        {
            swidget = config->value(QString("Widget")).toString();
            sname = config->value(QString("Name")).toString();
            srights = config->value(QString("Rights")).toString();

            //icon
            QIcon icon;
            sicon = config->value(QString("Icon")).toString();
            icon.addFile(sicon, QSize(), QIcon::Normal, QIcon::Off);

            //font
            QFont font;
            int ifont = config->value(QString("FontSize")).toInt();
            font.setPointSize(ifont);

            if(0==swidget.compare("settingsview"))
            {
                settingsview = new SettingsView(this->settings, this);
                settingsview->setFont(font);
                ui->tabWidgets->addTab(settingsview, icon, sname);

                connect(settingsview, SIGNAL(settingsChanged(QString)), this, SLOT(settingsChanged(QString)));
            }
            else if(0==swidget.compare("plcview"))
            {
                plcview = new PlcView(this);
                plcview->setFont(font);
                ui->tabWidgets->addTab(plcview, icon, sname);
            }
            else if(0==swidget.compare("jetprinterview"))
            {
                printerview = new JetPrinterView(this);
                printerview->setFont(font);
                ui->tabWidgets->addTab(printerview, icon, sname);

            }
            else if(0==swidget.compare("reportview"))
            {
                reportview = new ReportView(this);
                reportview->setFont(font);
                ui->tabWidgets->addTab(reportview, icon, sname);

            }
        }

        config->endGroup();
    }

    //toolbar
    info->setText("Nahravám: ToolBar");
    QAction* act1 = ui->toolBar->addAction(QIcon(":/images/user.ico"), QString("Přihlásit"), this, SLOT(userLogin()));
    ui->toolBar->addSeparator();
    QAction* act2 = ui->toolBar->addAction(QIcon(":/images/reload.ico"), QString("Obnovit"), this, SLOT(readSettingsFile()));
    QAction* act3 = ui->toolBar->addAction(QIcon(":/images/save.ico"), QString("Uložit"), this, SLOT(writeSettingsFile()));
    ui->toolBar->addSeparator();
    QAction* act4 = ui->toolBar->addAction(QIcon(":/images/screen.ico"), QString("Obrazovka"), this, SLOT(fullScreen()));
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    spacer->setVisible(true);
    ui->toolBar->addWidget(spacer);
    QAction* act5 = ui->toolBar->addAction(QIcon(":/images/info.ico"), QString("Info"), this, SLOT(infoScreen()));

    QLayout* lay = ui->toolBar->layout();
    for(int i = 0; i < lay->count(); ++i)
    {
        //set layout only for icon-text items
        QString text = ui->toolBar->actions().at(i)->text();
        if(!text.isEmpty())
            lay->itemAt(i)->setAlignment(Qt::AlignLeft);
    }

    act1->setEnabled(true);
    act2->setEnabled(false);
    act3->setEnabled(false);
    act4->setEnabled(true);
    act5->setEnabled(true);

    //statusbar
    info->setText("Nahravám: StatusBar");
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start();

    statusTime = new QLabel();
    statusTime->setText(QString("DateTime"));
    ui->statusBar->addPermanentWidget(statusTime);

    //recent files
    recentFileActs[0] = ui->actionFile1;
    recentFileActs[1] = ui->actionFile2;
    recentFileActs[2] = ui->actionFile3;
    recentFileActs[3] = ui->actionFile4;
    recentFileActs[4] = ui->actionFile5;
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
    separatorAct = ui->menuFile->actions().at(5);
    setWindowFilePath(QString());
    updateRecentFileActions();

    //database
    info->setText("Nahravám: Databáze");
    QString dbsHostName = config->value(QString("Database/HostName")).toString();
    QString dbsDatabase = config->value(QString("Database/Database")).toString();
    QString dbsUser = config->value(QString("Database/User")).toString();
    QString dbsPassword = config->value(QString("Database/Password")).toString();
    initializeDatabase(dbsHostName, dbsDatabase, dbsUser, dbsPassword);

    //plc
    info->setText("Nahravám: PLC");
    initializePlc(config);

    //printer
    info->setText("Nahravám: Tiskárna");
    initializePrinter(config);

    info->close();
    delete info;

    bool fullScreen = config->value(QString("Aplication/FullScreen")).toBool();
    if(fullScreen)
        showFullScreen();
}

void MainWindow::initializeAplication(QSettings *config)
{
    //aplication
    bool menuBarVisible = config->value(QString("Aplication/MenuBarVisible")).toBool();
    if(!menuBarVisible)
        menuBar()->hide();

    externalTextLength = config->value(QString("Aplication/ExternalTextLength")).toInt();
    ui->lineEdit->setMaxLength(externalTextLength);

    QString keyList = config->value(QString("Aplication/KeyboardLetters")).toString();

    QList<QPushButton*> numberButtonList;
    numberButtonList.append(ui->pushButton_1);
    numberButtonList.append(ui->pushButton_2);
    numberButtonList.append(ui->pushButton_3);
    numberButtonList.append(ui->pushButton_4);
    numberButtonList.append(ui->pushButton_5);
    numberButtonList.append(ui->pushButton_6);
    numberButtonList.append(ui->pushButton_7);
    numberButtonList.append(ui->pushButton_8);
    numberButtonList.append(ui->pushButton_9);
    numberButtonList.append(ui->pushButton_10);
    //number buttons group
    foreach(QPushButton* button, numberButtonList)
    {
        this->keyboardNumber->addButton(button);
    }
    connect(keyboardNumber, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonNumberClicked(QAbstractButton*)));

    QList<QPushButton*> textButtonList;
    textButtonList.append(ui->pushButtonText_1);
    textButtonList.append(ui->pushButtonText_2);
    textButtonList.append(ui->pushButtonText_3);
    textButtonList.append(ui->pushButtonText_4);
    textButtonList.append(ui->pushButtonText_5);
    textButtonList.append(ui->pushButtonText_6);
    textButtonList.append(ui->pushButtonText_7);
    textButtonList.append(ui->pushButtonText_8);
    textButtonList.append(ui->pushButtonText_9);
    textButtonList.append(ui->pushButtonText_10);
    //text buttons names
    int buttonIndex = 0;
    foreach(QString key, keyList.split("|"))
    {
        textButtonList.at(buttonIndex)->setText(key);
        buttonIndex++;
        if(buttonIndex>=textButtonList.count())
            break;
    };
    //text buttons group
    foreach(QPushButton* button, textButtonList)
    {
        this->keyboardText->addButton(button);
    }
    connect(keyboardText, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonTextClicked(QAbstractButton*)));
}

void MainWindow::buttonTextClicked(QAbstractButton* button)
{
    QString c = button->text();
    ui->lineEdit->insert(c);
    ui->lineEdit->setFocus();
}

void MainWindow::buttonNumberClicked(QAbstractButton* button)
{
    QString c = button->text();
    ui->lineEdit->insert(c);
    ui->lineEdit->setFocus();
}

void MainWindow::initializePrograms()
{
    QStringList groups = settings->childGroups();

    ui->comboBox->clear();

    ui->comboBox->addItem(QIcon(), "*** Zvolte program ***");

    for(int i = 0; i<groups.count(); i++)
    {
        QString group = groups.at(i);
        settings->beginGroup(group);

        QIcon icon;
        QString sname = settings->value(QString("Name")).toString();
        icon.addFile(QStringLiteral(":/images/settings.ico"), QSize(), QIcon::Normal, QIcon::Off);
        ui->comboBox->addItem(icon, sname, QVariant(group));
        settings->endGroup();
    }

    //printer job
    ui->labelPrinterText2->setText("Žádná");

    connect(ui->comboBox, SIGNAL(activated(QString)), this, SLOT(programSelected(QString)));
    connect(ui->pushButtonDown, SIGNAL(released()), this, SLOT(programDown()));
    connect(ui->pushButtonUp, SIGNAL(released()), this, SLOT(programUp()));

    //main window counter
    connect(ui->pushButtonCntUp, SIGNAL(released()), this, SLOT(counterUp()));
    connect(ui->pushButtonCntDown, SIGNAL(released()), this, SLOT(counterDown()));
    connect(ui->pushButtonCntReset, SIGNAL(released()), this, SLOT(counterReset()));

}

void MainWindow::programDown()
{
    int index = ui->comboBox->currentIndex();

    if(--index<1)
    {
        ui->comboBox->setCurrentIndex(ui->comboBox->count()-1);
    }
    else
        ui->comboBox->setCurrentIndex(index);

    emit programSelected(ui->comboBox->currentText());
}

void MainWindow::programUp()
{
    int index = ui->comboBox->currentIndex();

    if(++index>=ui->comboBox->count())
    {
        ui->comboBox->setCurrentIndex(1);
    }
    else
        ui->comboBox->setCurrentIndex(index);

    emit programSelected(ui->comboBox->currentText());
}

void MainWindow::programSelected(QString sprogram)
{
    int index = ui->comboBox->currentIndex();

    if(index>0)
    {
        QString group = ui->comboBox->currentData().toString();

        //printer job
        QString sjob = settings->value(group + QString("/Job")).toString();
        ui->labelPrinterText2->setText(sjob);
        ui->lineEdit->setFocus();

        bool binsert01 = settings->value(group + QString("/Insert01")).toBool();
        bool binsert02 = settings->value(group + QString("/Insert02")).toBool();
        bool bsensor01 = settings->value(group + QString("/Sensor01")).toBool();
        bool bsensor02 = settings->value(group + QString("/Sensor02")).toBool();
        bool bsensor03 = settings->value(group + QString("/Sensor03")).toBool();
        bool bstart = settings->value(group + QString("/StartButton")).toBool();

        plcSettings =   ((binsert01)?  1:0)
                        + ((binsert02)?  2:0)
                        + ((bsensor01)?  4:0)
                        + ((bsensor02)?  8:0)
                        + ((bsensor03)?  16:0)
                        + ((bstart)?     32:0);

        //set indicators
        ui->led_1->setPixmap((binsert01)? QPixmap(":/images/LEDs/led_green.png") : QPixmap(":/images/LEDs/led_gray.png"));
        ui->led_2->setPixmap((binsert02)? QPixmap(":/images/LEDs/led_green.png") : QPixmap(":/images/LEDs/led_gray.png"));
        ui->led_3->setPixmap((bsensor01)? QPixmap(":/images/LEDs/led_green.png") : QPixmap(":/images/LEDs/led_gray.png"));
        ui->led_4->setPixmap((bsensor02)? QPixmap(":/images/LEDs/led_green.png") : QPixmap(":/images/LEDs/led_gray.png"));
        ui->led_5->setPixmap((bsensor03)? QPixmap(":/images/LEDs/led_green.png") : QPixmap(":/images/LEDs/led_gray.png"));

        emit plcview->plcProgram(plcSettings);
        emit printerview->printerProgram(sjob);

        //printer external text
        ui->lineEdit->setText("");
        checkText("");
    }
    else
    {
        //printer job
        ui->labelPrinterText2->setText("Žádná");

        //set indicators
        ui->led_1->setPixmap(QPixmap(":/images/LEDs/led_gray.png"));
        ui->led_2->setPixmap(QPixmap(":/images/LEDs/led_gray.png"));
        ui->led_3->setPixmap(QPixmap(":/images/LEDs/led_gray.png"));
        ui->led_4->setPixmap(QPixmap(":/images/LEDs/led_gray.png"));
        ui->led_5->setPixmap(QPixmap(":/images/LEDs/led_gray.png"));

        emit plcview->plcProgram(0);

        //printer external text
        ui->lineEdit->setText("");
        checkText("");
    }
}

void MainWindow::initializeDatabase(QString dbsHostName, QString dbsDatabase, QString dbsUser, QString dbsPassword)
{
    db = QSqlDatabase::addDatabase("QODBC");
    db.setHostName(dbsHostName);
    db.setDatabaseName(dbsDatabase);
    db.setUserName(dbsUser);
    db.setPassword(dbsPassword);

    bool ok = db.open();
    if(!ok)
    {
        QString msg = "initializeDatabase() Error: " + db.lastError().text();
        QMessageBox::warning(this, "Warning", msg);

        lightDbs = new LightWidget(Qt::white, "Databáze: nepřipojeno");
        ui->statusBar->addWidget(lightDbs);
    }
    else
    {
        lightDbs = new LightWidget(Qt::green, "Databáze: připojeno");
        ui->statusBar->addWidget(lightDbs);

        reportview->initialize();
    }

    connect(reportview, SIGNAL(statusDatabase(QString)), this, SLOT(statusDatabase(QString)));
}

void MainWindow::initializePlc(QSettings *config)
{
    bool plcSim = config->value(QString("PLC/LocalSimulation")).toBool();
    if(plcSim)
    {
        QString plcIp = config->value(QString("PLC/Ip")).toString();
        int plcLocalPort = config->value(QString("PLC/LocalPort")).toInt();
        int plcRemotePort = config->value(QString("PLC/RemotePort")).toInt();

        serverPlc = new UdpServer(plcIp, plcRemotePort, plcLocalPort, this);
        connect(serverPlc, SIGNAL(response(QString,QString*)), this, SLOT(responsePlc(QString,QString*)));
    }

    if(plcview)
    {
        plcview->start();

        connect(plcview, SIGNAL(plcTextStatus(QString)), this, SLOT(plcTextStatus(QString)));
        connect(plcview, SIGNAL(plcTextInsert(QString)), this, SLOT(plcTextInsert(QString)));
        connect(plcview, SIGNAL(plcTextLength(QString)), this, SLOT(plcTextLength(QString)));
        connect(plcview, SIGNAL(plcColorInsert(QColor)), this, SLOT(plcColorInsert(QColor)));
        connect(plcview, SIGNAL(plcColorLength(QColor)), this, SLOT(plcColorLength(QColor)));
        connect(plcview, SIGNAL(plcColorSensor1(QColor)), this, SLOT(plcColorSensor1(QColor)));
        connect(plcview, SIGNAL(plcColorSensor2(QColor)), this, SLOT(plcColorSensor2(QColor)));
        connect(plcview, SIGNAL(plcColorSensor3(QColor)), this, SLOT(plcColorSensor3(QColor)));
    }

    bool ok = false;
    if(!ok)
    {
        lightPlc = new LightWidget(Qt::white, "PLC: nepřipojeno");
        ui->statusBar->addWidget(lightPlc);
    }
    else
    {
        lightPlc = new LightWidget(Qt::green, "PLC: připojeno");
        ui->statusBar->addWidget(lightPlc);
    }
    //toolbar status
    connect(plcview, SIGNAL(statusPlc(QString)), this, SLOT(statusPlc(QString)));

    //plc program
    QString plcProgramVar = config->value(QString("PLC/ProgramVar")).toString();
    UINT plcProgramNb = config->value(QString("PLC/ProgramNumber")).toUInt();
    QString plcSettingsVar = config->value(QString("PLC/SettingsVar")).toString();

    //plc screen
    QString plcStatusVar = config->value(QString("PLC/StatusVar")).toString();
    QString plcColorVar = config->value(QString("PLC/ColorVar")).toString();
    QString plcInsertVar = config->value(QString("PLC/InsertVar")).toString();
    QString plcLengthVar = config->value(QString("PLC/LengthVar")).toString();

    //print log
    QString plcCounterVar = config->value(QString("PLC/CounterVar")).toString();

    //total stop
    QString plcTotalStopVar = config->value(QString("PLC/TotalStopVar")).toString();

    if(plcview)
    {
        //plc program
        plcview->setProgramVar(plcProgramVar);
        plcview->setSettingsVar(plcSettingsVar);
        plcview->setProgramNb(plcProgramNb);

        //plc screen
        plcview->setStatusVar(plcStatusVar);
        plcview->setColorVar(plcColorVar);
        plcview->setInsertVar(plcInsertVar);
        plcview->setLengthVar(plcLengthVar);

        plcview->delayStartOfPlc(1000);

        //totalstop
        plcview->setTotalStopVar(plcTotalStopVar);

        //print log
        plcview->setCounterVar(plcCounterVar);
        connect(plcview, SIGNAL(plcLog(UINT)), this, SLOT(plcLog(UINT)));
        connect(this, SIGNAL(logPrintFinished(SLogPrintFinish)), reportview, SLOT(logPrintFinished(SLogPrintFinish)));
    }
}

void MainWindow::initializePrinter(QSettings *config)
{
    bool ok = false;
    bool printerSim = config->value(QString("Printer/LocalSimulation")).toBool();
    if(printerSim)
    {
        QString printerIp = config->value(QString("Printer/Ip")).toString();
        int printerPort = config->value(QString("Printer/Port")).toInt();

        serverPrinter = new TcpServer(printerPort, this);
        connect(serverPrinter, SIGNAL(response(QString,QString*)), this, SLOT(responsePrinter(QString,QString*)));
    }
    else
    {
        QString printerSer = config->value(QString("Printer/Serial")).toString();
        int printerBaudrate = config->value(QString("Printer/BaudRate")).toInt();

        ok = printerview->initializeSer(printerSer, printerBaudrate);
    }

    if(!ok)
    {
        lightPrinter = new LightWidget(Qt::white, "Tiskárna: nepřipojeno");
        ui->statusBar->addWidget(lightPrinter);
    }
    else
    {
        lightPrinter = new LightWidget(Qt::green, "Tiskárna: připojeno");
        ui->statusBar->addWidget(lightPrinter);
    }
    //toolbar status
    connect(printerview, SIGNAL(statusPrinter(QString)), this, SLOT(statusPrinter(QString)));

    //external text
    connect(ui->lineEdit, SIGNAL(textEdited(QString)), printerview, SLOT(printerExText(QString)));
    connect(ui->lineEdit, SIGNAL(textEdited(QString)), this, SLOT(checkText(QString)));

    //printer status
    connect(printerview, SIGNAL(printerState(QString)), this, SLOT(printerState(QString)));
    connect(this, SIGNAL(printerCounterReset()), printerview, SLOT(printerCounterReset()));
}

void MainWindow::plcLog(UINT productCnt)
{
    SLogPrintFinish data;

    //selected program
    data.prog = ui->comboBox->currentText();
    data.job = ui->labelPrinterText2->text();
    data.settings = QString::number(this->plcSettings, 2);
    data.label = ui->lineEdit->text();
    data.user = "";
    data.productCounter = productCnt;

    emit logPrintFinished(data);

    //main window counter
    productCounter++;
    ui->lcdCounter->display(QString::number(productCounter));
}

void MainWindow::checkText(QString text)
{
    if(text.length()!=externalTextLength)
    {
        QPalette palette;// = ui->lineEdit->palette();
        palette.setColor(QPalette::Base, Qt::red);
        ui->lineEdit->setPalette(palette);
    }
    else
    {
        QPalette palette;// = ui->lineEdit->palette();
        palette.setColor(QPalette::Base, Qt::white);
        ui->lineEdit->setPalette(palette);
    }
}
