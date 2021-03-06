// Copyright 2013 Yandex

#ifndef LTR_UTILITY_BOOST_LEXICAL_CAST_HPP_
#define LTR_UTILITY_BOOST_LEXICAL_CAST_HPP_

#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

#include "ltr/interfaces/printable.h"
#include "ltr/utility/boost/string_utils.h"
#include "ltr/utility/boost/any.h"
#include "ltr/utility/boost/lexical_cast_config.hpp"

using std::invalid_argument;
using std::string;
using std::vector;
using std::stringstream;

using ltr::Printable;
using ltr::utility::iequals;
using ltr::utility::split;
using ltr::utility::trim;
using ltr::utility::Any;
using ltr::utility::LexicalCastConfig;

namespace ltr {
namespace utility {
/**
 * Converts from string to bool
 *
 * \param input is string to be converted.
 * \param result is pointer to result variable.
 * \return is flag if conversion was succesfull.
 */
static bool fromString(const string& input, bool* result);
/**
 * Converts from string to Any.
 *
 * \param input is string to be converted.
 * \param result is pointer to result variable.
 * \return is flag if conversion was succesfull.
 */
static bool fromString(const string& input, Any* result);
/**
 * Converts from string to string.
 *
 * \tparam T is type of elements of the vector.
 * \param input is string to be converted.
 * \param result is pointer to result variable.
 * \return is flag if conversion was succesfull.
 */
static bool fromString(const string& input, string* result);
/**
 * Converts from string to vector.
 *
 * \tparam T is type of elements of the vector.
 * \param input is string to be converted.
 * \param result is pointer to result variable.
 * \return is flag if conversion was succesfull.
 */
template <class T>
bool fromString(const string& input, vector<T>* result);
/**
 * Converts from string to bool
 *
 * \tparam TypeTo is type of result value
 * \param input is string to be converted.
 * \param result is pointer to result variable.
 * \return is flag if conversion was succesfull.
 */
template <class TypeTo>
bool fromString(const string& input, TypeTo* result);

/**
 * Converts from bool to string.
 *
 * \param input is value to be converted.
 * \param result is pointer to result string.
 * \return flag if conversion was succesfull.
 */
static bool toString(const bool& input, string* result);
/**
 * Converts from vector to string.
 *
 * \tparam T is type of elements of the vector.
 * \param input is value to be converted.
 * \param result is pointer to result string.
 * \return flag if conversion was succesfull.
 */
template <class T>
static bool toString(const vector<T>& input, string* result);
/**
 * Converts from some type to string.
 *
 * \tparam T is type of value to be converted.
 * \param input is value to be converted.
 * \param result is pointer to result string.
 * \return flag if conversion was succesfull.
 */
template <class T>
static bool toString(const T& input, string* result);

/**
 * \brief Cast some type to or from string. Printable object may be casted to
 * string. Can cast from string to Any too. In this case value will be cast
 * firstly to the simpliest appropriate type. If value contains commas, then
 * it will be interpreted as vector.
 *
 * \param input is string to be converted.
 */
template <class TypeTo, class TypeFrom>
inline TypeTo lexical_cast(const TypeFrom& input) {
  string intermediate;
  if (!toString(input, &intermediate)) {
    throw invalid_argument("Bad lexical_cast: unable to cast to string");
  }
  trim(&intermediate);
  TypeTo result;
  if (!fromString(intermediate, &result)) {
    throw invalid_argument(
      string("Bad lexical_cast: unable to cast from string \"") +
      intermediate + "\"");
  }
  return result;
}

// Template realizations

static bool fromString(const string& input, bool* result)  {
  if (iequals(input, "true")) {
    *result = true;
    return true;
  }
  if (iequals(input, "false")) {
    *result = false;
    return true;
  }
  return false;
}

static bool fromString(const string& input, Any* result) {
  bool bool_value;
  if (fromString(input, &bool_value)) {
    *result = bool_value;
    return true;
  }

  vector<bool> vector_bool_value;
  if (fromString(input, &vector_bool_value)) {
    *result = vector_bool_value;
    return true;
  }

  int int_value;
  if (fromString(input, &int_value)) {
    *result = int_value;
    return true;
  }

  double double_value;
  if (fromString(input, &double_value)) {
    *result = double_value;
    return true;
  }

  vector<int> vector_int_value;
  if (fromString(input, &vector_int_value)) {
    *result = vector_int_value;
    return true;
  }

    // If it is a vector of at least two strings
  vector<string> vector_string_value;
  if (fromString(input, &vector_string_value)
      && vector_string_value.size() > 1) {
    *result = vector_string_value;
    return true;
  }

  string string_value;
  if (fromString(input, &string_value)) {
    *result = string_value;
    return true;
  }
  return false;
}

bool fromString(const string& input, string* result) {
  *result = input;
  return true;
}

template <class T>
bool fromString(const string& input, vector<T>* result)  {
  vector<string> split_list;
  split(input, ",", &split_list);
  trim(&split_list);
  for (int object_index = 0;
       object_index < (int)split_list.size();
       ++object_index) {
    T object;
    if (!fromString(split_list[object_index], &object)) {
      return false;
    }
    result->push_back(object);
  }
  return true;
}

template <class TypeTo>
bool fromString(const string& input, TypeTo* result) {
  stringstream out(input);
  out >> *result;
  if (!out || !out.eof()) {
    return false;
  }
  return true;
}

static bool toString(const bool& input, string* result) {
  if (input) {
    *result = "true";
  } else {
    *result = "false";
  }
  return true;
}

template <class T>
static bool toString(const vector<T>& input, string* result) {
  result->clear();
  for (int object_index = 0; object_index < (int)input.size(); ++object_index) {
    string object_string;
    if (!toString(input[object_index], &object_string)) {
      return false;
    }
    if (object_index > 0) {
      *result += ",";
    }
    *result += object_string;
  }
  return true;
}

template <class T>
static bool toString(const T& input, string* result) {
  stringstream out;
  LexicalCastConfig::getInstance().configure(&out);
  out << input;
  *result = out.str();
  return true;
}
};
};
#endif  // LTR_UTILITY_BOOST_LEXICAL_CAST_HPP_
