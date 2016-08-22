/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         graph.cpp
// Zweck:            Drawing the graph
//
// erstellt am:      18.06.2015
// zuletzt geändert: 29.11.2015
// ****************************************************************************
*/

#include "graph.h"
#include "mainwindow.h"

#include <QWidget>
#include <QPen>
#include <QPainter>
#include <QDebug>
#include <math.h>
#include <QList>
#include <QLine>
#include <QVector>
#include <QPoint>
#include <QString>
#include <QDateTime>

#define measureMax 280.0    //Graph max = measureMax + measureMin
#define measureMin -20.0

graph::graph(QWidget *parent) :
	QWidget(parent)
{
	setFixedWidth(1440*2);
}

void graph::paintEvent(QPaintEvent* event)  //Draw the diagramm
{
	//**********************************************************************************************************************************
	//define the different layout pens

	QPainter p(this);

	QPen linePen_C(Qt::blue);               //Define the layout of the graph capcitiv sensor
	linePen_C.setWidth(3);

	QPen linePen_L(Qt::darkGreen);          //Define the layout of the graph laser sensor
	linePen_L.setWidth(3);

	QPen gridPen(Qt::gray);                 //Define he Layout of the grid
	gridPen.setStyle(Qt::DashLine);

	QPen textPen;                //Define the layout of text
	textPen.setStyle(Qt::SolidLine);
	textPen.setWidth(1);

	QPen datePen;
	datePen.setStyle(Qt::SolidLine);
	datePen.setWidth(1);

	QPen scalePen;
	scalePen.setStyle(Qt::SolidLine);
	scalePen.setWidth(1);

	QPen dayPen;                 //Define the layout of the line between days
	dayPen.setStyle(Qt::SolidLine);
	dayPen.setWidth(3);

	QPen missingValuesPen(Qt::red);
	missingValuesPen.setStyle(Qt::SolidLine);
	missingValuesPen.setWidth(1);

	QPen fieldPen;
	fieldPen.setStyle(Qt::SolidLine);
	fieldPen.setWidth(1);

	QPen ValuePen(Qt::darkRed);
	ValuePen.setStyle(Qt::SolidLine);
	ValuePen.setWidth(2);

	if(color == false)		//Backround: white
	{
		p.fillRect(rect(), QBrush(Qt::white));  //set the backcollor of the diagram
		dayPen.setColor(Qt::black);
		scalePen.setColor(Qt::black);
		datePen.setColor(Qt::darkBlue);
		textPen.setColor(Qt::black);
		fieldPen.setColor(Qt::yellow);
	}
	else if(color == true)	//Backround: black
	{
		p.fillRect(rect(), QBrush(Qt::black));  //set the backcollor of the diagram
		dayPen.setColor(Qt::white);
		scalePen.setColor(Qt::white);
		datePen.setColor(Qt::blue);
		textPen.setColor(Qt::white);
		fieldPen.setColor(Qt::darkYellow);
	}
	//***********************************************************************************************************************************
	//prepare the values

	QVector<QPoint> line_C;
	QVector<QPoint> line_L;

	int x;
	int y;
    float k  = -rect().height()/measureMax;
    int d = rect().height()-k*(measureMin);
	// y = k * x + d

	for(int z=0; z<values_C.size(); z++)
	{
		x = x_minute.at(z)*2;
		y = k * values_C.at(z) + d;

		line_C.append(QPoint(x, y));
	}

	for(int u=0; u<values_L.size(); u++)
	{
		x = x_minute.at(u)*2;
        y = k * values_L.at(u) + d;

		line_L.append(QPoint(x, y));
	}

	//***********************************************************************************************************************************

	p.setPen(missingValuesPen);                 //draw missing values
    for(int i = 0; i < missingValues.size(); i++)
	{
		p.drawLine(missingValues.at(i), 0, missingValues.at(i), rect().height());
	}

	if(showBand == true)
	{		
		p.setPen(fieldPen);
		for(int i=0; i<x_minute.size(); i++)		//draw field between the two sensor-curves
		{
			int y1 = k * values_C.at(i)+d;
			int y2 = k * values_L.at(i)+d;

			p.drawLine(x_minute.at(i), y1, x_minute.at(i), y2);
		}
	}

	//*************************************************************************************************************************************
	//write the horizontal scale

    for(int i = 0; i <= rect().width(); i++)
	{
		if(i%600 == 0)
		{
            for(int ii = measureMin + 10; ii < measureMax; ii = ii+10)
			{
				p.setPen(scalePen);
				y = k * ii + d;
				p.drawText(QPoint(i+100, y), QString::number(ii));
			}
		}
	}

	//**************************************************************************************************************************************
	//Draw horizontal grid

    for(int i = measureMin + 10; i < measureMax; i = i+10)
	{
		p.setPen(gridPen);
        y = k * i + d;

		p.drawLine(0, y, rect().width(), y);
	}

	//**************************************************************************************************************************************
	//Draw vertical grid

	int TimeLabel=0;
	int date=0;
	for(int i=0; i<25*days; i++)
	{
		p.setPen(gridPen);
		p.drawLine(rect().width()/24*i/days, 0, rect().width()/24*i/days, rect().height()-25);

		p.setPen(textPen);                                                                                 //write text to HORIZONTAL axis
		p.drawText(QPoint(rect().width()/24*i/days, rect().height()-11), QString::number(TimeLabel));

		if(TimeLabel==24||TimeLabel==0)                                                                    //draw line between days and the date
		{
			p.setPen(dayPen);
			p.drawLine(rect().width()/24*i/days, 0, rect().width()/24*i/days, rect().height()-25);                  //draw Line

			p.setPen(datePen);
			if(date<dates.size())
			{
				p.drawText(QPoint(rect().width()/24*i/days, rect().height()/27), dates.at(date).toString("dd.MM"));     //Draw Date between days
				date++;
			}
		}

		TimeLabel++;
		if(TimeLabel>23)
		{
			TimeLabel=0;
		}
	}
	//***************************************************************************************************************************************
	//show Warning Values

	if(showWarningValues == true)
	{
		for(int ii=0; ii<WarningValues.size(); ii++)
		{
			p.setPen(ValuePen);
			int y_WarningValue =  k*WarningValues.at(ii)+d;

			p.drawLine(0, y_WarningValue, rect().width(), y_WarningValue);
		}
	}
	//*************************************************************************************************************************************
	//draw the curves as least becaus nothing writes over

	p.setPen(linePen_L);
	p.drawPoints(QPolygon(line_L));     // maybe p.drawPolyline();

	p.setPen(linePen_C);
	p.drawPoints(QPolygon(line_C));
}

void graph::setDays(int days)               //set the with of the graph for the number of shown days
{
	this->days=days;
	setFixedWidth(1440*days*2);
}

void graph::updateGraph(QList<int> x_minute, QList<int> values_C, QList<int> values_L, QList<QDate> dates, QList<int> missingValues, QList<int> WarningValues, bool showWarningValues, bool showBand)
{
	this->x_minute = x_minute;
	this->values_C = values_C;
	this->values_L = values_L;
	this->dates = dates;
	this->missingValues = missingValues;
	this->WarningValues = WarningValues;
	this->showWarningValues = showWarningValues;
	this->showBand = showBand;
}

void graph::clear(void)
{
	values_C.clear();
	values_L.clear();
	dates.clear();
	WarningValues.clear();
	missingValues.clear();
}

void graph::setBackgroundColor(int color)
{
	this->color = color;
}
