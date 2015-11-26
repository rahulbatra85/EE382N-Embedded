/*
 * main.c
 *
 *  Created on: Nov 24, 2015
 *      Author: rbatra
 */
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/flash.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/locator.h"
#include "utils/lwiplib.h"
#include "utils/uartstdio.h"
#include "buttons.h"
#include "display.h"
#include "http.h"

#define SYSTICKHZ               100
#define SYSTICKMS               (1000 / SYSTICKHZ)
#define SYSTICKUS               (1000000 / SYSTICKHZ)
#define SYSTICKNS               (1000000000 / SYSTICKHZ)

//*****************************************************************************
//
// Interrupt priority definitions.  The top 3 bits of these values are
// significant with lower values indicating higher priority interrupts.
//
//*****************************************************************************
#define SYSTICK_INT_PRIORITY    0x80
#define ETHERNET_INT_PRIORITY   0xC0



extern unsigned long g_ulButtonFlags;
extern unsigned char g_ucButtons;
extern unsigned char g_ucButtonsNotSendPress;
static unsigned char g_ucButtonClockA = 0;
static unsigned char g_ucButtonClockB = 0;

unsigned int g_ucEnetInit;

//Function Prototypes
void facAuthInit();
void facAuthSystemInit();

//Function Declaration
//System Tick Interrupt Handler
void SysTickIntHandler(void)
{
	unsigned long ulData, ulDelta;

    //
    // Call the lwIP timer handler.
    //
    lwIPTimer(SYSTICKMS);

    //Scan Buttons
    // Read the state of the push buttons.
    ulData = (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)) |
    			(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) << 3);

    // Determine the switches that are at a different state than the debounced state.
    ulDelta = ulData ^ g_ucButtons;

    // Increment the clocks by one.
    g_ucButtonClockA ^= g_ucButtonClockA;
    g_ucButtonClockB = ~g_ucButtonClockB;

    // Reset the clocks corresponding to switches that have not changed state.
    g_ucButtonClockA &= ulDelta;
    g_ucButtonClockB &= ulDelta;

    // Get the new debounced switch state.
    g_ucButtons &= g_ucButtonClockA | g_ucButtonClockB;
    g_ucButtons |= (~(g_ucButtonClockA | g_ucButtonClockB)) & ulData;

    // Determine the switches that just changed debounced state.
    ulDelta ^= (g_ucButtonClockA | g_ucButtonClockB);

    // See if the up button was just pressed.
    if((ulDelta & 0x1) && !(g_ucButtons & 0x1))
    {
    	// Set a flag to indicate that the up button was just pressed.
    	HWREGBITW(&g_ulButtonFlags, FLAG_BUTTON_UP_PRESS) = 1;
    }
    // See if the down button was just pressed.
    if((ulDelta & 0x2) && !(g_ucButtons & 0x2))
    {
    	// Set a flag to indicate that the down button was just pressed.
    	HWREGBITW(&g_ulButtonFlags, FLAG_BUTTON_DOWN_PRESS) = 1;
    }
    // See if the left button was just pressed.
    if((ulDelta & 0x4) && !(g_ucButtons & 0x4))
    {
    	// Set a flag to indicate that the left button was just pressed.
    	HWREGBITW(&g_ulButtonFlags, FLAG_BUTTON_LEFT_PRESS) = 1;
    }
    // See if the right button was just pressed.
    if((ulDelta & 0x8) && !(g_ucButtons & 0x8))
    {
    	// Set a flag to indicate that the right button was just pressed.
    	HWREGBITW(&g_ulButtonFlags, FLAG_BUTTON_RIGHT_PRESS) = 1;
    }
    // See if the select button was just pressed.
    if((ulDelta & 0x10) && !(g_ucButtons & 0x10))
    {
    	// Set a flag to indicate that the right button was just pressed.
    	HWREGBITW(&g_ulButtonFlags, FLAG_BUTTON_SELECT_PRESS) = 1;
    }
}


//*****************************************************************************
//
// Required by lwIP library to support any host-related timer functions.
//
//*****************************************************************************
void
lwIPHostTimerHandler(void)
{
    static unsigned long ulLastIPAddress = 0;
    unsigned long ulIPAddress;

    ulIPAddress = lwIPLocalIPAddrGet();

    // If IP Address has not yet been assigned, update the display accordingly
    if(ulIPAddress == 0)
    {
    }
    // Check if IP address has changed, and display if it has.
    else if(ulLastIPAddress != ulIPAddress)
    {
    	g_ucEnetInit = true;
    }
}


void facAuthSystemInit(){
    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);
}

void facAuthUARTInit(){
    // Initialize the UART for debug output.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioInit(0);
}

