#pragma once
#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QtCharts/QChartView>
#include <QtCharts/QlineSeries>
#include <QtCharts/QBarSeries>

class EnvironmentalLitterPage : public QWidget {
    Q_OBJECT

public:
    EnvironmentalLitterPage();

private:
    void setupUI();

    void updateChart();

    QChartView *chartView;
    QLineSeries *series;
    QLabel *label;
    QChart *chart;
    QCheckBox *checkbox;
    QBarSeries *barSeries;
    QCheckBox *litter_type_1;
    QCheckBox *litter_type_2;
    QCheckBox *litter_type_3;
    QCheckBox *litter_type_4;
};