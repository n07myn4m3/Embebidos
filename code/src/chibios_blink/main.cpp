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

//---------------------------------------------------
//        FPGA CLOCK
// -------------------------------------------------
#define PLL_A            0           /* PLL A */

/** Pin PCK2 (PA31 Peripheral B) */
const Pin pinPCK[] = PIN_PCK2;

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
  PIO_Configure(pinPCK, 1); //puede ser psk, gpio, etc

  PmcConfigurePllClock(PLL_A, (32 - 1), 1) ; //Configurar la frecuencia del PLL
  //COnfigurar los multiplexores
  /* If a new value for CSS field corresponds to PLL Clock, Program the PRES field first*/
  PmcMasterClockSelection( PMC_MCKR_CSS_MAIN_CLK, PMC_MCKR_PRES_CLK ); 
  /* Then program the CSS field. */
  PmcMasterClockSelection( PMC_MCKR_CSS_PLLA_CLK, PMC_MCKR_PRES_CLK );

  ConfigurePck( PMC_PCK_CSS_PLLA_CLK, PMC_PCK_PRES_CLK_2 ); //Dos multiplexores y un prescaler

  /* Configure EBI I/O for psram connection*/
  PIO_Configure(pinPsram, PIO_LISTSIZE(pinPsram)); //revisar aqui se definen todos los pines que pueden usarse por la fpga
  /* Complete SMC configuration between PSRAM and SMC waveforms.*/
  BOARD_ConfigurePSRAM(SMC) ; //cOnfigura un espacio de memoria externo, datos, tiempo de subida y bajada, etc por el momento no se
 //usa pero esta configurado

  while (TRUE) {
    chprintf((BaseChannel *) &SD2, "Hello World.\r\n");
    palClearPad(IOPORT3, BOARD_LED);
    chThdSleepMilliseconds(5000);
    palSetPad(IOPORT3, BOARD_LED);
    chThdSleepMilliseconds(5000);
  }

  return(0);
}
