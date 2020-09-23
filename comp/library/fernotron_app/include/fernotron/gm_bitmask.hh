#pragma once

#include <misc/int_macros.h>
#include <utility>
#include <stdint.h>
#include <string.h>
#include <misc/int_types.h>


typedef uint8_t gmBitMaskT[8];
typedef uint8_t gT, mT;

bool fer_stor_gmBitMask_load(const char *name, const gmBitMaskT *gm, int count);

using gm_pairT = std::pair<gT, mT>;

class Fer_GmIterator {

public:
  Fer_GmIterator(gT g = 0, mT m = 0, bool skip_groups = false) :
      mGm(0x3f & ((g << 3) | m)), mSkipGroups(skip_groups) {
  }

public:
  bool operator==(const Fer_GmIterator &rhs) const {
    return mGm == rhs.mGm;
  }
  bool operator!=(const Fer_GmIterator &rhs) const {
    return mGm != rhs.mGm;
  }
  operator bool() const {
    return 0 == (mGm & ~0x3f);
  }
  Fer_GmIterator& operator++() {
    if ((mGm & ~0x3f) == 0) {
      ++mGm;
    }
    if (mSkipGroups && (getM() == 0 || getG() == 0))
      ++mGm;

    return *this;
  }
public:
  mT getM() const {
    return mGm & 0x07;
  }
  gT getG() const {
    return mGm >> 3;
  }
private:
  uint8_t mGm;
  const bool mSkipGroups :1;
};

class Fer_GmBitMask {

public:
  Fer_GmBitMask() = default;

  explicit Fer_GmBitMask(gmBitMaskT bm) {
    if (bm)
      memcpy(mBm, bm, sizeof(mBm));
  }

  explicit Fer_GmBitMask(const char *name) {
    fer_stor_gmBitMask_load("MANU", &mBm, 1);
  }

public:
  operator gmBitMaskT*() {
    return &mBm;
  } //XXX
  uint8_t& operator[](int idx) {
    return mBm[idx];
  }

public:
  void clear() {
    memset(mBm, 0, sizeof(gmBitMaskT));
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
  bool isAllClear() const {
    for (int i = 0; i < 8; ++i)
      if (getByte(i))
        return false;
    return true;
  }
  void fromNibbleCounters(u32 um) {
    int g, m;
    clear();
    for (g = 0; g < 8; ++g, (um >>= 4)) {
      u8 u = um & 0x07;
      for (m = 1; m <= u; ++m) {
        setBit(g, m);
      }
    }
  }

public:
  class iterator: public Fer_GmIterator {
    Fer_GmBitMask *mPtr;
  public:
    iterator(Fer_GmBitMask *bm, gT g = 0, mT m = 0) :
        Fer_GmIterator(g, m), mPtr(bm) {
    }
    iterator& operator++() {
      while (Fer_GmIterator::operator++()) {
        if (mPtr->getBit(getG(), getM())) {
          break;
        }
      }
      return *this;
    }
    Fer_GmBitMask* operator->() const {
      return mPtr;
    }
  };

  iterator begin(gT g = 0, mT m = 0) {
    auto it = iterator(this, g, m);
    return it->getBit(0, 0) ? it : ++it;
  }

private:
  gmBitMaskT mBm = { };
};

