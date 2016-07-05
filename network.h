/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         network.h
//
// erstellt am:      18.06.2015
// zuletzt geändert: 19.10.2015
// ****************************************************************************
*/

#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QNetworkReply>
#include <QNetworkRequest>

class Network : public QObject
{
    Q_OBJECT

public:
    explicit Network(QObject *parent = 0);

	void Download(QString filenameData, QString Protokoll, QString URL, QString Username, QString Password);
    bool downloadFinished(void);
    bool ERROR_D(void);

signals:
    void finished(void);
	void failed(void);

public slots:

    void replyFinished (QNetworkReply *reply);

private slots:

private:

    QNetworkAccessManager *manager;
    bool DownloadERROR;
    QString filenameDataName;
    bool downloaded = false;
    QNetworkReply *networkReply;
};

#endif // NETWORK_H
