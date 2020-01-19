#ifndef bcd_h_
#define bcd_h_

u8 bcd2dec(u8 bcd);
u8 dec2bcd(u8 dec);

u8 dec2bcd_special(u8 dec); // like above, but replaces all 9 with 0xf in result


#endif
