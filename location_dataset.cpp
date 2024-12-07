#include "location_dataset.hpp"
#include <QDebug>

void LocationDataset::filterDataByLocation(const std::string &location) {

  for (int i = 0; i < Dataset::instance().data.size(); ++i) { // retrieving the data by making object from the dataset by instance()
    Sample sample = Dataset::instance().data[i]; // data is array of sample
    if (sample.getSamplingPoint().getLabel() == location) {
      data.push_back(sample);
    }
  }
  emit dataUpdated();
}

void LocationDataset::onLocationChanged(const QString &newLocation) {
  std::string newLocationStd = newLocation.toStdString();
  if (location != newLocationStd) {
    location = newLocationStd;
    filterDataByLocation(location);
    emit dataUpdated(); // not necessary
  }
}
