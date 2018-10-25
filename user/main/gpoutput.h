#ifndef gpoutput_h_
#define gpoutput_h_

enum gpo_cmd { gpoc_DOWN, gpoc_UP, gpoc_SWITCH};

void gpo_configure(uint8_t gpo_nmb, uint8_t g, uint8_t m, uint8_t gpo_cmd);

void gpo_action(uint8_t g, uint8_t m, uint8_t cmd);


#endif
