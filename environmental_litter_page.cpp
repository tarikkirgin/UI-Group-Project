#include "environmental_litter_page.hpp"
#include "location_dataset.hpp"
#include <QDebug>
#include <QtWidgets>
#include <QtCharts>

EnvironmentalLitterPage::EnvironmentalLitterPage() : QWidget() {
    setupUI();

    connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this, &EnvironmentalLitterPage::updateChart);
}

void EnvironmentalLitterPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // No outer margins
    mainLayout->setSpacing(10);

    // Title layout
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *title = new QLabel();
    title->setText("Environmental Litter Indicators");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("padding: 0px; margin: 0px; font-size: 16px; font-weight: bold; border: 1px solid red;"); // Border for debugging
    QFontMetrics fm(title->font());
    int textHeight = fm.height();
    title->setMinimumHeight(textHeight);
    title->setMaximumHeight(textHeight);
    topLayout->addWidget(title);

    // Horizontal layout for three sections
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(10);

    // Left column (checkbox layout)
    QVBoxLayout *leftLayout = new QVBoxLayout();

    QVBoxLayout *checkboxLayout = new QVBoxLayout();
    checkboxLayout->setContentsMargins(0, 0, 0, 0);
    checkboxLayout->setSpacing(1);

    QLabel *left_label = new QLabel();
    left_label->setText("Types of Litter:");
    left_label->setStyleSheet("margin-bottom: 1px; font-weight: bold; border: 1px solid green;"); 
    QFontMetrics font(left_label->font());
    int labeltextHeight = font.height();
    left_label->setMinimumHeight(labeltextHeight);
    left_label->setMaximumHeight(labeltextHeight);

    litter_type_1 = new QCheckBox("Litter Type 1");
    litter_type_2 = new QCheckBox("Litter Type 2");
    litter_type_3 = new QCheckBox("Litter Type 3");

    litter_type_1->setStyleSheet("border: 1px solid orange;"); // Border for debugging
    litter_type_2->setStyleSheet("border: 1px solid orange;"); // Border for debugging
    litter_type_3->setStyleSheet("border: 1px solid orange;"); // Border for debugging

    checkboxLayout->addWidget(left_label);
    checkboxLayout->addWidget(litter_type_1);
    checkboxLayout->addWidget(litter_type_2);
    checkboxLayout->addWidget(litter_type_3);

    leftLayout->addLayout(checkboxLayout);

    // Middle column
    QVBoxLayout *chartLayout = new QVBoxLayout();
    QChart *chart = new QChart();
    chart->setTitle("Litter levels by Type");
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartLayout->addWidget(chartView);



    // Right column
    QVBoxLayout *rightLayout = new QVBoxLayout();
    QLabel *right_label = new QLabel();
    right_label->setText("Right");
    right_label->setAlignment(Qt::AlignCenter);
    right_label->setStyleSheet("border: 1px solid yellow;"); // Border for debugging
    rightLayout->addWidget(right_label);

    // Add layouts to the horizontal layout
    hLayout->addLayout(leftLayout);
    hLayout->addLayout(chartLayout);
    hLayout->addLayout(rightLayout);

    // Add all sections to the main layout
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(hLayout);

    setLayout(mainLayout);

    // connect changing of checkboxes to the updateChart() function
    connect(litter_type_1, &QCheckBox::stateChanged, this, &EnvironmentalLitterPage::updateChart);
    connect(litter_type_2, &QCheckBox::stateChanged, this, &EnvironmentalLitterPage::updateChart);
    connect(litter_type_3, &QCheckBox::stateChanged, this, &EnvironmentalLitterPage::updateChart);
}

   

void EnvironmentalLitterPage::updateChart() {
    qDebug() << "Update chart triggered";
    QChart *chart = chartView->chart();

    // Clear existing series and axes
     // Clear existing series and axes
    while (!chart->series().isEmpty()){
        chart->removeSeries(chart->series().first());
    }

    while (!chart->axes().isEmpty()) {
        chart->removeAxis(chart->axes().first());
    }

    const auto &locationDataset = LocationDataset::instance().data;

    // Checkbox-to-determinand map
    QMap<QString, QCheckBox *> checkboxMap = {
        {"Bathing Water Profile : Other Litter (incl. plastics)", litter_type_1},
        {"TarryResidus", litter_type_2},
        {"SewageDebris", litter_type_3},
    };

    // Identify active categories
    QStringList activeCategories;
    for (auto it = checkboxMap.begin(); it != checkboxMap.end(); ++it) {
        if (it.value() && it.value()->isChecked()) {
            activeCategories.append(it.key());
        }
    }

    // Initialize totals for each category
    QMap<QString, double> litterTotals;
    for (const QString &category : activeCategories) {
        litterTotals[category] = 0.0;
    }

    // Aggregate values
    for (const Sample &sample : locationDataset) {
        QString determinandLabel = QString::fromStdString(sample.getDeterminand().getLabel());
        if (activeCategories.contains(determinandLabel)) {
            double value = sample.getResult().getValue();
            if (!std::isnan(value) && !std::isinf(value)) {
                litterTotals[determinandLabel] += value;
            }
        }
    }

    // Debug: Print totals to verify summation
    for (const QString &category : litterTotals.keys()) {
        qDebug() << "Category:" << category << "Total:" << litterTotals[category];
    }

    // Create bar series
    QBarSeries *barSeries = new QBarSeries();
    for (const QString &category : activeCategories) {
        QBarSet *set = new QBarSet(category);
        double total = litterTotals.value(category, 0.0);
        set->append(total); // Append the total value for the category
        barSeries->append(set);
    }

    // Add the series to the chart
    chart->addSeries(barSeries);

    // Setup X-Axis
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(activeCategories);
    chart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    // Setup Y-Axis
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Total Litter");
    double max_value = 0.0;
    for (double value : litterTotals) {
        if (!std::isnan(value) && !std::isinf(value)) {
            max_value = std::max(max_value, value);
        }
    }
    max_value = std::max(max_value, 10.0); // Ensure a minimum range
    axisY->setRange(0, max_value);
    chart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);

    // Update chart title
    if (activeCategories.isEmpty()) {
        chart->setTitle("No data to display");
    } else {
        chart->setTitle("Litter Levels by Type");
    }
}
