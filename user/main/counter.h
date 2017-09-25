#ifndef counter_h__
#define counter_h__

//#define ct_incr(ct, limit)  ((++ct, ct %= (limit)) == 0)
#define ct_incr(ct, limit) (!((++ct >= limit) ? (ct = 0) : 1))
#define ct_incrementP(ctp, limit) ((++*ctp, *ctp %= limit) == 0)

#endif

