/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan Wögerbauer
// Filename:         network.cpp
// Zweck:            Klasse: Daten aus dem Internet herunterladen
//
// erstellt am:      18.06.2015
// zuletzt geändert: 29.09.2015
// ****************************************************************************
*/

#include "network.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QFile>
#include <QUrl>
#include <QDebug>
#include <QMessageBox>

Network::Network(QObject *parent) :
    QObject(parent)
{
}

void Network::Download(QString filenameData, QString Protokoll, QString URL, QString Username, QString Password)                //download File from the internet
{
	downloaded = false;

	manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));   //If dowonload is finished write into file

	QUrl url = (Protokoll + URL + QString(filenameData));
	url.setUserName(Username);
	url.setPassword(Password);

	networkReply = manager->get(QNetworkRequest(url));
	filenameDataName=filenameData;          //filename for the downloaded data in reply Finished
}

void Network::replyFinished (QNetworkReply *reply)              //write downloaded Data in file
{
	DownloadERROR = false;

    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "ERROR!";
        qDebug() << reply->errorString();
        qDebug() << reply->error();

		DownloadERROR = true;
        emit failed();          //emit signal
    }
    else
    {
        QFile *file = new QFile(QString("/storage/emulated/0/HWWS/Daten/") + QString(filenameDataName));
        if(file->open(QFile::WriteOnly))
        {
            file->write(reply->readAll());                                      //Write Data in file

            file->flush();
            file->close();
        }
        delete file;
        qDebug()<< filenameDataName <<" Downloaded";
		DownloadERROR = false;

        emit finished();        //emit signal
    }
    reply->deleteLater();
}

bool Network::ERROR_D(void)
{
    return DownloadERROR;
}

bool Network::downloadFinished()
{
    return networkReply->isFinished();
}
