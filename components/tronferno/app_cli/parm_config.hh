#include <app_config/options.hh>
#include <config_kvs/settings.hh>
#include <uout/uout_writer.hh>
#include <app_uout/so_types.h>
#include <cli/cli_types.h>
#include <stdint.h>

#define ENABLE_RESTART 1 // allow software reset

extern void (*mcu_restart_cb)();

#define isValid_optStr(cfg, new) true
#define set_optStr(v, cb) config_save_item_s(comp_sett.get_kvsKey(cb), v)
#define set_optBlob(v, cb) config_save_item_b(comp_sett.get_kvsKey(cb), &v, sizeof v)
#define set_optStr_ifValid set_optStr
#define set_opt(t, v, cb) (config_save_item_##t(comp_sett.get_kvsKey(cb), v) && config_item_modified((cb)))
#define set_optN(t, v, cb) (config_save_item_n_##t(comp_sett.get_kvsKey(cb), v) && config_item_modified((cb)))

#define has_changed() SET_BIT(*changed_mask, so_key)

#define is_kt(k) (kt == otok::k_##k)
#define is_key(k) (strcmp(key, k) == 0)
#define is_val(k) (strcmp(val, k) == 0)
/**
 * @brief          print out content of a component setting
 * 
 * @param kt       key
 * @param val      can be a simple question mark or more specific
 * @param td       output writer
 * @return         success
 */
bool process_parmConfig_get_app(otok kt, const char *val, class UoutWriter &td);

/**
 * @brief          print out content of app setting
 * 
 * @param kt       key
 * @param val      can be a simple question mark or more specific
 * @param td       output writer
 * @return         success
 */
bool process_parmConfig_get_comp(otok kt, const char *val, class UoutWriter &td);

int process_parmConfig_assign(KvsType kvsType, const char *kvsKey, StoreFun storeFun, const char *val);

/**
 * @brief                     set and save a component setting
 * 
 * @param kt                  key 
 * @param key                 key as text (for unknown keys)
 * @param val                 value
 * @param td                  output writer
 * @param errors[out]         will be incremented for each occuring error, pass this to @ref parmConfig_reconfig_comp
 * @param changed_mask[out]   the corrosponding bit will be set for any changed setting
 * @return                    success
 */
bool process_parmConfig_comp(otok kt, const char *key, const char *val, class UoutWriter &td, int &errors, uint64_t &changed_mask);
/**
 * @brief                     set and save an app  setting
 * 
 * @param kt                  key 
 * @param key                 key as text (for unknown keys)
 * @param val                 value
 * @param td                  output writer
 * @param errors[out]         will be incremented for each occuring error, pass this to @ref parmConfig_reconfig_app
 * @param changed_mask[out]   the corrosponding bit will be set for any changed setting
 * @return                    success
 */
bool process_parmConfig_app(otok kt, const char *key, const char *val, class UoutWriter &td, int &errors, uint64_t &changed_mask);
/**
 * @brief                     Reconfigure a component according to which settings were changed.
 * 
 *                            The reconfiguring is done in main task.
 * 
 * @param changed_mask        each setting is represended by a bit
 */
void parmConfig_reconfig_comp(uint64_t changed_mask);
/**
 * @brief                     Reconfigure an app-component according to which settings were changed.
 * 
 *                            The reconfiguring is done in main task.
 * 
 * @param changed_mask        each setting is represended by a bit
 */
void parmConfig_reconfig_app(uint64_t changed_mask);
