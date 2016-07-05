/*
// ****************************************************************************
// Projekt:          Hochwasser-Warnsystem
// Compiler:         -
// Hersteller:       -
// Autor:            Stefan W.
// Filename:         graph.h
//
// erstellt am:      18.06.2015
// zuletzt geändert: 19.10.2015
// ****************************************************************************
*/

#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>
#include <QDateTime>
#include <QList>

class graph : public QWidget
{
    Q_OBJECT

public:
    explicit graph(QWidget *parent = 0);

    void setDays(int days);
	void updateGraph(QList<int> x_minute, QList<int> values_C, QList<int> values_L, QList<QDate> dates, QList<int> missingValues, QList<int> WarningValues, bool showWarningValues, bool showBand);
    void clear(void);
	void setBackgroundColor(int color);

signals:

public slots:

private:
    void paintEvent(QPaintEvent *event);

    int days=1;
	bool color=0;
    QList<QDate> dates;
    QList<int> values_C;
    QList<int> values_L;
    QList<int> x_minute;
    QList<int> missingValues;
	QList<int> WarningValues;
	bool showWarningValues;
	bool showBand;

};

#endif // GRAPH_H

