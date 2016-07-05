/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan Wögerbauer
// Filename:         notifier.h
// Zweck:            make the notificaion
//
// erstellt am:      24.10.2015
// zuletzt geändert: 24.10.2015
// ****************************************************************************
*/

#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QObject>
#include <QString>

class Notifier : public QObject
{
    Q_OBJECT
public:
    explicit Notifier(QObject *parent = 0);

signals:

public slots:
    void notification(QString message);

};

#endif // NOTIFIER_H
