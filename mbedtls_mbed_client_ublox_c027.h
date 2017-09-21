//----------------------------------------------------------------------------
// The confidential and proprietary information contained in this file may
// only be used by a person authorised under and to the extent permitted
// by a subsisting licensing agreement from ARM Limited or its affiliates.
//
// (C) COPYRIGHT 2016 ARM Limited or its affiliates.
// ALL RIGHTS RESERVED
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from ARM Limited or its affiliates.
//----------------------------------------------------------------------------
#ifndef MBEDTLS_MBED_CLIENT_UBLOX_C027_H_
#define MBEDTLS_MBED_CLIENT_UBLOX_C027_H_

#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

void custom_memory_free(void *ptr);
void *custom_memory_calloc(size_t num, size_t size);
#ifdef __cplusplus
}
#endif

/* System support */
#define MBEDTLS_HAVE_ASM

/* Crypto flags */
#define MBEDTLS_CIPHER_MODE_CTR
#define MBEDTLS_CMAC_C
#undef MBEDTLS_PLATFORM_TIME_ALT

/* mbed TLS feature support */
#undef MBEDTLS_ECP_DP_SECP256R1_ENABLED
#undef MBEDTLS_ECP_NIST_OPTIM
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY
#undef MBEDTLS_SSL_EXPORT_KEYS

/* mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_CTR_DRBG_C
#undef MBEDTLS_ECP_C
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_MD_C
#undef MBEDTLS_OID_C
#undef MBEDTLS_PK_C
#undef MBEDTLS_PK_PARSE_C
#undef MBEDTLS_PK_WRITE_C
#define MBEDTLS_SHA256_C
#undef MBEDTLS_SSL_COOKIE_C
#define MBEDTLS_SSL_CLI_C
#undef MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TLS_C
#undef MBEDTLS_X509_CRL_PARSE_C
#undef MBEDTLS_HMAC_DRBG_C
#undef MBEDTLS_ECDSA_DETERMINISTIC

// XXX mbedclient needs these: mbedtls_x509_crt_free, mbedtls_x509_crt_init, mbedtls_x509_crt_parse
#undef MBEDTLS_X509_USE_C
#undef MBEDTLS_X509_CRT_PARSE_C
// a bit wrong way to get mbedtls_ssl_conf_psk:

#undef MBEDTLS_SHA512_C
#undef MBEDTLS_ECDH_C
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#undef MBEDTLS_SSL_SERVER_NAME_INDICATION
#undef MBEDTLS_GCM_C

#undef MBEDTLS_ECDSA_C
#undef MBEDTLS_CERTS_C

// Remove RSA, save 20KB at total
#undef MBEDTLS_RSA_C
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED

// Remove error messages, save 10KB of ROM
#undef MBEDTLS_ERROR_C

// Remove selftesting and save 11KB of ROM
#undef MBEDTLS_SELF_TEST

// Reduces ROM size by 30 kB
#undef MBEDTLS_ERROR_STRERROR_DUMMY
#undef MBEDTLS_VERSION_FEATURES
#define MBEDTLS_DEBUG_C

// needed for parsing the certificates
#undef MBEDTLS_PEM_PARSE_C
// dep of the previous
#undef MBEDTLS_BASE64_C

// Needed by provisioning
#undef MBEDTLS_PEM_WRITE_C
#undef MBEDTLS_CTR_DRBG_MAX_REQUEST /*512*/

// Reduce IO buffer to save RAM, default is 16KB
#define MBEDTLS_SSL_MAX_CONTENT_LEN 512

// define to save 8KB RAM at the expense of ROM
#define MBEDTLS_AES_ROM_TABLES

// Save ROM and a few bytes of RAM by specifying our own ciphersuite list
//#define MBEDTLS_SSL_CIPHERSUITES MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
#define MBEDTLS_SSL_CIPHERSUITES /*MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA256,MBEDTLS_TLS_PSK_WITH_AES_256_CCM_8*/MBEDTLS_TLS_PSK_WITH_AES_128_CCM_8

//#define MBEDTLS_PLATFORM_MEMORY
//#define MBEDTLS_PLATFORM_STD_CALLOC   custom_memory_calloc
//#define MBEDTLS_PLATFORM_STD_FREE     custom_memory_free
#define MBEDTLS_ENTROPY_MAX_SOURCES   2

//#define MBEDTLS_TEST_NULL_ENTROPY
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#undef MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_MBED_CLIENT_UBLOX_C027_H_ */
