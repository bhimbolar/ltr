// Copyright 2012 Yandex

#ifndef LTR_FEATURE_CONVERTERS_UTILITY_UTILITY_H_
#define LTR_FEATURE_CONVERTERS_UTILITY_UTILITY_H_

#include <vector>

#include "ltr/feature_converters/feature_converter.h"

using std::vector;

namespace ltr {
namespace utility {

template <typename TElement>
void ApplyFeatureConverter(FeatureConverter::ConstPtr converter,
                           const DataSet<TElement>& argument,
                           DataSet<TElement>* value) {
  DataSet<TElement> result(converter->getNewFeatureInfo());
  for (size_t elementIdx = 0; elementIdx < argument.size(); ++elementIdx) {
    vector<Object> objectsInTElement;
    for (int object_index = 0;
         object_index < (int)argument[elementIdx].size();
         ++object_index) {
      Object objToAdd;
      converter->apply(argument[elementIdx][object_index], &objToAdd);
      objectsInTElement.push_back(objToAdd);
    }
    TElement telementToAdd(objectsInTElement);
    result.add(telementToAdd, argument.getWeight(elementIdx));
  }
  *value = result;
}
}
}
#endif  // LTR_FEATURE_CONVERTERS_UTILITY_UTILITY_H_
