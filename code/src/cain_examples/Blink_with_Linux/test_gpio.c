/*******************************************************************************
* File Name          : test_gpio.c
* Author             : Carlos Camargo
* Date First Issued  : 22/11/2012
* Description        : This file provides test for LED, USB_SEL, BT_EN and REC_EN.
********************************************************************************/

#include "board.h"
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>


static int dev_mem_fd;
static void *sys_controller;
static unsigned int* pio_base;


int gpio_map(void){
	dev_mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (dev_mem_fd < 0) {
          printf("Cannot open /dev/mem.\n");
	  return 0;
	}
	sys_controller = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, dev_mem_fd, IMX283_BASE_SYS);
	if (sys_controller == MAP_FAILED) {
          close(dev_mem_fd);
          printf("Cannot mmap.\n");
	  return 0;
	}
	pio_base = (unsigned int*)sys_controller;
        printf("GPIO maped successfully\n");
        return 0;
}

void init_gpio(void){

  struct device_t *device;
  device = devices;
  
  /*
   * Configure LED as GPIOs. 
   */

  pio_base[(2*( device->LED_PIO ) + PIO_MUXSEL + 16*(device->LED_PIN/16) + SET)/sizeof(unsigned int)] = (0x3 << 2*(device->LED_PIN ));
  /*
   * Configure LED as outputs. 
   */

  pio_base[(device->LED_PIO + PIO_DOE + SET)/sizeof(unsigned int)] = device->LED_MASK;     // Set as OUTPUT

  printf("LED configured as outputs\n");

}

int main(void) {

  struct device_t *device;
  device = devices;

  gpio_map();
  init_gpio();

  while(1){
    pio_base[(device->LED_PIO + PIO_DOUT + CLR)/sizeof(unsigned int)] = device->LED_MASK;
    printf("LED OFF\n");
    usleep (1000000);
    pio_base[(device->LED_PIO + PIO_DOUT + SET)/sizeof(unsigned int)] = device->LED_MASK;
    printf("LED ON\n");
    usleep (1000000);

  }
}


