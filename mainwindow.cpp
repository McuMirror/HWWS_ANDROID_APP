/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         mainwindow.cpp
// Zweck:            controlling the app (logic)
//
// erstellt am:      18.06.2015
// zuletzt geändert: 01.08.2016
// ****************************************************************************
*/


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addwarningwindow.h"
#include "graph.h"
#include "logindialog.h"
#include "network.h"
#include "notifier.h"
#include "dialogfirstuse.h"
#include "warning.h"
#include "dialogwaiting.h"

#include <QTabWidget>
#include <QString>
#include <QMessageBox>
#include <QLineEdit>
#include <QFile>
#include <QSaveFile>
#include <QDebug>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDialog>
#include <QTableView>
#include <QTime>
#include <QList>
#include <QSettings>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDir>
#include <QScrollBar>
#include <QAbstractScrollArea>
#include <QUdpSocket>

//included to use Java code
#include <QtAndroidExtras/QAndroidJniObject>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

    //*************************************** Timer Initialisierung ********************************

    updateTimer = new QTimer(this);
    updateTimer->start(60000);      //update data every minute
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(chooseFilesForDownload()));

    //**********************************************************************************************

    if(settings.value("FirstUse", "").toBool() == false) //show DialogFirstUse if the app is started first time
    {
        while(settings.value("FirstUse","").toBool() == false)
        {
            DialogFirstUse* DFU = new DialogFirstUse;

            if(DFU->exec() == QDialog::Accepted)
            {
                settings.setValue("FirstUse", true);
            }
        }
    }

    makeLayout();               //set the layout, texts
    makeDirectories();          //make directories on device if the aren't existing
    readWarningFile();
    readFromQSettings();        //read stored data
    writeTableWidgetWarnings();
    deleteOldData();            //delete data if it's older than 10 days


	if(LoginState == true)
	{
		updating = true;
		chooseFilesForDownload();
		ui->label_loginStatus->setText("angemeldet");
		ui->label_Url->setText(IP);
		ui->label_Username->setText(Username);
		ui->pushButton_login->setText("Abmelden");
		ui->label_value->setText(QString("??? ") + QChar(0x00B1) + QString(" ???") + QString(" cm"));

        QUdpSocket socket;
		socket.connectToHost(IP, 20);		//FTP Port 20
		if (!socket.waitForConnected(1000))	//check connection to the server
		{
            QMessageBox::critical(this, "HWWS", "Es konnte keine Verbindung zum Server hergestellt werden!\n"
												"Bitte überprüfen Sie ihre Internetverbindung ansonsten funktioniert die App nicht");
        }
	}
	else
	{
		ui->label_value->setText("abgemeldet");
		ui->label_loginStatus->setText("abgemeldet");
		ui->label_Url->setText("-");
		ui->label_Username->setText("-");
        updating = false;                       //don't download any file if not logged in
	}
}

MainWindow::~MainWindow()
{
	deleteOldData();

	delete ui;
}

