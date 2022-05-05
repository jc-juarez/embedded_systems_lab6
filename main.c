/*
 * main.c
 *
 * Created: 01/11/2017 08:47:35 p. m.
 *  Authors: Embedded Systems Team
 */ 

#include "sam.h"
#include "stdbool.h"
#include "myprintf.h"
#include "RTCControl.h"
#include <stdint.h>


void weird_delay() {
	uint64_t data = 50000000000000000000;
	for(uint64_t i = 0; i < (data * data * data * data * data * data); i++){
		for(uint64_t i = 0; i < (data * data * data * data * data * data); i++){
			for(uint64_t i = 0; i < (data * data * data * data); i++){
				for(uint64_t i = 0; i < (data * data * data * data * data * data); i++){
					for(uint64_t i = 0; i < (data * data * data * data * data * data); i++){
						uint64_t arg = data;
					}
				}
			}
		}
	}
}


#define BASE_ADDR 0x00
#define CLOCK_SLAVE_ADDR 0x68

typedef union {
	uint8_t reg;
	struct {
		uint8_t units_seconds : 4,
				dec_seconds : 3,
				reserved : 1;
	};
} seconds_type;

typedef union {
	uint8_t reg;
	struct {
		uint8_t units_minutes : 4,
				dec_minutes : 3,
				reserved : 1;
	};
} minutes_type;

typedef union {
	uint8_t reg;
	struct {
		uint8_t units_hours : 4,
				dec_hours : 2,
				hour_type : 1,
				reserved : 1;
	};
} hours_type;

typedef union {
	uint8_t reg;
	struct {
		uint8_t units_date : 4,
				dec_date : 2,
				reserved : 2;
	};
} date_type;

typedef union {
	uint8_t reg;
	struct {
		uint8_t units_months : 4,
				dec_months : 1,
				reserved : 3;
	};
} month_type;

typedef union {
	uint8_t reg;
	struct {
		uint8_t units_years : 4,
				dec_years : 4;
	};
} year_type;


void UARTInit(void);

int main(void)
{
	UARTInit();
	
	char buff[31]; buff[30]= '\0';
	/*
	mysnprintf(buff, sizeof buff, "testing %d %d %07d", 1, 2, 3);
	myprintf("\nBuff=%s",buff);
	mysnprintf(buff, sizeof buff, "faster %s %ccheaper%c", "and", 34, 34);
	myprintf("\nBuff=%s",buff);
	mysnprintf(buff, sizeof buff, "%x %% %X", 0xdeadf00d, 0xdeadf00d);
	myprintf("\nBuff=%s",buff);
	mysnprintf(buff, sizeof buff, "%09d%09d%09d%09d%09d", 1, 2, 3, 4, 5);
	myprintf("\nBuff=%s",buff);
	mysnprintf(buff, sizeof buff, "%d %u %d %u", 50, 50, -50, -50);
	myprintf("\nBuff=%s",buff);
	// Hello world entrance
	mysnprintf(buff, sizeof buff, "Hello world\n");
	myprintf("\nBuff=%s",buff);
	*/
	
	
	
	// Send data to starting from Base Address
	char ptrData[] = {BASE_ADDR, 0x00, 0x25, 0x06, 0x03, 0x04, 0x05, 0x22};
	sendI2CDataArray(CLOCK_SLAVE_ADDR, BASE_ADDR, ptrData, 8);
	StopCondition();
	char recData[8];
	while(1) {
		
		// Constant Data Fetch
		
		// Receive Data starting from Base Address
		receiveI2CDataArray(CLOCK_SLAVE_ADDR, BASE_ADDR, recData, 8);
		StopCondition();
		// Data Filtering
	
		seconds_type sec;
		minutes_type min;
		hours_type hours;
		date_type date;
		month_type months;
		year_type years;
	
		sec.reg = recData[0];
		min.reg = recData[1];
		hours.reg = recData[2];
		date.reg = recData[4];
		months.reg = recData[5];
		years.reg = recData[6];

		mysnprintf(buff, sizeof buff, "%d%d/%d%d/%d%d %d%d:%d%d:%d%d", date.dec_date, date.units_date, months.dec_months, months.units_months, years.dec_years, years.units_years, hours.dec_hours, hours.units_hours, min.dec_minutes, min.units_minutes, sec.dec_seconds, sec.units_seconds);
		myprintf("\nCurrent Time is: %s",buff);
		
		//weird_delay();
	}
	
	
	
	return 0;
}



