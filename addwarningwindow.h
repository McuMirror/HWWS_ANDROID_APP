/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan Wögerbauer
// Filename:         addwarningwindow.h
//
// erstellt am:      18.06.2015
// zuletzt geändert: 4.12.2015
// ****************************************************************************
*/

#ifndef ADDWARNINGWINDOW_H
#define ADDWARNINGWINDOW_H

#include <QDialog>
#include <QString>

namespace Ui {
class addWarningWindow;
}

class addWarningWindow : public QDialog
{
    Q_OBJECT

public:
	explicit addWarningWindow(QWidget *parent = 0);
    ~addWarningWindow();

    int getValue();
    QString getName();
	void setData(QString nameSend, int heightSend, bool StateWarningAtOverUnderheight);
	bool getStateWarningAtOverUnderheight(void);

private slots:
    void on_pushButton_OK_clicked(void);
    void on_pushButton_close_clicked(void);
	void on_pushButton_warningAtUnder_overheight_clicked(void);

private:
    Ui::addWarningWindow *ui;

    int value;
    QString name;
	bool StateWarningAtOverUnderheight;
};

#endif // ADDWARNINGWINDOW_H
