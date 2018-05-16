#include <stdint.h>

#include "ch.h"

/**********************************************************
 * Types
 */
struct p_example {
    volatile uint32_t data1;        //00
    volatile uint32_t data2;        //04
    volatile uint32_t data3;        //08
    volatile uint32_t data4;        //0C
    volatile uint32_t set_zeros;    //10
} ;

void write_data1(uint32_t data1);
void write_data2(uint32_t data2);
void write_data3(uint32_t data3);
void write_data4(uint32_t data4);
void write_zeros();
uint32_t read_data1();
uint32_t read_data2();
uint32_t read_data3();
uint32_t read_data4();

//extern p_example *p_example0;

