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

#define DISP_STAT_EMPTY		0
#define DISP_STAT_CODE_REQ	1
#define DISP_STAT_ACC_REQ	2
#define DISP_STAT_ACC_GRNT	3
#define DISP_STAT_ACC_DEN	4

void displayInit();
void displaySetStatus(int stat);
void displaySetSysStatus(int stat);
void displayMoveFocus();
unsigned int displayGetFocus();

void displayIncFL();
void displayDecFL();
void displayUpdate();
void displayInitComplete(unsigned long ulIPAddress);


#endif /* DISPLAY_H_ */
