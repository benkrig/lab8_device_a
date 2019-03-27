#include "lab8_device_a.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/uart.h"

volatile unsigned long count = 0;

void PortFunctionInit(void) {
    //
    // Enable Peripheral Clocks 
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Enable pin PF4 for GPIOInput
    //
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);

    //
    // Enable pin PF0 for GPIOInput
    //

    //
    //First open the lock and select the bits we want to modify in the GPIO commit register.
    //
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;

    //
    //Now modify the configuration of the pins that we unlocked.
    //
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
	
		//Enable pull-up on PF4 and PF0
		GPIO_PORTF_PUR_R |= 0x11; 
}

void InterruptInit(void) {
	IntEnable(INT_GPIOF);
	IntPrioritySet(INT_GPIOF, 0x00);
	GPIO_PORTF_IM_R |= 0x11;
	GPIO_PORTF_IS_R &= ~0x11;
	GPIO_PORTF_IBE_R &= ~0x11;
	GPIO_PORTF_IEV_R &= ~0x11;
}

void UARTInit(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

  GPIOPinConfigure(GPIO_PB1_U1TX);
  GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_1);


  UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}

void GPIOPortF_Handler(void)
{
	char str[3];
	//SW1 is pressed
	if(GPIO_PORTF_RIS_R&0x10)
	{
		// acknowledge flag for PF4
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4); 
		//counter imcremented by 1
		count++;
		sprintf(str, "%lu", count);
		UARTCharPut(UART1_BASE, str[0]); 
		UARTCharPut(UART1_BASE, str[1]); 
		UARTCharPut(UART1_BASE, str[2]); 
		UARTCharPut(UART1_BASE, '\n'); 
		UARTCharPut(UART1_BASE, '\r'); 
	}
	
	//SW2 is pressed
  if(GPIO_PORTF_RIS_R&0x01)
	{
		// acknowledge flag for PF0
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
		//counter imcremented by 1
		count++;
		sprintf(str, "%lu", count);
		UARTCharPut(UART1_BASE, str[0]); 
		UARTCharPut(UART1_BASE, str[1]); 
		UARTCharPut(UART1_BASE, str[2]); 
		UARTCharPut(UART1_BASE, '\n'); 
		UARTCharPut(UART1_BASE, '\r');
	}
}


int main(void) {
	PortFunctionInit();
	
	InterruptInit();
	
	UARTInit();
	
	IntMasterEnable();
	
	while(1) {}
}
