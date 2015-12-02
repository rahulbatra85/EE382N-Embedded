/*
 * Display.c
 *
 *  Created on: Nov 24, 2015
 *      Author: rbatra
 */

#include "display.h"
#include "drivers/rit128x96x4.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"

#define DISP_ELEM_NONE 		0
#define DISP_ELEM_FL0		0x2
#define DISP_ELEM_FL1		0x4
#define DISP_ELEM_FL2		0x8
#define DISP_ELEM_FL3		0x10
#define DISP_ELEM_FL4		0x20
#define DISP_ELEM_BT_SEND	0x40
#define DISP_ELEM_BT_CLEAR	0x80
#define DISP_ELEM_SYS_STAT	0x100
#define DISP_ELEM_IP_ADDR	0x200
#define DISP_ELEM_STAT		0x400
#define DISP_ELEM_TITLE_BAR	0x800
#define DISP_ELEM_NUM		12

#define DISP_ELEM_HL_NONE 		0
#define DISP_ELEM_HL_FL0		1
#define DISP_ELEM_HL_FL1		2
#define DISP_ELEM_HL_FL2		3
#define DISP_ELEM_HL_FL3		4
#define DISP_ELEM_HL_FL4		5
#define DISP_ELEM_HL_BT_SEND	6
#define DISP_ELEM_HL_BT_CLEAR	7
#define DISP_ELEM_HL_SYS_STAT	8
#define DISP_ELEM_HL_IP_ADDR	9
#define DISP_ELEM_HL_STAT		10
#define DISP_ELEM_HL_TITLE_BAR	11
#define DISP_ELEM_HL_NUM		12

unsigned char g_pucFL[6];
unsigned char g_pucStrFL[6];

unsigned char *g_pucDispSysStatus;
unsigned char g_aucDispSysStatus[4][30]={{"SysStat: Initing...\0"},
		{"SysStat: Connecting...\0"},
		{"SysStat: Connected\0"},
		{"SysStat: Disconnected\0"}};
unsigned char g_pucDispSysIPAdd[25]="IP Addr:   .   .   .   \0";

unsigned char *g_pucDispStatus;
unsigned char g_aucDispStatus[DISP_STAT_NUM][30]={{""},
		{"Code Req Sent\0"},
		{"Code Req Send Err\0"},
		{"Access Req Sent\0"},
		{"Access Req Send Err\0"},
		{"Access Granted\0"},
		{"Access Denied\0"}};

unsigned char *g_pucDispTitle;
unsigned char g_aucDispTitle[DISP_TTL_NUM][20]={
		{"Enter User ID\0"},
		{"Enter Access Code\0"}
};
unsigned char g_aucDispField[6][3]={{"0 \0"},
							{"0 \0"},
							{"0 \0"},
							{"0 \0"},
							{"0 \0"},
							{"0 \0"}
};
unsigned char g_pucDispButSend[]="SEND\0";
unsigned char g_pucDispButClear[]="CLEAR\0";
unsigned char g_pucDispLineBrk[]="------------------------------\0";

unsigned int g_ucToBeUpd;
unsigned int g_ucDispCurrHL;
unsigned char g_ucDispHLFL[DISP_ELEM_HL_NUM];
unsigned char g_ucDispBaseHL[DISP_ELEM_HL_NUM];

//System Status: Initializing/Connecting/Connected/Disconnected
//		IP Addr:_ _ _. _ _ _. _ _ _. _ _ _
//-------------------------------------------------------------
//Enter User ID:
//		_ _ _ _ _
//	  SEND 	CLEAR
//Or
//Enter Access Code:
//		_ _ _ _ _
//	  SEND  CLEAR
//-------------------------------------------------------------
//Status:
//	  /Code Request Sent/Access Request Sent/Access Granted/Access Denied

