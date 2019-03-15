#ifndef USER_MAIN_CUID_AUTO_SET_H_
#define USER_MAIN_CUID_AUTO_SET_H_


#include <fernotron/fer.h>

bool cu_auto_set(uint16_t id, unsigned timeout_secs);
bool cu_auto_set_check_timeout(void);
bool cu_auto_set_check(fer_sender_basic *fsb);
#endif /* USER_MAIN_CUID_AUTO_SET_H_ */