void MainWindow::makeLayout(void)
{
    qDebug()<<"Method: MakeLayout";

    QFont defaultHeight;
    defaultHeight.setPointSize(19);
    QFont headline;
    headline.setBold(true);
    headline.setPointSize(20);

    #define ButtonHeight 65
    #define MaxHeight 40
    #define MaxWidth 40

	//MainWindow
	ui->tabWidget->setTabText(0, "Aktuell");
	ui->tabWidget->setTabText(1, "Diagramm");
    ui->tabWidget->setTabText(2, "Warnungen");
	ui->tabWidget->setTabText(3, "Menü");

	//Tab Warnung
    ui->pushButton_downloadDefaultWarnings->setText("Standard wiederherstellen");
    ui->pushButton_downloadDefaultWarnings->setMaximumHeight(ButtonHeight);
	ui->pushButton_add->setText("Hinzufügen");
    ui->pushButton_add->setMaximumHeight(ButtonHeight);
	ui->pushButton_delete->setText("Löschen");
    ui->pushButton_delete->setMaximumHeight(ButtonHeight);
	ui->pushButton_edit->setText("Bearbeiten");
    ui->pushButton_edit->setMaximumHeight(ButtonHeight);
	ui->tableWidget_Warnings->setSortingEnabled(false);
	ui->tableWidget_Warnings->setColumnWidth(0, 410);
	ui->tableWidget_Warnings->setColumnWidth(1, 180);


    ui->label_savedWarningsText->setText("gespeicherte Warnungen:");
    ui->label_savedWarningsText->setFont(defaultHeight);
    ui->label_savedWarnings->setFont(defaultHeight);

	//Tab Diagramm
	ui->label_numberUpdateDays->setText("dargestellte Tage:");
    ui->label_numberUpdateDays->setFont(defaultHeight);

    ui->comboBox->addItem("1", 0);	//ComboBox shownDays
    ui->comboBox->addItem("2", 1);

    ui->comboBox->setMaximumHeight(50);

	//Tab Menü
    ui->textBrowser->setText("Daten sind ohne Gewähr\nDer Entwickler garantiert keine Richtigkeit der Daten oder Warnungen!"
                             " Er übernimmt auch keine Verantwortung für Folgeschäden");

	ui->labe_loginStatusText->setText("Login Status:");
    ui->labe_loginStatusText->setFont(defaultHeight);
    ui->label_loginStatus->setFont(defaultHeight);
	ui->label_Url_Text->setText("Url:");
    ui->label_Url->setFont(defaultHeight);
    ui->label_Url_Text->setFont(defaultHeight);
	ui->label_UsernameText->setText("Benutzername:");
    ui->label_UsernameText->setFont(defaultHeight);
    ui->label_Username->setFont(defaultHeight);
    ui->label_showWarningValuesGraphText->setText("Warnungswerte:");
    ui->label_showWarningValuesGraphText->setFont(defaultHeight);
    ui->label_Show_BandText->setText("Abweichung:");
    ui->label_Show_BandText->setFont(defaultHeight);
    ui->label_diagramColor->setText("Hintergrund:");
    ui->label_showHeightAsNotification->setText("aktuelle Höhe:");
    ui->label_showHeightAsNotification->setFont(defaultHeight);
    ui->label_diagramColor->setFont(defaultHeight);
    ui->label->setFont(headline);
    //ui->label->setAlignment(Qt::AlignCenter);
    ui->label->setText("Darstellung:");
    ui->label_headline_login->setFont(headline);
    //ui->label_headline_login->setAlignment(Qt::AlignCenter);
    ui->label_headline_login->setText("Server Login:");
    ui->label_headline_notification->setFont(headline);
    //ui->label_headline_notification->setAlignment(Qt::AlignCenter);
    ui->label_headline_notification->setText("Benachrichtigung & Voranzeige:");
    ui->pushButton_simulate->setText("Warnung simulieren");
    ui->pushButton_simulate->setMaximumHeight(ButtonHeight);
    ui->label_info->setFont(headline);
    ui->label_info->setText("Hilfe & Info:");
    ui->label_attentionBandwidthText->setText("Differenz:");
    ui->label_attentionBandwidthText->setFont(defaultHeight);

	ui->pushButton_login->setText("Anmelden");
    ui->pushButton_login->setMaximumHeight(ButtonHeight);
	ui->pushButton_shoWarningValues->setText("Anzeigen");
    ui->pushButton_shoWarningValues->setMaximumHeight(ButtonHeight);
	ui->pushButton_shoWarningValues->setCheckable(true);
	ui->pushButton_showBand->setText("Anzeigen");
    ui->pushButton_showBand->setMaximumHeight(ButtonHeight);
	ui->pushButton_showBand->setCheckable(true);
	ui->pushButton_BackroundColor->setText("weiß");
    ui->pushButton_BackroundColor->setMaximumHeight(ButtonHeight);
	ui->pushButton_BackroundColor->setCheckable(true);
    ui->pushButton_description->setText("Hilfe");
    ui->pushButton_description->setMaximumHeight(ButtonHeight);
    ui->pushButton_info->setText("Über");
    ui->pushButton_info->setMaximumHeight(ButtonHeight);
    ui->pushButton_showHeightAsNotification->setText("Anzeigen");
    ui->pushButton_showHeightAsNotification->setCheckable(true);
    ui->pushButton_showHeightAsNotification->setMaximumHeight(ButtonHeight);

    ui->comboBox_attentionBandwidth->addItem("5  cm", 0);
    ui->comboBox_attentionBandwidth->addItem("10 cm", 1);
    ui->comboBox_attentionBandwidth->addItem("15 cm", 2);
    ui->comboBox_attentionBandwidth->addItem("20 cm", 3);
    ui->comboBox_attentionBandwidth->addItem("25 cm", 4);

    ui->comboBox_attentionBandwidth->setMaximumHeight(50);

	//Tab Aktuell
	ui->label_temperatur_text->setText("Wassertemperatur:");
    ui->label_temperatur_text->setFont(defaultHeight);
	ui->label_time_last_value_text->setText("letzter gültiger Messwert:");
    ui->label_time_last_value_text->setFont(defaultHeight);
	ui->label_time_last_update_text->setText("letzte Aktualisierung:");
    ui->label_time_last_update_text->setFont(defaultHeight);
	ui->label_prognose_text->setText("Prognose:");
    ui->label_prognose_text->setFont(defaultHeight);

	ui->label_time_last_value->setText("-");
    ui->label_time_last_value->setFont(defaultHeight);
	ui->label_temperatur->setText("-");
    ui->label_temperatur->setFont(defaultHeight);
	ui->label_prognose->setText("-");
    ui->label_prognose->setFont(defaultHeight);
	ui->label_time->setText("-");
    ui->label_time->setFont(defaultHeight);
	ui->label_numberOfActiveWarnings->setText("-");
    ui->label_numberOfActiveWarnings->setFont(defaultHeight);

	ui->label_warnings->setFont(headline);
	//ui->label_warnings->setAlignment(Qt::AlignCenter);
	ui->label_warnings->setText("AKTIVE WARNUNGEN:");

    //ui->scrollArea->setStyleSheet(style);       //Set style
    ui->scrollArea->horizontalScrollBar()->setMaximumHeight(MaxHeight);
    ui->scrollArea->verticalScrollBar()->setMaximumWidth(MaxWidth);
    //ui->tableWidget_Warnings->setStyleSheet(style);
    ui->tableWidget_Warnings->horizontalScrollBar()->setMaximumHeight(MaxHeight);
    ui->tableWidget_Warnings->verticalScrollBar()->setMaximumWidth(MaxWidth);
    //ui->textBrowser->setStyleSheet(style);
    ui->textBrowser->horizontalScrollBar()->setMaximumHeight(MaxHeight);
    //ui->listWidget_aktiveWarnings->setStyleSheet(style);
    ui->listWidget_aktiveWarnings->horizontalScrollBar()->setMaximumHeight(MaxHeight);
    ui->listWidget_aktiveWarnings->verticalScrollBar()->setMaximumWidth(MaxWidth);

	ui->tableWidget_Warnings->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->label_attention_text->setText("ACHTUNG:");
    ui->label_attention_text->setFont(headline);
    ui->label_NumberOfAttention->setFont(defaultHeight);

    ui->tableWidget_attention->setColumnWidth(0, 410);
    ui->tableWidget_attention->setColumnWidth(1, 180);
    ui->tableWidget_attention->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QFont font = ui->label_value->font();                                        //Set layout of label
	font.setPointSize(60);                                                       //set text height
	font.setBold(true);
	//ui->label_value->setAlignment(Qt::AlignCenter);
    ui->label_value->setFont(font);
}

void MainWindow::readFromQSettings(void)
{
    qDebug()<<"Method: readFromQSettings";

	//****************************************************************************************************

    LoginState = settings.value("LoginState","").toBool();
    Username = settings.value("Username","").toString();
    Password = settings.value("Password","").toString();
    IP = settings.value("IP", "").toString();

	//*****************************************************************************************************
	//Settings from Server

    location = settings.value("Location", "").toString();
	if(location.length() == 0)
	{
		location = "unbekannt";
	}
    version = settings.value("Version", "").toString();
	if(version.length() == 0)
	{
		version = "unbekannt";
	}

    sensorHeight = settings.value("SensorHeight","").toInt();
	if(sensorHeight == 0)
	{
		sensorHeight = 1;
	}
    qDebug()<<"sensorHeight" << sensorHeight;
	//******************************************************************************************************

	bool button;
    button = settings.value("State_WarningsButton", "").toBool();
	ui->pushButton_shoWarningValues->setChecked(button);
    if(button == true)
    {
        ui->pushButton_shoWarningValues->setText("Ausblenden");
    }
    button = settings.value("State_BandButton", "").toBool();
	ui->pushButton_showBand->setChecked(button);
    if(button == true)
    {
        ui->pushButton_showBand->setText("Ausblenden");
    }
    button = settings.value("Color", "").toBool();
	ui->pushButton_BackroundColor->setChecked(button);
	if(button == true)
	{
		ui->pushButton_BackroundColor->setText("schwarz");
	}
	ui->widget->setBackgroundColor(button);

    safety_diff = settings.value("saftyDiff", "").toInt();

    ui->comboBox_attentionBandwidth->setCurrentIndex((safety_diff/5)-1);

    if(settings.value("showHeightNotification","").toBool() == true)
    {
        ui->pushButton_showHeightAsNotification->setChecked(true);
        ui->pushButton_showHeightAsNotification->setText("Ausblenden");
    }
    else
    {
        ui->pushButton_showHeightAsNotification->setChecked(false);
        ui->pushButton_showHeightAsNotification->setText("Anzeigen");
    }
}

