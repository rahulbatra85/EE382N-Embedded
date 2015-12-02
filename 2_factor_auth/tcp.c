/*
 * tcp.c
 *
 *  Created on: Nov 24, 2015
 *      Author: rbatra
 */




#include "utils/uartstdio.h"
#include "utils/lwiplib.h"
#include "utils/ustdlib.h"
#include "utils/locator.h"
#include "tcp.h"

unsigned char g_tcpMsg[1024];
unsigned char g_ucTcpDone;
unsigned char g_ucTcpErr;


err_t tcpConnectCallback(void *arg, struct tcp_pcb *tpcb, err_t err);
unsigned int tcpSendPacket(struct tcp_pcb *testpcb);
err_t tcpRecvCallback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
err_t tcpSendCallback(void *arg, struct tcp_pcb *tpcb, u16_t len);
void tcpErrCallback(void *arg, err_t err);
static void close_conn( struct tcp_pcb *pcb );

void tcpSendInit(struct ip_addr ip, unsigned int port)
{
    unsigned int data = 0xdeadbeef;
    struct tcp_pcb *testpcb;

    // create an ip
    //struct ip_addr ip;
    //IP4_ADDR(&ip, 192,168,2,2);    //IP of my server
    //IP4_ADDR(&ip, 173,194,64,141);

    //create the control block
    g_ucTcpDone = 0;
    g_ucTcpErr = 0;
    testpcb = tcp_new();    //testpcb is a global struct tcp_pcb
                            // as defined by lwIP


    //dummy data to pass to callbacks
    tcp_arg(testpcb, &data);

    //register callbacks with the pcb
    tcp_err( testpcb, tcpErrCallback);
    tcp_recv(testpcb, tcpRecvCallback);
    tcp_sent(testpcb, tcpSendCallback);

    //now connect
    tcp_connect(testpcb, &ip, port, tcpConnectCallback);
    UARTprintf("Tcp_Setup.\n");

}

void tcpCopyMsg(unsigned char *msg){
	//send code request
	unsigned i = 0;
	unsigned int len = ustrlen(msg);
	while(i<len){
		g_tcpMsg[i] = msg[i];
		i++;
	}
	g_tcpMsg[i]="\0";
}

// connection established callback, err is unused and only return 0
err_t tcpConnectCallback(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    UARTprintf("Connection Established.\n");
    UARTprintf("Now sending a packet\n");
    tcpSendPacket(tpcb);

    return 0;
}

unsigned int tcpSendPacket(struct tcp_pcb *testpcb)
{
    //char *string =  "Rahul\n";
    //char *request = "GET / HTTP/1.1\r\nHost: 192.168.2.2:9000\r\n\r\n";
    //char *request = "GET / HTTP/1.1\r\nHost: kithookup.appspot.com\r\n\r\n";

    unsigned int len = ustrlen(g_tcpMsg);
    UARTprintf("length: %d\n",len);
    //push to buffer
    err_t error = tcp_write(testpcb, g_tcpMsg, len, TCP_WRITE_FLAG_COPY);

    if (error) {
        UARTprintf("ERROR: Code: %d (tcp_send_packet :: tcp_write)\n", error);
        return 1;
    }

    // now send
    error = tcp_output(testpcb);
    if (error) {
        UARTprintf("ERROR: Code: %d (tcp_send_packet :: tcp_output)\n", error);
        return 1;
    }
    return 0;
}

err_t tcpRecvCallback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    UARTprintf("Data recieved.\n");
    if (p == NULL) {
        UARTprintf("The remote host closed the connection.\n");
        UARTprintf("Now I'm closing the connection.\n");
        close_conn(tpcb);
        g_ucTcpErr = 1;
        return ERR_ABRT;
    } else {
        UARTprintf("Number of pbufs %d\n", pbuf_clen(p));
        UARTprintf("Contents of pbuf %s\n", (char *)p->payload);
    }

    g_ucTcpDone = 1;

    return 0;
}

err_t tcpSendCallback(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    UARTprintf("Data Sent. Length: %d\n", len);

    return 0;
}

void tcpErrCallback(void *arg, err_t err)
{
    UARTprintf("Err Callback recieved. %d\n",err);
    g_ucTcpErr = 1;
    g_ucTcpDone = 1;
}

// Function for closing the connection.
static
void close_conn( struct tcp_pcb *pcb )
{
    tcp_arg    ( pcb, NULL );
    tcp_sent( pcb, NULL );
    tcp_recv( pcb, NULL );
    tcp_close( pcb );
}
