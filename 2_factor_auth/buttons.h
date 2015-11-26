/*
 * Switches.h
 *
 *  Created on: Nov 24, 2015
 *      Author: rbatra
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#define FLAG_BUTTON_UP_PRESS	0
#define FLAG_BUTTON_DOWN_PRESS	1
#define FLAG_BUTTON_LEFT_PRESS	2
#define FLAG_BUTTON_RIGHT_PRESS	3
#define FLAG_BUTTON_SELECT_PRESS 4




void buttonsInit();
void buttonsProcess();
void buttonsEnable();
void buttonsDisable();


#endif /* BUTTONS_H_ */
