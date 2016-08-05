#-------------------------------------------------
#
# Project created by QtCreator 2015-06-18T15:02:54
#
#-------------------------------------------------

QT       += core gui
QT	 += network
QT	 += androidextras

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET =    HWWS-APP
TEMPLATE =  app

SOURCES +=  main.cpp\
	    mainwindow.cpp \
	    addwarningwindow.cpp \
	    graph.cpp \
	    logindialog.cpp \
	    network.cpp \
	    dialogwaiting.cpp \
	    notifier.cpp \
	    dialogfirstuse.cpp \
	    warning.cpp

HEADERS  += mainwindow.h \
	    addwarningwindow.h \
	    graph.h \
	    logindialog.h \
	    network.h \
	    dialogwaiting.h \
	    notifier.h \
	    dialogfirstuse.h \
	    warning.h

FORMS    += mainwindow.ui \
	    addwarningwindow.ui \
	    logindialog.ui \
	    dialogwaiting.ui \
	    dialogfirstuse.ui \

CONFIG +=   mobility
MOBILITY = 

OTHER_FILES += \
	    android-sources/src/org/qtproject/example/notification/NotificationClient.java \
	    android-sources/AndroidManifest.xml \
	    TODO.txt \
            Description.txt \
            android-sources/src/org/qtproject/example/notification/StartServiceAfterBootReceiver.java \
            android-sources/src/org/qtproject/example/notification/Download_Check_Service.java \
            android-sources/src/org/qtproject/example/notification/FTP_Download.java \
            android-sources/src/org/qtproject/example/notification/MessageNotification.java \
            android-sources/src/org/qtproject/example/notification/NewMessageNotification_steady.java

RESOURCES +=

DISTFILES += \
            android-sources/src/org/qtproject/example/notification/Ping.java

