//*****************************************************************************
//ESTABLISHING A CONNECTION WITH THE AD7616
//
//Pat LaChapelle
//Document created: October 23, 2020
//last edited: October 23, 2020
//
//AD7616_initial-interface.c
//
//The purpose of this project is to establish a connection between the
//TM4C1294XL Tiva C launchpad and the AD7616 ADC in parallel hardware mode
//
//*****************************************************************************
//Including required dependencies...
//
//This utilizes the code from the Uart interface example, and therefore has all
//associated dependencies. I've additionally added dependencies for GPIO
//configuration.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"



//****************************************************************************
//
// System clock rate in Hz.
//
//****************************************************************************
uint32_t g_ui32SysClock;

//
//variable for writing data
//
static volatile uint8_t g_ui8_bits0_7;
static volatile uint8_t g_ui8_bits8_15;



//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************

void
UARTIntHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the interrrupt status.
    //
    ui32Status = ROM_UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART0_BASE, ui32Status);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(ROM_UARTCharsAvail(UART0_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        ROM_UARTCharPutNonBlocking(UART0_BASE,
                                   ROM_UARTCharGetNonBlocking(UART0_BASE));

        //
        // Blink the LED to show a character transfer is occuring.
        //
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);

        //
        // Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks.
        //
        SysCtlDelay(g_ui32SysClock / (1000 * 3));

        //
        // Turn off the LED
        //
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);
    }
}

//
//Function for sending text to the UART
//
//*****************************************************************************
void
UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
    }
}

//*****************************************************************************
//

void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}

void
Delay(uint32_t ui32Seconds)
{
    //
    // Loop while there are more seconds to wait.
    //
    while(ui32Seconds--)
    {
        //
        // Wait until the SysTick value is less than 1000.
        //
        while(ROM_SysTickValueGet() > 1000)
        {
        }

        //
        // Wait until the SysTick value is greater than 1000.
        //
        while(ROM_SysTickValueGet() < 1000)
        {
        }
    }
}



int
main(void)
{

    //
    // Set the clocking to run directly from the crystal at 120MHz.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);

    ROM_SysTickPeriodSet(g_ui32SysClock);
    ROM_SysTickEnable();

    //
    // Enable the GPIO ports
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    //
    //Configure the GPIO pins
    //
    GPIOPadConfigSet(GPIO_PORTK_BASE, (GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7), GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet(GPIO_PORTM_BASE, (GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7), GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet(GPIO_PORTA_BASE, (GPIO_PIN_7),GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet(GPIO_PORTP_BASE, (GPIO_PIN_3|GPIO_PIN_5),GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet(GPIO_PORTQ_BASE, (GPIO_PIN_6),GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet(GPIO_PORTC_BASE, (GPIO_PIN_6),GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);


    //
    // Enable GPIO pins for interface
    //
    GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE, GPIO_PIN_3|GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTQ_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6);

    //
    //The GPIO pins will begin set to ground, as is their initial configuration
    //The CS and RD pins are set to default high, while RESET and CONVST are low
    //
    GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_1, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_PIN_6);

    //
    //Configure UART
    //
    ConfigureUART();

    //
    //begin a reset
    //
    GPIOPinWrite(GPIO_PORTP_BASE, (GPIO_PIN_3|GPIO_PIN_5), GPIO_PIN_3);
    Delay(2);
    GPIOPinWrite(GPIO_PORTP_BASE, (GPIO_PIN_3|GPIO_PIN_5), 0);
    Delay(2);
    GPIOPinWrite(GPIO_PORTP_BASE, (GPIO_PIN_3|GPIO_PIN_5), GPIO_PIN_3);
    Delay(2);

    //
    //DELAY FOR A BIT
    //
    Delay(1);

    g_ui8_bits0_7=GPIOPinRead(GPIO_PORTK_BASE,(GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7));
    g_ui8_bits8_15=GPIOPinRead(GPIO_PORTM_BASE,(GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7));


    //
    // Print the initial value to the UART.
    //
    UARTprintf("The initial value on the UART: ");
    UARTprintf("%x", g_ui8_bits0_7 * g_ui8_bits8_15, "\n");

    //
    //Begin a conversion
    //
    GPIOPinWrite(GPIO_PORTP_BASE, (GPIO_PIN_3|GPIO_PIN_5), (GPIO_PIN_3|GPIO_PIN_5));

    Delay(1);

    //
    //WAIT UNTIL THE CONVERSION IS COMPLETE
    //
    while(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7)> 0){
        Delay(1);
    }

    //
    //send CONVST low again
    //
    GPIOPinWrite(GPIO_PORTP_BASE, (GPIO_PIN_3|GPIO_PIN_5), (GPIO_PIN_3));

    //
    //SEND cs low to initiate the read
    //
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0);
    Delay(1);

    //
    //send rd low to begin the read operation
    //
    GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_1, 0);
    Delay(1);

    //
    //Send read back to high
    //
    GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_1, GPIO_PIN_1);
    Delay(1);

    //
    //The value after converting channel A
    //

    g_ui8_bits0_7=GPIOPinRead(GPIO_PORTK_BASE,(GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7));
    g_ui8_bits8_15=GPIOPinRead(GPIO_PORTM_BASE,(GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7));


    //
    // Print the conversion value to the UART.
    //
    UARTprintf("The value after the conversion: ");
    UARTprintf("%x", g_ui8_bits0_7 * g_ui8_bits8_15, "\n");


}