void displayInit(){
	RIT128x96x4Init(1000000);

	g_pucFL[0] = 0;
	g_pucFL[0] = 0;
	g_pucFL[0] = 0;
	g_pucFL[0] = 0;
	g_pucFL[0] = 0;

	g_ucDispBaseHL[DISP_ELEM_HL_SYS_STAT] = 7;
	g_ucDispBaseHL[DISP_ELEM_HL_IP_ADDR] = 7;
	g_ucDispBaseHL[DISP_ELEM_HL_TITLE_BAR] = 9;
	g_ucDispBaseHL[DISP_ELEM_HL_FL0] = 2;
	g_ucDispBaseHL[DISP_ELEM_HL_FL1] = 2;
	g_ucDispBaseHL[DISP_ELEM_HL_FL2] = 2;
	g_ucDispBaseHL[DISP_ELEM_HL_FL3] = 2;
	g_ucDispBaseHL[DISP_ELEM_HL_FL4] = 2;
	g_ucDispBaseHL[DISP_ELEM_HL_BT_SEND] = 2;
	g_ucDispBaseHL[DISP_ELEM_HL_BT_CLEAR] = 2;
	g_ucDispBaseHL[DISP_ELEM_HL_STAT] = 2;

	g_pucDispSysStatus = g_aucDispSysStatus[DISP_SYS_STAT_INIT];
	RIT128x96x4StringDraw(g_pucDispSysStatus, 0, 0, g_ucDispBaseHL[DISP_ELEM_HL_SYS_STAT]);
	RIT128x96x4StringDraw(g_pucDispSysIPAdd, 8, 8, g_ucDispBaseHL[DISP_ELEM_HL_IP_ADDR]);
	RIT128x96x4StringDraw(g_pucDispLineBrk, 0, 16, 10);

	g_pucDispTitle = g_aucDispTitle[DISP_TTL_USERID];
	RIT128x96x4StringDraw(g_pucDispTitle, 0, 32, g_ucDispBaseHL[DISP_ELEM_HL_TITLE_BAR]);
	RIT128x96x4StringDraw(g_aucDispField[1], 38, 44, g_ucDispBaseHL[DISP_ELEM_HL_FL0]);
	RIT128x96x4StringDraw(g_aucDispField[2], 50, 44, g_ucDispBaseHL[DISP_ELEM_HL_FL1]);
	RIT128x96x4StringDraw(g_aucDispField[3], 62, 44, g_ucDispBaseHL[DISP_ELEM_HL_FL2]);
	RIT128x96x4StringDraw(g_aucDispField[4], 74, 44, g_ucDispBaseHL[DISP_ELEM_HL_FL3]);
	RIT128x96x4StringDraw(g_aucDispField[5], 86, 44, g_ucDispBaseHL[DISP_ELEM_HL_FL4]);
	RIT128x96x4StringDraw(g_pucDispButSend, 30, 60, g_ucDispBaseHL[DISP_ELEM_HL_BT_SEND]);
	RIT128x96x4StringDraw(g_pucDispButClear, 75, 60, g_ucDispBaseHL[DISP_ELEM_HL_BT_CLEAR]);
	RIT128x96x4StringDraw(g_pucDispLineBrk, 0, 68, 10);

	g_pucDispStatus = g_aucDispStatus[DISP_STAT_EMPTY];
	RIT128x96x4StringDraw(g_pucDispStatus, 10, 80, 10);

	g_ucDispCurrHL = DISP_ELEM_HL_NONE;
	int i;
	for(i=0; i<DISP_ELEM_HL_NUM; i++){
		g_ucDispHLFL[i] = 0;
	}
	g_ucToBeUpd = DISP_ELEM_NONE;
}

