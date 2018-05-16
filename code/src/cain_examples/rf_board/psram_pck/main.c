#include "ch.h"
#include "hal.h"
#include "test.h"
#include "chprintf.h"
#include "atmel_adc.h"
#include "pio.h"
#include "pmc.h"
#include "psram.h"
#include "board.h"

//---------------------------------------------------
//        FPGA CLOCK
// -------------------------------------------------

#define PLL_A            0           /* PLL A */
#define PLL_B            1           /* PLL B */

/** Pin PCK2 (PA31 Peripheral B) */
const Pin pinPCK[] = PIN_PCK2;


static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {
  (void)arg;
  while (TRUE) {
    palClearPad(IOPORT3, 17);
    chThdSleepMilliseconds(50);
    palSetPad(IOPORT3, 17);
    chThdSleepMilliseconds(50);
  }
  return(0);
}

/*
 * Application entry point.
 */
int main(void) {
    uint32_t i;
    uint32_t *ptr = (uint32_t *) PSRAM_BASE_ADDRESS;

    halInit();
    chSysInit();
    sdStart(&SD2, NULL);  /* Activates the serial driver 2 */

    // Configure PA31 as PCK2 (FPGA Clock)
    PIO_Configure(pinPCK, 1);
    PmcConfigurePllClock( PLL_A, (32 - 1), 1  ) ;
    /* If a new value for CSS field corresponds to PLL Clock, Program the PRES field first*/
    PmcMasterClockSelection( PMC_MCKR_CSS_MAIN_CLK, PMC_MCKR_PRES_CLK);
    /* Then program the CSS field. */
    PmcMasterClockSelection( PMC_MCKR_CSS_PLLA_CLK, PMC_MCKR_PRES_CLK ) ;
    ConfigurePck( PMC_PCK_CSS_PLLA_CLK, PMC_PCK_PRES_CLK ) ;
 
    /* Creates the blinker thread. */
    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    /* Configure EBI I/O for psram connection*/
    PIO_Configure(pinPsram, PIO_LISTSIZE(pinPsram));
    /* complete SMC configuration between PSRAM and SMC waveforms.*/
    BOARD_ConfigurePSRAM( SMC ) ;


  while (TRUE) {

    for (i = 0; i < 256; ++i) {
            ptr[i] =  i;
    }


    for (i = 0; i < 256; ++i) {
      chprintf((BaseChannel *)&SD2, ",%x \r\n", ptr[i] & 0xFF ) ;
    }

    chprintf( (BaseChannel *)&SD2, "123\n" ) ;
    chThdSleepMilliseconds(500);

  }



  return(0);
}
