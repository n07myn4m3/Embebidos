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
// librerias del adc
#include "test.h"
#include "atmel_adc.h"
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


/////////////////////////////////////////////////////////////////// ACELEROMETRO

//  CONFIGURATION   //
//Change this 3 variables if you want to fine tune the skecth to your needs.
int16_t buffersize=1000;     //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
int16_t acel_deadzone=8;     //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)
int16_t giro_deadzone=1;     //Giro error allowed, make it lower to get more precision, but sketch may not converge  (default:1)

//#define MPU 0x68
int16_t ax, ay, az,gx, gy, gz;
int     mean_ax,mean_ay,mean_az,mean_gx,mean_gy,mean_gz,state=0;
int16_t ax_offset,ay_offset,az_offset,gx_offset,gy_offset,gz_offset;
int16_t cero=0;
char    validacion=0;

//Es necesario considerar las variables como globales.

    //Para el acelerometro
    int16_t AcX;
    int16_t AcY;
    int16_t AcZ;
    //int Acc[2];
    int16_t GyX;
    int16_t GyY;
    //int Gy[2];
    //int Angle[2];

	//Para el ADC
	#define PLL_A            0           /* PLL A */
	#define PLL_B            1           /* PLL B */

	int ADC_Val;


Twid twid_;
/** TWI clock frequency in Hz. */
#define TWCK 400000

#define BOARD_MCK 64000000

static const Pin pins[] = {PIN_TWD0, PIN_TWCK0};


int abs(int valor ){

if (valor>=0) valor=valor;
else valor=-1*valor;
return valor;
}


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


//////////////////////////////////////////////////////////////////////////para escribir mas datos
//code/ext/ChibiOS/os/hal/platforms/AT91SAM3S/

/**   TWID_WRITE()
 * \brief Asynchronously sends data to a slave on the TWI bus. An optional callback
 * function is invoked whenever the transfer is complete.
 * \param pTwid  Pointer to a Twid instance.
 * \param address  TWI slave address.
 * \param iaddress  Optional slave internal address.
 * \param isize  Number of internal address bytes.
 * \param pData  Data buffer for storing received bytes.
 * \param num  Data buffer to send.
 * \param pAsync  Asynchronous transfer descriptor.
 * \return 0 if the transfer has been started; otherwise returns a TWI error code.
 */


