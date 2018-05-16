#include "pmc.h"

/* These headers were introduced in C99 by working group ISO/IEC JTC1/SC22/WG14. */
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/
/** Base address of chip select */
#define PSRAM_BASE_ADDRESS         (0x61000000)

/** 8-bits Data bus on EBI */
#define PIN_DATA_BUS_ON_EBI         {0x000000ff, PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP}
/** Read Signal (output) */
#define PIN_NRD_ON_EBI              {1 << 11, PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP}
/** Write Enable Signal (output) */
#define PIN_NWE_ON_EBI              {1 << 8,  PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP}
/** Static Memory Controller Chip Select Lines */
#define PIN_NCS1_ON_EBI             {1 << 15, PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP}
/** Address Bus A0-A20 */
#define PIN_ADDR_BUS_ON_EBI         {0xfffc0000, PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP},\
                                    {0x019c0003, PIOA, ID_PIOA, PIO_PERIPH_C, PIO_PULLUP}

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/

/** PIOs configuration for EBI connect to Psram. */
const Pin pinPsram[] = {PIN_DATA_BUS_ON_EBI, \
                        PIN_NRD_ON_EBI, \
                        PIN_NWE_ON_EBI, \
                        PIN_NCS1_ON_EBI,\
                        PIN_ADDR_BUS_ON_EBI
                        };

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/
/**
 * \brief Go/No-Go test of the first 10K-Bytes of external PSRAM access.
   \return 0 if test is failed else 1.
 */

static uint8_t AccessPsramTest(void)
{
    uint32_t i;
    uint32_t *ptr = (uint32_t *) PSRAM_BASE_ADDRESS;

    for (i = 0; i < 10 * 1024; ++i) {
        if (i & 1) {
            ptr[i] = 0x55AA55AA | (1 << i);
        }
        else {
            ptr[i] = 0xAA55AA55 | (1 << i);
        }
    }
    for (i = 0; i < 10 * 1024; ++i) {
        if (i & 1) {
            if (ptr[i] != (0x55AA55AA | (1 << i))) {
                return 0;
            }
        }
        else {
            if (ptr[i] != (0xAA55AA55 | (1 << i))) {
                return 0;
            }
        }
    }
    return 1;
}


/*----------------------------------------------------------------------------
 *         Global functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Application entry point for smc_psram example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
    /* Disable watchdog */
    WDT_Disable( WDT );

    /* Output example information */
    printf("\n\r\n\r\n\r");
    printf("-- SMC Psram Example %s --\n\r", SOFTPACK_VERSION);
    printf("-- %s\n\r", BOARD_NAME);
    printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

    /* Configure EBI I/O for psram connection*/
    printf("-I- Configure EBI I/O for psram connection.\n\r");
    PIO_Configure(pinPsram, PIO_LISTSIZE(pinPsram));

    /* complete SMC configuration between PSRAM and SMC waveforms.*/
    BOARD_ConfigurePSRAM( SMC ) ;

    /* Test external PSRAM access */
    printf("-I- CTest external PSRAM access. \n\r");
    if (AccessPsramTest()) {
        printf("-I- Access Psram successful.\n\r");
    }
    else {
        printf("-I- Access Psram failed.\n\r");
    }
    return 0;
}
