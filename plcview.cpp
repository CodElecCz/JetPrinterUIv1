#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QSignalMapper>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include <QMetaObject>
#include "mainwindow.h"
#include "plcview.h"
#include "ui_plcview.h"
#include "ui_warningdialog.h"
#include "spinboxdelegate.h"

typedef struct SPlcViewStatus
{
    const char* msg;
    UINT16 status;
} PlcViewStatus;

static const PlcViewStatus PlcViewStatusDef[] =
{
    {"Čekám na založení",   0},
    {"Založeno",            1},
    {"Čekám na ustálení",   2},
    {"Kontroluji senzory",  3},
    {"Nastavuji tisk",      4},
    {"Čekám na tlačítko",   5},
    {"Tisknu",              6},
    {"Čekám na dokončení",  7},
    {"Odeber tyč",          100},
    {"Chybný stav senzorů", 0xffff}
};

static const char* PlcViewInsertDef[]
{
    "Nezaloženo",
    "Založeno"
};

static const PlcViewStatus PlcViewLengthDef[]
{
    {"Krátká", 64},
    {"Dlouhá", 192}
};

static QColor PlcViewStatusColor[]
{
    Qt::red,    //NOK
    Qt::green,  //OK
    Qt::gray    //ignored
};

PlcView::PlcView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlcView)
{
    ui->setupUi(this);

    omc8000Lib = new OMC8000Lib();
    plcViewModel = new PlcViewModel(nullptr, QString(""), ui->treeView);

    ui->treeView->setModel(this->plcViewModel);
    ui->treeView->setItemDelegateForColumn(PlcViewTreeColumn_Write, new SpinBoxDelegate());

    initialize();
    readSettings();
}

PlcView::~PlcView()
{
    stop();
    writeSettings();
    delete ui;
}

void PlcView::initialize()
{
    QString plcFilePath = QCoreApplication::applicationDirPath() + "/plc.xml";

    QFile plcFile(plcFilePath);
    if(!plcFile.exists())
    {
        QString msg = "Plc file not found: " + plcFilePath;
        QMessageBox::question(this, "Warning", msg, QMessageBox::Ok);
    }

    loadSettings(plcFilePath);
}

void PlcView::writeSettings()
{
    QString s("");
    QSettings settings("QT", "JetPrinterUI");
    settings.beginGroup("PLCTreeView");

    for(int i = 0; i<PlcViewTreeColumnSize; i++)
    {
        s.sprintf("%d", i);
        settings.setValue(s, ui->treeView->columnWidth(i));
    }

    settings.endGroup();
}

void PlcView::readSettings()
{
    const int def[PlcViewTreeColumnSize] = {200,100,50,50,80,80,150,400};
    QString s("");
    QSettings settings("QT", "JetPrinterUI");

    settings.beginGroup("PLCTreeView");

    for(int i = 0; i<PlcViewTreeColumnSize; i++)
    {
        s.sprintf("%d", i);
        int val = settings.value(s, def[i]).toInt();
        ui->treeView->setColumnWidth(i, val);
    }
    settings.endGroup();
}

void PlcView::loadSettings(QString fileName)
{
    if(!fileName.isNull() && !fileName.isEmpty())
    {
        closeSettings();

        this->plcViewModel->loadModelData(omc8000Lib, fileName);

        ui->treeView->collapseAll();
    }
}

void PlcView::closeSettings()
{
    stop();

    PlcViewModel *model = new PlcViewModel(nullptr, QString(""), ui->treeView);
    ui->treeView->setModel(model);
    ui->treeView->reset();

    delete this->plcViewModel;
    this->plcViewModel = model;

    omc8000Lib->Clear();
}

void PlcView::on_treeView_clicked(const QModelIndex &index)
{
    try
    {
        int column = index.column();
        switch(column)
        {
            case PlcViewTreeColumn_Write:
                {
                    PlcViewItem *item = static_cast<PlcViewItem*>(index.internalPointer());
                    if(item!=NULL)
                    {
                        INode *node = item->node();
                        if(node!=NULL)
                        {
                            bool val;

                            switch(node->Type())
                            {
                                case NodeType_Bit:
                                    node->Write(&val);
                                    node->Write(!val);
                                    break;
                            }
                        }
                    }
                }
                break;
        }
    }
    catch(INodeException& e)
    {
        QMessageBox::warning(this, "Warning", QString(e.what()));
    }
}

