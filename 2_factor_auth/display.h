/*
 * Display.h
 *
 *  Created on: Nov 24, 2015
 *      Author: rbatra
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_


#define DISP_SYS_STAT_INIT	0
#define DISP_SYS_STAT_CONT	1
#define DISP_SYS_STAT_CONN	2
#define DISP_SYS_STAT_DC	3

#define DISP_STAT_EMPTY			0
#define DISP_STAT_CODE_REQ		1
#define DISP_STAT_CODE_REQ_ERR	2
#define DISP_STAT_ACC_REQ		3
#define DISP_STAT_ACC_REQ_ERR	4
#define DISP_STAT_ACC_GRNT		5
#define DISP_STAT_ACC_DEN		6
#define DISP_STAT_NUM			7

#define DISP_TTL_USERID			0
#define DISP_TTL_ACCESS_CODE	1
#define DISP_TTL_NUM 			2

void displayInit();
void displaySetStatus(int stat);
void displaySetSysStatus(int stat);
void displaySetTitleBar(int stat);
void displayMoveFocus();
unsigned int displayGetFocus();
void displaySetFocusFL();
void displayResetFL();

void displayIncFL();
void displayDecFL();
unsigned char* displayGetFL();
void displayUpdate();
void displayAllUpdate();
void displayInitComplete(unsigned long ulIPAddress);



#endif /* DISPLAY_H_ */
