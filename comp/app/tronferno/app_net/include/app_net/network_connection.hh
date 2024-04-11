#pragma once


enum nwConnection {
  nwNone, nwWlanSta, nwWlanAp, nwLan, nwLanOrWlanSta, nwLEN,
};

extern nwConnection network_default, network_fallback;

bool nwc_connect(nwConnection nw, bool force);

bool nwc_connect_default(nwConnection nw);
inline void nwc_connect_default() {nwc_connect_default(network_default);}

bool nwc_connect_fallback(nwConnection nw);
inline void nwc_connect_fallback() {nwc_connect_fallback(network_fallback);}

bool nwc_disconnect();
