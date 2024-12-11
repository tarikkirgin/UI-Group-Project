#include "dataset.hpp"
#include "sample.hpp"
#include <QString>
#include <vector>

class PollutantDataset : public Dataset {
  Q_OBJECT

public:
  static PollutantDataset &instance() {
    static PollutantDataset instance;
    return instance;
  }

  QString getPollutant() const { return pollutant; }
  std::vector<Sample> data; // this is key data

public slots:
  void onPollutantChanged(const QString &newPollutant);
  void onComplianceChanged(const QString &newCompliance);

private:
  QString pollutant;
  QString compliance;
  void filterDataByCompliance(const QString &compliance);
  void filterDataByPollutant(const QString &pollutant);
};
