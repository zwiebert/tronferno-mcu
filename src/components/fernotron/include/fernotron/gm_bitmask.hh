#include "types.h"
#include <misc/int_macros.h>
#include <utility>

typedef uint8_t gT, mT;
using gm_pairT = std::pair<gT, mT>;

#pragma GCC push_options
#pragma GCC optimize ("O0")

class gm_iterator {
  uint16_t mGm;
public:
  gm_iterator(gT g, mT m) :
      mGm(0x3f & ((g << 3) | m)) {
  }
  gm_iterator& operator++() {
    if ((mGm & ~0x3f) == 0) {
      ++mGm;
    }
    return *this;
  }

  mT getM() const {
    return mGm & 0x07;
  }
  gT getG() const {
    return mGm >> 3;
  }
  bool operator==(const gm_iterator &rhs) const {
    return mGm == rhs.mGm;
  }
  bool operator!=(const gm_iterator &rhs) const {
    return mGm != rhs.mGm;
  }
  operator bool() const {
    return 0 == (mGm & ~0x3f);
  }
};

class GmBitMask {

private:
  gm_bitmask_t mBm;

public:
  void clear() {
    memset(mBm, 0, sizeof(gm_bitmask_t));
  }
  uint8_t getByte(gT g) const {
    return mBm[g];
  }
  void setByte(gT g, uint8_t b) {
    mBm[g] = b;
  }
  bool getBit(gT g, mT m) const {
    return GET_BIT(mBm[g], m);
  }
  void putBit(gT g, mT m, bool val) {
    PUT_BIT(mBm[g], m, val);
  }
  void clearBit(gT g, mT m) {
    CLR_BIT(mBm[g], m);
  }
  void setBit(gT g, mT m) {
    SET_BIT(mBm[g], m);
  }
  GmBitMask(): mBm{} {}
  GmBitMask(gm_bitmask_t bm) { memcpy(mBm, bm, sizeof(mBm)); }

  class iterator: public gm_iterator {
    GmBitMask *mPtr;
  public:
    iterator(GmBitMask *bm, gT g=0, mT m=0) :
        gm_iterator(g, m), mPtr(bm) {
    }
    iterator& operator++() {
      while (gm_iterator::operator++()) {
        if (mPtr->getBit(getG(), getM())) {
          break;
        }
      }
      return *this;
    }
    GmBitMask* operator->() const {
      return mPtr;
    }
  };

  iterator begin() {
    auto it = iterator(this);
    return it->getBit(0, 0) ? it : ++it;
  }

};



#pragma GCC pop_options