void MainWindow::makeDirectories(void)
{
    qDebug()<<"Method: makeDirectories";

	QDir create_1 ("/storage/emulated/0/HWWS");         //creating the folders if they're not existing
	if (!create_1.exists())
	{
		create_1.mkdir("/storage/emulated/0/HWWS");
	}

	QDir create_2 ("/storage/emulated/0/HWWS/Daten");
	if(!create_2.exists())
	{
		create_2.mkdir("/storage/emulated/0/HWWS/Daten");
	}
}

void MainWindow::login(void)
{
	DW->close();
    LoginState = true;
    updating = true;

    settings.setValue("LoginState", LoginState);
	qDebug()<<"loginSate: "<<LoginState;

	Username = L->returnUserName();
	Password = L->returnPassword();
    writeLoginDataFile(L->returnUserName(), L->returnPassword(), L->returnIP(), "false");

    settings.setValue("Username", L->returnUserName());
    settings.setValue("Password",L->returnPassword());
    settings.setValue("IP", IP);

	ui->label_loginStatus->setText("angemeldet");
	ui->label_Url->setText(IP);
	ui->label_Username->setText(Username);
	ui->pushButton_login->setText("Abmelden");
	L->clear(false, true, false);   //delete the password in die loginWindow

	ui->label_value->setText(QString("??? ") + QChar(0x00B1) + QString(" ???") + QString(" cm"));

	download_ConfigFile();
	chooseFilesForDownload();
}

void MainWindow::download_ConfigFile(void)
{
	if(LoginState == true)
	{
		QPalette pal;
		pal.setColor(QPalette::Background, Qt::green);
		DW->setPalette(pal);
		DW->show();

		Network* downloadConfigFile = new Network;

		downloadConfigFile->Download("config.txt", "ftp://", IP + QString("/hwws/"), Username, Password);
		connect(downloadConfigFile, SIGNAL(finished()), this, SLOT(setConfigFileSettings()));
		connect(downloadConfigFile, SIGNAL(failed()), this, SLOT(DownloadSettingsFailed()));
	}
}

void MainWindow::setConfigFileSettings(void)
{
	bool ok1 = false;
	bool ok2 = false;
	bool invalidFile = false;
	bool invalidConvert = false;
	bool openFail = false;
	int sensorHeight_read = 0;
	QString version_read = 0;
	QString location_read;

	QFile file(QString("/storage/emulated/0/HWWS/Daten/config.txt"));

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		openFail = true;
		qDebug() << "failed to open: Config File";
	}

	QTextStream inData(&file);     //read files
	while ( !inData.atEnd() )
	{
		QString line = inData.readLine();
		QStringList sl = line.split(QChar(':'));
		if(sl.length() != 3)         //if there is no data then ERROR
		{
			invalidFile = true;
		}

		sensorHeight_read = sl.at(0).toInt(&ok1);
		version_read = sl.at(1);
		location_read = sl.at(2);
	}
	file.close();

	if(!ok1 || !ok2 )
	{
		invalidConvert = true;
	}

	if(invalidFile == false || invalidConvert == false || openFail == false)
	{
		sensorHeight = sensorHeight_read;
		version = version_read;
		location = location_read;

		DownloadSettingsState = true;
		DW->close();
	}

	qDebug()<< "Sensorhöhe:" << sensorHeight;
	qDebug()<< "Version:" << version;
	qDebug()<< "Messstandort:" << location;

    settings.setValue("SensorHeight", sensorHeight);
    settings.setValue("Version", version);
    settings.setValue("Location", location);

	if(sensorHeight == 0)
	{
		download_ConfigFile();
	}
}

void MainWindow::on_pushButton_add_clicked(void)   //add new warning
{
	addWarningWindow A;
    QPalette pal;
    pal.setColor(QPalette::Background, Qt::green);
    A.setPalette(pal);

	if(A.exec() == QDialog::Accepted)
    {
        if(A.getValue() < -sensorHeight || A.getValue() > sensorHeight)
        {
            QMessageBox::critical(this, "HWWS", tr("Bitte geben Sie eine gültige 'kritische Höhe' ein! (min: -%1, max: %1)\n"
                                                   "Der aktuelle Wert kann zu einem nicht Auslösen der Warnung führen!").arg(sensorHeight));
        }

        int rows = ui->tableWidget_Warnings->rowCount();
		ui->tableWidget_Warnings->setRowCount(rows+1);             // increase row count
		ui->tableWidget_Warnings->setColumnCount(2);
		ui->tableWidget_Warnings->setRowHeight(rows, 60);
		ui->tableWidget_Warnings->columnWidth(340);

		QTableWidgetItem* nameItem = new QTableWidgetItem(A.getName());
		ui->tableWidget_Warnings->setItem(rows, 0, nameItem);

		QChar symb;
		if(A.getStateWarningAtOverUnderheight() == false)
		{
			symb = QChar(0x2191);
		}
		else
		{
			symb = QChar(0x2193);
		}

		QTableWidgetItem* valueItem = new QTableWidgetItem(QString::number(A.getValue()) + " " + symb);
        ui->tableWidget_Warnings->setItem(rows, 1, valueItem);



        Warnings.append(new Warning);
        Warnings.last()->setName(A.getName());
		Warnings.last()->setHeight(A.getValue());
		Warnings.last()->setStateWarningAtOverUnderheight(A.getStateWarningAtOverUnderheight());
		//qDebug()<<"new Warning"<< Warnings.last()->getTriggerState();

        saveWarningFile();
        ui->label_savedWarnings->setText(QString::number(ui->tableWidget_Warnings->rowCount()));


	}
}

void MainWindow::on_pushButton_delete_clicked(void)  //delete marked warning
{
	if((ui->tableWidget_Warnings->rowCount() != 0) && (ui->tableWidget_Warnings->selectedItems().size()==1))
	{
		int indexDelete = ui->tableWidget_Warnings->currentRow();
		QTableWidgetItem* ToDelete = ui->tableWidget_Warnings->item(indexDelete, 0);
		QString nameToDelete = ToDelete->text();        //name of door

		if(QMessageBox::question(this, tr("Warnung löschen"), tr("Wollen Sie die Warnung %1 wirklich löschen?").arg(nameToDelete)) == QMessageBox::Yes)
		{
			ui->tableWidget_Warnings->removeRow(ui->tableWidget_Warnings->currentRow());          //remove from QTableWidget
			qDebug()<<"delete Warning: "<< Warnings.at(indexDelete)->getName();
			Warnings.removeAt(indexDelete);
		}
		//saveWarnings();
		saveWarningFile();
	}
}

