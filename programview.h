#ifndef PROGRAMVIEW_H
#define PROGRAMVIEW_H

#include <QWidget>
#include <QToolBar>
#include <QModelIndex>
#include <QBitArray>
#include <QSettings>

namespace Ui {
class ProgramView;
}

class ProgramView : public QWidget
{
    Q_OBJECT

public:
    explicit ProgramView(QWidget *parent = 0);
    explicit ProgramView(QSettings *settings, QString group, QWidget *parent = 0);
    ~ProgramView();

    //settings.ini
    void writeSettings(QSettings *settings);
    void readSettings(QSettings *settings);

private:
    void initialize();

signals:
    void settingsChanged(QString group);

public slots:

private slots:
    void stateChanged(int) { settingsChanged(group); }
    void textChanged(QString) { settingsChanged(group); }

public:
    QString group;

private:
    Ui::ProgramView *ui;

};

#endif // PROGRAMVIEW_H
