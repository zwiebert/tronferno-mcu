#pragma once


#include "config_kvs/settings.hh"
#include "config_kvs/comp_settings.hh"
#include "app_settings/app_settings.hh"

#include <iterator>

class AllSettings final: public Settings1stBase {
public:

  virtual const SettingsData *get_SettingsData(const otok optKey) const {

    for (auto it = std::begin(m_data); it != std::end(m_data); ++it) {
      if (auto result = (*it)->get_SettingsData(optKey); result)
        return result;
    }
    return nullptr;
  }

  virtual soCfgFunT get_soCfgFun(const otok optKey) const {

    for (auto it = std::begin(m_data); it != std::end(m_data); ++it) {
      if (auto result = (*it)->get_soCfgFun(optKey); result)
        return result;
    }
    return nullptr;
  }

  virtual StoreFun get_storeFun(const otok optKey) const {

    for (auto it = std::begin(m_data); it != std::end(m_data); ++it) {
      if (auto result = (*it)->get_storeFun(optKey); result)
        return result;
    }
    return STF_none;
  }

  virtual const char* get_kvsKey(const otok optKey) const {

    for (auto it = std::begin(m_data); it != std::end(m_data); ++it) {
      if (auto result = (*it)->get_kvsKey(optKey); result)
        return result;
    }
    return nullptr;
  }

  virtual KvsType get_kvsType(const otok optKey) const {

    for (auto it = std::begin(m_data); it != std::end(m_data); ++it) {
      if (auto result = (*it)->get_kvsType(optKey); result)
        return result;
    }
    return CBT_none;
  }

protected:
  const Settings1stBase *m_data[2] = { &comp_sett, &app_sett };
};

extern const AllSettings all_settings;