void MainWindow::chooseFilesForDownload(void)
{
	if(updating == true)
	{
		QUdpSocket socket;
		socket.connectToHost(IP, 20);		//FTP Port 20
		if (socket.waitForConnected(1000))	//check connection to the server
		{
			downloadFail = false;
			QString filename;
			QDate date = QDate::currentDate();

			qDebug()<<"shown days"<<shownDays;

			QList<Network*> networks;
			networks.clear();
			for(int i=0; i<shownDays; i++)
			{
				networks.append(new Network);
			}

			for(int i=0; i<shownDays; i++)
			{
				filename = QString("data_")+date.addDays(-i).toString("yyyy_MM_dd")+QString(".txt");

				networks.at(i)->Download(filename, "ftp://", IP + QString("/hwws/"), Username, Password);     //Download the file from ftp server
				connect(networks.at(i), SIGNAL(finished()), this, SLOT(updateGraphData()));
				connect(networks.at(i), SIGNAL(failed()), this, SLOT(downlodFromServerFailed()));
				// Download all files because you can't be sure that all files are valid
			}

			ui->label_time->setText("aktualisieren");
		}
		else
		{
			QMessageBox::critical(this, "HWWS", "ERROR: Es konnte keine Verbindung zum Server Hergestellt werden!"
								  "Bitte überprüfen Sie ihre Internetverbindung");
		}
	}
}

void MainWindow::delay(int millisecondsToWait)
{
	QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
	while( QTime::currentTime() < dieTime )
	{
		QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
	}
}

void MainWindow::updateGraphData (void)    //update the graph and output
{
	QString filename;

	QDate date = QDate::currentDate();
	QTime time = QTime::currentTime();

	QList<int> values_C;
	QList<int> values_L;
	QList<QDate> dates;
	QList<int> x_minute;
	QList<int> missingValues;

	bool openFail = false;
	bool invalidCharacter = false;
	bool invalidFile = false;
	bool invalidConvert = false;

	values_C.clear();
	values_L.clear();

	//*******************************************************************************************************************************

	for(int i=shownDays; i>0; i--)                                                   //read data
	{
		filename = QString("data_")+date.addDays(-i+1).toString("yyyy_MM_dd")+QString(".txt");    //calculate the filename
		qDebug() << "try to open: " << filename;

		dates.append(date.addDays(-i+1));

		QFile file(QString("/storage/emulated/0/HWWS/Daten/")+filename);   //open files from directory

		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			openFail = true;
			qDebug() << "failed to open: " << filename;
		}

		QTextStream inData(&file);     //read files
		while (!inData.atEnd())
		{
			invalidCharacter = false;
			invalidFile = false;
			invalidConvert = false;

			QString line = inData.readLine();
			QStringList sl = line.split(QChar(':'));
			if(sl.length() != 6)         //if there is no data then ERROR
			{
				invalidFile = true;
			}

			bool ok1;       //check the convertation
			bool ok2;
			bool ok3;
			bool ok4;
			bool ok5;

            int hour =      sl.at(0).toInt(&ok1);
            int minute =    sl.at(1).toInt(&ok2);
            int height_C =  sl.at(3).toInt(&ok3);
            int height_L =  sl.at(4).toInt(&ok4);
            int temp =      sl.at(5).toInt(&ok5);

			//checking the values
			if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5)
			{
				invalidConvert = true;
            }

			// invalid character
            if( !((hour>=0 && hour<=23) && (minute>=0 && minute<=59) && ((height_C >= sensorHeight*(-1)) && height_C <= sensorHeight) &&
                  (height_L>=sensorHeight*(-1) && height_L<=sensorHeight)))
			{
				invalidCharacter = true;
			}

			if((invalidFile == false) && (invalidConvert == false) && (invalidCharacter == false)) //ignore invalid characters or any ohter fail
			{
				x_minute.append((hour*60+minute+(shownDays-i)*1440)+1);               //calculate the x-coordinate and write in a list

				values_C.append(height_C);                                //write the value in list
				values_L.append(height_L);

				minute_last_value = minute;
				hour_last_value = hour;
				temp_last_value = temp;
			}
            else
            {
                qDebug() << "invalid value";
                qDebug() << "invalid File: "        << invalidFile;
                qDebug() << "invalid Character: "   << invalidCharacter;
                qDebug() << "invalid Convert: "     << invalidConvert;
            }
		}

		file.close();
	}
	//********************************************************************************************************************************
	//check if any data is missing, mark this position in graph red
    for(int i = 0; i < time.toString("H").toInt()*60 + time.toString("m").toInt()+(shownDays-1)*1440; i++)
	{
        int count = 0;
        for(int ii = 0; ii < x_minute.size(); ii++)
		{
			if(x_minute.at(ii) != i+1)
			{
				count++;
			}
		}
		if(count == x_minute.size())
		{
			//qDebug()<< i+1<<" missing values";
			missingValues.append(i+1);
		}
	}
	//********************************************************************************************************************************

	WarningHeights.clear();
	for(int i=0; i<Warnings.size(); i++)
	{
		WarningHeights.append(Warnings.at(i)->getHeight());
	}

	ui->widget->updateGraph(x_minute, values_C, values_L, dates, missingValues, WarningHeights, ui->pushButton_shoWarningValues->isChecked(),
							ui->pushButton_showBand->isChecked());

	QTime timeLastValue;
	timeLastValue.setHMS(hour_last_value, minute_last_value, 0,0);
	ui->label_time_last_value->setText(timeLastValue.toString());       //writing the label time last value

	ui->label_temperatur->setText(QString::number(temp_last_value) + " °C");

	if(downloadFail == false)     //if there is a download failure the prognose and the last value would be false
	{
		lastValue_C = values_C.last();
		lastValue_L = values_L.last();

        if(values_C.length() >= 30 && values_L.length() >= 30 && x_minute.length() >= 30)
        {
            steigung_C = calculatePitch(values_C, x_minute, 30);
            steigung_L = calculatePitch(values_L, x_minute, 30);
        }
        else
        {
            steigung_C = calculatePitch(values_C, x_minute, values_C.length());
            steigung_L = calculatePitch(values_L, x_minute, values_L.length());
        }
        if(values_C.length() >= 5 && values_L.length() >= 5 && x_minute.length() >= 5)
        {
            steigung_C_5min = calculatePitch(values_C, x_minute, 5);
            steigung_L_5min = calculatePitch(values_L, x_minute, 5);
        }
        else
        {
            steigung_C = calculatePitch(values_C, x_minute, values_C.length());
            steigung_L = calculatePitch(values_L, x_minute, values_L.length());
        }

        if(steigung_C > 0 && steigung_L > 0)
        {
            ui->label_prognose->setText("steigend " + QString(QChar(0x2191)));
        }
        else if(steigung_C < 0 && steigung_L < 0)
        {
            ui->label_prognose->setText("sinkend " + QString(QChar(0x2193)));
        }
        else if(steigung_C == 0 && steigung_L == 0)
        {
            ui->label_prognose->setText("konstant " + QString(QChar(0x2192)));
        }
        else
        {
            ui->label_prognose->setText("ERROR");
        }
	}
	else
	{
		lastValue_C = 0;
		lastValue_L = 0;
    }

	//********************************************************************************************************************************

	float diff = abs((float)values_C.last() - (float)values_L.last());

	if((diff > 20) && (fmod(diff/2, 1.0) != 0) && ((values_C.last()+values_L.last())/2 >=100))
	{
		ui->label_value->setText(QString::number((values_C.last()+values_L.last())/2)  +
                                 QChar(0x00B1) + QString::number(diff/2)+ QString("cm"));

	}
	else
	{
        ui->label_value->setText(QString::number((values_C.last()+values_L.last())/2) + QString(" ") +
                                 QChar(0x00B1) + QString(" ")+ QString::number(diff/2)+ QString(" cm"));

	}

    if(timeLastValue < time.addSecs(-300))      //if last value is olde rthan 5 minutes, mark value in red
	{
		QPalette palette = ui->label_value->palette();
		palette.setColor(ui->label_value->foregroundRole(), Qt::red);
        ui->label_value->setPalette(palette);
	}
	else
	{
		QPalette palette = ui->label_value->palette();
		palette.setColor(ui->label_value->foregroundRole(), Qt::black);
        ui->label_value->setPalette(palette);
	}

	//write the current value in Label

    if(values_C.size() == 0 || downloadFail == true || values_L.size() == 0 || openFail == true || invalidCharacter == true || invalidFile == true ||
            invalidConvert == true )
	{
		ui->label_value->setText("ERROR");
		ui->label_prognose->setText("-");
		ui->label_temperatur->setText("-");
		ui->label_time_last_value->setText("-");
	}

	QString timeLastUpdate;                                                             //Shows the time of the last update
	timeLastUpdate = QDateTime::currentDateTime().toString("hh:mm:ss");
	ui->label_time->setText(timeLastUpdate);

    ui->widget->update();

    checkWarnings();
    //*******************************************************************************************************************************
}

