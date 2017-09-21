/*
 * Copyright (c) 2016 ARM Limited. All rights reserved.
 */

#include <string.h>
#include "nsdynmemLIB.h"
#include "randLIB.h"
#include "mbedtls/entropy.h"
#include "mbedtls/entropy_poll.h"
#include "mbed-trace/mbed_trace.h"
/*
 * This is dummy HW entropy for testing hardwares that do not have real HW entropy.
 * Code is copied from coap-service that adds weak entropy and initializes randLib.
 */
#if defined (TARGET_NRF52_DK) || defined(TARGET_UBLOX_C027)

#define TRACE_GROUP "hwpl"
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    (void)data;
    tr_debug("\n\rmbedtls_hardware_poll\r\n");
    char *c = (char*)ns_dyn_mem_temporary_alloc(len);
    if( !c ){
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }
    memset(c, 0, len);
    for(uint16_t i=0; i < len; i++){
        *(c + i) = (char)randLIB_get_8bit();
    }
    memmove(output, c, len);
    *olen = len;

    ns_dyn_mem_free(c);
    tr_debug("\n\rmbedtls_hardware_poll end\r\n");
    return( 0 );
}
#endif
