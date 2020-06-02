#ifndef JETPRINTERVIEW_H
#define JETPRINTERVIEW_H

#include <QWidget>
#include <QToolBar>
#include <QModelIndex>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QListWidget>
#include <QButtonGroup>
#include "JetPrinterLib/JetPrinterProtocol.h"

namespace Ui {
class JetPrinterView;
}

class JetPrinterView : public QWidget
{
    Q_OBJECT

public:
    explicit JetPrinterView(QWidget *parent = 0);
    ~JetPrinterView();

    void jobList(QStringList *jobList);
    bool initializeSer(QString ser, int baudRate);

private:
    void initialize();

    void writeSettings();
    void readSettings();

    void labelList();
    void labelPreviewDemo();
    void labelPreview(QStringList script);

    void scriptRead(QString jobFileName, QString* script);

signals:
    //toolbar status
    void statusPrinter(QString state = "");
    //main window printer status
    void printerState(QString state);

public slots:
    void printerProgram(QString job); //program selected
    void printerExText(QString text); //external text
    void printerCounterSet(int val);
    void printerCounterReset()
    {
        printerCounterSet(0);
    }

private slots:
    void labelActivated(QString);
    void printerCmd(int index);
    void printeInfo();

private:
    Ui::JetPrinterView *ui;

    QGraphicsScene *scene;

    QString family5x5;
    QString family7x5;
    QString jobsFilePath;

    JetPrinterProtocol *jetPrinter;
    UINT nozzleStatus;
    UINT printerStatus;
    bool refreshLoadedJob;

    QTimer          *timer;
    QButtonGroup    *buttons;
};

#endif // JETPRINTERVIEW_H
