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
//Para las operaciones matematicas vrgas
#include <math.h>

//---------------------------------------------------
//ACELEROMETRO
//Ratios de conversion
#define A_R 16384.0
#define G_R 131.0
//Conversion de radianes a grados 180/PI
#define RAD_A_DEG 57.295779
//---------------------------------------------------


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


/////////////////////


static WORKING_AREA(Area_acelero, 1024);

static msg_t Acelero_Thread(void *arg) {
  (void)arg;

    uint8_t MPUbuffer[6];
    int16_t AcX;
    int16_t AcY;
    int16_t AcZ;
    float Acc[2];
    int16_t GyX;
    int16_t GyY;
    float Gy[2];
    float Angle[2];
while(true){
    //La direccion del MPU es la 0x68
    //1) Despertar el MPU
	I2CWriteBytes(0x68,0x6B,0);
    //2) Pedir el registro 0x3B - corresponde al AcX
	I2CReadBytes(0x68,0x3B,MPUbuffer,6);
	AcX = (((uint16_t)MPUbuffer[0] <<8) | MPUbuffer[1]);
    AcY = (((uint16_t)MPUbuffer[2] <<8) | MPUbuffer[3]);
    AcZ = (((uint16_t)MPUbuffer[4] <<8) | MPUbuffer[5]);
	chprintf((BaseChannel *) &SD2, "Datos crudos:\r\n");
	chprintf((BaseChannel *) &SD2,"%d, %d, %d \r\n", AcX, AcY, AcZ);
    //3) A partir de los valores del acelerometro, se calculan 
    //  los angulos Y, X respectivamente, con la formula de la 
    //  tangente.
    Acc[1] = atan(-1*(AcX/A_R)/sqrt(pow((AcY/A_R),2) + pow((AcZ/A_R),2)))*RAD_A_DEG;
    Acc[0] = atan((AcY/A_R)/sqrt(pow((AcX/A_R),2) + pow((AcZ/A_R),2)))*RAD_A_DEG;
    chprintf((BaseChannel *) &SD2, "Datos procesados:\r\n");
    chprintf((BaseChannel *) &SD2,"%d, %d \r\n", Acc[1], Acc[0]);
    //4) Leer informacion del giroscopio
    I2CWriteBytes(0x68,0x6B,0);
    I2CReadBytes(0x68,0x43,MPUbuffer,4);
    GyX = (((uint16_t)MPUbuffer[0] <<8) | MPUbuffer[1]);
    GyY = (((uint16_t)MPUbuffer[2] <<8) | MPUbuffer[3]);
	//5) Calculo del angulo del Giroscopio
	Gy[0] = GyX/G_R;
	Gy[1] = GyY/G_R;
    //6) Aplicar el Filtro Complementario
    Angle[0] = 0.98 *(Angle[0]+Gy[0]*0.010) + 0.02*Acc[0];
    Angle[1] = 0.98 *(Angle[1]+Gy[1]*0.010) + 0.02*Acc[1];
    //7) Imprimir la informacion en pantalla
    chprintf((BaseChannel *) &SD2,"Angle X %d, Angle Y %d, \r\n", Angle[0], Angle[1]);
    //8) Nuestra dt sera, pues, 0.010, que es el intervalo de tiempo en cada medida
    chThdSleepMilliseconds(10);
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
  //chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(Area_acelero, sizeof(Area_acelero), NORMALPRIO, Acelero_Thread, NULL);
  //chThdCreateStatic(AreaMPU, sizeof(AreaMPU), NORMALPRIO, MPUThread, NULL);
  //chThdCreateStatic(AreaGPS, sizeof(AreaGPS), ABSPRIO, GPSThread, NULL);




  

  return(0);
}
