/**
    Purpose: Sets the main logic of IEDD project

    @author Alexis Cuero Losada
    @version 1.1 18/01/10
*/

#define _GLIBCXX_USE_CXX11_ABI 0

#include "board.h"
#include "ch.h"
#include "chprintf.h"
#include "hal.h"
#include "pio.h"
#include "pmc.h"

#include "psram.h"
#include "soc-hw.h"

//---------------------------------------------------
//        FPGA CLOCK
// -------------------------------------------------
#define PLL_A            0           /* PLL A */

/** Pin PCK2 (PA31 Peripheral B) */
const Pin pinPCK[] = PIN_PCK2;

static WORKING_AREA(waThread1, 128);

static msg_t Thread1(void *arg) {
  (void)arg;

  while (TRUE) {
    palClearPad(IOPORT3, BOARD_LED);
    chThdSleepMilliseconds(50);
    palSetPad(IOPORT3, BOARD_LED);
    chThdSleepMilliseconds(50);
  }

  return(0);
}

/*
* Application entry point.
*/
int main(void) {
  // Initialize ChibiOS HAL
  halInit();
  chSysInit();

  //Start Serial.
  sdStart(&SD2, NULL);

  // Configure PA31 as PCK2 (FPGA Clock)
  PIO_Configure(pinPCK, 1);

  PmcConfigurePllClock(PLL_A, (32 - 1), 1) ;

  /* If a new value for CSS field corresponds to PLL Clock, Program the PRES field first*/
  PmcMasterClockSelection( PMC_MCKR_CSS_MAIN_CLK, PMC_MCKR_PRES_CLK );
  /* Then program the CSS field. */
  PmcMasterClockSelection( PMC_MCKR_CSS_PLLA_CLK, PMC_MCKR_PRES_CLK );
  //ConfigurePck(PMC_PCK_CSS_PLLA_CLK, PMC_PCK_PRES_CLK_2);
  ConfigurePck( PMC_PCK_CSS_PLLA_CLK, PMC_PCK_PRES_CLK_2 );

  /* Configure EBI I/O for psram connection*/
  PIO_Configure(pinPsram, PIO_LISTSIZE(pinPsram));
  /* Complete SMC configuration between PSRAM and SMC waveforms.*/
  BOARD_ConfigurePSRAM(SMC) ;

  // Creates blink thread
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);


  while(true) {
    chprintf((BaseChannel *) &SD2, "\e[1;1H\e[2J");
    chprintf((BaseChannel *) &SD2, "** Start **\r\n");
    write_data1(0x13);
    write_data2(0x24);
    write_data3(0x57);
    write_data4(0x90);
    
    chprintf((BaseChannel *) &SD2, "Write operation complete.\r\n");
    
    chprintf((BaseChannel *) &SD2, "Register 1 = 0x%x\r\n", read_data1());
    chprintf((BaseChannel *) &SD2, "Register 2 = 0x%x\r\n", read_data2());
    chprintf((BaseChannel *) &SD2, "Register 3 = 0x%x\r\n", read_data3());
    chprintf((BaseChannel *) &SD2, "Register 4 = 0x%x\r\n", read_data4());
    
    chprintf((BaseChannel *) &SD2, "Reseting registers.\r\n");
    
    write_zeros();
    
    chprintf((BaseChannel *) &SD2, "Register 1 = 0x%x\r\n", read_data1());
    chprintf((BaseChannel *) &SD2, "Register 2 = 0x%x\r\n", read_data2());
    chprintf((BaseChannel *) &SD2, "Register 3 = 0x%x\r\n", read_data3());
    chprintf((BaseChannel *) &SD2, "Register 4 = 0x%x\r\n", read_data4());
    
    chprintf((BaseChannel *) &SD2, "** End **\r\n");
    
    chThdSleepMilliseconds(100);
  }
  return(0);
}
