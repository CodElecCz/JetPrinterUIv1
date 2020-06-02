#ifndef REPORTVIEW_H
#define REPORTVIEW_H

#include <QWidget>
#include <QToolBar>
#include <QModelIndex>
#include <QDate>

namespace Ui {
class ReportView;
}

typedef struct SLogPrintFinish
{
    QString prog;
    QString job;
    QString settings;
    QString label;
    QString user;
    uint productCounter;
}SLogPrintFinish;

class ReportView : public QWidget
{
    Q_OBJECT

public:
    explicit ReportView(QWidget *parent = 0);
    ~ReportView();

    void initialize();

private:

    //save aplication data
    void writeSettings();
    void readSettings();

signals:
    void statusDatabase(QString state = "");

public slots:
    //database log
    void logPrintFinished(SLogPrintFinish data);

private slots:
    void calendarClicked(QDate);
    void listChanged(QString);

private:
    Ui::ReportView *ui;
};

#endif // REPORTVIEW_H