void PlcView::start()
{
    try
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        omc8000Lib->Start(true);
    }
    catch(INodeException& e)
    {
        QApplication::restoreOverrideCursor();

        QMessageBox::warning(this, "Warning", QString(e.what()));
    }
    QApplication::restoreOverrideCursor();
}

void PlcView::stop()
{
    try
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        omc8000Lib->Stop(true);
    }
    catch(INodeException& e)
    {
        QApplication::restoreOverrideCursor();

        QMessageBox::warning(this, "Warning", QString(e.what()));
    }
    QApplication::restoreOverrideCursor();
}

void PlcView::getTextStatus(QVariant var)
{
    QString stat = "Nedefinovaný";
    size_t nb = sizeof(PlcViewStatusDef)/sizeof(PlcViewStatusDef[0]);
    int i;

    for(i = 0; i<nb; i++)
    {
        if(PlcViewStatusDef[i].status==var.toUInt())
            break;
    }

    emit plcTextStatus((i<nb)? PlcViewStatusDef[i].msg : stat);

    emit statusPlc("");
}

void PlcView::getTextInsert(QVariant var)
{
    bool bval = var.toBool();

    emit plcTextInsert((bval)? PlcViewInsertDef[1] : PlcViewInsertDef[0]);
    emit plcColorInsert((bval)? PlcViewStatusColor[1] : PlcViewStatusColor[0]);

    emit statusPlc("");
}

void PlcView::getTextLength(QVariant var)
{
    QString smask = this->lengthVar.split("|")[1];
    BYTE mask = smask.toUInt(nullptr, 2);

    QString stat = "Nerozpoznána";
    size_t nb = sizeof(PlcViewLengthDef)/sizeof(PlcViewLengthDef[0]);
    int i;

    for(i = 0; i<nb; i++)
    {
        if(PlcViewLengthDef[i].status==(UINT16)(var.toUInt()&mask))
            break;
    }

    emit plcTextLength((i<nb)? PlcViewLengthDef[i].msg : stat);

    emit statusPlc("");
}


void PlcView::getColor(QVariant var)
{
    UINT32_UT val32;
    val32.dword = var.toUInt();

    if(val32.byte.byte0<3)
        emit plcColorLength( PlcViewStatusColor[val32.byte.byte0]);
    if(val32.byte.byte1<3)
        emit plcColorSensor1(PlcViewStatusColor[val32.byte.byte1]);
    if(val32.byte.byte2<3)
        emit plcColorSensor2( PlcViewStatusColor[val32.byte.byte2]);
    if(val32.byte.byte3<3)
        emit plcColorSensor3( PlcViewStatusColor[val32.byte.byte3]);

    emit statusPlc("");
}

void PlcView::getPlcLog(QVariant var)
{
    UINT counter = var.toUInt();

    emit plcLog(counter);

    emit statusPlc("PLC: připojeno");
}

void PlcView::getTotalStopDialog(QVariant var)
{
    bool val = var.toBool();

    qDebug()<<"TotalStop triggered [" + QString::number(var.toUInt()) + "]";

    if(val)
    {
        QDialog *dlg = new QDialog(0,0);
        Ui_WarningDialog warningUi;
        warningUi.setupUi(dlg);
        dlg->show();
    }
}

void PlcView::delayStartOfPlc(int delay)
{
    timerVar = new QTimer(this);
    timerVar->setSingleShot(true);

    connect(timerVar, SIGNAL(timeout()), this, SLOT(delayStart()));

    timerVar->start(delay);
}

