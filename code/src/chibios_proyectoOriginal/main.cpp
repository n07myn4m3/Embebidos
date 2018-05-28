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
#include "string.h"
#include "atmel_twid.h"
#include "pio.h"
#include "atmel_twi.h"


Twid twid_;
/** TWI clock frequency in Hz. */
#define TWCK 400000

#define BOARD_MCK 64000000

static const Pin pins[] = {PIN_TWD0, PIN_TWCK0};


void I2CInit() {
  /* Configure TWI */
  chSysLock();
  PIO_Configure(pins, PIO_LISTSIZE(pins));
  PMC->PMC_WPMR = 0x504D4300; /* Disable write protect */
  PMC->PMC_PCER0 = 1 << ID_TWI0;
  PMC->PMC_WPMR = 0x504D4301; /* Enable write protect */
  TWI_ConfigureMaster(TWI0, TWCK, BOARD_MCK);
  TWID_Initialize(&twid_, TWI0);
  chSysUnlock();
}

uint8_t I2CReadByte_sub(uint8_t address, uint8_t subAddress) {
  // TODO (andres.calderon): Handle timeouts, handle errors
  chSysLock();
  uint8_t ret;
  uint8_t data;

  if (TWID_Read(&twid_, address, subAddress, 1, &data, 1, 0) == 0) ret = data;
  chSysUnlock();
  return ret;
}

void I2CWriteByte(uint8_t address, uint8_t data) {
  // TODO (andres.calderon): Handle timeouts, handle errors
  chSysLock();
  TWID_Write(&twid_, address, 0, 0, &data, 1, 0);
  chSysUnlock();
}

uint8_t I2CReadByte(uint8_t address) {
  // TODO (andres.calderon): Handle timeouts, handle errors
  chSysLock();
  uint8_t ret;
  uint8_t data;

  if (TWID_Read(&twid_, address, 0, 0, &data, 1, 0) == 0) ret = data;
  chSysUnlock();
  return ret;
}

void I2CWriteBytes(uint8_t address, uint8_t subAddress, uint8_t data) {
  // TODO (andres.calderon): Handle timeouts, handle errors
  chSysLock();
  TWID_Write(&twid_, address, subAddress, 1, &data, 1, 0);
  chSysUnlock();
}


uint8_t I2CReadBytes(uint8_t address, uint8_t subAddress, uint8_t* dest,
                       uint8_t count) {
  chSysLock();
  uint8_t ret = 0;
  // TODO (andres.calderon): Handle timeouts, handle errors
  if (TWID_Read(&twid_, address, subAddress, 1, dest, count, 0) == 0) {
    ret = count;
  }
  chSysUnlock();
  return ret;
}

static WORKING_AREA(waThread1, 128);

static msg_t Thread1(void *arg) {
  (void)arg;

  while (TRUE) {
    palClearPad(IOPORT3, BOARD_LED);
    chThdSleepMilliseconds(1000);
    palSetPad(IOPORT3, BOARD_LED);
    chThdSleepMilliseconds(1000);
  }

  return(0);
}


/*
static WORKING_AREA(AreaGPS, 1024);

static msg_t GPSThread(void *arg) {
  (void)arg;

    char UartBuffer[100];
    uint32_t UartBufferPtr = 0;
    char *GPGGA[16];
    char *GPRMC[16];
    uint8_t MPUbuffer[2];
    int16_t x;
    while(true) {
      int i=0;
      int j=0;
      int MPUStatus;
      char byte_u8 = sdGet(&SD1);
        if (UartBufferPtr >= 100){
          UartBufferPtr = 0;
        }
        if (byte_u8 == '\r' || byte_u8 == '\n'){
          UartBuffer[UartBufferPtr++] = '\0';
          UartBufferPtr = 0;
          char *GPRMC_c = strstr(UartBuffer, "$GPRMC");
          if (GPRMC_c != NULL){
            char *pt2;
            pt2 = strtok (UartBuffer,",");
            while (pt2 != NULL) {
              GPRMC[j]=pt2;
              pt2 = strtok (NULL, ",");
              j++;
            }
            I2CWriteBytes(0x68,0x6B,0); //Wake up MPU
            I2CReadBytes(0x68,0x3B,MPUbuffer,2);
            x = (((uint16_t)MPUbuffer[0] <<8) | MPUbuffer[1]);
            if(x>30000 || x<-30000){
              MPUStatus = 1;
            }else{
              MPUStatus = 0;
            }
            chprintf((BaseChannel *) &SD2, "LAT=");
            chprintf((BaseChannel *) &SD2, GPRMC[3]);
            chprintf((BaseChannel *) &SD2, ";");
            chprintf((BaseChannel *) &SD2, "CLAT=");
            chprintf((BaseChannel *) &SD2, GPRMC[4]);
            chprintf((BaseChannel *) &SD2, ";");
            chprintf((BaseChannel *) &SD2, "LON=");
            chprintf((BaseChannel *) &SD2, GPRMC[5]);
            chprintf((BaseChannel *) &SD2, ";");
            chprintf((BaseChannel *) &SD2, "CLON=");
            chprintf((BaseChannel *) &SD2, GPRMC[6]);
            chprintf((BaseChannel *) &SD2, ";");
            chprintf((BaseChannel *) &SD2, "SP=");
            chprintf((BaseChannel *) &SD2, GPRMC[7]);
            chprintf((BaseChannel *) &SD2, ";");
            chprintf((BaseChannel *) &SD2, "VAL=");
            chprintf((BaseChannel *) &SD2, GPRMC[2]);
            chprintf((BaseChannel *) &SD2, ";");
            chprintf((BaseChannel *) &SD2, "MPU=");
            chprintf((BaseChannel *)&SD2,"%0d", MPUStatus);
            chprintf((BaseChannel *) &SD2, "\r\n");
          }
        }//if 126
        else if (byte_u8 >= ' ' && byte_u8 <= '~'){
          UartBuffer[UartBufferPtr++] = byte_u8;
        }
      }//while
      return(0);
}
*/



/////////////////////


static WORKING_AREA(Area_acelero, 1024);

static msg_t Acelero_Thread(void *arg) {
  (void)arg;

//by mc
    uint8_t MPUbuffer[2];
    int16_t x;
while(true){
    //La direccion del MPU es la 0x68
    //1) Despertar el MPU
	I2CWriteBytes(0x68,0x6B,0);
    //2) Pedir el registro 0x3B - corresponde al AcX
	I2CReadBytes(0x68,0x3B,MPUbuffer,2);
	x = (((uint16_t)MPUbuffer[0] <<8) | MPUbuffer[1]);
	chprintf((BaseChannel *) &SD2, "aqui:\r\n");
	chprintf((BaseChannel *) &SD2,"%d \r\n", x);
}
return (0);
}



/////////////////////////

/*
* Application entry point.
*/
int main(void) {
  // Initialize ChibiOS HAL
  halInit();

  chSysInit();
  sdStart(&SD2, NULL);  /* Activates the serial driver 2 */

  I2CInit();

  // Creates threads
 // chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(Area_acelero, sizeof(Area_acelero), NORMALPRIO, Acelero_Thread, NULL);
  //chThdCreateStatic(AreaMPU, sizeof(AreaMPU), NORMALPRIO, MPUThread, NULL);
  //chThdCreateStatic(AreaGPS, sizeof(AreaGPS), ABSPRIO, GPSThread, NULL);

 /* while(true) {
	I2CWriteBytes(0x68,0x6B,0);
    chprintf((BaseChannel *) &SD2, "aqui\r\n");
    chThdSleepMilliseconds(100);
  }
*/


  

  return(0);
}
