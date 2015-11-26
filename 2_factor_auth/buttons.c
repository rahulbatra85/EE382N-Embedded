/*
 * Switches.c
 *
 *  Created on: Nov 24, 2015
 *      Author: rbatra
 */

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "buttons.h"
#include "display.h"

unsigned int g_uiButtonsEnabled;
unsigned long g_ulButtonFlags;
unsigned char g_ucButtons = 0x1f;
unsigned char g_ucStr[32];
unsigned char g_ucButtonsNotSendPress;


void buttonsInit(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); //Enable Port E

	//PE0
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0); //Set PE0 as input
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA,
	               	   GPIO_PIN_TYPE_STD_WPU); //Set weak pull-up on PE0

	//PE1
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1); //Set PE1 as input
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA,
	            		GPIO_PIN_TYPE_STD_WPU); //Set weak pull-up on PE1

	//PE2
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_2); //Set PE2 as input
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA,
	                     GPIO_PIN_TYPE_STD_WPU); //Set weak pull-up on PE2

	//PE3
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_3); //Set PE3 as input
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA,
	                     GPIO_PIN_TYPE_STD_WPU); //Set weak pull-up on PE3

	//PF1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //Enable Port F
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1); //Set PF1 as input
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA,
	                     GPIO_PIN_TYPE_STD_WPU); //Set weak pull-up on PF1

	g_uiButtonsEnabled = 0;

	UARTprintf("buttonsInit\n");
}

void buttonsProcess(){
	unsigned char buttons;
	//if buttons are enabled
	if(g_uiButtonsEnabled){
		//if no button is held
		if((g_ucButtons & 0x1f) == 0x1f){
			IntMasterDisable();
			buttons =  g_ulButtonFlags;
		    g_ulButtonFlags = 0;
			IntMasterEnable();
			//Select Button Press
			if(buttons & 0x10){
				g_ucButtonsNotSendPress = 0;
			}
			//Right Button Press
			else if(buttons & 0x8){
				//shiftFocus to next active element
				displayMoveFocus();
				//UARTprintf("BUTTONS: Right Pressed\n");
			}
			//Up button Press or Down button press
			else if((buttons &0x1)){
					displayIncFL();
				//UARTprintf("BUTTONS: Up is Pressed\n");
			}//Down button Press or Down button press
			else if((buttons &0x2)){
					displayDecFL(); //Update field
				//UARTprintf("BUTTONS: Up is Pressed\n");
			} else {

			}
		}
	}
}

void buttonsEnable(){
	g_uiButtonsEnabled = 1;
}

void buttonsDisable(){
	g_uiButtonsEnabled = 0;
}

