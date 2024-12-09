#include "dataset.hpp"
#include "csv.hpp"
#include "sample.hpp"
#include <QDebug>
#include <stdexcept>

using namespace std;



void Dataset::loadData(const string &filename) { //store arrary of sample (called data) with data.
  csv::CSVReader reader(filename);

  data.clear();

  for (const auto &row : reader) {
    SamplingPoint sampling_point{
        row["sample.samplingPoint.notation"].get<>(),
        row["sample.samplingPoint.label"].get<>(),
        row["sample.samplingPoint.northing"].get<int>(),
        row["sample.samplingPoint.easting"].get<int>()};

    Determinand determinand{
        row["determinand.label"].get<>(),
        row["determinand.definition"].get<>(),
        row["determinand.unit.label"].get<>(),
        row["determinand.notation"].get<int>(),
    };

    Result result{
        row["result"].get<double>(),
        row["resultQualifier.notation"].get<>() == "<",
    };

    Sample sample{sampling_point,
                  row["sample.sampleDateTime"].get<>(),
                  row["sample.purpose.label"].get<>(),
                  row["sample.sampledMaterialType.label"].get<>(),
                  row["sample.isComplianceSample"].get<>() == "FALSE",
                  determinand,
                  result};

    data.push_back(sample);
  }

  emit dataUpdated();
}

std::vector<std::string> Dataset::getLocations() { // getting string array of location from data
  std::vector<std::string> locations; //get empty location string array

  std::unordered_set<std::string> uniqueLocations;

  for (const auto &sample : data) {
    const SamplingPoint &samplingPoint = sample.getSamplingPoint();
    uniqueLocations.insert(samplingPoint.getLabel());
  }

  locations.assign(uniqueLocations.begin(), uniqueLocations.end());

  return locations;
}

  std::vector<std::string> Dataset::getPollutants() { // returning string arrary of pollutant from data!!
  std::vector<std::string> pollutants;
  
  pollutants.clear();
  for (const auto &sample : data) {
    const Determinand &determinand = sample.getDeterminand();
    pollutants.push_back(determinand.getLabel());
  }

  return pollutants;
}

  std::vector<bool> Dataset::getCompliances() { // returning string arrary of compliance from data!!
  std::vector<bool> compliances;
  
  compliances.clear();
  for (const auto &sample : data) {
    const bool compliance = sample.getIsComplianceSample();

    compliances.push_back(compliance);
  }
  return compliances;
}