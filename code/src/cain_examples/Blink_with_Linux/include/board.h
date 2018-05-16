/*******************************************************************************
* File Name          : board.h
* Author             : Carlos Camargo
* Date First Issued  : 22/11/2012
* Description        : This file provides all the GPIO functions.
********************************************************************************/


/* IMX233 */
#define IMX283_BASE_SYS	(0x80018000)

/* GPIO assignment (MUX_REG)*/


#define PIO_CTRL	(0)		/* PIO enable */
#define PIO_MUXSEL	(0x100)		/* output enable */
#define PIO_DRIVE	(0x300)		/* output disable */
#define PIO_PULL	(0x600)		/* set output data */
#define PIO_DOUT	(0x700)		/* clear output data */
#define PIO_DIN 	(0x900)		/* pin data status */
#define PIO_DOE 	(0xB00)		/* pull-up enable */


#define SET             (0x4)
#define CLR             (0x8)
#define TOG             (0xC)

#define PIO0	(0*4)  //0x00
#define PIO1	(4*4)  //0x10
#define PIO2	(8*4)  //0x20
#define PIO3	(12*4) //0x30

#define NC	(0)			/* not connected */
#define P0	(1 << 0)
#define P1	(1 << 1)
#define P2	(1 << 2)
#define P3	(1 << 3)
#define P4	(1 << 4)
#define P5	(1 << 5)
#define P6	(1 << 6)
#define P7	(1 << 7)
#define P8	(1 << 8)
#define P9	(1 << 9)
#define P10	(1 << 10)
#define P11	(1 << 11)
#define P12	(1 << 12)
#define P13	(1 << 13)
#define P14	(1 << 14)
#define P15	(1 << 15)
#define P16	(1 << 16)
#define P17	(1 << 17)
#define P18	(1 << 18)
#define P19	(1 << 19)
#define P20	(1 << 20)
#define P21	(1 << 21)
#define P22	(1 << 22)
#define P23	(1 << 23)
#define P24	(1 << 24)
#define P25	(1 << 25)
#define P26	(1 << 26)
#define P27	(1 << 27)
#define P28	(1 << 28)
#define P29	(1 << 29)
#define P30	(1 << 30)
#define P31	(1 << 31)


struct device_t
{
	char* name;

	int          LED_PIO;	
	unsigned int LED_PIN;
	unsigned int LED_MASK;	

};

/*
GPMI_D07       LED         BANK0 | PIN7  | SELECT 00 | MUXREG 14-15
*/


static struct device_t devices[] =
{
    { "stamp", PIO0, 7, P7},
    { .name = 0 },
};





