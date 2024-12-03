#include "dataset.hpp"
#include "csv.hpp"
#include "sample.hpp"
#include <QDebug>
#include <stdexcept>

using namespace std;

void Dataset::loadData(const string &filename) {
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

std::vector<std::string> Dataset::getLocations() {
  std::vector<std::string> locations;

  std::unordered_set<std::string> uniqueLocations;

  for (const auto &sample : data) {
    const SamplingPoint &samplingPoint = sample.getSamplingPoint();
    uniqueLocations.insert(samplingPoint.getLabel());
  }

  locations.assign(uniqueLocations.begin(), uniqueLocations.end());

  return locations;
}
