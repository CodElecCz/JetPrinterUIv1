#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QSignalMapper>
#include <QFileDialog>
#include <QMessageBox>
#include <QBitArray>
#include "mainwindow.h"
#include "settingsview.h"
#include "ui_settingsview.h"

SettingsView::SettingsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsView)
{
    ui->setupUi(this);
}

SettingsView::SettingsView(QSettings *settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsView)
{
    ui->setupUi(this);
    this->initialize(settings);
}

SettingsView::~SettingsView()
{
    delete ui;
}

void SettingsView::initialize(QSettings *settings)
{
    QStringList groups = settings->childGroups();

    for(int i = 0; i<groups.count(); i++)
    {
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/settings.ico"), QSize(), QIcon::Normal, QIcon::Off);

        QString group = groups.at(i);

        settings->beginGroup(group);
        QString name = settings->value(QString("Name")).toString();
        QString job = settings->value(QString("Job")).toString();
        settings->endGroup();

        ProgramView *programview = new ProgramView(settings, group, this);
        ui->toolBox->addItem(programview, icon, name);
        connect(programview, SIGNAL(settingsChanged(QString)), this, SLOT(settingsProgramChanged(QString)));

        programViewList.append(programview);
    }

    //remove default page
    ui->toolBox->removeItem(0);
    ui->program01->hide();
}

void SettingsView::settingsProgramChanged(QString group)
{
    emit settingsChanged(group);
}

void SettingsView::writeSettings(QSettings *settings)
{
    for(int i = 0; i< programViewList.count(); i++)
    {
        programViewList[i]->writeSettings(settings);

        //name
        settings->beginGroup(programViewList[i]->group);
        QString name = settings->value(QString("Name")).toString();
        settings->endGroup();
        ui->toolBox->setItemText(i, name);
    }
}

void SettingsView::readSettings(QSettings *settings)
{
    for(int i = 0; i< programViewList.count(); i++)
    {
        //name
        settings->beginGroup(programViewList[i]->group);
        QString name = settings->value(QString("Name")).toString();
        settings->endGroup();
        ui->toolBox->setItemText(i, name);

        programViewList[i]->readSettings(settings);
    }
}