void displayInitComplete(unsigned long ulIPAddress){
	g_pucDispSysStatus = g_aucDispSysStatus[DISP_SYS_STAT_CONN];//Set to connected

	unsigned char *pucTemp = (unsigned char *)&ulIPAddress;
	usprintf(g_pucDispSysIPAdd+8, "%d.%d.%d.%d", pucTemp[0], pucTemp[1],
			pucTemp[2], pucTemp[3]);

	UARTprintf("IPAddr: %s\n",g_pucDispSysIPAdd);

	//Update Base Highlight values
	g_ucDispBaseHL[DISP_ELEM_HL_SYS_STAT] = 5;
	g_ucDispBaseHL[DISP_ELEM_HL_IP_ADDR] = 5;
	g_ucDispBaseHL[DISP_ELEM_HL_TITLE_BAR] = 10;
	g_ucDispBaseHL[DISP_ELEM_HL_FL0] = 5;
	g_ucDispBaseHL[DISP_ELEM_HL_FL1] = 5;
	g_ucDispBaseHL[DISP_ELEM_HL_FL2] = 5;
	g_ucDispBaseHL[DISP_ELEM_HL_FL3] = 5;
	g_ucDispBaseHL[DISP_ELEM_HL_FL4] = 5;
	g_ucDispBaseHL[DISP_ELEM_HL_BT_SEND] = 5;
	g_ucDispBaseHL[DISP_ELEM_HL_BT_CLEAR] = 5;
	g_ucDispBaseHL[DISP_ELEM_HL_STAT] = 10;

	//Set FL0 as highlighted
	g_ucDispCurrHL = DISP_ELEM_HL_FL0;
	g_ucDispHLFL[g_ucDispCurrHL] = 10;

	//Update everything
	g_ucToBeUpd =  DISP_ELEM_FL0|DISP_ELEM_FL1|DISP_ELEM_FL2|DISP_ELEM_FL3|DISP_ELEM_FL4|DISP_ELEM_BT_SEND
					|DISP_ELEM_BT_CLEAR|DISP_ELEM_SYS_STAT|DISP_ELEM_IP_ADDR|DISP_ELEM_STAT;
}

void displaySetTitleBar(int stat){
	g_pucDispTitle = g_aucDispTitle[stat];
	g_ucToBeUpd |=	DISP_ELEM_TITLE_BAR;
}

void displaySetSysStatus(int stat){
	g_pucDispSysStatus = g_aucDispSysStatus[stat];
	g_ucToBeUpd |=	DISP_ELEM_SYS_STAT;
}

void displaySetStatus(int stat){
	g_pucDispStatus =  g_aucDispStatus[stat];
	g_ucToBeUpd |=	DISP_ELEM_STAT;
}

void displaySetFocusFL(){
	g_ucToBeUpd |= (1<<g_ucDispCurrHL);
	g_ucDispHLFL[g_ucDispCurrHL] = 0;

	g_ucDispCurrHL = DISP_ELEM_HL_FL0;
	g_ucToBeUpd |= (1<<g_ucDispCurrHL);
	g_ucDispHLFL[g_ucDispCurrHL] = 10;
}

void displayMoveFocus(){
	g_ucToBeUpd |= (1<<g_ucDispCurrHL);
	g_ucDispHLFL[g_ucDispCurrHL] = 0;
	if(g_ucDispCurrHL == DISP_ELEM_HL_BT_CLEAR){
		g_ucDispCurrHL = DISP_ELEM_HL_FL0;
	} else{
		g_ucDispCurrHL = g_ucDispCurrHL + 1;
	}
	g_ucToBeUpd |= (1<<g_ucDispCurrHL);
	g_ucDispHLFL[g_ucDispCurrHL] = 10;
}

unsigned int displayGetFocus(){
	return g_ucDispCurrHL;
}

void displayIncFL(){
	if((g_ucDispCurrHL >= DISP_ELEM_HL_FL0) && (g_ucDispCurrHL<=DISP_ELEM_HL_FL4)){
		if(g_pucFL[g_ucDispCurrHL] != 9){
			g_pucFL[g_ucDispCurrHL]++;
		} else{
			g_pucFL[g_ucDispCurrHL] = 0;
		}
		g_aucDispField[g_ucDispCurrHL][0] = 0x30 + g_pucFL[g_ucDispCurrHL];
		g_ucToBeUpd |= (1<<g_ucDispCurrHL);
	}

	UARTprintf("incFL: %d%d%d%d%d\n",g_pucFL[1],g_pucFL[2],
			g_pucFL[3],g_pucFL[4],g_pucFL[5]);
}

