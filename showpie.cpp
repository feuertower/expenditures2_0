
#include "includes.h"

QChartView* showPie(QString heading, std::vector<chartFragment> frag)
{
    QPieSeries *series = new QPieSeries();    
    for(auto addFrag : frag)
    {
        series->append(addFrag.name, addFrag.value);
    }

    int seriesSize = series->count();
    for(int i = 0; i < seriesSize; i++)
    {
        QPieSlice* slice = series->slices().at(i);
        chartFragment fragment = frag.at(i);
        slice->setBrush(fragment.color);
        slice->setLabelVisible(true);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(heading);
    chart->legend()->setVisible(false);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}
