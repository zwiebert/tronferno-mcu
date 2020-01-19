#ifndef USER_MAIN_CUID_AUTO_SET_H_
#define USER_MAIN_CUID_AUTO_SET_H_


#include <fernotron/fer.h>

typedef enum  { CUAS_NONE, CUAS_SCANNING, CUAS_TIME_OUT, CUAS_SUCCESS } cuas_state_T;

bool cu_auto_set(u16 id, unsigned timeout_secs);
bool cu_auto_set_check_timeout(void);
bool cu_auto_set_check(fer_sender_basic *fsb);

cuas_state_T cuas_getState();

#endif /* USER_MAIN_CUID_AUTO_SET_H_ */
