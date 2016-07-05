/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         logindialog.h
//
// erstellt am:      18.06.2015
// zuletzt geändert: 19.10.2015
// ****************************************************************************
*/

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QString>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);

    QString returnUserName(void);
    QString returnPassword(void);
    QString returnIP(void);
    void clear(bool user, bool password, bool Url);

    ~LoginDialog();

private slots:
    void on_pushButton_OK_clicked();
    void on_lineEdit_IP_login_editingFinished();
    void on_lineEdit_user_editingFinished();
    void on_lineEdit_password_editingFinished();
    void on_pushButton_cancel_clicked();

private:
    Ui::LoginDialog *ui;

    QSettings settingsIP;
    QSettings settingsUser;
    QSettings settingsPassword;
};

#endif // LOGINDIALOG_H