void MainWindow::downlodFromServerFailed(void)
{
    downloadFail = true;
    ui->label_value->setText("ERROR");
	QMessageBox::critical(this, "HWWS", "Download ERROR");
}

void MainWindow::saveWarnings (void)
{
    settings.remove("WarningList");

    settings.beginWriteArray("WarningList");
	qDebug()<<"Warnings lenght:"<<Warnings.length();
	for(int i=0; i<Warnings.length(); i++)
	{
        settings.setArrayIndex(i);
        settings.setValue("Name", Warnings.at(i)->getName());
        settings.setValue("Height", Warnings.at(i)->getHeight());
        settings.setValue("TriggerState", Warnings.at(i)->getTriggerState());
	}
    settings.endArray();
}

void MainWindow::deleteOldData (void)
{
	QStringList dataNamesIst;
	QStringList dataNamesSoll;
	QStringList filter;
    QDate date = QDate::currentDate();

	filter << "data_*.txt";      //filter for files

	dataNamesIst = QDir ("/storage/emulated/0/HWWS/Daten").entryList(filter); //list of txt-files in folder

	for(int i=0; i<10; i++)                //create a list of the valid files
	{
        QString filename = QString("data_") + date.addDays(-i).toString("yyyy_MM_dd")+QString(".txt");
		dataNamesSoll.append(filename);
        //qDebug() << QString("Dateinamen soll") << dataNamesSoll.at(i);
	}

    for(int i = 0; i<dataNamesIst.size(); i++)
	{
        int zaeler = 0;

        for(int name = 0; name < dataNamesSoll.size(); name++)
		{
			if(dataNamesIst.at(i) != dataNamesSoll.at(name))
			{
				zaeler++;
			}
		}
        if(zaeler == dataNamesSoll.size())                         //delete the file if it's not in the valid name list
		{
			QFile toDelete;
			toDelete.setFileName(QString("/storage/emulated/0/HWWS/Daten/") + dataNamesIst.at(i));
			toDelete.remove();
			qDebug() << dataNamesIst.at(i) <<" deleted";
		}
	}
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
	if((index == 0) || (index == 1))    //update data if tab changed to "aktuell" or "diagramm"
	{
		chooseFilesForDownload();
	}
}

void MainWindow::checkWarnings(void)
{
    readWarningFile();
    ui->listWidget_aktiveWarnings->clear();            //delete List in Table Widget
    ui->tableWidget_Warnings->setHorizontalHeaderItem(0, new QTableWidgetItem("Name/Ort"));
    ui->tableWidget_Warnings->setHorizontalHeaderItem(1, new QTableWidgetItem("Höhe [cm]"));
	delay(100);
	int count = 0;

	if((downloadFail == false) && (LoginState == true)) //if there was no error while downloading all warnings will be checked
	{
        ui->listWidget_aktiveWarnings->clear();
		for(int i=0; i<Warnings.size(); i++)
		{
            if(Warnings.at(i)->getTriggerState() == true)
			{
				count++;

                ui->tableWidget_Warnings->item(i, 0)->setBackground(Qt::red);
                ui->tableWidget_Warnings->item(i, 1)->setBackground(Qt::red);

				QListWidgetItem *newItem = new QListWidgetItem;
				newItem->setText(Warnings.at(i)->getName());
				ui->listWidget_aktiveWarnings->insertItem(1, newItem);
			}
			else
			{
                ui->tableWidget_Warnings->item(i, 0)->setBackground(Qt::white);
                ui->tableWidget_Warnings->item(i, 1)->setBackground(Qt::white);
			}
		}
		ui->label_numberOfActiveWarnings->setText(QString::number(count));   
    }
    checkWarningAttentions();
}

