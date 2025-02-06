/*
 * \file  opt_map_consteval.cc
 * \brief implements otok_string => enum_otok look-up
 * \note  XXX: its not using std::sort, std::swap, ... because they are not constexpr in the current esp32 gcc version.
 */

#include "app_misc/opt_map.hh"
#include <stdint.h>
#include <utility>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <array>

///////////////////// private /////////////////////////////////////////
/// \brief compare two null terminated strings (less than) for use as template argument
struct cmp_str {
  constexpr bool operator()(char const *a, char const *b) const {
    return std::strcmp(a, b) < 0;
  }
};

constexpr int my_strcmp(const char *str1, const char *str2) {
  for (int i = 0; str1[i] || str2[i]; ++i) {
    if (str1[i] != str2[i])
      return (str1[i] - str2[i]);
  }
  return 0;
}

constexpr bool my_compare(const char *str1, const char *str2) {
  return strcmp(str1, str2) < 0;
}

struct OkPair {
  otokBaseT second = 0;
  constexpr operator const char *() const {
    return otok_strings[second];
  }
  constexpr operator otokBaseT() const {
    return second;
  }
  constexpr operator otok() const {
    return (otok) second;
  }
  constexpr bool operator<(const OkPair &other) {
    return strcmp(*this, other) < 0;
  }
};

template<typename T>
constexpr void swap(T &xp, T &yp) {
  T temp = xp;
  xp = yp;
  yp = temp;
}

template<typename Array>
constexpr void bubbleSort(Array &arr) {
  const int n = (int) arr.size();
  for (int i = 0; i < n - 1; ++i) {
    // Last i elements are already in place
    for (int j = 0; j < n - i - 1; ++j) {
      if (arr[j + 1] < arr[j])
        swap(arr[j], arr[j + 1]);
    }
  }
}

template<class Iter, class T, class Compare>
constexpr Iter binary_find(Iter begin, Iter end, const T &val, Compare comp) {
  // Finds the lower bound in at most log(last - first) + 1 comparisons
  Iter i = std::lower_bound(begin, end, val, comp);

  if (i != end && !comp(val, *i))
    return i; // found
  else
    return end; // not found
}

/**
 * \brief map (CLI) option strings to enum
 */
constexpr class OptMap {
public:
  constexpr OptMap() {
    for (int i=0; i < (int)otok::SIZE; ++i) {
      OkArr[i].second = i;
    }
    //std::sort(OkArr.begin(), OkArr.end());
    bubbleSort(OkArr);
  }

  constexpr otok findToken(const char *key) const {
    auto it = binary_find(OkArr.begin(), OkArr.end(), key, my_compare);
    if (it == OkArr.end())
      return otok::NONE;

    return *it;
  }

private:
  /// \brief an array of sorted otok_string-indices
  std::array<OkPair, (size_t) otok::SIZE> OkArr;
} opt_map;



///////////////////// public ///////////////////////////////
otok optMap_findToken(const char *key) {
  return opt_map.findToken(key);
}

