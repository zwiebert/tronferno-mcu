#include "cli_app/opt_map.hh"

#include <misc/int_types.h>
#include <misc/allocator_malloc.hh>
#include <map>
#include <utility>




using keyMapT = std::map<const char*, otokBaseT, std::less<const char *>, AllocatorMalloc<std::pair<const char *const, otokBaseT>>>;

class OptMap {
public:
  OptMap(const char *const key_names[], int len) {
    for (otokBaseT i=0; i < len; ++i) {
      mKeyMap.emplace(std::make_pair(key_names[i], i));
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

static const OptMap opt_map(parmPair_keys, static_cast<otokBaseT>(otok::SIZE));

otok optMap_findToken(const char *key) {
  return opt_map.get(key);
}