void MainWindow::on_pushButton_login_clicked(void)
{
	if(LoginState == false)
	{
		Network* testConnection = new Network;

        QPalette pal;
        pal.setColor(QPalette::Background, Qt::green);
        L->setPalette(pal);

		if(L->exec() == QDialog::Accepted)
		{
            IP = L->returnIP();

			DW->setPalette(pal);
			DW->show();

			QUdpSocket socket;
			socket.connectToHost(IP, 20);		//FTP Port 20
			if (socket.waitForConnected(1000))	//check connection to the server
			{
				testConnection->Download("test.txt", "ftp://", IP + QString("/hwws/"), L->returnUserName(), L->returnPassword());
				connect(testConnection, SIGNAL(finished()), this, SLOT(login()));
                connect(testConnection, SIGNAL(failed()),   this, SLOT(LoginFailed()));
			}
			else
			{
				qDebug() << "No Connection to " << IP;
				DW->close();
				QMessageBox::critical(this, "HWWS-Login", "Es konnte keine Verbindung zum Server hergestellt werden. "
														  "Bitte überprüfen Sie ihre Internetverbindung und den Url. "
                                                          "\nBeachten Sie, dass das Übertragungsprotokoll wie ftp:// oder "
                                                          "http:// nich angegeben werden darf.");
			}
		}
	}
	else        //logout
	{
		ui->pushButton_login->setText("Anmelden");
        LoginState =    false;
        updating =      false;
        settings.setValue("LoginState", LoginState);
        Username.clear();                               //delete global Username
        Password.clear();                               //delete global Password
        settings.setValue("Username",Username);         //Save this Settings
        settings.setValue("Password", Password);

        location =  "unbekannt";
        version =   "unbekannt";
		sensorHeight = 0;
        settings.setValue("SensorHeight", sensorHeight);
        settings.setValue("Version", version);
        settings.setValue("Location", location);

		ui->label_value->setText("abgemeldet");
		ui->label_prognose->setText("-");
		ui->label_time->setText("-");
		ui->label_loginStatus->setText("abgemeldet");
		ui->label_Url->setText("-");
		ui->label_Username->setText("-");
		ui->label_time_last_value->setText("-");
		ui->label_temperatur->setText("-");
        ui->label_numberOfActiveWarnings->setText("-");

		ui->widget->clear();        //delete the graph
		ui->widget->update();
        ui->listWidget_aktiveWarnings->clear();

        writeLoginDataFile(" ", " ", " ", "false");
	}
}

void MainWindow::on_pushButton_shoWarningValues_clicked(void)
{
	bool state = ui->pushButton_shoWarningValues->isChecked();

	if(state == true)
	{
		ui->pushButton_shoWarningValues->setText("Ausblenden");
	}
	else
	{
		ui->pushButton_shoWarningValues->setText("Anzeigen");
	}

	//save the state of the Button
    settings.setValue("State_WarningsButton", state);
}

void MainWindow::on_pushButton_showBand_clicked(void)
{
	bool state = ui->pushButton_showBand->isChecked();

	if(state == true)
	{
		ui->pushButton_showBand->setText("Ausblenden");
	}
	else
	{
		ui->pushButton_showBand->setText("Anzeigen");
	}

	//save the state of the Button
    settings.setValue("State_BandButton", state);
}

void MainWindow::on_pushButton_edit_clicked(void)
{
	if((ui->tableWidget_Warnings->rowCount() != 0) && (ui->tableWidget_Warnings->selectedItems().size()==1))
	{
		int index = ui->tableWidget_Warnings->currentRow();
		qDebug()<<"TableWidget Index"<<index;

		addWarningWindow A;
        QPalette pal;
        pal.setColor(QPalette::Background, Qt::green);
        A.setPalette(pal);
		A.setData(Warnings.at(index)->getName(), Warnings.at(index)->getHeight(), Warnings.at(index)->getStateWarningAtOverUnderheight());

		if(A.exec() == QDialog::Accepted)
		{
            if(A.getValue() < -sensorHeight || A.getValue() > sensorHeight)
            {
                QMessageBox::critical(this, "HWWS", tr("Bitte geben Sie eine gültige 'kritische Höhe' ein! (min: -%1, max: %1)\n"
                                                       "Der aktuelle Wert kann zu einem nicht Auslösen der Warnung führen!").arg(sensorHeight));
            }

			Warnings.at(index)->setName(A.getName());
			Warnings.at(index)->setHeight(A.getValue());
			Warnings.at(index)->setStateWarningAtOverUnderheight(A.getStateWarningAtOverUnderheight());

			QTableWidgetItem* nameItem = new QTableWidgetItem(A.getName());
			ui->tableWidget_Warnings->setItem(index, 0, nameItem);

			QChar symb;
			if(A.getStateWarningAtOverUnderheight() == false)
			{
				symb = QChar(0x2191);
			}
			else
			{
				symb = QChar(0x2193);
			}

			QTableWidgetItem* valueItem = new QTableWidgetItem(QString::number(A.getValue()) + " " + symb);
			ui->tableWidget_Warnings->setItem(index, 1, valueItem);

			Warnings.at(index)->setTriggerState(false);

			//saveWarnings();
			saveWarningFile();
		}
	}
}

void MainWindow::on_pushButton_simulate_clicked(void)
{
    notificationClient->notification("WARNUNG: kritische Höhe für MUSTER");
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
	qDebug()<<"ComboBox index"<<index;

	switch(index)
	{
	case 0:
		shownDays = 1;
		break;

	case 1:
		shownDays = 2;
		break;
	}

	ui->widget->setDays(shownDays);
	chooseFilesForDownload();
    //ui->widget->update();
}

void MainWindow::on_pushButton_description_clicked(void)
{
	DialogFirstUse* DFU = new DialogFirstUse;
	DFU->exec();
}

void MainWindow::on_pushButton_info_clicked(void)
{
	QString text = "Version:\n" + QString(version) + "\n\nMessstandort:\n " + QString(location);
	QMessageBox::information(this, "HWWS", text);
}

void MainWindow::on_pushButton_BackroundColor_clicked(void)
{
	bool state = ui->pushButton_BackroundColor->isChecked();

	if(state == true)
	{
		ui->pushButton_BackroundColor->setText("schwarz");
	}
	else
	{
		ui->pushButton_BackroundColor->setText("weiß");
	}

    settings.setValue("Color", state);
	ui->widget->setBackgroundColor(state);
	ui->widget->update();
}

void MainWindow::LoginFailed(void)
{
	DW->close();
	QMessageBox::critical(this, "HWWS-Login", "Passwort oder Benutzername falsch");

	if(LoginState == false)
	{
		on_pushButton_login_clicked();
	}
}

void MainWindow::DownloadSettingsFailed(void)
{
	DW->close();
	QMessageBox::critical(this, "HWWS", "ERROR: Download Config File");

	if(DownloadSettingsState == false)
	{
		download_ConfigFile();
	}
}

void MainWindow::saveWarningFile(void)
{
    QFile warnings("/storage/emulated/0/HWWS/Daten/Warnings.txt");

    if(!warnings.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical(this, "HWWS", "ERROR: Warnungs-Datei konnte nicht geöffnet werden");
	}
    else
    {
        //QMessageBox::information(this, "HWWS", "Writing Warning file");
        QTextStream out(&warnings);

        for(int i=0; i<Warnings.size(); i++)
        {
            QString Triggered;
            if(Warnings.at(i)->getTriggerState())
            {
                Triggered = "true";
            }
            else
            {
                Triggered = "false";
            }

            QString StateOverUnderHeight;
            if(Warnings.at(i)->getStateWarningAtOverUnderheight())
            {
                StateOverUnderHeight = "true";
            }
            else
            {
                StateOverUnderHeight = "false";
            }

            qDebug()<< Warnings.at(i)->getTriggerState();
            out << Warnings.at(i)->getName() << ":" << Warnings.at(i)->getHeight() << ":" << Triggered << ":" <<
                   StateOverUnderHeight + QString("\n");
        }
    }

    warnings.flush();
    warnings.close();

	qDebug()<< "warning-file saved";
}