void displayDecFL(){
	if((g_ucDispCurrHL >= DISP_ELEM_HL_FL0) && (g_ucDispCurrHL<=DISP_ELEM_HL_FL4)){
		if(g_pucFL[g_ucDispCurrHL] != 0){
			g_pucFL[g_ucDispCurrHL]--;
		} else{
			g_pucFL[g_ucDispCurrHL] = 9;
		}
		g_aucDispField[g_ucDispCurrHL][0] = 0x30 + g_pucFL[g_ucDispCurrHL];
		g_ucToBeUpd |= (1<<g_ucDispCurrHL);
		UARTprintf("decFL: %d%d%d%d%d\n",g_pucFL[1],g_pucFL[2],
					g_pucFL[3],g_pucFL[4],g_pucFL[5]);
	}
}

void displayResetFL(){
	g_pucFL[1] = 0;
	g_pucFL[2] = 0;
	g_pucFL[3] = 0;
	g_pucFL[4] = 0;
	g_pucFL[5] = 0;

	g_aucDispField[1][0] = 0x30;
	g_aucDispField[2][0] = 0x30;
	g_aucDispField[3][0] = 0x30;
	g_aucDispField[4][0] = 0x30;
	g_aucDispField[5][0] = 0x30;
	g_ucToBeUpd |= DISP_ELEM_FL0|DISP_ELEM_FL1|DISP_ELEM_FL2|DISP_ELEM_FL3|DISP_ELEM_FL4;
}

unsigned char* displayGetFL(){
	usprintf(g_pucStrFL, "%d%d%d%d%d\0", g_pucFL[1],g_pucFL[2],
			g_pucFL[3],g_pucFL[4],g_pucFL[5]);
	return g_pucStrFL;
}

void displayAllUpdate(){
	if(g_ucToBeUpd){
		if(g_ucToBeUpd & DISP_ELEM_SYS_STAT){
			RIT128x96x4StringDraw(g_pucDispSysStatus, 0, 0, g_ucDispBaseHL[DISP_ELEM_HL_SYS_STAT]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_SYS_STAT);
		}
		if(g_ucToBeUpd & DISP_ELEM_IP_ADDR){
			RIT128x96x4StringDraw(g_pucDispSysIPAdd, 8, 8, g_ucDispBaseHL[DISP_ELEM_HL_IP_ADDR]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_IP_ADDR);
		}
		if(g_ucToBeUpd & DISP_ELEM_TITLE_BAR){
			RIT128x96x4StringDraw(g_pucDispTitle, 0, 32, g_ucDispBaseHL[DISP_ELEM_HL_TITLE_BAR]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_TITLE_BAR);
		}
		if(g_ucToBeUpd & DISP_ELEM_STAT){
			RIT128x96x4StringDraw(g_pucDispStatus, 10, 80, g_ucDispBaseHL[DISP_ELEM_HL_STAT]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_STAT);
		}
		if((g_ucToBeUpd & DISP_ELEM_BT_SEND) || (g_ucToBeUpd & DISP_ELEM_HL_BT_CLEAR)){
			RIT128x96x4StringDraw(g_pucDispButSend, 30, 60, g_ucDispBaseHL[DISP_ELEM_HL_BT_SEND]+g_ucDispHLFL[DISP_ELEM_HL_BT_SEND]);
			RIT128x96x4StringDraw(g_pucDispButClear, 75, 60, g_ucDispBaseHL[DISP_ELEM_HL_BT_CLEAR]+g_ucDispHLFL[DISP_ELEM_HL_BT_CLEAR]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_BT_SEND);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_BT_CLEAR);
		}
		if(g_ucToBeUpd & DISP_ELEM_FL0){
			RIT128x96x4StringDraw(g_aucDispField[1], 38, 44, g_ucDispBaseHL[DISP_ELEM_HL_FL0]+g_ucDispHLFL[DISP_ELEM_HL_FL0]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_FL0);
		}
		if(g_ucToBeUpd & DISP_ELEM_FL1){
			RIT128x96x4StringDraw(g_aucDispField[2], 50, 44,g_ucDispBaseHL[DISP_ELEM_HL_FL1]+g_ucDispHLFL[DISP_ELEM_HL_FL1]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_FL1);
		}
		if(g_ucToBeUpd & DISP_ELEM_FL2){
			RIT128x96x4StringDraw(g_aucDispField[3], 62, 44, g_ucDispBaseHL[DISP_ELEM_HL_FL2]+g_ucDispHLFL[DISP_ELEM_HL_FL2]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_FL2);
		}
		if(g_ucToBeUpd & DISP_ELEM_FL3){
			RIT128x96x4StringDraw(g_aucDispField[4], 74, 44, g_ucDispBaseHL[DISP_ELEM_HL_FL3]+g_ucDispHLFL[DISP_ELEM_HL_FL3]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_FL3);
		}
		if(g_ucToBeUpd & DISP_ELEM_FL4){
			RIT128x96x4StringDraw(g_aucDispField[5], 86, 44, g_ucDispBaseHL[DISP_ELEM_HL_FL4]+g_ucDispHLFL[DISP_ELEM_HL_FL4]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_FL4);
		}

		g_ucToBeUpd = DISP_ELEM_NONE;
	}

}

