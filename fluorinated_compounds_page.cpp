#include "fluorinated_compounds_page.hpp"
#include "dataset.hpp"
#include <QDebug>

FluorinatedInfoDialog::FluorinatedInfoDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("About Fluoride Compounds"));
    setMinimumSize(400, 300);

    auto layout = new QVBoxLayout(this);

    auto addInfoSection = [&](const QString& title, const QString& content) {
        auto label = new QLabel(QString("<b>%1</b>").arg(title));
        layout->addWidget(label);
        label = new QLabel(content);
        label->setWordWrap(true);
        layout->addWidget(label);
        layout->addSpacing(10);
    };

    addInfoSection("What are Fluorinated Compounds?",
        "Fluorinated compounds include a series of fluorinated chemical substances, such as PFAS (per- and polyfluoroalkyl substances). \n"
        "These compounds are widely used in industry due to their unique chemical properties, but they also bring about environmental problems.");
    
    addInfoSection("Environmental Persistence",
        "• Extremely slow degradation in the environment\n"
        "• Have bioaccumulative properties\n"
        "• Can migrate long distances in environmental media");

    addInfoSection("Potential Impacts",
        "• May affect the endocrine systems of organisms\n"
        "• May impact immune functions\n"
        "• Pose threats to aquatic ecosystems\n"
        "• Bioaccumulate through the food chain");

    addInfoSection("Monitoring Standards",
        QString("The current safety threshold is %1 μg/L. Measures must be taken if this threshold is exceeded.")
        .arg(FluorinatedCompoundsPage::SAFETY_THRESHOLD));

    auto closeButton = new QPushButton(tr("Close"), this);
    layout->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

FluorinatedCompoundsPage::FluorinatedCompoundsPage(QWidget* parent) : QWidget(parent) {
    setupUI();
    // Connect to dataset updates
    connect(&Dataset::instance(), &Dataset::dataUpdated, this, &FluorinatedCompoundsPage::updateChart);
    // Initial update
    updateChart();
}

void FluorinatedCompoundsPage::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    // Title layout with proper styling
    auto titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(0, 0, 0, 0);

    auto titleLabel = new QLabel(tr("Monitoring of fluoride compounds"));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("padding: 0px; margin: 0px; font-size: 16px; font-weight: bold;");
    QFontMetrics fm(titleLabel->font());
    int textHeight = fm.height();
    titleLabel->setMinimumHeight(textHeight);
    titleLabel->setMaximumHeight(textHeight);
    titleLayout->addWidget(titleLabel);

    // Status indicators layout
    statusLayout = new QHBoxLayout();
    statusLayout->setContentsMargins(0, 0, 0, 0);
    statusLayout->setSpacing(20);
    statusLayout->setAlignment(Qt::AlignCenter);

    // Chart layout
    auto chartLayout = new QVBoxLayout();
    chart = new QChart();
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(300);
    chartLayout->addWidget(chartView);

    // Info button
    infoButton = new QPushButton(tr("View Details"));
    connect(infoButton, &QPushButton::clicked, this, &FluorinatedCompoundsPage::showInfoDialog);

    // Add all layouts to main layout
    mainLayout->addLayout(titleLayout);
    mainLayout->addLayout(statusLayout);
    mainLayout->addLayout(chartLayout);
    mainLayout->addWidget(infoButton);

    setLayout(mainLayout);
}

void FluorinatedCompoundsPage::processData() {
    compoundData.clear();
    const auto& dataset = Dataset::instance();
    
    for (const auto& sample : dataset.data) {
        
        QString determinandLabel = QString::fromStdString(sample.getDeterminand().getLabel());
        if (!determinandLabel.startsWith("PFOS")) {
            continue;
        }

        CompoundData data;
        QString timeStr = QString::fromStdString(sample.getTime());
        data.location = QString::fromStdString(sample.getSamplingPoint().getLabel());
        data.time = QDateTime::fromString(timeStr, Qt::ISODate);
        data.value = sample.getResult().getValue();
        
        
        if (data.time.isValid() && 
            !std::isnan(data.value) && 
            !data.location.isEmpty() && 
            data.value >= 0) { 
            
            compoundData.push_back(data);
            qDebug() << "Processing PFOS data point:" 
                     << "Type:" << determinandLabel
                     << "Location:" << data.location 
                     << "Time:" << data.time 
                     << "Value:" << data.value;
        }
    }
    

    std::sort(compoundData.begin(), compoundData.end(), 
              [](const CompoundData& a, const CompoundData& b) {
                  return a.time < b.time;
              });
              
    qDebug() << "Total PFOS data points processed:" << compoundData.size();
}

