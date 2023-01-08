/**
 * \file     fernotron/gm_set.hh
 * \brief    Provide set of groups/members with iterator
 */

#pragma once

#include <utils_misc/int_macros.h>
#include <utility>
#include <stdint.h>
#include <string.h>
#include <utils_misc/int_types.h>


typedef uint8_t gT, mT;

using gm_pairT = std::pair<gT, mT>;

/**
 * \brief Counter to iterate over all set and unset groups/members
 */
class Fer_Gm_Counter {

public:
  /**
   * \param g,m          Group/member to start iteration from
   * \param skip_groups  skip groups (g=N,m=0). Only visit members (m=1..7).
   */
  Fer_Gm_Counter(gT g = 0, mT m = 0, bool skip_groups = false) :
      mGm(0x3f & ((g << 3) | m)), mSkipGroups(skip_groups) {
  }

public:
  /// \brief    Compare counters
  bool operator==(const Fer_Gm_Counter &rhs) const {
    return mGm == rhs.mGm;
  }

  /// \brief   Compare counters
  bool operator!=(const Fer_Gm_Counter &rhs) const {
    return mGm != rhs.mGm;
  }

  /// \brief   Test if end was reached (e.g. for (Fer_Gm_Counter it(1,1,true); it; ++it) {...})
  /// \return  false if end was reached
  operator bool() const {
    return 0 == (mGm & ~0x3f);
  }
  /// \brief   Increment counter
  Fer_Gm_Counter& operator++() {
    if ((mGm & ~0x3f) == 0) {
      ++mGm;
    }
    if (mSkipGroups && (getM() == 0 || getG() == 0))
      ++mGm;

    return *this;
  }
public:
  /// \brief Get current member
  mT getM() const {
    return mGm & 0x07;
  }
  /// \brief Get current group
  gT getG() const {
    return mGm >> 3;
  }
private:
  uint8_t mGm; ///< Byte-counter (group = high nibble, member = low nibble)
  const bool mSkipGroups :1;  ///< If set, skip groups (skip any g==0 or m==0)
};

/**
 * \brief    Set of groups and/or members
 */
class Fer_GmSet {
public:
  Fer_GmSet() = default;

  explicit Fer_GmSet(const char *name) {
    store_load(name);
  }

  /**
   * \brief   Construct set from nibble counter (one group per nibble)
   * \param um  each nibble except of nibble 0 contains a count (1-7) of members of a group to set
   */
  explicit Fer_GmSet(uint32_t um) {
    int g, m;
    clear();
    for (g = 0; g < 8; ++g, (um >>= 4)) {
      uint8_t u = um & 0x07;
      for (m = 1; m <= u; ++m) {
        setMember(g, m);
      }
    }
  }

public:
  uint8_t& operator[](int idx) {
    return mBm[idx];
  }

public:
  const Fer_GmSet& operator&=(const Fer_GmSet &other) {
    for (auto i = 0; i < 8; ++i) {
      mBm[i] &= other.mBm[i];
    }
    return *this;
  }
  const Fer_GmSet& operator|=(const Fer_GmSet &other) {
    for (auto i = 0; i < 8; ++i) {
      mBm[i] |= other.mBm[i];
    }
    return *this;
  }
  friend Fer_GmSet operator&(const Fer_GmSet &lhs, const Fer_GmSet &rhs) {
    Fer_GmSet result = lhs;
    result &= rhs;
    return result;
  }
  friend Fer_GmSet operator|(const Fer_GmSet &lhs, const Fer_GmSet &rhs) {
    Fer_GmSet result = lhs;
    result |= rhs;
    return result;
  }
  Fer_GmSet operator~() const {
    Fer_GmSet result {};
    for (auto i = 0; i < 8; ++i) {
      result[i] = ~mBm[i];
    }
    return result;
  }
public:
  /**
   * \brief  Set/clear M0 according to M1..M7
   */
  const Fer_GmSet&  updateGroup() {
    for (auto i = 0; i < 8; ++i) {
      PUT_BIT(mBm[i], 0, (mBm[i] & 0xFE) != 0);
    }
    return *this;
  }
public:
  /// \brief  Make the set empty
  void clear() {
    memset(mBm, 0, sizeof mBm);
  }
  /// \brief Get a group as bit-mask
  uint8_t getGroup(gT g) const {
    return mBm[g];
  }
  /// \brief Set a group as bit-mask
  void setGroup(gT g, uint8_t b) {
    mBm[g] = b;
  }
  /// \brief Test if a Member is set
  bool getMember(gT g, mT m) const {
    return GET_BIT(mBm[g], m);
  }

  /**
   * \brief     Add or remove a member or group
   * \param g   group number
   * \param m   member number. 0 adds the group with no members.
   * \param val true to add. false to remove
   */
  void putMember(gT g, mT m, bool val) {
    PUT_BIT(mBm[g], m, val);
  }
  /// \brief Remove member.
  void clearMember(gT g, mT m) {
    CLR_BIT(mBm[g], m);
  }
  /// \brief Add member.
  void setMember(gT g, mT m) {
    SET_BIT(mBm[g], m);
  }
  /// \brief Test for empty set
  bool isAllClear() const {
    for (int i = 0; i < 8; ++i)
      if (getGroup(i))
        return false;
    return true;
  }
  /**
   * \brief   Construct set from nibble counter (one group per nibble)
   * \note    All Previous content of this set will be deleted.
   * \param um  each nibble except of nibble 0 contains a count (1-7) of members of a group to set
   */
  void fromNibbleCounters(uint32_t um) {
    int g, m;
    clear();
    for (g = 0; g < 8; ++g, (um >>= 4)) {
      uint8_t u = um & 0x07;
      for (m = 1; m <= u; ++m) {
        setMember(g, m);
      }
    }
  }

public:
  /// \brief iterate over an Fer_GmSet
  template<class T>
  class iteratorT: public Fer_Gm_Counter {
    T *mPtr;
  public:
    /**
     * \param bm      Pointer to container
     * \param g,m     group and member to start iteration from
     */
    iteratorT(T *bm, gT g = 0, mT m = 0, bool skip_groups = false) :
        Fer_Gm_Counter(g, m, skip_groups), mPtr(bm) {
      if (!mPtr->getMember(getG(), getM()))
        operator++();
    }
    /// \brief Get next group or member
    iteratorT& operator++() {
      while (Fer_Gm_Counter::operator++()) {
        if (mPtr->getMember(getG(), getM())) {
          break;
        }
      }
      return *this;
    }
    /// \brief  Get a pointer to the container
    T* operator->() const {
      return mPtr;
    }
  };

  using iterator = iteratorT<Fer_GmSet>;
  using const_iterator = iteratorT<const Fer_GmSet>;

  /**
   * \brief       Get an iterator
   * \param g,m   Group/member to start with
   */
  iterator begin(gT g = 0, mT m = 0, bool skip_groups = false) {
    auto it = iterator(this, g, m, skip_groups);
    return it;
  }

  const_iterator begin(gT g = 0, mT m = 0, bool skip_groups = false) const {
    auto it = const_iterator(this, g, m, skip_groups);
    return it;
  }

public:

  bool store_load(const char *name);
  bool store_save(const char *name);
#ifdef HOST_TESTING
public:
  operator uint8_t*() { return mBm; }
#endif
private:
  uint8_t mBm[8] = { };
};

static_assert(8 == sizeof (Fer_GmSet));
