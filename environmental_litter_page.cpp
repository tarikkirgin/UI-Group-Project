#include "environmental_litter_page.hpp"
#include "location_dataset.hpp"
#include <QDebug>
#include <QtWidgets>
#include <QtCharts>
#include "pollutant_card.hpp"
#include "flowlayout.h"




EnvironmentalLitterPage::EnvironmentalLitterPage() : QWidget() {
    setupUI();

    connect(&LocationDataset::instance(), &LocationDataset::dataUpdated, this, &EnvironmentalLitterPage::updateChart);
}

void EnvironmentalLitterPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // No outer margins
    mainLayout->setSpacing(10);

    // Title layout
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *title = new QLabel();
    title->setText("Environmental Litter Indicators");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("padding: 0px; margin: 0px; font-size: 16px; font-weight: bold; ");
    QFontMetrics fm(title->font());
    int textHeight = fm.height();
    title->setMinimumHeight(textHeight);
    title->setMaximumHeight(textHeight);
    titleLayout->addWidget(title);

    // checkbox part
    QHBoxLayout *topLayout = new QHBoxLayout();

    materialDropdown = new QComboBox();
    materialDropdown->addItem("All materials");


    QHBoxLayout *checkboxRowLayout = new QHBoxLayout();
    checkboxRowLayout->setContentsMargins(0,0,0,0);
    checkboxRowLayout->setSpacing(20);

    checkboxRowLayout->setAlignment(Qt::AlignCenter);

    litter_type_1 = new QCheckBox("Other Litter (incl. plastics)");
    litter_type_2 = new QCheckBox("Tarry Residue");
    litter_type_3 = new QCheckBox("Sewage Debris");

    litter_type_1->setChecked(true);
    litter_type_2->setChecked(true);
    litter_type_3->setChecked(true);


    checkboxRowLayout->addWidget(litter_type_1);
    checkboxRowLayout->addWidget(litter_type_2);
    checkboxRowLayout->addWidget(litter_type_3);

    topLayout->addWidget(materialDropdown);
    topLayout->addLayout(checkboxRowLayout);

    connect(materialDropdown, &QComboBox::currentTextChanged, this, &EnvironmentalLitterPage::updateChart);


    // gtrid section
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setSpacing(10);

    // Horizontal layout for three sections

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(10);

    // Middle column
    QVBoxLayout *chartLayout = new QVBoxLayout();
    QChart *chart = new QChart();
    chart->setTitle("Litter levels by Type");
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(300);
    chartLayout->addWidget(chartView);

    



    // Right column
    QVBoxLayout *rightLayout = new QVBoxLayout();
    pieChart = new QChart();
    pieChartView = new QChartView(pieChart);
    pieChartView->setRenderHint(QPainter::Antialiasing);
    pieChartView->setMinimumHeight(300);
    rightLayout->addWidget(pieChartView);

    // Add layouts to the horizontal layout
    gridLayout->addLayout(chartLayout, 0, 0);
    gridLayout->addLayout(rightLayout, 0, 1);

    gridLayout->setColumnStretch(0,2);
    gridLayout->setColumnStretch(1,1);

    // pollutant card layout
    QWidget *cardContainer = new QWidget();
    FlowLayout *flowLayout = new FlowLayout(cardContainer, -1, 20, 20);

    for (auto litterType = determinandsMap.begin(); litterType != determinandsMap.end(); litterType++){
        PollutantCard *pollutant_card = new PollutantCard(litterType.key().toStdString(), litterType.value());
        pollutant_card->setMaximumWidth(350);
        flowLayout->addWidget(pollutant_card);
        pollutantCards.push_back(pollutant_card);
    }




    // Add all sections to the main layout
    mainLayout->addLayout(titleLayout);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(gridLayout);
    mainLayout->addWidget(cardContainer);

    setLayout(mainLayout);

    // connect changing of checkboxes to the updateChart() function
    connect(litter_type_1, &QCheckBox::stateChanged, this, &EnvironmentalLitterPage::updateChart);
    connect(litter_type_2, &QCheckBox::stateChanged, this, &EnvironmentalLitterPage::updateChart);
    connect(litter_type_3, &QCheckBox::stateChanged, this, &EnvironmentalLitterPage::updateChart);
    

}

   

