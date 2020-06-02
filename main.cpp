#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(JetPrinterUI);

    QApplication a(argc, argv);
    QStringList args = a.arguments();

    MainWindow w((args.size()>1)? args[1]:QString(""));
    w.show();

    return a.exec();
}
