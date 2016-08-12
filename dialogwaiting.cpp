/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         dialogwaiting.cpp
// Zweck:            UI
//
// erstellt am:      24.10.2015
// zuletzt geändert: 24.10.2015
// ****************************************************************************
*/

#include "dialogwaiting.h"
#include "ui_dialogwaiting.h"

#include <QWidget>
#include <QApplication>
#include <QScreen>

DialogWaiting::DialogWaiting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWaiting)
{
    ui->setupUi(this);

	QScreen *screen = QApplication::screens().at(0);
	//int screenWidth = screen->availableSize().width();
	int screenHeight = screen->availableSize().height();

    QFont defaultHeight;
	defaultHeight.setPointSize(screenHeight / 60);

    ui->label->setText("Bitte Warten: Daten werden geladen");
    ui->label->setFont(defaultHeight);
}

DialogWaiting::~DialogWaiting()
{
    delete ui;
}