void FluorinatedCompoundsPage::updateChart() {
    qDebug() << "Updating fluorinated compounds chart";
    
    // Process the latest data
    processData();
    
    // Clear existing chart
    chart->removeAllSeries();
    while (!chart->axes().isEmpty()) {
        chart->removeAxis(chart->axes().first());
    }

    // Create series map
    QMap<QString, QLineSeries*> seriesMap;
    
    for (const auto& data : compoundData) {
        if (!seriesMap.contains(data.location)) {
            auto series = new QLineSeries(chart);
            series->setName(data.location);
            seriesMap[data.location] = series;
        }
        
        auto series = seriesMap[data.location];
        series->append(data.time.toMSecsSinceEpoch(), data.value);
    }

    // Add series to chart
    for (auto series : seriesMap.values()) {
        chart->addSeries(series);
    }

    // Create axes
    auto axisX = new QDateTimeAxis;
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText(tr("Dates"));
    chart->addAxis(axisX, Qt::AlignBottom);

    auto axisY = new QValueAxis;
    axisY->setTitleText(tr("Concentration (μg/L)"));
    axisY->setMin(0);
    chart->addAxis(axisY, Qt::AlignLeft);

    // Add safety threshold line
    auto threshold = new QLineSeries(chart);
    threshold->setName(tr("Safety Threshold"));
    
    if (!compoundData.empty()) {
        auto minTime = compoundData.front().time.toMSecsSinceEpoch();
        auto maxTime = compoundData.back().time.toMSecsSinceEpoch();
        threshold->append(minTime, SAFETY_THRESHOLD);
        threshold->append(maxTime, SAFETY_THRESHOLD);
    }
    
    QPen pen(Qt::red);
    pen.setStyle(Qt::DashLine);
    threshold->setPen(pen);
    chart->addSeries(threshold);

    // Attach axes to series
    for (auto series : seriesMap.values()) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }
    threshold->attachAxis(axisX);
    threshold->attachAxis(axisY);

    chart->setTitle(tr("Trends in Fluoride Concentrations"));
    
    // Update status indicators
    updateStatusIndicators();
}

QColor FluorinatedCompoundsPage::getStatusColor(double value) const {
    if (value > SAFETY_THRESHOLD * 1.5) return Qt::red;
    if (value > SAFETY_THRESHOLD) return QColor(255, 165, 0); // Orange
    return Qt::green;
}

void FluorinatedCompoundsPage::updateStatusIndicators() {
    // Clear existing labels
    for (auto label : statusLabels) {
        statusLayout->removeWidget(label);
        delete label;
    }
    statusLabels.clear();

    // Create a new label
    QMap<QString, double> latestValues;
    for (const auto& data : compoundData) {
        latestValues[data.location] = data.value;
    }

    for (auto it = latestValues.begin(); it != latestValues.end(); ++it) {
        auto container = new QWidget(this);
        auto layout = new QVBoxLayout(container);
        
        auto locationLabel = new QLabel(it.key(), container);
        auto valueLabel = new QLabel(QString("%1 μg/L").arg(it.value(), 0, 'f', 3), container);
        
        QFont valueFont = valueLabel->font();
        valueFont.setPointSize(12);
        valueFont.setBold(true);
        valueLabel->setFont(valueFont);
        
        valueLabel->setStyleSheet(QString("color: %1")
            .arg(getStatusColor(it.value()).name()));
        
        layout->addWidget(locationLabel);
        layout->addWidget(valueLabel);
        
        statusLayout->addWidget(container);
        statusLabels.push_back(locationLabel);
        statusLabels.push_back(valueLabel);
    }
}

void FluorinatedCompoundsPage::showInfoDialog() {
    FluorinatedInfoDialog dialog(this);
    dialog.exec();
}
