#ifndef USER_MAIN_SHUTTER_STATE_H_
#define USER_MAIN_SHUTTER_STATE_H_

typedef u8 gm_bitmask_t[8];

// read/save one or more elements of type gm_bitmask_t
int read_gm_bitmask(const char *name, const gm_bitmask_t *gm, int count);
int save_gm_bitmask(const char *name, gm_bitmask_t *gm, int count);


#endif
