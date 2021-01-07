#ifndef PLCVIEW_H
#define PLCVIEW_H

#include <QWidget>
#include <QToolBar>
#include <QModelIndex>
#include "OMC8000Lib/OMC8000Lib.h"
#include "plcviewmodel.h"

namespace Ui {
class PlcView;
}

class PlcView : public QWidget
{
    Q_OBJECT

public:
    explicit PlcView(QWidget *parent = 0);
    ~PlcView();

    void start();
    void stop();

    void viewColumn(int column);
    void hideColumn(int column, bool hide = false);

    void loadSettings(QString fileName);
    void closeSettings();

    OMC8000Lib* getLib() {return omc8000Lib;}

    //plc screen
    void setStatusVar(QString sVar) {this->statusVar = sVar;}
    void setColorVar(QString sVar) {this->colorVar = sVar;}
    void setInsertVar(QString sVar) {this->insertVar = sVar;}
    void setLengthVar(QString sVar) {this->lengthVar = sVar;}
    void setTotalStopVar(QString sVar) {this->totalStopVar = sVar;}
    void delayStartOfPlc(int delay);

    //plc program
    void setProgramVar(QString sVar) {this->programVar = sVar;}
    void setSettingsVar(QString sVar) {this->settingsVar = sVar;}
    void setProgramNb(UINT sVar) {this->programNb = sVar;}

    //print log
    void setCounterVar(QString sVar) {this->counterVar = sVar;}

private:
    void initialize();
    void writeSettings();
    void readSettings();

    void OnValueChanged(INode* node, UINT32 val);

signals:
    //plc screen
    void plcTextStatus(QString text);
    void plcTextInsert(QString text);
    void plcTextLength(QString text);
    void plcColorInsert(QColor color);
    void plcColorLength(QColor color);
    void plcColorSensor1(QColor color);
    void plcColorSensor2(QColor color);
    void plcColorSensor3(QColor color);
    //print log
    void plcLog(UINT productCounter);
    //toolbar status
    void statusPlc(QString state = "");

public slots:
    void plcProgram(UINT settings);

private slots:
    void on_treeView_clicked(const QModelIndex &index);

    //plc screen
    void getTextStatus(QVariant var);
    void getTextInsert(QVariant var);
    void getTextLength(QVariant var);
    void getTotalStopDialog(QVariant var);
    void getColor(QVariant var);
    void delayStart();

    //print log
    void getPlcLog(QVariant var);

private:
    Ui::PlcView         *ui;
    PlcViewModel        *plcViewModel;
    OMC8000Lib          *omc8000Lib;

    //plc screen
    QString             statusVar;
    QString             insertVar;
    QString             lengthVar;
    QString             totalStopVar;
    QString             colorVar;
    QTimer              *timerVar;

    //plc program
    QString             programVar;
    QString             settingsVar;
    UINT                programNb;

    //print log
    QString             counterVar;
};

#endif // PLCVIEW_H
