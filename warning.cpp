/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan Wögerbauer
// Filename:         warning.cpp
// Zweck:            includes the settings of a warning
//
// erstellt am:      24.10.2015
// zuletzt geändert: 24.10.2015
// ****************************************************************************
*/

#include "warning.h"
#include <QString>

Warning::Warning()
{
}
void Warning::setName(QString Name)
{
	this->Name = Name;
}

void Warning::setHeight(int height)
{
	this->height = height;
}

void Warning::setTriggerState(bool triggered)
{
	this->triggered = triggered;
}

QString Warning::getName(void)
{
	return Name;
}

int Warning::getHeight(void)
{
	return height;
}

bool Warning::getTriggerState(void)
{
	return triggered;
}

void Warning::setStateWarningAtOverUnderheight(bool StateWarningAtOverUnderheight)
{
	this->StateWarningAtOverUnderheight = StateWarningAtOverUnderheight;
}

bool Warning::getStateWarningAtOverUnderheight(void)
{
	return StateWarningAtOverUnderheight;
}
