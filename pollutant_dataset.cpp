#include "pollutant_dataset.hpp"
#include "location_dataset.hpp"
#include "compliance_page.hpp"
#include <QDebug>
#include <QString>

void PollutantDataset::filterDataByPollutant(const QString &pollutant) {
    
  data.clear(); // this is data from PollutantDataset Members
  for (int i = 0; i < LocationDataset::instance().data.size(); ++i) { // retrieving the data by making object from the dataset by instance()
    Sample sample = LocationDataset::instance().data[i]; // dataDeterminad is array of determinand
    if (QString::fromStdString(sample.getDeterminand().getLabel()) == pollutant) {
      data.push_back(sample);
    }
  }
  emit dataUpdated();
}
//updating sample data when new section is chosen.
void PollutantDataset::onPollutantChanged(const QString &newPollutant) {
  if (pollutant != newPollutant) {
    pollutant = newPollutant;
    filterDataByPollutant(pollutant);
     // not necessary
  }
  emit dataUpdated();
}

void PollutantDataset::filterDataByCompliance(const QString &compliance) {
  std::vector<Sample> tempData;
  for (int i = 0; i < PollutantDataset::instance().data.size(); ++i) { // retrieving the data by making object from the dataset by instance()
    Sample sample = PollutantDataset::instance().data[i]; // dataDeterminad is array of determinand
    if (QString::fromStdString(compliancePage::instance().getLevel(sample)) == compliance) {
      tempData.push_back(sample);
    }
  }
  data.clear();
  data = tempData;
  emit dataUpdated();
}

void PollutantDataset::onComplianceChanged(const QString &newCompliance) {

  if (compliance != newCompliance) {
      compliance = newCompliance;
      filterDataByCompliance(compliance);
   } 
   emit dataUpdated(); // not necessary 
}









