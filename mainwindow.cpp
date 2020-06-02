#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QProxyStyle>
#include <QDateTime>
#include <QMainWindow>
#include <QLayout>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_infodialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    keyboardNumber(new QButtonGroup(this)),
    keyboardText(new QButtonGroup(this)),
    plcSettings(0),
    productCounter(0)
{
    ui->setupUi(this);

    readAppSettings();

    this->configFilePath = QCoreApplication::applicationDirPath() + "/config.ini";
    initialize();
}

MainWindow::MainWindow(QString configFileName, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    keyboardNumber(new QButtonGroup(this)),
    keyboardText(new QButtonGroup(this)),
    plcSettings(0),
    productCounter(0)
{
    ui->setupUi(this);

    readAppSettings();

    if(configFileName.isEmpty())
        configFileName = "/config.ini";

    this->configFilePath = QCoreApplication::applicationDirPath() + "/" + configFileName;
    initialize();
}

MainWindow::~MainWindow()
{
    if(timer)
        timer->stop();

    if(plcview)
        plcview->stop();

    writeAppSettings();
    delete ui;
}

void MainWindow::updateTime()
{
    QDateTime dt= QDateTime::currentDateTime();
    statusTime->setText(dt.toString(QString("hh:mm:ss dd:MM:yyyy")));
}

void MainWindow::statusDatabase(QString state)
{
    if(state.length()>0)
        lightDbs->setText(state);

    lightDbs->blink();
}

void MainWindow::statusPlc(QString state)
{
    if(state.length()>0)
    {
        lightPlc->setText(state);

        if(state.contains("připojeno"))
        {
            lightPlc->setColor(Qt::green);
        }
        else if(state.contains("chyba"))
        {
            lightPlc->setColor(Qt::red);
        }
    }

    lightPlc->blink();
}

void MainWindow::statusPrinter(QString state)
{
    if(state.length()>0)
    {
        lightPrinter->setText(state);

        if(state.contains("připojeno"))
        {
            lightPrinter->setColor(Qt::green);
        }
        else if(state.contains("chyba"))
        {
            lightPrinter->setColor(Qt::red);
        }
    }

    lightPrinter->blink();
}

void MainWindow::settingsChanged(QString group)
{
    //settings were changed
    ui->toolBar->actions().at(EToolBarItem_Read)->setEnabled(true);
    ui->toolBar->actions().at(EToolBarItem_Write)->setEnabled(true);
}

void MainWindow::readSettingsFile()
{
    //read settings.ini
    settingsview->readSettings(this->settings);

    initializePrograms();

    ui->toolBar->actions().at(EToolBarItem_Read)->setEnabled(false);
    ui->toolBar->actions().at(EToolBarItem_Write)->setEnabled(false);
}

void MainWindow::writeSettingsFile()
{
    //save settings.ini
    settingsview->writeSettings(this->settings);

    initializePrograms();

    ui->toolBar->actions().at(EToolBarItem_Read)->setEnabled(false);
    ui->toolBar->actions().at(EToolBarItem_Write)->setEnabled(false);
}

void MainWindow::on_actionRead_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                          tr("Open File"),
                                          QDir::currentPath(),
                                          tr("Setting files (*.ini)"));

    openConfigFile(filePath);
}

void MainWindow::on_actionWrite_triggered()
{

}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        openConfigFile(action->data().toString());
    }
}

void MainWindow::fullScreen()
{
    if(MainWindow::isFullScreen())
        showMaximized();
    else
        showFullScreen();
}

void MainWindow::infoScreen()
{
   QDialog *info = new QDialog(0,0);
   Ui_infoDialog infoUi;
   infoUi.setupUi(info);
   info->show();
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowFilePath(curFile);

    QSettings settings("QT", "JetPrinterUI");
    QStringList files = settings.value("RecentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
}

 void MainWindow::updateRecentFileActions()
 {
     QSettings settings("QT", "JetPrinterUI");
     QStringList files = settings.value("RecentFileList").toStringList();

     int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

     for (int i = 0; i < numRecentFiles; ++i) {
         QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
         recentFileActs[i]->setText(text);
         recentFileActs[i]->setData(files[i]);
         recentFileActs[i]->setVisible(true);
     }
     for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
         recentFileActs[j]->setVisible(false);

     separatorAct->setVisible(numRecentFiles > 0);
 }

 QString MainWindow::strippedName(const QString &fullFileName)
 {
     return QFileInfo(fullFileName).fileName();
 }

 void MainWindow::openConfigFile(QString filePath)
 {
     QApplication::setOverrideCursor(Qt::WaitCursor);

     if (!filePath.isNull() && !filePath.isEmpty())
     {
         this->configFilePath = filePath;
         initialize();

         setCurrentFile(filePath);
     }

     QApplication::restoreOverrideCursor();
}

void MainWindow::writeAppSettings()
{
    QSettings settings("QT", "JetPrinterUI");

    settings.beginGroup("MainWindow");
    settings.setValue("isFullScreen", MainWindow::isFullScreen());
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("toolBarArea", toolBarArea(ui->toolBar));

    settings.endGroup();
}

 void MainWindow::readAppSettings()
{
    QSettings settings("QT", "JetPrinterUI");

    settings.beginGroup("MainWindow");
    if(settings.value("isFullScreen").toBool())
    {
        showFullScreen();
    }
    else
    {
        resize(settings.value("size", QSize(800, 600)).toSize());
        move(settings.value("pos", QPoint(100, 100)).toPoint());
    }
    addToolBar((Qt::ToolBarArea)settings.value("toolBarArea").toUInt(), ui->toolBar);

    settings.endGroup();
}
