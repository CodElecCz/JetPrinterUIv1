#include "loadview.h"
#include "ui_loadview.h"

LoadView::LoadView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoadView)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
}

LoadView::~LoadView()
{
    delete ui;
}

void LoadView::setText(QString msg)
{
    ui->label_3->setText(msg);
    QApplication::processEvents();
}
