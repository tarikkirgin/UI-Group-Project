#include "location_dataset.hpp"
#include <QDebug>
//this this gonna be used for onLocationChanged
void LocationDataset::filterDataByLocation(const std::string &location) {
  data.clear();
  for (int i = 0; i < Dataset::instance().data.size(); ++i) {
    Sample sample = Dataset::instance().data[i];
    if (sample.getSamplingPoint().getLabel() == location) {
      data.push_back(sample);
    }
  }
  emit dataUpdated();
}
//checking if the new location given is same as the exisitng location and change the sample data if it's different
void LocationDataset::onLocationChanged(const QString &newLocation) {
  std::string newLocationStd = newLocation.toStdString();
  if (location != newLocationStd) {
    location = newLocationStd;  
    filterDataByLocation(location);
    emit dataUpdated(); // not necessary
  }
}

std::vector<std::string> LocationDataset::getPollutants() { // returning string arrary of pollutant from data!!
  std::vector<std::string> pollutants;
  
  pollutants.clear();
  for (const auto &sample : data) {
    const Determinand &determinand = sample.getDeterminand();
    pollutants.push_back(determinand.getLabel());
  }

  return pollutants;
}

std::vector<bool> LocationDataset::getCompliances() { // returning string arrary of compliance from data!!
  std::vector<bool> compliances;
  
  compliances.clear();
  for (const auto &sample : data) {
    const bool compliance = sample.getIsComplianceSample();

    compliances.push_back(compliance);
  }
  return compliances;
}