void displayUpdate(){
	if(g_ucToBeUpd){
		if(g_ucToBeUpd & DISP_ELEM_STAT){
			RIT128x96x4StringDraw(g_pucDispStatus, 10, 80, g_ucDispBaseHL[DISP_ELEM_HL_STAT]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_STAT);
		}
		if((g_ucToBeUpd & DISP_ELEM_BT_SEND) || (g_ucToBeUpd & DISP_ELEM_HL_BT_CLEAR)){
			RIT128x96x4StringDraw(g_pucDispButSend, 30, 60, g_ucDispBaseHL[DISP_ELEM_HL_BT_SEND]+g_ucDispHLFL[DISP_ELEM_HL_BT_SEND]);
			RIT128x96x4StringDraw(g_pucDispButClear, 75, 60, g_ucDispBaseHL[DISP_ELEM_HL_BT_CLEAR]+g_ucDispHLFL[DISP_ELEM_HL_BT_CLEAR]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_BT_SEND);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_BT_CLEAR);
		}
		if(g_ucToBeUpd & DISP_ELEM_FL0){
			RIT128x96x4StringDraw(g_aucDispField[1], 38, 44, g_ucDispBaseHL[DISP_ELEM_HL_FL0]+g_ucDispHLFL[DISP_ELEM_HL_FL0]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_FL0);
		}
		if(g_ucToBeUpd & DISP_ELEM_FL1){
			RIT128x96x4StringDraw(g_aucDispField[2], 50, 44,g_ucDispBaseHL[DISP_ELEM_HL_FL1]+g_ucDispHLFL[DISP_ELEM_HL_FL1]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_FL1);
		}
		if(g_ucToBeUpd & DISP_ELEM_FL2){
			RIT128x96x4StringDraw(g_aucDispField[3], 62, 44, g_ucDispBaseHL[DISP_ELEM_HL_FL2]+g_ucDispHLFL[DISP_ELEM_HL_FL2]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_FL2);
		}
		if(g_ucToBeUpd & DISP_ELEM_FL3){
			RIT128x96x4StringDraw(g_aucDispField[4], 74, 44, g_ucDispBaseHL[DISP_ELEM_HL_FL3]+g_ucDispHLFL[DISP_ELEM_HL_FL3]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_FL3);
		}
		if(g_ucToBeUpd & DISP_ELEM_FL4){
			RIT128x96x4StringDraw(g_aucDispField[5], 86, 44, g_ucDispBaseHL[DISP_ELEM_HL_FL4]+g_ucDispHLFL[DISP_ELEM_HL_FL4]);
			g_ucToBeUpd = (g_ucToBeUpd & ~DISP_ELEM_FL4);
		}

		g_ucToBeUpd = DISP_ELEM_NONE;
	}
}
