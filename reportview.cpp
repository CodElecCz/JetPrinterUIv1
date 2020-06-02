#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QSignalMapper>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include "mainwindow.h"
#include "reportview.h"
#include "ui_reportview.h"

ReportView::ReportView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReportView)
{
    ui->setupUi(this);
    this->readSettings();
}

ReportView::~ReportView()
{
    this->writeSettings();
    delete ui;
}

void ReportView::initialize()
{
    QSqlQuery query;
    bool ok = query.exec("SELECT * FROM massag.history WHERE date>=CURDATE() GROUP BY label ORDER BY date");
    if(!ok)
    {
        QString serror = "query.exec() Error: " + query.lastError().text();
        qDebug(serror.toUtf8().data());
    }
    else
        emit statusDatabase();

    ui->listWidget->clear();
    ui->listWidget->setFont(QFont("MS Shell Dlg 2", 15));
    while (query.next())
    {
        QString slabel = query.value(1).toString();

        ui->listWidget->addItem(slabel);
    }

    if(ui->listWidget->count()>0)
    {
        QListWidgetItem *item = ui->listWidget->item(0);
        ui->listWidget->setCurrentItem(item);
        listChanged(item->text());
    }

    connect(ui->calendarWidget, SIGNAL(clicked(QDate)), this, SLOT(calendarClicked(QDate)));
    connect(ui->listWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(listChanged(QString)));
}

void ReportView::calendarClicked(QDate date)
{
    QString sdate = date.toString("yyyy-MM-dd");

    QSqlQuery query;
    bool ok = query.exec("SELECT * FROM massag.history WHERE date>='" + sdate + " 00:00:00' AND date<='" + sdate + "23:59:59' GROUP BY label ORDER BY date");
    if(!ok)
    {
        QString serror = "query.exec() Error: " + query.lastError().text();
        qDebug(serror.toUtf8().data());
    }
    else
        emit statusDatabase();

    ui->listWidget->clear();
    ui->listWidget->setFont(QFont("MS Shell Dlg 2", 15));
    while (query.next())
    {
        QString slabel = query.value(1).toString();

        ui->listWidget->addItem(slabel);
    }

    if(ui->listWidget->count()>0)
    {
        QListWidgetItem *item = ui->listWidget->item(0);
        ui->listWidget->setCurrentItem(item);
        listChanged(item->text());
    }
}

void ReportView::listChanged(QString label)
{
    QString sdate = ui->calendarWidget->selectedDate().toString("yyyy-MM-dd");

    QSqlQuery query;
    bool ok = query.exec("SELECT * FROM massag.history WHERE date>='" + sdate + " 00:00:00' AND date<='" + sdate + "23:59:59' AND label='" + label + "' ORDER BY date");
    if(!ok)
    {
        QString serror = "query.exec() Error: " + query.lastError().text();
        qDebug(serror.toUtf8().data());
    }
    else
        emit statusDatabase();

    ui->tableWidget->setRowCount(0);
    int r = 0;
    while (query.next())
    {
        QString sdate = query.value(0).toString();
        QString slabel = query.value(1).toString();
        QString sprogram = query.value(2).toString();
        QString sjob = query.value(3).toString();
        QString soperator = query.value(4).toString();
        QString ssettings = query.value(5).toString();
        QString scounter = query.value(6).toString();

        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setRowHeight(r, 18);

        QTableWidgetItem *item;
        item= new QTableWidgetItem(QIcon(), sdate);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(r, 0, item);

        item= new QTableWidgetItem(QIcon(), slabel);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(r, 1, item);

        item= new QTableWidgetItem(QIcon(), sprogram);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(r, 2, item);

        item= new QTableWidgetItem(QIcon(), sjob);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(r, 3, item);

        item= new QTableWidgetItem(QIcon(), soperator);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(r, 4, item);

        item= new QTableWidgetItem(QIcon(), ssettings);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(r, 5, item);

        item= new QTableWidgetItem(QIcon(), scounter);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(r, 6, item);

        r++;
    }
}

void ReportView::logPrintFinished(SLogPrintFinish data)
{
    QString msg = "INSERT INTO `massag`.`history` (`date`, `label`, `program`, `job`, `operator`, `settings`, `counter`) VALUES (NOW(),";
    msg.append("'" + data.label + "',");
    msg.append("'" + data.prog + "',");
    msg.append("'" + data.job + "',");
    msg.append("'" + data.user + "',");
    msg.append("'" + data.settings + "',");
    msg.append("'" + QString::number(data.productCounter,10) + "')");

    //qDebug(msg.toUtf8().data());

    QSqlQuery query;
    bool ok = query.exec(msg);
    if(!ok)
    {
        QString serror = "query.exec() Error: " + query.lastError().text();
        qDebug(serror.toUtf8().data());
    }
    else
        emit statusDatabase();
}

void ReportView::writeSettings()
{
    QString s("");
    QSettings settings("QT", "JetPrinterUI");
    settings.beginGroup("ReportView");

    for(int i = 0; i<ui->tableWidget->columnCount(); i++)
    {
        int width = ui->tableWidget->columnWidth(i);

        settings.setValue("width" + QString::number(i), width);
    }

    settings.endGroup();
}

void ReportView::readSettings()
{
    QString s("");
    QSettings settings("QT", "JetPrinterUI");

    settings.beginGroup("ReportView");

    for(int i = 0; i<ui->tableWidget->columnCount(); i++)
    {
        int width = settings.value("width" + QString::number(i), 150).toInt();

        ui->tableWidget->setColumnWidth(i, width);
    }


    settings.endGroup();
}
