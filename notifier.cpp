/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan Wögerbauer
// Filename:         notifier.cpp
// Zweck:            make the notification
//
// erstellt am:      24.10.2015
// zuletzt geändert: 24.10.2015
// ****************************************************************************
*/

#include "notifier.h"
#include <QDebug>
#include <QString>
#include <QAndroidJniObject>

Notifier::Notifier(QObject *parent) :
    QObject(parent)
{
}

void Notifier::notification(QString message)
{
    //message
    QAndroidJniObject javaNotification = QAndroidJniObject::fromString(message);
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "notify",
                                              "(Ljava/lang/String;)V",
                                              javaNotification.object<jstring>());
}