// Init System
void UARTInit(void) {
	    	/* Initialize the SAM system */
    SystemInit();
	/* Switch to 8MHz clock (disable prescaler) */
	SYSCTRL->OSC8M.bit.PRESC = 0;
	
	/* port mux configuration*/
	PORT->Group[0].DIR.reg |= (1 << 10);                  /* Pin 10 configured as output */
	PORT->Group[0].PINCFG[PIN_PA11].bit.PMUXEN = 1;       /* Enabling peripheral functions */
	PORT->Group[0].PINCFG[PIN_PA10].bit.PMUXEN = 1;       /* Enabling peripheral functions */
	
	// Config for SDA and SCL
	PORT->Group[0].PINCFG[PIN_PA22].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN | PORT_PINCFG_PULLEN; /* SDA */
	PORT->Group[0].PINCFG[PIN_PA23].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN | PORT_PINCFG_PULLEN; /* SCL */
	
	/*PMUX: even = n/2, odd: (n-1)/2 */
	PORT->Group[0].PMUX[5].reg |= 0x02;                   /* Selecting peripheral function C */
	PORT->Group[0].PMUX[5].reg |= 0x20;                   /* Selecting peripheral function C */
	
	// Another found on I2C File
	/* PMUX: even = n/2, odd: (n-1)/2 */
	PORT->Group[0].PMUX[11].reg |= 0x02u;
	PORT->Group[0].PMUX[11].reg |= 0x20u;
	
	/* APBCMASK */
	//PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;			  /* SERCOM 0 enable*/
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;

	/* APBCMASK */
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM3;

	/*GCLK configuration for sercom0 module: using generic clock generator 0, ID for sercom0, enable GCLK*/
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM0_GCLK_ID_CORE) |
	GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);

	// Config for SERCOM3
	/*GCLK configuration for sercom3 module*/
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID (SERCOM3_GCLK_ID_CORE) |
	GCLK_CLKCTRL_ID (SERCOM3_GCLK_ID_SLOW) |
	GCLK_CLKCTRL_GEN(4) |
	GCLK_CLKCTRL_CLKEN;
	GCLK->GENCTRL.reg |= GCLK_GENCTRL_SRC_OSC8M|GCLK_GENCTRL_GENEN|GCLK_GENCTRL_ID(4);
	
	// Final Config on SERCOM3
	/* set configuration for SERCOM3 I2C module */
	SERCOM3->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN; /* smart mode enable */
	while (SERCOM3->I2CM.SYNCBUSY.reg); // waiting loading
	
	/* calculate BAUDRATE */
	uint64_t tmp_baud =((8000000/100000)-10-(8000000*250 /1000000000))/2;
	SERCOM3->I2CM.BAUD.bit.BAUD = SERCOM_I2CM_BAUD_BAUD((uint32_t)tmp_baud);
	while (SERCOM3->I2CM.SYNCBUSY.reg); // waiting loading
	// value equals 0x22 or decimal 34
	
	SERCOM3->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_ENABLE   |/* enable module */
	SERCOM_I2CM_CTRLA_MODE_I2C_MASTER |		/* i2c master mode */
	SERCOM_I2CM_CTRLA_SDAHOLD(3);		 /* SDA hold time to 600ns */
	while (SERCOM3->I2CM.SYNCBUSY.reg);  /* waiting loading */

	SERCOM3->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSSTATE(1); /* set to idle state */
	while (SERCOM3->I2CM.SYNCBUSY.reg);  /* waiting loading */
	 
	 // Final Config ends here 
	
	/* configure SERCOM0 module for UART as Standard Frame, 8 Bit size, No parity, BAUDRATE:9600*/

	SERCOM0->USART.CTRLA.reg =
	SERCOM_USART_CTRLA_DORD | SERCOM_USART_CTRLA_MODE_USART_INT_CLK |
	SERCOM_USART_CTRLA_RXPO(3/*PAD3*/) | SERCOM_USART_CTRLA_TXPO(1/*PAD2*/);
	
	uint64_t br = (uint64_t)65536 * (8000000 - 16 * 9600) / 8000000;
	
	SERCOM0->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0/*8 bits*/);

	SERCOM0->USART.BAUD.reg = (uint16_t)br;

	SERCOM0->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
}
