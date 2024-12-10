#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QtCharts>
#include <QDateTime>

class FluorinatedCompoundsPage : public QWidget {
    Q_OBJECT

public:
    static constexpr double SAFETY_THRESHOLD = 0.1;
    explicit FluorinatedCompoundsPage(QWidget* parent = nullptr);

private:
    struct CompoundData {
        QString location;
        QDateTime time;
        double value;
    };

    // UI elements
    QVBoxLayout* mainLayout;
    QComboBox* locationSelector; 
    QComboBox* compoundSelector;    
    QPushButton* infoButton;
    QChart* chart;
    QChartView* chartView;

    // Data
    QVector<CompoundData> compoundData;

    // Methods
    void setupUI();
    void processData();
    void updateChart();
    QColor getStatusColor(double value) const;
    void showInfoDialog();

private slots:
    void updateLocationList();
};

class FluorinatedInfoDialog : public QDialog {
    Q_OBJECT

public:
    explicit FluorinatedInfoDialog(QWidget* parent = nullptr);
};
