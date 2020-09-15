#pragma once

#include <misc/int_macros.h>
#include <utility>
#include <stdint.h>
#include <string.h>
#include <misc/int_types.h>


typedef uint8_t gmBitMaskT[8];
typedef uint8_t gT, mT;

bool fer_gmByName_load(const char *name, const gmBitMaskT *gm, int count);

using gm_pairT = std::pair<gT, mT>;


#ifdef D_NO_OPT
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif

class gm_iterator {
  uint8_t mGm;
  const bool mSkipGroups: 1;
public:
  gm_iterator(gT g = 0, mT m = 0, bool skip_groups = false) :
      mGm(0x3f & ((g << 3) | m)), mSkipGroups(skip_groups) {
  }
  gm_iterator& operator++() {
    if ((mGm & ~0x3f) == 0) {
      ++mGm;
    }
    if (mSkipGroups && (getM() == 0 || getG() == 0))
      ++mGm;

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
  gmBitMaskT mBm = {};

public:
  void clear() {
    memset(mBm, 0, sizeof(gmBitMaskT));
  }
  uint8_t getByte(gT g) const {
    return mBm[g];
  }
  uint8_t& operator[](int idx) { return mBm[idx]; }
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

  bool isAllClear() const {
    for (int i = 0; i < 8; ++i)
      if (getByte(i))
        return false;
    return true;
  }

  void fromNibbleCounters(u32 um) {
    int g, m;
    clear();
    for (g=0; g < 8; ++g, (um >>=4)) {
      u8 u = um & 0x07;
      for (m=1; m <= u; ++m) {
        setBit(g,m);
      }
    }
  }

  GmBitMask() = default;
  explicit GmBitMask(gmBitMaskT bm) {
    if (bm)
      memcpy(mBm, bm, sizeof(mBm));
  }

  explicit GmBitMask(const char *name) {
    fer_gmByName_load("MANU", &mBm, 1);
  }

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

  iterator begin(gT g = 0, mT m = 0) {
    auto it = iterator(this, g, m);
    return it->getBit(0, 0) ? it : ++it;
  }


  operator gmBitMaskT*() { return &mBm; } //XXX
};


#ifdef D_NO_OPT
#pragma GCC pop_options
#endif
