// Watchdog.cpp

#include "pch.h"


#ifdef ARDUINO

#ifdef EXCLUDE_THIS

#include "arduino.h"

#include <samd.h>

// https://forum.arduino.cc/index.php?topic=472108.0
// I needed to add these or it would not compile.
// Each cycle is about 1ms.
#define WDT_CONFIG_PER_3    0x3u    // 64 clock cycles
#define WDT_CONFIG_PER_4    0x4u    // 128 clock cycles
#define WDT_CONFIG_PER_5    0x5u    // 256 clock cycles
#define WDT_CONFIG_PER_6    0x6u    // 512 clock cycles
#define WDT_CONFIG_PER_7    0x7u    // 1024 clock cycles
#define WDT_CONFIG_PER_8    0x8u    // 2048 clock cycles
#define WDT_CONFIG_PER_9    0x9u    // 4096 clock cycles
#define WDT_CONFIG_PER_10   0xAu    // 8192 clock cycles
#define WDT_CONFIG_PER_11   0xBu    // 16384 clock cycles


void watchdog_setup(void)
{
	// Set up the generic clock (GCLK2) used to clock the watchdog timer at 1.024kHz
	REG_GCLK_GENDIV = GCLK_GENDIV_DIV(4) |				// Divide the 32.768kHz clock source by divisor 32, where 2^(4 + 1): 32.768kHz/32=1.024kHz
		GCLK_GENDIV_ID(2);								// Select Generic Clock (GCLK) 2
	while (GCLK->STATUS.bit.SYNCBUSY);					// Wait for synchronization

	REG_GCLK_GENCTRL = GCLK_GENCTRL_DIVSEL |			// Set to divide by 2^(GCLK_GENDIV_DIV(4) + 1)
		GCLK_GENCTRL_IDC |								// Set the duty cycle to 50/50 HIGH/LOW
		GCLK_GENCTRL_GENEN |							// Enable GCLK2
		GCLK_GENCTRL_SRC_OSCULP32K |					// Set the clock source to the ultra low power oscillator (OSCULP32K)
		GCLK_GENCTRL_ID(2);								// Select GCLK2         
	while (GCLK->STATUS.bit.SYNCBUSY);					// Wait for synchronization

	// Feed GCLK2 to WDT (Watchdog Timer)
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |				// Enable GCLK2 to the WDT
		GCLK_CLKCTRL_GEN_GCLK2 |						// Select GCLK2
		GCLK_CLKCTRL_ID_WDT;							// Feed the GCLK2 to the WDT
	while (GCLK->STATUS.bit.SYNCBUSY);					// Wait for synchronization

	REG_WDT_CONFIG = WDT_CONFIG_PER_3;					// Set the WDT reset timeout to 64 ms
	while (WDT->STATUS.bit.SYNCBUSY);					// Wait for synchronization

	REG_WDT_CTRL = WDT_CTRL_ENABLE;						// Enable the WDT in normal mode
	while (WDT->STATUS.bit.SYNCBUSY);					// Wait for synchronization
}


void watchdog_clear(void)
{
	if (!WDT->STATUS.bit.SYNCBUSY)						// Check if the WDT registers are synchronized
	{
		REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;			// Clear the watchdog timer
	}
}

#endif
#endif
