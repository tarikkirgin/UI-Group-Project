// fluorinated_compounds_page.hpp
#pragma once

#include <QWidget>
#include <QtCharts>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include "dataset.hpp"

class FluorinatedCompoundsPage : public QWidget {
    Q_OBJECT
public:
    explicit FluorinatedCompoundsPage(QWidget* parent = nullptr);
    

    static constexpr double SAFETY_THRESHOLD = 0.1; // μg/L

private:
    void setupUI();
    void createChart();
    void updateStatusIndicators();
    void showInfoDialog();
    QColor getStatusColor(double value) const;
    void processData();

    QChart* chart;
    QChartView* chartView;
    QVBoxLayout* mainLayout;
    QHBoxLayout* statusLayout;
    std::vector<QLabel*> statusLabels;
    QPushButton* infoButton;
    
    struct CompoundData {
        QString location;
        QDateTime time;
        double value;
    };
    std::vector<CompoundData> compoundData;
};

class FluorinatedInfoDialog : public QDialog {
    Q_OBJECT
public:
    explicit FluorinatedInfoDialog(QWidget* parent = nullptr);
};
