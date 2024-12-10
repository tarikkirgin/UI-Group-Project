#include "fluorinated_compounds_page.hpp"
#include <QGridLayout>
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
        

    auto closeButton = new QPushButton(tr("close"), this);
    layout->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

FluorinatedCompoundsPage::FluorinatedCompoundsPage(QWidget* parent) : QWidget(parent) {
    setupUI();
    
    // 添加调试信息
    qDebug() << "当前应用程序路径:" << QCoreApplication::applicationDirPath();
    qDebug() << "当前工作目录:" << QDir::currentPath();
    
    // 尝试直接使用程序目录下的文件路径
    QString filePath = QCoreApplication::applicationDirPath() + "/Y-2024.csv";
    qDebug() << "尝试打开文件:" << filePath;
    
    if (loadDataFromCSV(filePath)) {
        createChart();
        updateStatusIndicators();
    } else {
        qDebug() << "Failed to load data file";
        
        // 如果失败，尝试列出目录内容
        QDir dir(QCoreApplication::applicationDirPath());
        qDebug() << "目录内容:";
        for(const QString& file : dir.entryList()) {
            qDebug() << file;
        }
    }
}


bool FluorinatedCompoundsPage::loadDataFromCSV(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file:" << filename;
        return false;
    }

    int loadedCount = 0;
    QTextStream in(&file);
    bool headerProcessed = false;
    compoundData.clear();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        // Skip header row
        if (!headerProcessed) {
            headerProcessed = true;
            continue;
        }

        // Only look for "Fluoride" specifically
        if (!line.contains("Fluoride", Qt::CaseInsensitive)) {
            continue;
        }

        QStringList parts = line.split(",");
        if (parts.size() < 2) continue;

        CompoundData data;
        
        // Extract location ID (NE-XXXXXXXX format)
        QRegularExpression locationRegex("NE-[0-9A-Z]+");
        auto locationMatch = locationRegex.match(line);
        if (locationMatch.hasMatch()) {
            data.location = locationMatch.captured(0);
        }

        // Extract date (yyyy-MM-ddThh:mm:ss format)
        QRegularExpression dateRegex("\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}");
        auto dateMatch = dateRegex.match(line);
        if (dateMatch.hasMatch()) {
            data.time = QDateTime::fromString(dateMatch.captured(0), "yyyy-MM-ddThh:mm:ss");
        }

        // Extract and normalize concentration value
        QRegularExpression valueRegex(",([0-9.]+),");
        auto valueMatch = valueRegex.match(line);
        if (valueMatch.hasMatch()) {
            bool ok;
            // Convert to more reasonable range (assuming original values are in ng/L)
            data.value = valueMatch.captured(1).toDouble(&ok) / 1000.0; // Convert to μg/L
            if (!ok) continue;
        }

        // Only add complete data
        if (data.time.isValid() && !data.location.isEmpty() && data.value >= 0) {
            compoundData.push_back(data);
            loadedCount++;
            qDebug() << "Added Fluoride record:" << data.time.toString() << data.location << data.value << "μg/L";
        }
    }

    file.close();
    qDebug() << "Successfully loaded" << loadedCount << "fluoride records";
    return !compoundData.empty();
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

void FluorinatedCompoundsPage::createChart() {
    chart = new QChart();
    chart->setTitle(tr("Fluoride Concentration Trends"));
    
    // Create series for each location
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
    axisX->setTitleText(tr("Date"));
    chart->addAxis(axisX, Qt::AlignBottom);

    auto axisY = new QValueAxis;
    axisY->setTitleText(tr("Concentration (μg/L)"));
    axisY->setMin(0);
    // Adjust max based on actual data
    double maxVal = 0;
    for (const auto& data : compoundData) {
        maxVal = qMax(maxVal, data.value);
    }
    axisY->setMax(qCeil(maxVal * 1.1)); // Add 10% margin
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

    // Attach axes
    for (auto series : seriesMap.values()) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }
    threshold->attachAxis(axisX);
    threshold->attachAxis(axisY);

    chartView->setChart(chart);

    auto statusLabel = new QLabel(this);
    mainLayout->addWidget(statusLabel);
    if (compoundData.empty()) {
        statusLabel->setText(tr("No data loaded"));
        statusLabel->setStyleSheet("color: red");
    }
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
