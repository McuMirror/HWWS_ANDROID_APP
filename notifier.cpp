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
