/*
 * http.h
 *
 *  Created on: Nov 24, 2015
 *      Author: rbatra
 */

#ifndef HTTP_H_
#define HTTP_H_

void tcpSendInit(struct ip_addr ip, unsigned int port);
void tcpCopyMsg(unsigned char *msg);

#endif /* HTTP_H_ */
