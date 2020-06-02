#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QSignalMapper>
#include <QFileDialog>
#include <QMessageBox>
#include "mainwindow.h"
#include "programview.h"
#include "ui_programview.h"

ProgramView::ProgramView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgramView)
{
    ui->setupUi(this);
}

ProgramView::ProgramView(QSettings *settings, QString group, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgramView)
{
    ui->setupUi(this);
    this->group = group;

    initialize();
    readSettings(settings);
}

ProgramView::~ProgramView()
{
    delete ui;
}

void ProgramView::initialize()
{
    connect(ui->lineEdit1, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(ui->lineEdit2, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(ui->checkBox1, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui->checkBox2, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui->checkBox3, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui->checkBox4, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui->checkBox5, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(ui->checkBox6, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
}

void ProgramView::writeSettings(QSettings *settings)
{
    QString name = ui->lineEdit1->text();
    QString job = ui->lineEdit2->text();
    Qt::CheckState insert01 = ui->checkBox1->checkState();
    Qt::CheckState insert02 = ui->checkBox2->checkState();
    Qt::CheckState sensor01 = ui->checkBox3->checkState();
    Qt::CheckState sensor02 = ui->checkBox4->checkState();
    Qt::CheckState sensor03 = ui->checkBox5->checkState();
    Qt::CheckState startstop = ui->checkBox6->checkState();

    settings->beginGroup(group);

    settings->setValue(QString("Name"), name);
    settings->setValue(QString("Job"), job);
    settings->setValue(QString("Insert01"), (insert01==Qt::Checked)? QString("true"):QString("false"));
    settings->setValue(QString("Insert02"), (insert02==Qt::Checked)? QString("true"):QString("false"));
    settings->setValue(QString("Sensor01"), (sensor01==Qt::Checked)? QString("true"):QString("false"));
    settings->setValue(QString("Sensor02"), (sensor02==Qt::Checked)? QString("true"):QString("false"));
    settings->setValue(QString("Sensor03"), (sensor03==Qt::Checked)? QString("true"):QString("false"));
    settings->setValue(QString("StartButton"), (startstop==Qt::Checked)? QString("true"):QString("false"));

    settings->endGroup();
}

void ProgramView::readSettings(QSettings *settings)
{
    settings->beginGroup(group);

    QString name = settings->value(QString("Name")).toString();
    QString job = settings->value(QString("Job")).toString();
    bool insert01 = settings->value(QString("Insert01")).toBool();
    bool insert02 = settings->value(QString("Insert02")).toBool();
    bool sensor01 = settings->value(QString("Sensor01")).toBool();
    bool sensor02 = settings->value(QString("Sensor02")).toBool();
    bool sensor03 = settings->value(QString("Sensor03")).toBool();
    bool startstop = settings->value(QString("StartButton")).toBool();

    settings->endGroup();

    ui->lineEdit1->setText(name);
    ui->lineEdit2->setText(job);
    ui->checkBox1->setCheckState(insert01? Qt::Checked : Qt::Unchecked);
    ui->checkBox2->setCheckState(insert02? Qt::Checked : Qt::Unchecked);
    ui->checkBox3->setCheckState(sensor01? Qt::Checked : Qt::Unchecked);
    ui->checkBox4->setCheckState(sensor02? Qt::Checked : Qt::Unchecked);
    ui->checkBox5->setCheckState(sensor03? Qt::Checked : Qt::Unchecked);
    ui->checkBox6->setCheckState(startstop? Qt::Checked : Qt::Unchecked);
}


