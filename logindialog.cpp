/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         logindialog.cpp
// Zweck:            login
//
// erstellt am:      18.06.2015
// zuletzt geändert: 17.10.2015
// ****************************************************************************
*/

#include "logindialog.h"
#include "ui_logindialog.h"

#include <QMessageBox>
#include <QLineEdit>
#include <QSettings>
#include <QPushButton>
#include <QString>
#include <QScreen>


LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

	QScreen *screen = QApplication::screens().at(0);
	//int screenWidth = screen->availableSize().width();
	int screenHeight = screen->availableSize().height();

    QFont defaultHeight;
	defaultHeight.setPointSize(screenHeight / 60);

	int ButtonHeight = screenHeight / 20;

    ui->label_User->setText("Benutzername:");
    ui->label_User->setFont(defaultHeight);
    ui->label_password->setText("Passwort:");
    ui->label_password->setFont(defaultHeight);
	ui->label_IP_login->setText("Server Url/IP:");
    ui->label_IP_login->setFont(defaultHeight);

    ui->pushButton_OK->setText("OK");
    ui->pushButton_OK->setMaximumHeight(ButtonHeight);
    ui->pushButton_cancel->setText("Abbrechen");
    ui->pushButton_cancel->setMaximumHeight(ButtonHeight);

	ui->lineEdit_password->setEchoMode(QLineEdit::Password);

    ui->label_headline->setAlignment(Qt::AlignCenter);
    QFont font = ui->label_headline->font();                                        //Set layout of label
    font.setBold(true);
	font.setPointSize(screenHeight / 60);
    ui->label_headline->setFont(font);
    ui->label_headline->setText("HWWS Login");

    QString IP = settingsIP.value("Url", "").toString();
    ui->lineEdit_IP_login->setText(IP);

	QString User = settingsUser.value("name", "").toString();
    ui->lineEdit_user->setText(User);

	QString Password = settingsUser.value("password", "").toString();
    ui->lineEdit_password->setText(Password);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_pushButton_OK_clicked()
{

    QString User=ui->lineEdit_user->text();
    QString Password=ui->lineEdit_password->text();

    if(User.length()<5 || Password.length()<4)
    {
        QMessageBox::critical(this, "HWWS", "ungültiger Benutzername oder ungültiges Passwort");
    }

    else
    {
        QDialog::accept();
    }
}

QString LoginDialog::returnUserName(void)
{
    QString User = ui->lineEdit_user->text();
    return User;
}

QString LoginDialog::returnPassword(void)
{
    QString Password = ui->lineEdit_password->text();
    return Password;
}

QString LoginDialog::returnIP(void)
{
    return ui->lineEdit_IP_login->text();
}

void LoginDialog::on_lineEdit_IP_login_editingFinished()
{
	QString Url = ui->lineEdit_IP_login->text();
	bool invalidCharacter = false;

	for(int i=0; i<Url.length(); i++)
	{
		int ch=Url.at(i).unicode();

        if(!((ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || (ch>='0' && ch<='9') || (ch=='.') || (ch == '/')))	// invalid character
		{
			QMessageBox::critical(this, "HWWS", "Url enthält ungültige Zeichen, beachten Sie, dass Lehrzeichen und Sonderzeichen nicht erlaubt sind.");
			invalidCharacter = true;
		}
	}

	if(invalidCharacter == false)
	{
		settingsIP.setValue("Url", Url);
	}
}

void LoginDialog::on_lineEdit_user_editingFinished()
{
    QString User = ui->lineEdit_user->text();
	bool invalidCharacter = false;

	for(int i=0; i<User.length(); i++)
	{
		int ch=User.at(i).unicode();

		if(! ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || (ch>='0' && ch<='9') ))	// invalid character
		{
			QMessageBox::critical(this, "HWWS", "Benutzername enthält ungültige Zeichen, beachten Sie, dass Lehrzeichen und Sonderzeichen nicht erlaubt sind.");
			invalidCharacter = true;
		}
	}

	if(invalidCharacter == false)
	{
		 settingsUser.setValue("name", User);
	}
}

void LoginDialog::on_lineEdit_password_editingFinished()
{
    QString password = ui->lineEdit_password->text();
    settingsPassword.setValue("password", password);
}

void LoginDialog::on_pushButton_cancel_clicked()
{
    QDialog::reject();
}

void LoginDialog::clear(bool user, bool password, bool Url)
{
    if(user == true)
    {
         settingsUser.setValue("name", "");
    }
    if(password == true)
    {
         settingsPassword.setValue("password", "");
    }
    if(Url ==true)
    {
         settingsIP.setValue("Url", "");
    }
}
