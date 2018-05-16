#include <stdbool.h>

#include "soc-hw.h"

//struct type *name = (type *) base_address;
p_example *p_example0 = (p_example *) 0x63000100;

/**********************************************************
 * Example Peripheral Functions
 */
 
/*
 * It writes a 8-bit number in the internal register 1 of p_example
 * instance.
 * @param data1 is the number to be write in register 1 of
 * the peripheral.
 */
void write_data1(uint32_t data1) {
  p_example0->data1 = data1;
  chThdSleepMilliseconds(1);
}

/*
 * It writes a 8-bit number in the internal register 2 of p_example
 * instance.
 * @param data2 is the number to be write in register 2 of
 * the peripheral.
 */
void write_data2(uint32_t data2) {
  p_example0->data2 = data2;
  chThdSleepMilliseconds(1);
}

/*
 * It writes a 8-bit number in the internal register 3 of p_example
 * instance.
 * @param data3 is the number to be write in register 3 of
 * the peripheral.
 */
void write_data3(uint32_t data3) {
  p_example0->data3 = data3;
  chThdSleepMilliseconds(1);
}

/*
 * It writes a 8-bit number in the internal register 4 of p_example
 * instance.
 * @param data4 is the number to be write in register 4 of
 * the peripheral.
 */
void write_data4(uint32_t data4) {
  p_example0->data4 = data4;
  chThdSleepMilliseconds(1);
}

/*
 * It writes zeros in all four internal register of p_example
 */
void write_zeros() {
  p_example0->set_zeros = 0;;
  chThdSleepMilliseconds(1);
}

/*
 * It reads a 8-bit number of the internal register 1 of p_example
 * instance.
 * @return the value of the internal register 1.
 */
uint32_t read_data1() {
  return p_example0->data1;
}

/*
 * It reads a 8-bit number of the internal register 2 of p_example
 * instance.
 * @return the value of the internal register 2.
 */
uint32_t read_data2() {
  return p_example0->data2;
}

/*
 * It reads a 8-bit number of the internal register 3 of p_example
 * instance.
 * @return the value of the internal register 3.
 */
uint32_t read_data3() {
  return p_example0->data3;
}

/*
 * It reads a 8-bit number of the internal register 4 of p_example
 * instance.
 * @return the value of the internal register 4.
 */
uint32_t read_data4() {
  return p_example0->data4;
}
