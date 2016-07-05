/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan Wögerbauer
// Filename:         main.cpp
// Zweck:            open the MainWindow
//
// erstellt am:      18.06.2015
// zuletzt geändert: 07.06.2015
// ****************************************************************************
*/

#include "mainwindow.h"
#include <QApplication>
#include <QStyle>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QApplication::setStyle("windows");
    MainWindow w;

    w.show();

    return a.exec();
}