void I2CdevwriteWords(uint8_t address, uint8_t subAddress, uint16_t data, uint32_t num){ //num=tamaño byte 1=1byte 2=2byte ....4=4byte


	uint8_t mpu_txbuf[num], i;
	for(i=0;i<num; i += 2)
	{
                mpu_txbuf[i] = (uint8_t)(data >> 8)& 0xff;  //MSB
//		mpu_txbuf[i] = uint8_t(data[i] >> 8) & 0xff;  //MSB
		mpu_txbuf[i+1] = (uint8_t)(data) & 0xff; 	//LSB
	}


  chSysLock();
  //TWID_Write(&twid_, address, subAddress, 1, &data, num, 0);
  TWID_Write(&twid_, address, subAddress, 1, mpu_txbuf, num, 0); //como es un arreglo no tengo que ponerle el ampersan &mpu...
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



/*   nuevo de esta pagina para calibrar el sensor  https://github.com/jevermeister/MPU6050-ChibiOS/tree/devel/MPU6050
*/


/*
bool_t I2CdevwriteBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
	  //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b = 0;
    //if (I2CReadBytes(devAddr, regAddr, &b, I2CDEV_DEFAULT_READ_TIMEOUT) != 0) {
    if (I2CReadBytes(devAddr, regAddr, &b, 1) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
        I2CWriteBytes(devAddr, regAddr, b);
    } else {
        return FALSE;
    }
}



void MPUsetClockSource(uint8_t source) {
    //I2CdevwriteBits(MPUdevAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);//0x68,0x6B,2,3,0x01
    I2CdevwriteBits(0x68,0x6B,2,3,source);//0x68,0x6B,2,3,0x01
}

void MPUsetFullScaleGyroRange(uint8_t range) {
   // I2CdevwriteBits(MPUdevAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);//0x68,0x1B,4,2,0x00
    I2CdevwriteBits(0x68,0x1B,4,2,range);
}

void MPUsetFullScaleAccelRange(uint8_t range) {
    //I2CdevwriteBits(MPUdevAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);//0x68,0x1C,4,2,0x00
    I2CdevwriteBits(0x68,0x1C,4,2,range);
}
*/
/*
void MPUsetSleepEnabled(bool_t enabled) {
    I2CdevwriteBit(MPUdevAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled); //0x68,0x6B,6,false
}
*/


void MPUinitialize() {
    //MPUsetClockSource(MPU6050_CLOCK_PLL_XGYRO); //0x01
 //MPUsetClockSource(0x01); //0x01
    //MPUsetFullScaleGyroRange(MPU6050_GYRO_FS_250);//0x00
    //MPUsetFullScaleAccelRange(MPU6050_ACCEL_FS_2);//0x00
 //MPUsetFullScaleGyroRange(0x00);//0x00
 //MPUsetFullScaleAccelRange(0x00);//0x00
    //MPUsetSleepEnabled(FALSE); // thanks to Jack Elston for pointing this one out! 

        I2CWriteBytes(0x68, 0x6B, 0x01);    
        I2CWriteBytes(0x68, 0x1B, 0x00);   
        I2CWriteBytes(0x68, 0x1C, 0x00);   
        I2CWriteBytes(0x68, 0x6B, 0);    
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

//---------------------------------------------------------------

void datosmpu(){
    uint8_t MPUbuffer[6];
    //int16_t AcX;
    //int16_t AcY;
    //int16_t AcZ;
    //int16_t GyX;
    //int16_t GyY;



    //2) Pedir el registro 0x3B - corresponde al AcX
	I2CReadBytes(0x68,0x3B,MPUbuffer,6);
	AcX = (((uint16_t)MPUbuffer[0] <<8) | MPUbuffer[1]);
    AcY = (((uint16_t)MPUbuffer[2] <<8) | MPUbuffer[3]);
    AcZ = (((uint16_t)MPUbuffer[4] <<8) | MPUbuffer[5]);

    //3) Leer informacion del giroscopio
    I2CWriteBytes(0x68,0x6B,0);
    I2CReadBytes(0x68,0x43,MPUbuffer,4);
    GyX = (((uint16_t)MPUbuffer[0] <<8) | MPUbuffer[1]);
    GyY = (((uint16_t)MPUbuffer[2] <<8) | MPUbuffer[3]);
		

	if (validacion==1)AcZ=AcZ-8192;
    //PARA VERIFICAR EL FUNCIONAMIENTO DEL CODIGO
	//chprintf((BaseChannel *) &SD2, "%d, %d, %d, %d, %d \r\n",AcX, AcY, AcZ, GyX, GyY);
	
	chThdSleepMilliseconds(2);

}

//---------------------------------------------------------------




void datos1(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
    uint8_t MPUbuffer[14];
    I2CReadBytes(0x68,0x3B,MPUbuffer,14);
    *ax = (((int16_t)MPUbuffer[0]) << 8) | MPUbuffer[1];
    *ay = (((int16_t)MPUbuffer[2]) << 8) | MPUbuffer[3];
    *az = (((int16_t)MPUbuffer[4]) << 8) | MPUbuffer[5]; // el 6 y 7 no dr toman porque son los de la temperatura
    *gx = (((int16_t)MPUbuffer[8]) << 8) | MPUbuffer[9];
    *gy = (((int16_t)MPUbuffer[10]) << 8) | MPUbuffer[11];
    *gz = (((int16_t)MPUbuffer[12]) << 8) | MPUbuffer[13];
}

void meansensors(){
  long i=0,buff_ax=0,buff_ay=0,buff_az=0,buff_gx=0,buff_gy=0,buff_gz=0;

  while (i<(buffersize+101)){
    // read raw accel/gyro measurements from device
    datos1(&ax, &ay, &az, &gx, &gy, &gz);
    
    if (i>100 && i<=(buffersize+100)){ //First 100 measures are discarded
      buff_ax=buff_ax+ax;
      buff_ay=buff_ay+ay;
      buff_az=buff_az+az;
      buff_gx=buff_gx+gx;
      buff_gy=buff_gy+gy;
      buff_gz=buff_gz+gz;
    }
    if (i==(buffersize+100)){
      mean_ax=buff_ax/buffersize;
      mean_ay=buff_ay/buffersize;
      mean_az=buff_az/buffersize;
      mean_gx=buff_gx/buffersize;
      mean_gy=buff_gy/buffersize;
      mean_gz=buff_gz/buffersize;
    }
    i++;
    chThdSleepMilliseconds(2); //Needed so we don't get repeated measures
  }
}

void calibration(){
  ax_offset=-mean_ax/8;
  ay_offset=-mean_ay/8;
  az_offset=(16384-mean_az)/8;

  gx_offset=-mean_gx/4;
  gy_offset=-mean_gy/4;
  gz_offset=-mean_gz/4;
  while (1){
    int ready=0;

	I2CdevwriteWords(0x68,0x06, ax_offset, 2);
	I2CdevwriteWords(0x68,0x08, ay_offset, 2);
	I2CdevwriteWords(0x68,0x0A, az_offset, 2);//acelero z
	I2CdevwriteWords(0x68,0x13, gx_offset, 2);
	I2CdevwriteWords(0x68,0x15, gy_offset, 2);
	I2CdevwriteWords(0x68,0x17, gz_offset, 2);//giroz

    meansensors();

chprintf((BaseChannel *) &SD2, "...  \r\n");
chprintf((BaseChannel *) &SD2, "Medidas3, %d, %d, %d, %d, %d, %d \r\n",ax, ay, az, gx, gy, gz);
chprintf((BaseChannel *) &SD2, "mean3, %d, %d, %d, %d, %d, %d \r\n",mean_ax,mean_ay,mean_az,mean_gx,mean_gy,mean_gz);
//chprintf((BaseChannel *) &SD2, "fabs, %d, %d, %d, %d, %d, %d \r\n",abs(mean_ax),abs((mean_ay)),abs((mean_az)),abs(mean_gx),abs(mean_gy),abs(mean_gz));


    if (abs(mean_ax)<=acel_deadzone) ready++;
    else ax_offset=ax_offset-mean_ax/acel_deadzone;

    if (abs(mean_ay)<=acel_deadzone) ready++;
    else ay_offset=ay_offset-mean_ay/acel_deadzone;

    if (abs(16384-mean_az)<=acel_deadzone) ready++;
    else az_offset=az_offset+(16384-mean_az)/acel_deadzone;

    if (abs(mean_gx)<=giro_deadzone) ready++;
    else gx_offset=gx_offset-mean_gx/(giro_deadzone+1);

    if (abs(mean_gy)<=giro_deadzone) ready++;
    else gy_offset=gy_offset-mean_gy/(giro_deadzone+1);

    if (abs(mean_gz)<=giro_deadzone) ready++;
    else gz_offset=gz_offset-mean_gz/(giro_deadzone+1);

    if (ready==6) break;
chprintf((BaseChannel *) &SD2, "mean3, %d, \r\n",ready);
  }
}



void calibracion(){

//	chprintf((BaseChannel *) &SD2, "\n leyendo sensor 1 medidas...");

//inicializar offsets

I2CdevwriteWords(0x68,0x06, 0, 2);
I2CdevwriteWords(0x68,0x08, 0, 2);
I2CdevwriteWords(0x68,0x0A, 0, 2);//acelero z
I2CdevwriteWords(0x68,0x13, 0, 2);
I2CdevwriteWords(0x68,0x15, 0, 2);
I2CdevwriteWords(0x68,0x17, 0, 2);//giroz

//    chThdSleepMilliseconds(2000); 
//chprintf((BaseChannel *) &SD2, "%d, %d, %d, %d, %d, %d \r\n",ax, ay, az, gx, gy, gz);
//chprintf((BaseChannel *) &SD2, "%d, %d, %d, %d, %d, %d \r\n",mean_ax,mean_ay,mean_az,mean_gx,mean_gy,mean_gz);


//	chprintf((BaseChannel *) &SD2, "\n leyendo sensor 2 medidas...");
//chprintf((BaseChannel *) &SD2, "datos con offset \r\n");
    meansensors();
//chprintf((BaseChannel *) &SD2, "Medidas, %d, %d, %d, %d, %d, %d \r\n",ax, ay, az, gx, gy, gz);
//chprintf((BaseChannel *) &SD2, "mean, %d, %d, %d, %d, %d, %d \r\n",mean_ax,mean_ay,mean_az,mean_gx,mean_gy,mean_gz);


  //  chThdSleepMilliseconds(1000);     
    calibration();
  //  chThdSleepMilliseconds(1000); 

    meansensors();
chprintf((BaseChannel *) &SD2, "finish \r\n");

chprintf((BaseChannel *) &SD2, "Medidas con offset, %d, %d, %d, %d, %d, %d \r\n",ax, ay, az, gx, gy, gz);
chprintf((BaseChannel *) &SD2, "tus offsets, %d, %d, %d, %d, %d, %d \r\n",ax_offset,ay_offset,az_offset,gx_offset,gy_offset,gz_offset);


	I2CdevwriteWords(0x68,0x06, ax_offset, 2);
	I2CdevwriteWords(0x68,0x08, ay_offset, 2);
	I2CdevwriteWords(0x68,0x0A, az_offset, 2);//acelero z
	I2CdevwriteWords(0x68,0x13, gx_offset, 2);
	I2CdevwriteWords(0x68,0x15, gy_offset, 2);
	I2CdevwriteWords(0x68,0x17, gz_offset, 2);//giroz


// calibracion completa 

validacion=1;


}


///////////////////////////////////////////////////////////////////////////////////////

static WORKING_AREA(Area_acelero, 1024);

static msg_t Acelero_Thread(void *arg) {
	(void)arg;
	/* uint8_t MPUbuffer[6];
	int16_t AcX;
	int16_t AcY;
	int16_t AcZ;
	float Acc[2];
	int16_t GyX;
	int16_t GyY;
	float Gy[2];
	float Angle[2];*/
	chThdSleepMilliseconds(5000);

	//INICIALIZAR MPU
	MPUinitialize();

	//INICIALIZAR ADC
	ADC_Initialize( ADC);
	   /* startup = 15:    640 periods of ADCClock
		* for prescal = 11
		*     prescal: ADCClock = MCK / ( (PRESCAL+1) * 2 ) => 48MHz / ((11+1)*2) = 2MHz
		*     ADC clock = 2 MHz
		*/

    //PARA CALIBRAR EL MPU
	//calibracion();

    //PARA CONFIGURAR EL ADC
	ADC_cfgFrequency( ADC, 15, 11 ); //
	ADC_check( ADC, 48000000 ); // Board Clock 48000000
	ADC->ADC_CHER = 0x00000001;  // Enable Channels 0 
	ADC->ADC_MR |= 0x80;
	ADC_StartConversion(ADC); /* Start conversion */
//datosmpu();
	while(true){

		while( !( (ADC->ADC_ISR & ADC_ISR_EOC0) ) );
				chThdSleepMilliseconds(10);
					ADC_Val = ADC->ADC_CDR[0];
		datosmpu();

		chprintf((BaseChannel *) &SD2, "%d, %d, %d, %d, %d, %d \r\n",AcX, AcY, AcZ, GyX, GyY, ADC_Val*3300/4096);

	}
return (0);
}

///////////////////////////////////////////////////////////////////////////////////////


static WORKING_AREA(Area_Adc_electro, 1024);

static msg_t Adc_electro_Thread(void *arg) {
  (void)arg;

   /* ADC configuration*/
   ADC_Initialize( ADC);
   /* startup = 15:    640 periods of ADCClock
    * for prescal = 11
    *     prescal: ADCClock = MCK / ( (PRESCAL+1) * 2 ) => 48MHz / ((11+1)*2) = 2MHz
    *     ADC clock = 2 MHz
    */
   ADC_cfgFrequency( ADC, 15, 11 ); //
   ADC_check( ADC, 48000000 ); // Board Clock 48000000
   ADC->ADC_CHER = 0x00000001;  // Enable Channels 0 
   ADC->ADC_MR |= 0x80;
   ADC_StartConversion(ADC); /* Start conversion */

	while(true){
		while( !( (ADC->ADC_ISR & ADC_ISR_EOC0) ) );
		chThdSleepMilliseconds(500);
		    ADC_Val = ADC->ADC_CDR[0];
		//chprintf((BaseChannel *)&SD2, "Datos ADC vrgs %d, \r\n", ADC_Val*3300/4096) ;
	}
return (0);
}


///////////////////////////////////////////////////////////////////////////////////////

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
	//------------------------
	//chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
	chThdCreateStatic(Area_acelero, sizeof(Area_acelero), NORMALPRIO, Acelero_Thread, NULL);
	//chThdCreateStatic(Area_Adc_electro, sizeof(Area_Adc_electro), NORMALPRIO, Adc_electro_Thread, NULL);


	/*while(true) {
    //El objetivo es acumular los datos en un unico vector que sera luego interpretado por python
    chprintf((BaseChannel *) &SD2, "%d, %d, %d, %d, %d, %d \r\n",AcX, AcY, AcZ, GyX, GyY, ADC_Val*3300/4096);
	}*/

	return(0);
}
