#include "dataset.hpp"
#include "sample.hpp"
#include "QString"
#include <vector>

class PollutantDataset : public Dataset {
  Q_OBJECT

public:
  static PollutantDataset &instance() {
    static PollutantDataset instance;
    return instance;
  }

  std::string getPollutant() const { return pollutant; }
  std::vector<Sample> data; // this is key data

public slots:
  void onPollutantChanged(const QString &newPollutant);
  void onComplianceChanged(const QString &newCompliance);

private:
  std::string pollutant;
  bool compliance;
  void filterDataByCompliance(const bool &compliance);
  void filterDataByPollutant(const std::string &pollutant);
};
