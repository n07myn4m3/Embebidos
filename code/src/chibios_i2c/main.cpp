/*
PRUEBA LECTURA I2C
 */

#include "ch.h"
#include "hal.h"
#include "test.h"
#include "chprintf.h"
#include "atmel_adc.h"
#include "pio.h"
#include "pmc.h"
#include "board.h"

#include "./i2c.h"

creator::I2C i2c;

// BLINK PARA REVISAR QUE EL CODIGO ES FUNCIONAL

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
  // Initialize ChibiOS HAL
  halInit();
  chSysInit();
  i2c.Init();
 
  //Start Serial.
  sdStart(&SD2, NULL);
  
  /* Creates the blinker thread. */
   chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  while(true) {
    chprintf((BaseChannel *) &SD2, "kill me\r\n");
    chThdSleepMilliseconds(1000);
  	i2c.WriteByte(0x01, 0x50);
  }


}


