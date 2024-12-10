#include "fluorinated_compounds_page.hpp"
#include "location_dataset.hpp"
#include <QDebug>
#include <QComboBox>

// Dialog implementation
FluorinatedInfoDialog::FluorinatedInfoDialog(QWidget *parent)
    : QDialog(parent) {
  setWindowTitle(tr("About Fluoride Compounds"));
  setMinimumSize(400, 300);

  auto layout = new QVBoxLayout(this);

  auto addInfoSection = [&](const QString &title, const QString &content) {
    auto label = new QLabel(QString("<b>%1</b>").arg(title));
    layout->addWidget(label);
    label = new QLabel(content);
    label->setWordWrap(true);
    layout->addWidget(label);
    layout->addSpacing(10);
  };

  addInfoSection(
      "What are Fluorinated Compounds?",
      "Fluorinated compounds include a series of fluorinated chemical "
      "substances, such as PFAS (per- and polyfluoroalkyl substances). \n"
      "These compounds are widely used in industry due to their unique "
      "chemical properties, but they also bring about environmental problems.");

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
                 QString("The current safety threshold is %1 μg/L. Measures "
                         "must be taken if this threshold is exceeded.")
                     .arg(FluorinatedCompoundsPage::SAFETY_THRESHOLD));

  auto closeButton = new QPushButton(tr("Close"), this);
  layout->addWidget(closeButton);
  connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

// Main page implementation
FluorinatedCompoundsPage::FluorinatedCompoundsPage(QWidget* parent)
    : QWidget(parent) {
    setupUI();
    
    connect(&Dataset::instance(), &Dataset::dataUpdated,
            this, &FluorinatedCompoundsPage::updateLocationList);
    connect(&LocationDataset::instance(), &LocationDataset::dataUpdated,
            this, &FluorinatedCompoundsPage::updateLocationList);
            
    updateLocationList();
}

void FluorinatedCompoundsPage::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    auto titleLabel = new QLabel(tr("Monitoring of fluoride compounds"));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "padding: 0px; margin: 0px; font-size: 16px; font-weight: bold;");
    QFontMetrics fm(titleLabel->font());
    titleLabel->setMinimumHeight(fm.height());
    titleLabel->setMaximumHeight(fm.height());
    
    // Add compound selector
    compoundSelector = new QComboBox(this);
    compoundSelector->addItems({"PFOS", "PFBA", "PFNS"});
    compoundSelector->setMinimumWidth(100);
    connect(compoundSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FluorinatedCompoundsPage::updateChart);
    
    // Add location selector
    locationSelector = new QComboBox(this);
    locationSelector->setMinimumWidth(200);
    connect(locationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FluorinatedCompoundsPage::updateChart);
    
    auto controlLayout = new QHBoxLayout();
    controlLayout->addWidget(new QLabel(tr("Compound Type:")));
    controlLayout->addWidget(compoundSelector);
    controlLayout->addSpacing(20);
    controlLayout->addWidget(new QLabel(tr("Select Location:")));
    controlLayout->addWidget(locationSelector);
    controlLayout->addStretch();

    // Chart layout
    chart = new QChart();
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(300);

    // Info button
    infoButton = new QPushButton(tr("View Details"));
    connect(infoButton, &QPushButton::clicked, this,
            &FluorinatedCompoundsPage::showInfoDialog);

    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(chartView);
    mainLayout->addWidget(infoButton);

    setLayout(mainLayout);
}

void FluorinatedCompoundsPage::updateLocationList() {
    QString currentLocation = locationSelector->currentText();
    locationSelector->clear();
    
    const auto& allData = Dataset::instance().data;
    QMap<QString, int> locationDataCount;  
    // Used to count the number of data points per location

    QString selectedCompound = compoundSelector->currentText();
    
    // First count the number of valid data points at each location
    for (const auto& sample : allData) {
        QString determinandLabel = QString::fromStdString(sample.getDeterminand().getLabel());
        if (determinandLabel.startsWith(selectedCompound)) {
            QString location = QString::fromStdString(sample.getSamplingPoint().getLabel());
            if (!location.isEmpty()) {
                double value = sample.getResult().getValue();
                if (!std::isnan(value) && value >= 0) {
                    locationDataCount[location]++;
                }
            }
        }
    }
    
    // Add only locations with multiple data points
    QStringList locationList;
    for (auto it = locationDataCount.constBegin(); it != locationDataCount.constEnd(); ++it) {
        if (it.value() > 1) {  // Only add locations with more than one data point
            locationList.append(it.key());
        }
    }
    
    if (locationList.isEmpty()) {
        locationSelector->addItem(tr("No data available"));
        locationSelector->setEnabled(false);
    } else {
        std::sort(locationList.begin(), locationList.end());
        locationSelector->addItems(locationList);
        locationSelector->setEnabled(true);
        
        //Restore previous selection (if possible)
        int index = locationSelector->findText(currentLocation);
        if (index >= 0) {
            locationSelector->setCurrentIndex(index);
        }
    }
    
    updateChart();
}

