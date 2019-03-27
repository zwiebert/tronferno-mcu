/*
 * http_server.h
 *
 *  Created on: 26.03.2019
 *      Author: bertw
 */

#ifndef SRC_USERIO_HTTP_SERVER_H_
#define SRC_USERIO_HTTP_SERVER_H_

typedef enum { HQT_NONE, } hts_query_t;

void hts_query(hts_query_t qtype, const char *qstr, int qstr_len);

void hts_start_server(void);
void hts_stop_server(void);


#endif /* SRC_USERIO_HTTP_SERVER_H_ */
