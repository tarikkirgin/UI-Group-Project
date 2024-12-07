#include "fluorinated_compounds_page.hpp"
#include <random> 
#include <QChart>
#include <QToolTip>
#include <QDateTime>

// Constructor
FluorinatedCompoundsPage::FluorinatedCompoundsPage(QWidget* parent)
    : QWidget(parent)
    , isMapView(false)
{
    // Add some test data
    testData = {
        {"Location A", 0.05, "2024-01-01"},
        {"Location B", 0.12, "2024-01-01"},
        {"Location C", 0.08, "2024-01-01"},
        {"Location D", 0.15, "2024-01-01"}
    };
    
    setupUI();
    setupChart();
}

// View A: Set the map view
void FluorinatedCompoundsPage::setupMap() {
    // Hide the chart view
    if (chartView) {
        chartView->hide();
    }

    // Create a map container
    mapWidget = new QWidget(this);
    QGridLayout* mapLayout = new QGridLayout(mapWidget);
    
    // Create a map background
    mapLabel = new QLabel(mapWidget);
    mapLabel->setStyleSheet("background-color: #f0f0f0;"); // Temporary background color
    mapLabel->setMinimumSize(400, 300);
    
    mapLayout->addWidget(mapLabel);
    mainLayout->addWidget(mapWidget, 1, 0, 1, 2);
    
    // Add a location marker
    for (const auto& data : testData) {
        QPushButton* marker = new QPushButton(mapWidget);
        marker->setFixedSize(20, 20);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> disX(0, 380);
        std::uniform_int_distribution<> disY(0, 280);
        marker->move(disX(gen), disY(gen));
        
        // Set the color
        QString color = getComplianceColor(data.value);
        marker->setStyleSheet(QString("background-color: %1; border-radius: 10px;").arg(color));
        
        // Add a tooltip
        QString tooltipText = QString(
            "Location: %1\n"
            "PFAS Level: %2 µg/L\n"
            "Status: %3\n"
            "Last Updated: %4"
        ).arg(data.location)
         .arg(data.value)
         .arg(data.value > PFAS_THRESHOLD ? "Exceeds threshold" : "Within safe levels")
         .arg(data.timestamp);
        
        marker->setToolTip(tooltipText);
        
        locationMarkers[data.location] = marker;
    }
}

// View B: Set up a time series chart
void FluorinatedCompoundsPage::setupChart() {
    QChart* chart = new QChart();
    series = new QLineSeries();
    
    chart->addSeries(series);
    chart->setTitle("PFAS Levels Over Time");
    
    // Set the axes
    QValueAxis* axisX = new QValueAxis();
    QValueAxis* axisY = new QValueAxis();
    
    axisX->setTitleText("Time");
    axisY->setTitleText("PFAS Level (µg/L)");
    
    // Set the Y-axis range to include the safety threshold
    axisY->setRange(0, PFAS_THRESHOLD * 2);
    
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    
    // Add a threshold guide
    QLineSeries* thresholdLine = new QLineSeries();
    thresholdLine->append(0, PFAS_THRESHOLD);
    thresholdLine->append(10, PFAS_THRESHOLD);
    thresholdLine->setPen(QPen(Qt::red, 1, Qt::DashLine));
    chart->addSeries(thresholdLine);
    thresholdLine->attachAxis(axisX);
    thresholdLine->attachAxis(axisY);

    connect(series, &QLineSeries::clicked, this, &FluorinatedCompoundsPage::showPopupInfo);
    
    chartView->setChart(chart);
}

QString FluorinatedCompoundsPage::getComplianceColor(double value) {
    if (value <= PFAS_THRESHOLD * 0.5) return "#00FF00"; // green
    if (value <= PFAS_THRESHOLD) return "#FFFF00"; // yellow
    return "#FF0000"; // red
}

void FluorinatedCompoundsPage::showPopupInfo(const QPointF& point) {
    QString info = QString(
        "PFAS Level: %1 µg/L\n"
        "Threshold: %2 µg/L\n"
        "Status: %3\n\n"
        "Environmental Impact:\n"
        "- Long-term persistence in environment\n"
        "- Potential groundwater contamination\n\n"
        "Health Implications:\n"
        "- May affect immune system\n"
        "- Potential developmental effects"
    ).arg(point.y(), 0, 'f', 3)
     .arg(PFAS_THRESHOLD)
     .arg(point.y() > PFAS_THRESHOLD ? "Exceeds threshold" : "Within safe levels");

    QToolTip::showText(QCursor::pos(), info);
}

void FluorinatedCompoundsPage::updateChart() {
    if (series) {
        series->clear();
        // Add test data points
        for (int i = 0; i < testData.size(); ++i) {
            series->append(i, testData[i].value);
        }
    }
}

void FluorinatedCompoundsPage::switchToMap() {
    if (chartView) {
        chartView->hide();
    }
    setupMap();
    isMapView = true;
}

void FluorinatedCompoundsPage::switchToTimeSeries() {
    if (mapWidget) {
        mapWidget->hide();
    }
    if (chartView) {
        chartView->show();
    }
    setupChart();
    isMapView = false;
}

void FluorinatedCompoundsPage::showLocationDetails(const QString& location) {
    // Displays location details
    for (const auto& data : testData) {
        if (data.location == location) {
            QString info = QString("Location: %1\nPFAS Level: %2 µg/L\nStatus: %3")
                .arg(location)
                .arg(data.value)
                .arg(data.value > PFAS_THRESHOLD ? "Exceeds threshold" : "Within safe levels");
            QToolTip::showText(QCursor::pos(), info);
            break;
        }
    }
}

void FluorinatedCompoundsPage::setupUI() {
    mainLayout = new QGridLayout(this);
    
    // Create a view toggle button
    mapViewButton = new QPushButton("Map View", this);
    timeSeriesButton = new QPushButton("Time Series", this);
    
    connect(mapViewButton, &QPushButton::clicked, this, &FluorinatedCompoundsPage::switchToMap);
    connect(timeSeriesButton, &QPushButton::clicked, this, &FluorinatedCompoundsPage::switchToTimeSeries);

    // Create a chart view
    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Create a threshold label
    thresholdLabel = new QLabel(QString("PFAS Threshold: %1 µg/L").arg(PFAS_THRESHOLD), this);

    // Layout arrangement
    mainLayout->addWidget(mapViewButton, 0, 0);
    mainLayout->addWidget(timeSeriesButton, 0, 1);
    mainLayout->addWidget(chartView, 1, 0, 1, 2);
    mainLayout->addWidget(thresholdLabel, 2, 0, 1, 2);
}