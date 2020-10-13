/**
 * \file     fernotron/gm_set.hh
 * \brief    Provide set of groups/members with iterator
 */

#pragma once

#include <misc/int_macros.h>
#include <utility>
#include <stdint.h>
#include <string.h>
#include <misc/int_types.h>


typedef uint8_t gmSetT[8];
typedef uint8_t gT, mT;


/**
 * \brief          Load one or more Fer_GmSet from persistent storage.
 * \param name     File name of storage
 * \param[out] gm  Destination pointer to an array of capacity of at least COUNT
 * \param count    Number of sets to read from file
 * \return         true for success
 */
bool fer_stor_gmSet_load(const char *name, gmSetT gm[], int count);

using gm_pairT = std::pair<gT, mT>;

/**
 * \brief Iterator to iterate over all set groups/members in a set
 */
class Fer_GmSet_Iterator {

public:
  Fer_GmSet_Iterator(gT g = 0, mT m = 0, bool skip_groups = false) :
      mGm(0x3f & ((g << 3) | m)), mSkipGroups(skip_groups) {
  }

public:
  bool operator==(const Fer_GmSet_Iterator &rhs) const {
    return mGm == rhs.mGm;
  }
  bool operator!=(const Fer_GmSet_Iterator &rhs) const {
    return mGm != rhs.mGm;
  }
  operator bool() const {
    return 0 == (mGm & ~0x3f);
  }
  Fer_GmSet_Iterator& operator++() {
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

/**
 * \brief    Set of groups and/or members
 */
class Fer_GmSet {

public:
  Fer_GmSet() = default;

  explicit Fer_GmSet(gmSetT bm) {
    if (bm)
      memcpy(mBm, bm, sizeof(mBm));
  }

  explicit Fer_GmSet(const char *name) {
    fer_stor_gmSet_load("MANU", &mBm, 1);
  }

public:
  operator gmSetT*() {
    return &mBm;
  } //XXX
  uint8_t& operator[](int idx) {
    return mBm[idx];
  }

public:
  void clear() {
    memset(mBm, 0, sizeof(gmSetT));
  }
  uint8_t getGroup(gT g) const {
    return mBm[g];
  }
  void setGroup(gT g, uint8_t b) {
    mBm[g] = b;
  }
  bool getMember(gT g, mT m) const {
    return GET_BIT(mBm[g], m);
  }
  void putMember(gT g, mT m, bool val) {
    PUT_BIT(mBm[g], m, val);
  }
  void clearMember(gT g, mT m) {
    CLR_BIT(mBm[g], m);
  }
  void setMember(gT g, mT m) {
    SET_BIT(mBm[g], m);
  }
  bool isAllClear() const {
    for (int i = 0; i < 8; ++i)
      if (getGroup(i))
        return false;
    return true;
  }
  void fromNibbleCounters(u32 um) {
    int g, m;
    clear();
    for (g = 0; g < 8; ++g, (um >>= 4)) {
      u8 u = um & 0x07;
      for (m = 1; m <= u; ++m) {
        setMember(g, m);
      }
    }
  }

public:
  class iterator: public Fer_GmSet_Iterator {
    Fer_GmSet *mPtr;
  public:
    iterator(Fer_GmSet *bm, gT g = 0, mT m = 0) :
        Fer_GmSet_Iterator(g, m), mPtr(bm) {
    }
    iterator& operator++() {
      while (Fer_GmSet_Iterator::operator++()) {
        if (mPtr->getMember(getG(), getM())) {
          break;
        }
      }
      return *this;
    }
    Fer_GmSet* operator->() const {
      return mPtr;
    }
  };

  iterator begin(gT g = 0, mT m = 0) {
    auto it = iterator(this, g, m);
    return it->getMember(0, 0) ? it : ++it;
  }

private:
  gmSetT mBm = { };
};

static_assert(sizeof (gmSetT) == sizeof (Fer_GmSet));
