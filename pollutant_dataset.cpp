#include "pollutant_dataset.hpp"
#include "location_dataset.hpp"
#include <QDebug>

void PollutantDataset::filterDataByPollutant(const std::string &pollutant) {
    
  data.clear(); // this is data from PollutantDataset Members
  for (int i = 0; i < LocationDataset::instance().data.size(); ++i) { // retrieving the data by making object from the dataset by instance()
    Sample sample = LocationDataset::instance().data[i]; // dataDeterminad is array of determinand
    if (sample.getDeterminand().getLabel() == pollutant) {
      data.push_back(sample);
    }
  }
  emit dataUpdated();
}
//updating sample data when new section is chosen.
void PollutantDataset::onPollutantChanged(const QString &newPollutant) {
  std::string newPollutantStd = newPollutant.toStdString();
  if (pollutant != newPollutantStd) {
    pollutant = newPollutantStd;
    filterDataByPollutant(pollutant);
    emit dataUpdated(); // not necessary
  }
}

void PollutantDataset::filterDataByCompliance(const bool &compliance = false) {
    
  data.clear(); // this is data from PollutantDataset Members
  for (int i = 0; i < LocationDataset::instance().data.size(); ++i) { // retrieving the data by making object from the dataset by instance()
    Sample sample = LocationDataset::instance().data[i]; // dataDeterminad is array of determinand
    if (sample.getIsComplianceSample() == compliance) {
      data.push_back(sample);
    }
  }
  emit dataUpdated();
}

void PollutantDataset::onComplianceChanged(const QString &newCompliance) {
  std::string newComplianceStd = newCompliance.toStdString();

    if (newComplianceStd == "FALSE") {
      filterDataByCompliance(false);
    } else {
      filterDataByCompliance(true);
    }
    emit dataUpdated(); // not necessary
  
}









