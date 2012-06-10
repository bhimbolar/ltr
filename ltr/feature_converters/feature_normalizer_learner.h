// Copyright 2011 Yandex

#ifndef LTR_FEATURE_CONVERTERS_FEATURE_NORMALIZER_LEARNER_H_
#define LTR_FEATURE_CONVERTERS_FEATURE_NORMALIZER_LEARNER_H_

#include <boost/shared_ptr.hpp>

#include <vector>
#include <algorithm>
#include <limits>
#include <string>
#include <sstream>

#include "ltr/feature_converters/feature_converter_learner.h"
#include "ltr/feature_converters/per_feature_linear_converter.h"
#include "ltr/utility/numerical.h"
#include "ltr/data/utility/data_set_statistics.h"

using std::vector;
using std::string;
using std::logic_error;

namespace ltr {

template <class TElement>
class FeatureNormalizerLearner
    : public FeatureConverterLearner<TElement, PerFeatureLinearConverter> {
 public:
  typedef boost::shared_ptr<FeatureNormalizerLearner> Ptr;

  explicit FeatureNormalizerLearner(const ParametersContainer& parameters =
      ParametersContainer()) {
    this->setDefaultParameters();
    this->copyParameters(parameters);
    this->checkParameters();
  }

  void setDefaultParameters();
  void checkParameters() const;

  string toString() const;
 private:
  vector<double> feature_min_statistic_;
  vector<double> feature_max_statistic_;

  size_t featureCount() const;  
  virtual void learnImpl(
      const DataSet<TElement>& data_set,
      PerFeatureLinearConverter* per_feature_linear_converter);
};

template <typename TElement>
void
FeatureNormalizerLearner<TElement>::learnImpl(
    const DataSet<TElement>& data_set, 
    PerFeatureLinearConverter* per_feature_linear_converter) {
  utility::calcMinMaxStatistics(data_set, &feature_min_statistic_,
      &feature_max_statistic_);
  per_feature_linear_converter->resize(this->featureCount());
  double normalizationIntervalBegin =
      this->parameters().template Get<double>("NormalizationIntervalBegin");
  double normalizationIntervalEnd =
      this->parameters().template Get<double>("NormalizationIntervalEnd");

  for (size_t feature_idx = 0;
      feature_idx < this->featureCount();
      ++feature_idx) {
    double delta = feature_max_statistic_[feature_idx]
                                          - feature_min_statistic_[feature_idx];
    double coefficient;
    double shift;
    if (utility::DoubleEqual(delta, 0)) {
      coefficient = 0.0;
      shift = (normalizationIntervalEnd - normalizationIntervalBegin) / 2;
    } else {
      coefficient =
          (normalizationIntervalEnd - normalizationIntervalBegin) / delta;
      shift = normalizationIntervalBegin -
          coefficient * feature_min_statistic_[feature_idx];
    }
    per_feature_linear_converter->setCoefficient(feature_idx, coefficient);
    per_feature_linear_converter->setShift(feature_idx, shift);
  }
}

template <typename TElement>
string FeatureNormalizerLearner<TElement>::toString() const {
  std::stringstream str;
  std::fixed(str);
  str.precision(2);
  str << "Feature normalizer feature converter learner with parameters:";
  str << " NormalizationIntervalBegin = ";
  str << this->parameters().template Get<double>("NormalizationIntervalBegin");
  str << ", NormalizationIntervalEnd = ";
  str << this->parameters().template Get<double>("NormalizationIntervalEnd");
  return str.str();
}

template <typename TElement>
size_t FeatureNormalizerLearner<TElement>::featureCount() const {
  return feature_min_statistic_.size();
}

template <typename TElement>
void FeatureNormalizerLearner<TElement>::setDefaultParameters() {
  this->clearParameters();
  this->addNewParam("NormalizationIntervalBegin", 0.0);
  this->addNewParam("NormalizationIntervalEnd", 1.0);
}

template <typename TElement>
void FeatureNormalizerLearner<TElement>::checkParameters() const {
  const ParametersContainer &params = this->parameters();
  if (params.Get<double>("NormalizationIntervalBegin") >=
      params.Get<double>("NormalizationIntervalEnd")) {
    throw logic_error("Bad parameters for FeatureNormalizerLearner");
  }
}
}
#endif  // LTR_FEATURE_CONVERTERS_FEATURE_NORMALIZER_LEARNER_H_