void FluorinatedCompoundsPage::processData() {
    qDebug() << "Start processing data...";
    compoundData.clear();
    const auto& allData = Dataset::instance().data;
    QString selectedLocation = locationSelector->currentText();
    QString selectedCompound = compoundSelector->currentText();
    
    if (selectedLocation.isEmpty() || selectedLocation == tr("No data available")) {
        return;
    }
    
    QVector<CompoundData> tempData;  // Temporarily store all data points
    
    for (const auto& sample : allData) {
        QString determinandLabel = QString::fromStdString(sample.getDeterminand().getLabel());
        QString location = QString::fromStdString(sample.getSamplingPoint().getLabel());
        
        if (!determinandLabel.startsWith(selectedCompound) || location != selectedLocation) {
            continue;
        }

        QString timeStr = QString::fromStdString(sample.getTime());
        QDateTime time = QDateTime::fromString(timeStr, Qt::ISODate);
        double value = sample.getResult().getValue();

        if (time.isValid() && !std::isnan(value) && value >= 0) {
            CompoundData data;
            data.location = location;
            data.time = time;
            data.value = value;
            tempData.push_back(data);
        }
    }
    
    // Use these data only if the number of data points is greater than 1
    if (tempData.size() > 1) {
        compoundData = tempData;
        // Sort by time
        std::sort(compoundData.begin(), compoundData.end(),
                  [](const CompoundData& a, const CompoundData& b) {
                      return a.time < b.time;
                  });
    }
    
    
}

void FluorinatedCompoundsPage::updateChart() {
    QString selectedCompound = compoundSelector->currentText();
    QString selectedLocation = locationSelector->currentText();
    
    qDebug() << "Updating chart for compound:" << selectedCompound 
             << "at location:" << selectedLocation;
    
    processData();

    chart->removeAllSeries();
    while (!chart->axes().isEmpty()) {
        chart->removeAxis(chart->axes().first());
    }

    if (compoundData.empty()) {
        chart->setTitle(tr("No data available for selected combination"));
        return;
    }

    // Create series
    auto series = new QLineSeries(chart);
    series->setName(QString("%1 - %2").arg(selectedCompound, selectedLocation));
    
    // Set line style
    QPen pen(Qt::blue);
    pen.setWidth(2);
    series->setPen(pen);

    // Add data points
    for (const auto &data : compoundData) {
        series->append(data.time.toMSecsSinceEpoch(), data.value);
    }
    
    chart->addSeries(series);

    // Set up axes
    auto axisX = new QDateTimeAxis;
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText(tr("Dates"));
    chart->addAxis(axisX, Qt::AlignBottom);

    auto axisY = new QValueAxis;
    axisY->setTitleText(tr("Concentration (μg/L)"));
    axisY->setMin(0);
    
    double maxValue = 0;
    for (const auto &data : compoundData) {
        maxValue = qMax(maxValue, data.value);
    }
    axisY->setMax(maxValue * 1.2);
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chart->setTitle(tr("%1 Concentration Trends - %2")
                   .arg(selectedCompound)
                   .arg(selectedLocation));
    
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
}

QColor FluorinatedCompoundsPage::getStatusColor(double value) const {
    if (value > SAFETY_THRESHOLD * 1.5)
        return Qt::red;
    if (value > SAFETY_THRESHOLD)
        return QColor(255, 165, 0); // Orange
    return Qt::green;
}

void FluorinatedCompoundsPage::showInfoDialog() {
    FluorinatedInfoDialog dialog(this);
    dialog.exec();
}
