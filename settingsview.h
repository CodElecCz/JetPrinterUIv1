#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include <QWidget>
#include <QToolBar>
#include <QModelIndex>
#include <QSettings>
#include "programview.h"

namespace Ui {
class SettingsView;
}

class SettingsView : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsView(QWidget *parent = 0);
    explicit SettingsView(QSettings *settings, QWidget *parent = 0);
    ~SettingsView();

    //settings.ini
    void writeSettings(QSettings *settings);
    void readSettings(QSettings *settings);

private:
    void initialize(QSettings *settings);

signals:
   void settingsChanged(QString group);

public slots:

private slots:
    void settingsProgramChanged(QString group);

private:
    Ui::SettingsView    *ui;
    QList<ProgramView*>  programViewList;
};

#endif // SETTINGSVIEW_H