void PlcView::delayStart()
{
    QStringList varList;
    varList<<statusVar.split("|")[0];
    varList<<insertVar.split("|")[0];
    varList<<lengthVar.split("|")[0];
    varList<<colorVar.split("|")[0];
    varList<<counterVar.split("|")[0];
    varList<<totalStopVar.split("|")[0];

    try
    {
        UINT32 val = 0;
        foreach(QString var, varList)
        {
            INode *node = getLib()->GetNodeInterfaceFromPath(var.toLatin1().data());

            //init
            node->Read(&val);
            OnValueChanged(node, val);
            //callback
            node->OnValueChanged += event_handler(this, &PlcView::OnValueChanged);
        }

        emit statusPlc("PLC: připojeno");
    }
    catch(INodeException& e)
    {
        QMessageBox::warning(this, "Warning", QString(e.what()));

        emit statusPlc("PLC: chyba");
    }
}


void PlcView::OnValueChanged(INode* node, UINT32 val)
{
    QString *nodeId = new QString(node->Id());
    //qDebug()<<"PlcViewModel::OnValueChanged() [" + *nodeId + "] threadId:0x" + QString::number((int)thread()->currentThreadId(), 16);

    if (thread()!=QThread::currentThread())
    {
        if(statusVar.contains(*nodeId))
        {
            QVariant *var = new QVariant(val);
            QGenericArgument arg("QVariant", var);
            QMetaObject::invokeMethod(this, "getTextStatus", arg);
        }
        else if(colorVar.contains(*nodeId))
        {
            QVariant *var = new QVariant(val);
            QGenericArgument arg("QVariant", var);
            QMetaObject::invokeMethod(this, "getColor", arg);
        }
        else if(insertVar.contains(*nodeId))
        {
            QVariant *var = new QVariant(val);
            QGenericArgument arg("QVariant", var);
            QMetaObject::invokeMethod(this, "getTextInsert", arg);
        }
        else if(lengthVar.contains(*nodeId))
        {
            QVariant *var = new QVariant(val);
            QGenericArgument arg("QVariant", var);
            QMetaObject::invokeMethod(this, "getTextLength", arg);
        }
        else if(counterVar.contains(*nodeId))
        {
            QVariant *var = new QVariant(val);
            QGenericArgument arg("QVariant", var);
            QMetaObject::invokeMethod(this, "getPlcLog", arg);
        }
        else if(totalStopVar.contains(*nodeId))
        {
            QVariant *var = new QVariant(val);
            QGenericArgument arg("QVariant", var);
            QMetaObject::invokeMethod(this, "getTotalStopDialog", arg);
        }
    }
    else
    {
        if(statusVar.contains(*nodeId))
        {
            emit getTextStatus(QVariant(val));
        }
        else if(colorVar.contains(*nodeId))
        {
            emit getColor(QVariant(val));
        }
        else if(insertVar.contains(*nodeId))
        {
            emit getTextInsert(QVariant(0));
        }
        else if(lengthVar.contains(*nodeId))
        {
            emit getTextLength(QVariant(val));
        }
    }
}

void PlcView::plcProgram(UINT settings)
{
    try
    {
        UINT32_UT pval, sval;

        if(settings==0)
        {
            pval.dword = 0;
            getLib()->Write(this->programVar.toLatin1().data(), pval.word.word0);
        }
        else
        {
            pval.dword = this->programNb;
            getLib()->Write(this->programVar.toLatin1().data(), pval.word.word0);

            sval.dword = settings;
            getLib()->Write(this->settingsVar.toLatin1().data(), sval.byte.byte0);
        }
    }
    catch(INodeException& e)
    {
        QMessageBox::warning(this, "Warning", QString(e.what()));
    }
}

void PlcView::viewColumn(int column)
{
    bool isHidden = ui->treeView->isColumnHidden(column);
    ui->treeView->setColumnHidden(column, !isHidden);
    if(isHidden)
    {
        int width = ui->treeView->columnWidth(column);
        if(width<10)
            ui->treeView->setColumnWidth(column, 100);
    }
}

void PlcView::hideColumn(int column, bool hide)
{
    if(!hide)
    {
        int width = ui->treeView->columnWidth(column);
        if(width<10)
            ui->treeView->setColumnWidth(column, 100);
    }
    ui->treeView->setColumnHidden(column, hide);
}


