/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         mainwindow.h
//
// erstellt am:      18.06.2015
// zuletzt geändert: 27.02.2015
// ****************************************************************************
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QTime>
#include <QSettings>

#include <notifier.h>
#include <warning.h>
#include <logindialog.h>
#include <dialogwaiting.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_add_clicked(void);
    void on_pushButton_delete_clicked(void);
    void chooseFilesForDownload (void);
    void delay(int millisecondsToWait);
    void updateGraphData (void);
	void downlodFromServerFailed(void);
    void saveWarnings (void);
	void deleteOldData (void);
    void on_tabWidget_currentChanged(int index);
    void checkWarnings (void);
    void login(void);
    void on_pushButton_login_clicked(void);
    void makeLayout(void);
    void readFromQSettings(void);
    void makeDirectories(void);
	void on_pushButton_shoWarningValues_clicked(void);
	void on_pushButton_showBand_clicked(void);
	void on_pushButton_edit_clicked(void);
    void on_pushButton_simulate_clicked(void);
	void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_description_clicked(void);
    void on_pushButton_info_clicked(void);
	void download_ConfigFile(void);
	void setConfigFileSettings(void);
    void on_pushButton_BackroundColor_clicked(void);
	void LoginFailed(void);
	void DownloadSettingsFailed(void);
	void saveWarningFile(void);
	void readWarningFile(void);
    void writeLoginDataFile(QString user, QString password, QString url, QString showNotification);
    void on_pushButton_downloadDefaultWarnings_clicked(void);
    void writeTableWidgetWarnings(void);
    void checkWarningAttentions(void);
    void save_safty_diff(void);
    void on_comboBox_attentionBandwidth_currentIndexChanged(int index); 
    void on_pushButton_showHeightAsNotification_clicked();
    double calculatePitch(QList<int> values, QList<int> x_minute, double numberOfValues);

private:
    Ui::MainWindow *ui;

    QTimer *Timer;          //Updates the clock in status bar
    QTimer *updateTimer;    //Updates the data of the value

    int lastValue_C;
    int lastValue_L;
    int shownDays=1;
    int hour_last_value;
    int minute_last_value;
    int temp_last_value;
	int sensorHeight;
    int safety_diff = 5;

    QSettings settings;

	QList<int> WarningHeights;          //Warning value
	QList<Warning*> Warnings;

	bool downloadFail = false;
    bool LoginState;
    bool updating;
	bool DownloadSettingsState = false;

	QString version;
	QString Username;
    QString Password;
    QString IP;
	QString location;

    double steigung_C;
    double steigung_L;

    double steigung_C_5min;
    double steigung_L_5min;

    Notifier* notificationClient = new Notifier;
	LoginDialog* L = new LoginDialog;          //show login-field
	DialogWaiting* DW = new DialogWaiting;

public slots:

signals:
    void readingFinished(void);

};

#endif // MAINWINDOW_H
