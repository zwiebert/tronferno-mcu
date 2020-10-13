#include "app/opt_map.hh"

#include <misc/int_types.h>
#include <misc/allocator_malloc.hh>
#include <map>
#include <utility>
#include <cstring>
#include <algorithm>
#include <iterator>

///////////////////// private /////////////////////////////////////////
/// \brief compare two null terminated strings (less than) for use as template argument
struct cmp_str {
  bool operator()(char const *a, char const *b) const {
    return std::strcmp(a, b) < 0;
  }
};

template<class Iter, class T, class Compare>
Iter binary_find(Iter begin, Iter end, T val, Compare comp)
{
    // Finds the lower bound in at most log(last - first) + 1 comparisons
    Iter i = std::lower_bound(begin, end, val, comp);

    if (i != end && !comp(val, *i))
        return i; // found
    else
        return end; // not found
}

///////////////////// public ///////////////////////////////
otok optMap_findToken(const char *key) {
  auto it = binary_find(std::begin(otok_strings), std::end(otok_strings),
      key, [](const char *a, const char *b) { return strcmp(a,b) < 0; });
  if (it == std::end(otok_strings))
    return otok::NONE;
  auto dist = std::distance(std::begin(otok_strings), it);

  return static_cast<otok>(dist);
}
