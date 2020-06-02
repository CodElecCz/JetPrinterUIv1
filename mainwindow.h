#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QTimer>
#include <QtSql/QSqlDatabase>
#include "jetprinterview.h"
#include "plcview.h"
#include "reportview.h"
#include "settingsview.h"
#include "lightwidget.h"
#include "tcpserver.h"
#include "udpserver.h"
#include "ui_mainwindow.h"

namespace Ui {
class MainWindow;
}

enum EToolBarItems
{
    EToolBarItem_Login = 0,
    EToolBarItem_Separator1,
    EToolBarItem_Read,
    EToolBarItem_Write,
    EToolBarItem_Separator2,
    EToolBarItem_Screen,
};

enum EStatusBarItems
{
    EStatusBarItem_Database = 0,
    EStatusBarItem_Plc,
    EStatusBarItem_Printer
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    explicit MainWindow(QString configFileName, QWidget *parent = 0);
    ~MainWindow();

private:
    void initialize();
    void initializeAplication(QSettings *config);
    void initializePrograms();
    void initializeDatabase(QString dbsHostName, QString dbsDatabase, QString dbsUser, QString dbsPassword);
    void initializePlc(QSettings *config);
    void initializePrinter(QSettings *config);

    void openConfigFile(QString file);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

    //aplication data
    void writeAppSettings();
    void readAppSettings();

signals:
    //database log
    void logPrintFinished(SLogPrintFinish data);
    void printerCounterReset();

public slots:
    void plcTextStatus(QString text) { ui->labelPlcText1->setText(text); }
    void plcTextInsert(QString text) { ui->labelPlcText2->setText(text); }
    void plcTextLength(QString text) { ui->labelPlcText3->setText(text); }
    void plcColorInsert(QColor color)
    {
        QPalette palette = ui->labelPlcText2->palette();
        palette.setColor(ui->labelPlcText2->foregroundRole(), color);
        ui->labelPlcText2->setPalette(palette);
    }
    void plcColorLength(QColor color)
    {
        QPalette palette = ui->labelPlcText3->palette();
        palette.setColor(ui->labelPlcText3->foregroundRole(), color);
        ui->labelPlcText3->setPalette(palette);
    }
    void plcColorSensor1(QColor color)
    {
        QPalette palette = ui->labelPlcText4->palette();
        palette.setColor(ui->labelPlcText4->foregroundRole(), color);
        ui->labelPlcText4->setPalette(palette);
    }
    void plcColorSensor2(QColor color)
    {
        QPalette palette = ui->labelPlcText5->palette();
        palette.setColor(ui->labelPlcText5->foregroundRole(), color);
        ui->labelPlcText5->setPalette(palette);
    }
    void plcColorSensor3(QColor color)
    {
        QPalette palette = ui->labelPlcText6->palette();
        palette.setColor(ui->labelPlcText6->foregroundRole(), color);
        ui->labelPlcText6->setPalette(palette);
    }

    void plcLog(UINT productCounter);

    void printerState(QString status)
    {
        ui->labelPrinterText3->setText(status);

        if(status.contains("Tiskne"))
        {
            QPixmap pix (":/images/LEDs/led_green.png");
            ui->labelPrinter->setPixmap(pix);

            QPalette palette = ui->labelPrinterText3->palette();
            palette.setColor(ui->labelPrinterText3->foregroundRole(), Qt::green);
            ui->labelPrinterText3->setPalette(palette);
        }
        else
        {
            QPixmap pix (":/images/LEDs/led_red.png");
            ui->labelPrinter->setPixmap(pix);

            QPalette palette = ui->labelPrinterText3->palette();
            palette.setColor(ui->labelPrinterText3->foregroundRole(), Qt::red);
            ui->labelPrinterText3->setPalette(palette);
        }
    }

private slots:
    void on_actionLogin_triggered() {;}
    void on_actionRead_triggered();
    void on_actionWrite_triggered();

    void userLogin() {;}
    void openRecentFile();
    void fullScreen();
    void infoScreen();

    //settings.ini
    void settingsChanged(QString group);
    void readSettingsFile();
    void writeSettingsFile();

    //statusbar
    void statusPlc(QString state);
    void statusDatabase(QString state);
    void statusPrinter(QString state);
    void updateTime();

    //program
    void programSelected(QString);
    void programUp();
    void programDown();

    //keyboard
    void buttonTextClicked(QAbstractButton* );
    void buttonNumberClicked(QAbstractButton* );
    void checkText(QString text);

    //local server - simulation
    void responsePlc(QString cmd, QString *response);
    void responsePrinter(QString cmd, QString *response);

    //main window counter
    void counterUp()
    {
        //main window counter
        productCounter++;
        ui->lcdCounter->display(QString::number(productCounter));
    }
    void counterDown()
    {
        //main window counter
        if(productCounter>0)
        {
            productCounter--;
            ui->lcdCounter->display(QString::number(productCounter));
        }
    }
    void counterReset()
    {
        productCounter = 0;
        ui->lcdCounter->display(QString::number(productCounter));

        emit printerCounterReset();
    }

private:
    ReportView      *reportview;
    JetPrinterView  *printerview;
    PlcView         *plcview;
    SettingsView    *settingsview;
    QTabWidget      *tabWidget;

    Ui::MainWindow *ui;

    //recent open files
    QString curFile;
    QAction *separatorAct;
    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];

    //settings.ini
    QSettings *settings;
    QString settingsFilePath;

    //config.ini
    QString configFilePath;

    //database
    QSqlDatabase db;

    //statusbar
    LightWidget     *lightDbs;
    LightWidget     *lightPlc;
    LightWidget     *lightPrinter;
    QLabel          *statusTime;
    QTimer          *timer;

    UdpServer       *serverPlc;
    TcpServer       *serverPrinter;

    //keyboard
    QButtonGroup    *keyboardNumber;
    QButtonGroup    *keyboardText;

    //plc
    UINT            plcSettings;

    //text input check
    int             externalTextLength;

    //product counter
    int             productCounter;
};

#endif // MAINWINDOW_H
