#include "sample.hpp"

SamplingPoint::SamplingPoint(const std::string &notation,
                             const std::string &label, int northing,
                             int easting)
    : notation(notation), label(label), northing(northing), easting(easting) {}

Determinand::Determinand(const std::string &label,
                         const std::string &definition, const std::string &unit,
                         int notation)
    : label(label), definition(definition), unit(unit), notation(notation) {}

Result::Result(double value, bool is_less_than)
    : value(value), is_less_than(is_less_than) {}

Sample::Sample(SamplingPoint &sampling_point, const std::string &time,
               const std::string &purpose,
               const std::string &sampled_material_type,
               bool is_compliance_sample, Determinand &determinand,
               Result &result)
    : sampling_point(sampling_point), time(time), purpose(purpose),
      sampled_material_type(sampled_material_type),
      is_compliance_sample(is_compliance_sample), determinand(determinand),
      result(result) {}
