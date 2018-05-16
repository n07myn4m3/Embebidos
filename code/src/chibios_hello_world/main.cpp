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

static WORKING_AREA(waThread1, 128);

static msg_t Thread1(void *arg) {
  (void)arg;

  while (TRUE) {
    palClearPad(IOPORT3, BOARD_LED);
    chThdSleepMilliseconds(200);
    palSetPad(IOPORT3, BOARD_LED);
    chThdSleepMilliseconds(200);
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
  
  while(true) {
    chprintf((BaseChannel *) &SD2, "Hello World\r\n");
    chThdSleepMilliseconds(100);
  }

  // Creates blink thread
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  return(0);
}
