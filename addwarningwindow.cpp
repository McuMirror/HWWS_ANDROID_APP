/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         addalrmwindow.cpp
// Zweck:            adding a alarm
//
// erstellt am:      18.06.2015
// zuletzt geändert: 4.12.2015
// ****************************************************************************
*/

#include "addwarningwindow.h"
#include "ui_addwarningwindow.h"

#include <QString>
#include <QMessageBox>

addWarningWindow::addWarningWindow(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::addWarningWindow)
{
	ui->setupUi(this);

	//writing the labels
    QFont defaultHeight;
    defaultHeight.setPointSize(20);

    #define ButtonHeight 65

	ui->label_name->setText("Name:");
    ui->label_name->setFont(defaultHeight);
	ui->label_value->setText("Höhe [cm]:");
    ui->label_value->setFont(defaultHeight);
	ui->pushButton_close->setText("Abbrechen");
    ui->pushButton_close->setMaximumHeight(ButtonHeight);
	ui->pushButton_OK->setText("OK");
    ui->pushButton_OK->setMaximumHeight(ButtonHeight);
	ui->pushButton_warningAtUnder_overheight->setText("Warnung bei Überschreitung");
    ui->pushButton_warningAtUnder_overheight->setMaximumHeight(ButtonHeight);
	ui->pushButton_warningAtUnder_overheight->setCheckable(true);
}

addWarningWindow::~addWarningWindow()
{
	delete ui;
}

void addWarningWindow::on_pushButton_OK_clicked(void)
{
	name = ui->lineEdit_name->text();
	value = ui->spinBox_value->value();

	if(name.isEmpty())
	{
		QMessageBox::critical(this, "HWWS", "ERROR: Bitte geben Sie einen Namen ein");
		return;
	}

	// TODO some more checks on input data
	// Only close when both arguments are not Empty and valid
	QDialog::accept();
}

void addWarningWindow::on_pushButton_close_clicked(void)
{
	QDialog::reject();
}

int addWarningWindow::getValue()
{
	return value;
}

QString addWarningWindow::getName()
{
	return name;
}
void addWarningWindow::setData(QString nameSend, int heightSend, bool StateWarningAtOverUnderheight)
{
	this->name = nameSend;
	this->value = heightSend;
	this->StateWarningAtOverUnderheight = StateWarningAtOverUnderheight;

	ui->lineEdit_name->setText(name);
	ui->spinBox_value->setValue(value);

	if(StateWarningAtOverUnderheight == true)
	{
		ui->pushButton_warningAtUnder_overheight->setText("Warnung bei Unterschreitung");
		ui->pushButton_warningAtUnder_overheight->setChecked(true);
	}
	else
	{
		ui->pushButton_warningAtUnder_overheight->setText("Warnung bei Überschreitung");
		ui->pushButton_warningAtUnder_overheight->setChecked(false);
	}
}

void addWarningWindow::on_pushButton_warningAtUnder_overheight_clicked(void)
{
	if(ui->pushButton_warningAtUnder_overheight->isChecked())
	{
		ui->pushButton_warningAtUnder_overheight->setText("Warnung bei Unterschreitung");
	}
	else
	{
		ui->pushButton_warningAtUnder_overheight->setText("Warnung bei Überschreitung");
	}
}

bool addWarningWindow::getStateWarningAtOverUnderheight(void)
{
	return ui->pushButton_warningAtUnder_overheight->isChecked();
}
