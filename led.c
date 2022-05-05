#include "sam.h"
/*#include "gclk.h"
#include "pm.h"
#include "port.h"
#include "tc.h"*/

#define PINCFG_CONFIG_VALUE 0b00000000

int main(void) {
	
	/* Initialize the SAM system */
	SystemInit();
	
	//PORT configuration for general-purpose PIN
	PORT->Group[0].PINCFG[17].reg = PINCFG_CONFIG_VALUE;
	PORT->Group[0].DIRSET.reg = PORT_PA17;
	//TIMER configuration
	//Configure the POWER MANAGER to enable the TC3 module
	PM->APBCMASK.reg |= PM_APBCMASK_TC3;
	//Configure the GENERIC CLOCK CONTROL used by the TC3 module
	GCLK->GENDIV.reg = 0;
	GCLK->GENCTRL.reg = 0x10600;
	GCLK->CLKCTRL.reg = 0x401B;
	//Determine the initial count
	//Time = Toscillator * (2^TIMERMODE - COUNT) * Prescaler
	TC3->COUNT16.COUNT.reg = 65307;
	//Configure the TC3 peripheral
	TC3->COUNT16.CTRLA.reg = 0x0700;
	TC3->COUNT16.CTRLBSET.reg = 0;
	TC3->COUNT8.INTFLAG.reg = 0x01;
	//Enabling the TC3 peripheral
	TC3->COUNT16.CTRLA.reg |= 0x2;
	//Loop to sync the TC clock with the processor/main clock
	do
	{
	} while (TC3->COUNT16.STATUS.bit.SYNCBUSY);
	
	while (1){
		if (TC3->COUNT16.INTFLAG.bit.OVF == 1){
			PORT->Group[0].OUTTGL.reg = (1<<17);
			TC3->COUNT16.INTFLAG.reg = 0x01;
			TC3->COUNT16.COUNT.reg = 65307;
		}
	}
	
}