void MainWindow::readWarningFile(void)
{
	Warnings.clear();

    QFile warnings("/storage/emulated/0/HWWS/Daten/Warnings.txt");              //open file
    if(!warnings.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Fatal Error", "ERROR: Alarm-Datei konnte nicht geöffnet werden");
    }
    QTextStream in(&warnings);                                        //read files
    while (!in.atEnd())                                             //read out data from file
    {
        QString line = in.readLine();
        QStringList sl = line.split(QChar(':'));
        if(sl.length() != 4)         //if there is no data then ERROR
        {
            QMessageBox::critical(this, "HWWS", "ERROR: Warnungen konnten nicht geladen werden");
        }
        else
        {
            Warnings.append(new Warning);
            Warnings.last()->setName(sl.at(0));
            Warnings.last()->setHeight(sl.at(1).toInt());

            if(sl.at(2) == "true")
            {
                Warnings.last()->setTriggerState(true);
            }
            else
            {
                Warnings.last()->setTriggerState(false);
            }

            if(sl.at(3) == "true")
            {
                Warnings.last()->setStateWarningAtOverUnderheight(true);
            }
            else
            {
                Warnings.last()->setStateWarningAtOverUnderheight(false);
            }
        }
    }
    warnings.close();
    ui->label_savedWarnings->setText(QString::number(ui->tableWidget_Warnings->rowCount()));

    emit readingFinished();
}

void MainWindow::writeLoginDataFile(QString user, QString password, QString url, QString showNotification)
{
	QFile secret("/storage/emulated/0/HWWS/Daten/secret");
	if(!secret.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical(this, "HWWS", "ERROR: Passwort/Benutzer konnte nicht gespeichert werden");
	}

	QTextStream out(&secret);
    out << user + QString(":") + password + QString(":") + url + QString(":") + showNotification + QString("\n");

	secret.flush();
	secret.close();

	qDebug()<< "secret-file saved";
}

void MainWindow::on_pushButton_downloadDefaultWarnings_clicked(void)
{
    if(QMessageBox::question(this,"Standard-Warnungen wiederherstellen", "Wollen Sie alle gespeicherten Warnungen löschen "
                                                                         "und die Standard-Warnungen wiederherstellen?") == QMessageBox::Yes)
    {
        if(LoginState == true)
        {
            QUdpSocket socket;
            socket.connectToHost(IP, 20);		//FTP Port 20
            if(socket.waitForConnected(1000))	//check connection to the server
            {
                QPalette pal;
                pal.setColor(QPalette::Background, Qt::green);
                DW->setPalette(pal);
                DW->show();

                downloadFail = false;
                QString filename;
                Network* DownloadDefaultWarnngs = new Network;

                filename = "Warnings.txt";

                DownloadDefaultWarnngs->Download(filename, "ftp://", IP + QString("/hwws/"), Username, Password);     //Download the file from ftp server
                connect(DownloadDefaultWarnngs, SIGNAL(finished()), this, SLOT(readWarningFile()));
                connect(this, SIGNAL(readingFinished()), this, SLOT(writeTableWidgetWarnings()));
                connect(DownloadDefaultWarnngs, SIGNAL(failed()), this, SLOT(downlodFromServerFailed()));
            }
            else
            {
                QMessageBox::critical(this, "HWWS", "ERROR: Es konnte keine Verbindung zum Server Hergestellt werden!"
                                                    "Bitte überprüfen Sie ihre Internetverbindung");
            }
        }
        else
        {
            QMessageBox::critical(this, "HWWS", "ERROR: Sie müssen angemeldet sein um die Daten wiederherstellen zu können!");
        }
    }
}

void MainWindow::writeTableWidgetWarnings(void)
{
    ui->tableWidget_Warnings->clear();
    ui->tableWidget_Warnings->setRowCount(0);

    for(int i=0; i<Warnings.size(); i++)
    {
        int rows = ui->tableWidget_Warnings->rowCount();
        ui->tableWidget_Warnings->setRowCount(rows+1);                     // increase row count
        ui->tableWidget_Warnings->setColumnCount(2);
        ui->tableWidget_Warnings->setRowHeight(rows, 60);
        ui->tableWidget_Warnings->columnWidth(350);
        QTableWidgetItem* nameItem = new QTableWidgetItem(Warnings.at(i)->getName());
        ui->tableWidget_Warnings->setItem(rows, 0, nameItem);

        QChar symb;
        if(Warnings.at(i)->getStateWarningAtOverUnderheight() == false)
        {
            symb = QChar(0x2191);
        }
        else
        {
            symb = QChar(0x2193);
        }

        QTableWidgetItem* valueItem = new QTableWidgetItem(QString::number(Warnings.at(i)->getHeight()) + " " + symb);
        ui->tableWidget_Warnings->setItem(rows, 1, valueItem);
    }
    disconnect(this, SIGNAL(readingFinished()), this, SLOT(writeTableWidgetWarnings()));
    DW->close();
}

