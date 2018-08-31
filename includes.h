#ifndef INCLUDES_H
#define INCLUDES_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
QT_CHARTS_USE_NAMESPACE

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSignalMapper>

#include <string.h>
#include <vector>

struct chartFragment{
    QString name;
    float value;
    QBrush color;
};

QChartView *showPie(QString heading, std::vector<chartFragment> frag);


#endif // INCLUDES_H
