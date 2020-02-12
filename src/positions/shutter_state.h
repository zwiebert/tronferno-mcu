#ifndef USER_MAIN_SHUTTER_STATE_H_
#define USER_MAIN_SHUTTER_STATE_H_

typedef u8 gm_bitmask_t[8];
#define gm_PutBit(gm, g, m, val) PUT_BIT(gm[(g)], (m), (val))
#define gm_ClrBit(gm, g, m) CLR_BIT(gm[(g)], (m))
#define gm_GetBit(gm, g, m) GET_BIT(gm[(g)], (m))
#define gm_SetBit(gm, g, m) SET_BIT(gm[(g)], (m))

typedef u8 shutterGroupPositionsT[8];

// read/save one or more elements of type gm_bitmask_t
int read_gm_bitmask(const char *name, const gm_bitmask_t *gm, int count);
int save_gm_bitmask(const char *name, gm_bitmask_t *gm, int count);

int read_g_positions(u8 g, const shutterGroupPositionsT positions);
int save_g_positions(u8 g, shutterGroupPositionsT positions);

#endif