void MainWindow::checkWarningAttentions(void)
{
    ui->tableWidget_attention->clear();            //delete List in Table Widget
    ui->tableWidget_attention->setRowCount(0);
    ui->tableWidget_attention->setHorizontalHeaderItem(0, new QTableWidgetItem("Name/Ort"));
    ui->tableWidget_attention->setHorizontalHeaderItem(1, new QTableWidgetItem("Zeit"));
    delay(100);
    int count = 0;

    if((downloadFail == false) && (LoginState == true)) //if there was no error while downloading all warnings will be checked
    {
        for(int i=0; i<Warnings.size(); i++)
        {
            if(Warnings.at(i)->getStateWarningAtOverUnderheight() == false)
            {
                if((Warnings.at(i)->getHeight() <= lastValue_C + safety_diff || Warnings.at(i)->getHeight() <= lastValue_L + safety_diff) && Warnings.at(i)->getTriggerState() == false)
                {
                    count++;

                    ui->tableWidget_Warnings->item(i, 0)->setBackground(Qt::yellow);
                    ui->tableWidget_Warnings->item(i, 1)->setBackground(Qt::yellow);

                    int rows = ui->tableWidget_attention->rowCount();
                    ui->tableWidget_attention->setRowCount(rows+1);             // increase row count
                    ui->tableWidget_attention->setColumnCount(2);
                    ui->tableWidget_attention->setRowHeight(rows, 60);
                    ui->tableWidget_attention->columnWidth(340);

                    QTableWidgetItem* nameItem = new QTableWidgetItem(Warnings.at(i)->getName());
                    ui->tableWidget_attention->setItem(rows, 0, nameItem);

                    double k=0;
                    if(((steigung_C_5min > 0) && (steigung_L_5min >0)) || ((steigung_C_5min < 0) && (steigung_L_5min < 0)) || ((steigung_C_5min == 0) && (steigung_L_5min == 0)))
                    {
                        qDebug()<<"steigung_C_5min=" << steigung_C_5min;
                        qDebug()<<"steigung_L_5min=" << steigung_L_5min;
                        k = (steigung_C_5min + steigung_L_5min) /(double)2.0;
                    }
                    else
                    {
                        QTableWidgetItem* valueItem = new QTableWidgetItem("ERROR");
                        ui->tableWidget_attention->setItem(rows, 1, valueItem);
                        return;
                    }
                    qDebug()<<"k=" <<k;
                    double dy = Warnings.at(i)->getHeight() - (lastValue_C + lastValue_L)/2.0;
                    double dx = dy/k;

                    QTime now = QTime::currentTime();
                    QTime triggerTime = now.addSecs(dx*(double)60);
                    if(k > 0)
                    {
                            QTableWidgetItem* valueItem = new QTableWidgetItem("ca. " + triggerTime.toString());
                            ui->tableWidget_attention->setItem(rows, 1, valueItem);
                    }
                    else
                    {
                        QChar symb= QChar(0x221E);
                        QTableWidgetItem* valueItem = new QTableWidgetItem(symb);
                        ui->tableWidget_attention->setItem(rows, 1, valueItem);
                    }
                }
                else
                {
                    if(Warnings.at(i)->getTriggerState() == false)
                    {
                        ui->tableWidget_Warnings->item(i, 0)->setBackground(Qt::white);
                        ui->tableWidget_Warnings->item(i, 1)->setBackground(Qt::white);
                    }
                }
            }
            else
            {
                if((Warnings.at(i)->getHeight() >= lastValue_C - safety_diff || Warnings.at(i)->getHeight() >= lastValue_L - safety_diff) && Warnings.at(i)->getTriggerState() == false)
                {
                    count++;
                    ui->tableWidget_Warnings->item(i, 0)->setBackground(Qt::yellow);
                    ui->tableWidget_Warnings->item(i, 1)->setBackground(Qt::yellow);

                    int rows = ui->tableWidget_attention->rowCount();
                    ui->tableWidget_attention->setRowCount(rows+1);             // increase row count
                    ui->tableWidget_attention->setColumnCount(2);
                    ui->tableWidget_attention->setRowHeight(rows, 60);
                    ui->tableWidget_attention->columnWidth(340);

                    QTableWidgetItem* nameItem = new QTableWidgetItem(Warnings.at(i)->getName());
                    ui->tableWidget_attention->setItem(rows, 0, nameItem);

                    double k=0;
                    if((steigung_C_5min > 0 && steigung_L_5min >0) || (steigung_C_5min < 0 && steigung_L_5min < 0) || (steigung_C_5min == 0 && steigung_L_5min == 0))
                    {
                        k = (steigung_C_5min + steigung_L_5min) /(double)2.0;
                    }
                    else
                    {
                        QTableWidgetItem* valueItem = new QTableWidgetItem("ERROR");
                        ui->tableWidget_attention->setItem(rows, 1, valueItem);
                        return;
                    }
                    double dy = Warnings.at(i)->getHeight() - (lastValue_C + lastValue_L)/2.0;
                    double dx = dy/k;
                    qDebug()<<"k=" <<k;

                    QTime now = QTime::currentTime();
                    QTime triggerTime = now.addSecs(dx*(double)60);
                    if(k < 0)
                    {
                        QTableWidgetItem* valueItem = new QTableWidgetItem("ca. " + triggerTime.toString());
                        ui->tableWidget_attention->setItem(rows, 1, valueItem);
                    }
                    else
                    {
                        QChar symb= QChar(0x221E);
                        QTableWidgetItem* valueItem = new QTableWidgetItem(symb);
                        ui->tableWidget_attention->setItem(rows, 1, valueItem);
                    }
                }
                else
                {
                    if(Warnings.at(i)->getTriggerState() == false)
                    {
                        ui->tableWidget_Warnings->item(i, 0)->setBackground(Qt::white);
                        ui->tableWidget_Warnings->item(i, 1)->setBackground(Qt::white);
                    }
                }
            }
        }
        ui->label_NumberOfAttention->setText(QString::number(count));
    }
}

void MainWindow::on_comboBox_attentionBandwidth_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        safety_diff = 5;
        break;

    case 1:
        safety_diff = 10;
        break;

    case 2:
        safety_diff = 15;
        break;

    case 3:
        safety_diff = 20;
        break;

    case 4:
        safety_diff = 25;
        break;
    }
    save_safty_diff();
}

void MainWindow::save_safty_diff(void)
{
    settings.setValue("saftyDiff", safety_diff);
}

void MainWindow::on_pushButton_showHeightAsNotification_clicked()
{
    if(ui->pushButton_showHeightAsNotification->isChecked() == true)
    {
        ui->pushButton_showHeightAsNotification->setText("Ausblenden");
        writeLoginDataFile(Username, Password, IP, "true");
    }
    else
    {
        ui->pushButton_showHeightAsNotification->setText("Anzeigen");
        writeLoginDataFile(Username, Password, IP, "false");
    }
    settings.setValue("showHeightNotification", ui->pushButton_showHeightAsNotification->isChecked());

}

double MainWindow::calculatePitch(QList<int> values, QList<int> x_minute, double numberOfValues)
{
    double summe_x  = 0;
    double summe_y  = 0;
    double zaeler   = 0;
    double nenner   = 0;
    double steigung = 0;

    for(int i = 1; i <= numberOfValues; i++)
    {
        summe_y = summe_y + values.at(values.length() - i);
        summe_x = summe_x + x_minute.at(x_minute.length() - i);
    }

    for(int i = 1; i <= numberOfValues; i++)
    {
        double y_y = values.at(values.length() - i) - (1/numberOfValues) * summe_y;
        double x_x = x_minute.at(x_minute.length() - i) - (1/numberOfValues * summe_x);

        zaeler  = zaeler + (x_x * y_y);
        nenner  = nenner + pow(x_x, 2);
    }

    steigung = (double)zaeler/(double)nenner;

    qDebug() << "summe_y= " << summe_y;
    qDebug() << "summe_x= " << summe_x;
    qDebug() << "zähler = " << zaeler;
    qDebug() << "nenner = " << nenner;
    qDebug() << "steigung=" << steigung;

    return steigung;
}
