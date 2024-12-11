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
    QString selectedCompound = compoundSelector->currentText();
    
    // 统计每个位置的有效数据点数量
    for (const auto& sample : allData) {
        QString determinandLabel = QString::fromStdString(sample.getDeterminand().getLabel());
        if (determinandLabel.startsWith(selectedCompound)) {
            QString location = QString::fromStdString(sample.getSamplingPoint().getLabel());
            if (!location.isEmpty()) {
                double value = sample.getResult().getValue();
                if (!std::isnan(value) && value >= 0) {
                    locationDataCount[location]++;
                    qDebug() << "Location:" << location << "Count:" << locationDataCount[location];
                }
            }
        }
    }
    
    // 只添加有2个以上数据点的位置
    QStringList locationList;
    for (auto it = locationDataCount.constBegin(); it != locationDataCount.constEnd(); ++it) {
        if (it.value() > 2) {  // 改为大于2个数据点
            locationList.append(it.key());
            qDebug() << "Added location:" << it.key() << "with" << it.value() << "data points";
        }
    }
    
    if (locationList.isEmpty()) {
        locationSelector->addItem(tr("No data available"));
        locationSelector->setEnabled(false);
        qDebug() << "No locations with more than 2 data points";
    } else {
        std::sort(locationList.begin(), locationList.end());
        locationSelector->addItems(locationList);
        locationSelector->setEnabled(true);
        
        int index = locationSelector->findText(currentLocation);
        if (index >= 0) {
            locationSelector->setCurrentIndex(index);
        }
        qDebug() << "Added" << locationList.size() << "locations to selector";
    }
    
    updateChart();
}

void FluorinatedCompoundsPage::processData() {
    qDebug() << "开始处理数据...";
    compoundData.clear();
    const auto& allData = Dataset::instance().data;
    QString selectedLocation = locationSelector->currentText();
    QString selectedCompound = compoundSelector->currentText();
    
    if (selectedLocation.isEmpty() || selectedLocation == tr("No data available")) {
        qDebug() << "No valid location selected";
        return;
    }
    
    QVector<CompoundData> tempData;
    
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
            qDebug() << "Found valid data point:" << timeStr << value;
        }
    }
    
    // 只有当数据点数量大于2时才使用这些数据
    if (tempData.size() > 2) {
        compoundData = tempData;
        std::sort(compoundData.begin(), compoundData.end(),
                  [](const CompoundData& a, const CompoundData& b) {
                      return a.time < b.time;
                  });
        qDebug() << "Processed" << compoundData.size() << "data points for charting";
    } else {
        qDebug() << "Not enough data points:" << tempData.size() << "found (need >2)";
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
        qDebug() << "No data available for charting";
        return;
    }

    // Create series and set data
    auto series = new QLineSeries(chart);
    series->setName(QString("%1 - %2").arg(selectedCompound, selectedLocation));
    
    QPen pen(Qt::blue);
    pen.setWidth(2);
    series->setPen(pen);

    for (const auto &data : compoundData) {
        series->append(data.time.toMSecsSinceEpoch(), data.value);
        qDebug() << "Adding point to chart:" << data.time.toString() << data.value;
    }
    
    chart->addSeries(series);

    // 设置坐标轴
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
    
    qDebug() << "Chart updated successfully";
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
