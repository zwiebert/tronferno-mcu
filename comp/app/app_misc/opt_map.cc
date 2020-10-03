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

using keyMapT = std::map<const char*, otokBaseT, cmp_str, AllocatorMalloc<std::pair<const char *const, otokBaseT>>>;

/**
 * \brief map (CLI) option strings to enum
 */
static const class OptMap {
public:
  OptMap() {
    for (auto it = std::begin(otok_strings); it != std::end(otok_strings); ++it) {
      mKeyMap.emplace(std::make_pair(*it, std::distance(std::begin(otok_strings), it)));
    }
  }
public:
  /**
   * \brief get enum for KEY or otok::NONE
   */
  otok get(const char *const key) const {
    if (!key)
      return otok::NONE;
    auto it = mKeyMap.find(key);
    if (it == mKeyMap.end())
      return otok::NONE;
    return static_cast<otok>(it->second);
  }
private:
  keyMapT mKeyMap;
} opt_map;


///////////////////// public ///////////////////////////////
otok optMap_findToken(const char *key) {
  return opt_map.get(key);
}
