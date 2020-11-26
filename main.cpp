#include "textfinder.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //a.beep();
    TextFinder w;
    w.show();

    return a.exec();

/*
    QApplication a(argc, argv);
    QLineSeries *series = new QLineSeries();
    series->append(0, 6);
    series->append(2, 4);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);
    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Simple line chart example");
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(400, 300);
    window.show();
    return a.exec();
    */
}
