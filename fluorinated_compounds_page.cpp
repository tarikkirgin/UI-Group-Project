#include "fluorinated_compounds_page.hpp"
#include <QGridLayout>

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
        

    auto closeButton = new QPushButton(tr("close"), this);
    layout->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

FluorinatedCompoundsPage::FluorinatedCompoundsPage(QWidget* parent) : QWidget(parent) {
    setupUI();
    processData();
    createChart();
    updateStatusIndicators();
}

void FluorinatedCompoundsPage::setupUI() {
    mainLayout = new QVBoxLayout(this);
    
    // Title
    auto titleLabel = new QLabel(tr("Monitoring of fluoride compounds"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Status indication area
    statusLayout = new QHBoxLayout();
    mainLayout->addLayout(statusLayout);

    // Chart area
    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(chartView);

    // Info button
    infoButton = new QPushButton(tr("View Details"), this);
    connect(infoButton, &QPushButton::clicked, this, &FluorinatedCompoundsPage::showInfoDialog);
    mainLayout->addWidget(infoButton);
}

void FluorinatedCompoundsPage::processData() {
    const auto& dataset = Dataset::instance();
    
    for (const auto& sample : dataset.data) {
        CompoundData data;
        data.location = QString::fromStdString(sample.getSamplingPoint().getLabel());
        data.time = QDateTime::fromString(QString::fromStdString(sample.getTime()), Qt::ISODate);
        data.value = sample.getResult().getValue();
        compoundData.push_back(data);
    }
}

void FluorinatedCompoundsPage::createChart() {
    chart = new QChart();
    chart->setTitle(tr("Trends in fluoride concentrations"));
    
    // Create folded series
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

    // Add series to the chart
    for (auto series : seriesMap.values()) {
        chart->addSeries(series);
    }

    // Create axes
    auto axisX = new QDateTimeAxis;
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText(tr("dates"));
    chart->addAxis(axisX, Qt::AlignBottom);

    auto axisY = new QValueAxis;
    axisY->setTitleText(tr("concentration (μg/L)"));
    axisY->setMin(0);
    chart->addAxis(axisY, Qt::AlignLeft);

    // Add a safety threshold reference line
    auto threshold = new QLineSeries(chart);
    threshold->setName(tr("safety threshold"));
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

    // Associate series to axes
    for (auto series : seriesMap.values()) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }
    threshold->attachAxis(axisX);
    threshold->attachAxis(axisY);

    chartView->setChart(chart);
}

QColor FluorinatedCompoundsPage::getStatusColor(double value) const {
    if (value > SAFETY_THRESHOLD * 1.5) return Qt::red;
    if (value > SAFETY_THRESHOLD) return QColor(255, 165, 0); // Orange
    return Qt::green;
}

void FluorinatedCompoundsPage::updateStatusIndicators() {
    // Clear esxisting label
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
