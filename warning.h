/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         warning.h
// Zweck:            warning class
//
// erstellt am:      24.10.2015
// zuletzt geändert: 24.10.2015
// ****************************************************************************
*/

#ifndef WARNING_H
#define WARNING_H

#include<QString>
#include<QSettings>

class Warning
{
public:

	Warning();
	void setName(QString Name);
	void setHeight(int height);
	void setTriggerState(bool triggered);
	void setStateWarningAtOverUnderheight(bool StateWarningAtOverUnderheight);
	void setEditable(bool editable);

	QString getName(void);
	int getHeight(void);
	bool getTriggerState(void);
	bool getStateWarningAtOverUnderheight(void);
	bool getEditable(void);
private:

	QString Name;
	int height;
	bool triggered = false;
	bool StateWarningAtOverUnderheight;
	bool editable;

};

#endif // WARNING_H