void facAuthInterruptInit(){
    // Configure SysTick for a periodic interrupt.
    SysTickPeriodSet(SysCtlClockGet() / SYSTICKHZ);
    SysTickEnable();
    SysTickIntEnable();

    // Enable processor interrupts.
    IntMasterEnable();
}

void facAuthEthernetInit() {

    // Enable and Reset the Ethernet Controller
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
    SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);

    // Enable Port F for Ethernet LEDs.
    //  LED0        Bit 3   Output
    //  LED1        Bit 2   Output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);
}

void facAuthLwipInit(){
    unsigned long ulUser0, ulUser1;
    unsigned char pucMACArray[8];

    //
    // Configure the hardware MAC address for Ethernet Controller filtering of
    // incoming packets.
    //
    // For the LM3S6965 Evaluation Kit, the MAC address will be stored in the
    // non-volatile USER0 and USER1 registers.  These registers can be read
    // using the FlashUserGet function, as illustrated below.
    //
    FlashUserGet(&ulUser0, &ulUser1);
    if((ulUser0 == 0xffffffff) || (ulUser1 == 0xffffffff))
    {
        //
        // We should never get here.  This is an error if the MAC address has
        // not been programmed into the device.  Exit the program.
        //
        //RIT128x96x4Enable(1000000);
        //RIT128x96x4StringDraw("MAC Address", 0, 16, 15);
        //RIT128x96x4StringDraw("Not Programmed!", 0, 24, 15);
        while(1)
        {
        }
    }

    // Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
    // address needed to program the hardware registers, then program the MAC
    // address into the Ethernet Controller registers.
    pucMACArray[0] = ((ulUser0 >>  0) & 0xff);
    pucMACArray[1] = ((ulUser0 >>  8) & 0xff);
    pucMACArray[2] = ((ulUser0 >> 16) & 0xff);
    pucMACArray[3] = ((ulUser1 >>  0) & 0xff);
    pucMACArray[4] = ((ulUser1 >>  8) & 0xff);
    pucMACArray[5] = ((ulUser1 >> 16) & 0xff);

    //Update display
    displaySetSysStatus(2);
    displayUpdate();

    //
    // Initialze the lwIP library, using DHCP.
    lwIPInit(pucMACArray, 0, 0, 0, IPADDR_USE_DHCP);

    // Setup the device locator service.
    LocatorInit();
    LocatorMACAddrSet(pucMACArray);
    LocatorAppTitleSet("fac_auth_sys");


}

//Initialization Routine
void facAuthInit(){
	facAuthSystemInit();//initialize clocks
	facAuthUARTInit();//Uart
	displayInit(); //displayInit
	facAuthEthernetInit(); //httpEthernetInit()
	buttonsInit(); //Button Init
	facAuthInterruptInit();//Interrupts - Systick

	g_ucEnetInit = 0;
	facAuthLwipInit(); //httpLwipInit

	//
	//Set Interrupt Priority
    // Set the interrupt priorities.  We set the SysTick interrupt to a higher
    // priority than the Ethernet interrupt to ensure that the file system
    // tick is processed if SysTick occurs while the Ethernet handler is being
    // processed.  This is very likely since all the TCP/IP and HTTP work is
    // done in the context of the Ethernet interrupt.
    //
    IntPriorityGroupingSet(4);
    IntPrioritySet(INT_ETH, ETHERNET_INT_PRIORITY);
    IntPrioritySet(FAULT_SYSTICK, SYSTICK_INT_PRIORITY);

}

//Main entry point of the application
int main(void)
{
	unsigned long ulIPAddress;

	//Step 0
	facAuthInit();

	//wait until IP addr is not assigned
	while(!g_ucEnetInit){}

    ulIPAddress = lwIPLocalIPAddrGet();

	//update display to show connected status and IP address
	displayInitComplete(ulIPAddress);
	displayUpdate();

	//Step 1
	//Highlight userID section and enable buttons
	buttonsEnable();

	//loop until send button is not pressed
	g_ucButtonsNotSendPress = 1;
	while(g_ucButtonsNotSendPress){
		displayUpdate();
		buttonsProcess();
	}

	//Disable buttons
	buttonsDisable();

	//send code request
	displaySetStatus(DISP_STAT_CODE_REQ);
	displayUpdate();

	while(1){

	}
	//update display status(error or request sent)


	//Step 2
	//update display to show Enter Access Code section

	//Enable buttons

	//loop until send or clear button is pressed

	//Disable buttons

	//send code request

	//update display status(error or request sent)

	//process data received
		//if access granted, then open solenoid
		//else just display access denied and go back to Enter Access Code screen

}
