#pragma once
#include <QWidget>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QMap>

class FluorinatedCompoundsPage : public QWidget {
    Q_OBJECT

public:
    explicit FluorinatedCompoundsPage(QWidget* parent = nullptr);

private slots:
    void updateChart();
    void switchToMap();
    void switchToTimeSeries();
    void showLocationDetails(const QString& location);
    void showPopupInfo(const QPointF& point);

private:
    void setupUI();
    void setupChart();
    void setupMap();
    void updateMap();
    QString getComplianceColor(double value);
    double getPfasLevel(const QString& location);

    // UI components
    QGridLayout* mainLayout;
    QChartView* chartView;
    QLineSeries* series;
    QPushButton* mapViewButton;
    QPushButton* timeSeriesButton;
    QLabel* thresholdLabel;
    
    // Map components
    QWidget* mapWidget;
    QMap<QString, QPushButton*> locationMarkers;
    QLabel* mapLabel;
    
    // Constants
    static constexpr double PFAS_THRESHOLD = 0.1; // µg/L
    bool isMapView;
    
    // testing 
    struct PFASData {
        QString location;
        double value;
        QString timestamp;
    };
    QVector<PFASData> testData;
};
