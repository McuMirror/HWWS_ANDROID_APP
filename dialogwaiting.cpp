#include "dialogwaiting.h"
#include "ui_dialogwaiting.h"

#include <QWidget>
#include <QApplication>

DialogWaiting::DialogWaiting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWaiting)
{
    ui->setupUi(this);

    QFont defaultHeight;
    defaultHeight.setPointSize(20);

    ui->label->setText("Bitte Warten: Daten werden geladen");
    ui->label->setFont(defaultHeight);
}

DialogWaiting::~DialogWaiting()
{
    delete ui;
}
