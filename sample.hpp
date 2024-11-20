#pragma once

#include <string>

class SamplingPoint {
public:
  SamplingPoint(const std::string &, const std::string &, int, int);
  std::string getNotation() const { return notation; }
  std::string getLabel() const { return label; }
  double getNorthing() const { return northing; }
  double getEasting() const { return easting; }

private:
  std::string notation;
  std::string label;
  int northing;
  int easting;
};

class Determinand {
public:
  Determinand(const std::string &, const std::string &, const std::string &,
              int);
  std::string getLabel() const { return label; }
  std::string getDefinition() const { return definition; }
  std::string getUnit() const { return unit; }
  int getNotation() const { return notation; }

private:
  std::string label;
  std::string definition;
  std::string unit;
  int notation;
};

class Result {
public:
  Result(double, bool);
  double getValue() const { return value; }
  bool getIsLessThan() const { return is_less_than; }

private:
  double value;
  bool is_less_than; // less_than = true corresponds to a 'resultQualifier' of
                     // '<'
};

class Sample {
public:
  Sample(SamplingPoint &, const std::string &, const std::string &,
         const std::string &, bool, Determinand &, Result &);
  SamplingPoint getSamplingPoint() const { return sampling_point; }
  std::string getTime() const { return time; }
  std::string getPurpose() const { return purpose; }
  std::string getSampledMaterialType() const { return sampled_material_type; }
  bool getIsComplianceSample() const { return is_compliance_sample; }
  Determinand getDeterminand() const { return determinand; }
  Result getResult() const { return result; }

private:
  SamplingPoint sampling_point;
  std::string time;
  std::string purpose;
  std::string sampled_material_type;
  bool is_compliance_sample;
  Determinand determinand;
  Result result;
};
