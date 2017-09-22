int ser_putc_block(char c);
int ser_getc_block(void);
int ser_putc_nonblock(char c);
int ser_getc_nonblock(void);

void init_usart(unsigned long);
void init_usart_rx_buffer(void);



