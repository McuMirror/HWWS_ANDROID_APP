/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         dialogfirstuse.cpp
// Zweck:            short program description, warnings
//
// erstellt am:      26.10.2015
// zuletzt geändert: 21.12.2015
// ****************************************************************************
*/

#include "dialogfirstuse.h"
#include "ui_dialogfirstuse.h"

#include <QString>

DialogFirstUse::DialogFirstUse(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFirstUse)
{
    ui->setupUi(this);

	QString style = "QScrollBar:horizontal {min-height: 30px;}" "QScrollBar:vertical {min-width: 30px;}";
	ui->textBrowser->setStyleSheet(style);
	ui->textBrowser->setReadOnly(true);

    ui->pushButton_accept->setText("Akzeptieren / OK");
    ui->pushButton_accept->setMaximumHeight(65);

	ui->textBrowser->setText("!!! ACHTUNG !!! \nDaten sind ohne Gewähr \nMit dem Verwenden dieser App nehmen sie die unten "
							 "bereitgestellten Informationen zur Kenntnis"                         //short description
                             "\n\nIm Tab 'Warnungen' können Warnungen erstellt werden, bei welchen die App automatisch eine Benachrichtigung sendet."
							 " Wichtig dabei ist, dass die richtige Einstellung für die Warnung (bei Überschreitung bzw. Unterschreitung) verwendet wird."
							 "\n\nEine Warnung wird gesendet sobald einer von zwei Sensorwerten über/unter dem angegebenen Wert liegt"
                             "\n\nIm Tab 'Aktuell' wird der aktuelle Mittelwert beider Sensoren angezeigt. Die Abweichung wird aus den Werten "
							 "der Sensoren berechnet"
                             "\nBeide Sensoren werden im Diagramm mit unterschiedlichen Farben Dargestellt:\n\nLaser-Sensor 1: grün\nLaser-Sensor 2: blau"
                             "\n\nIst die letzte Messung länger als 5 Minuten alt, so wird der letzte Messwert rot angezeigt"
                             "\n\nBevor Sie diese App nutzen können müssen Sie sich im Tab 'Menü' einloggen! \nNach der Installation muss das Handy gegebenfalls neu gestartet werden."
                             "\nIn den Benachrichtigungen wird der aktuelle Status der APP angezeigt."
							 "\n\nDiese App verwendet Daten von einem Server, dies bedeutet, dass Daten aus dem Internet heruntergeladen werden, "
							 "dies kann zusätzliche Kosten verursachen");
}

DialogFirstUse::~DialogFirstUse()
{
    delete ui;
}

void DialogFirstUse::on_pushButton_accept_clicked()
{
    QDialog::accept();
}