void EnvironmentalLitterPage::updateChart() {
    
    qDebug() << "Update chart triggered";

    

    
    if (!materialDropdown) {
        qDebug() << "materialDropdown is not initialized!";
        return;
    }

        const auto &data = LocationDataset::instance().data;
        QSet<QString> materialTypes;

        for (const Sample &sample : data) {
            if (sample.getResult().getValue() != 0){
                materialTypes.insert(QString::fromStdString(sample.getSampledMaterialType()));
                qDebug() << "Material type found:" << sample.getSampledMaterialType();
            }
            
        }
        for (const QString &materialType : materialTypes) {
            if (materialDropdown->findText(materialType) == -1){ // if NOT in lready
                materialDropdown->addItem(materialType);
            }
            
        }


    QString selectedMaterial = materialDropdown->currentText();
    if (selectedMaterial == "All materials") {
        qDebug() << "Empty?";
        selectedMaterial.clear();
    }


    QChart *chart = chartView->chart();

    // Clear existing series and axes
    while (!chart->series().isEmpty()) {
        chart->removeSeries(chart->series().first());
    }

    while (!chart->axes().isEmpty()) {
        chart->removeAxis(chart->axes().first());
    }

    const auto &locationDataset = LocationDataset::instance().data;

    // Map checkboxes to categories
    QMap<QString, QCheckBox *> checkboxMap = {
        {"BWP - O.L.", litter_type_1},
        {"TarryResidus", litter_type_2},
        {"SewageDebris", litter_type_3},
    };

    // Find active categories based on checkbox selection
    QStringList activeCategories;
    for (auto it = checkboxMap.begin(); it != checkboxMap.end(); ++it) {
        if (it.value() && it.value()->isChecked()) {
            activeCategories.append(it.key());
        }
    }

    // Collect totals per category - go through each record and get the determinand - if activecategories has this determinand, then add the value to litterTotals[determinandLabel]
    QMap<QString, double> litterTotals;
    for (const Sample &sample : locationDataset) {
        QString determinandLabel = QString::fromStdString(sample.getDeterminand().getLabel());
        QString materialLabel = QString::fromStdString(sample.getSampledMaterialType());
        if ((selectedMaterial.isEmpty() || materialLabel == selectedMaterial) && activeCategories.contains(determinandLabel)) {
            litterTotals[determinandLabel] += sample.getResult().getValue();
        }
    }

    // Debugging: Ensure you have computed the right totals
    qDebug() << "Litter Totals:" << litterTotals;

    // Set up the bar chart series
    QBarSeries *barSeries = new QBarSeries();


    // For each category selected, add that category's litterTotal to the set
    QBarSet *set = new QBarSet("");
    chart->legend()->setVisible(false);
    for (int i = 0; i < activeCategories.size(); ++i) {
        set->append(litterTotals[activeCategories[i]]);
    }

    connect(set, &QBarSet::hovered, [=](bool status, int index){
        if (status){
            QToolTip::showText(QCursor::pos(), QString("Value: %1").arg(set->at(index)));
        }
    });
    barSeries->append(set);

    // Set up X and Y axis properly
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(activeCategories);
    chart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    double maxValue = 0;
    for (auto value : litterTotals.values()) {
        maxValue = std::max(maxValue, value);
    }
    double axisMax = std::ceil(maxValue);
    axisY->setRange(0, axisMax);
    chart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);

    chart->addSeries(barSeries);



    // Set chart title - if its empty say that if not show real title
    chart->setTitle(activeCategories.isEmpty() ? "No data to display" : "Litter Levels by Type");

    // PIE CHART
    pieChart->removeAllSeries();
    QMap<QString, double> totals;
    double totalSum = 0.0;

    // like earlier go through each record only this time get total of all data so can be used to work out percent for pie chart
    for (const Sample &sample : locationDataset) {
        QString determinandLabel = QString::fromStdString(sample.getDeterminand().getLabel());
        if (activeCategories.contains(determinandLabel)) {
            double value = sample.getResult().getValue();
            if (!std::isnan(value) && !std::isinf(value)) {
                totals[determinandLabel] += value;
                totalSum += value;
            }
        }
    }

    // Create the Pie chart
    QPieSeries *pieSeries = new QPieSeries();
    if (totalSum > 0) {
        for (auto it = totals.begin(); it != totals.end(); ++it) {
            pieSeries->append(it.key(), it.value());
        }
    } else {
        pieSeries->append("No Data", 1.0);
    }


    pieChart->addSeries(pieSeries);
    pieChart->setTitle("Litter Distribution");
}
