#pragma once
#include <QWidget>
#include "pollutant_card.hpp"
#include <QLabel>
#include <QCheckBox>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QPieSeries>

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
    QChart *pieChart;
    QChartView *pieChartView;
    QPieSeries *pieSeries;
    QPieSlice *slice;
    QComboBox *materialDropdown;
    QCheckBox *litter_type_1;
    QCheckBox *litter_type_2;
    QCheckBox *litter_type_3;
    QCheckBox *litter_type_4;

    QMap<QString, double> determinandsMap = {
        {"BWP - O.L.", 10}, {"SewageDebris", 3}, {"TarryResidus", 5}};

    QList<PollutantCard *> pollutantCards;
};