#include "app/opt_map.hh"

#include <misc/int_types.h>
#include <misc/allocator_malloc.hh>
#include <map>
#include <utility>
#include <cstring>

struct cmp_str
{
   bool operator()(char const *a, char const *b) const
   {
      return std::strcmp(a, b) < 0;
   }
};


using keyMapT = std::map<const char*, otokBaseT, cmp_str, AllocatorMalloc<std::pair<const char *const, otokBaseT>>>;

class OptMap {
public:
  OptMap() {
    for (otokBaseT i=0; i < static_cast<otokBaseT>(otok::SIZE); ++i) {
      mKeyMap.emplace(std::make_pair(otok_strings[i], i));
    }
  }
  otok get(const char *const key) const {
    if (!key)
      return otok::NONE;
    auto it =  mKeyMap.find(key);
    if (it == mKeyMap.end())
      return otok::NONE;
    return static_cast<otok>(it->second);
  }
private:
  keyMapT mKeyMap;
};

const OptMap opt_map;

otok optMap_findToken(const char *key) {
  return opt_map.get(key);
}
