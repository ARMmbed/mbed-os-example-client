/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __SECURITY_H__
#define __SECURITY_H__

#include <inttypes.h>

#define MBED_DOMAIN "b02ac40a-a501-44d9-be29-e6e0e5303f85"
#define MBED_ENDPOINT_NAME "cccdb1b6-9502-4001-9a5a-69621147ac87"

const uint8_t SERVER_CERT[] = "-----BEGIN CERTIFICATE-----\r\n"
"MIIBmDCCAT6gAwIBAgIEVUCA0jAKBggqhkjOPQQDAjBLMQswCQYDVQQGEwJGSTEN\r\n"
"MAsGA1UEBwwET3VsdTEMMAoGA1UECgwDQVJNMQwwCgYDVQQLDANJb1QxETAPBgNV\r\n"
"BAMMCEFSTSBtYmVkMB4XDTE1MDQyOTA2NTc0OFoXDTE4MDQyOTA2NTc0OFowSzEL\r\n"
"MAkGA1UEBhMCRkkxDTALBgNVBAcMBE91bHUxDDAKBgNVBAoMA0FSTTEMMAoGA1UE\r\n"
"CwwDSW9UMREwDwYDVQQDDAhBUk0gbWJlZDBZMBMGByqGSM49AgEGCCqGSM49AwEH\r\n"
"A0IABLuAyLSk0mA3awgFR5mw2RHth47tRUO44q/RdzFZnLsAsd18Esxd5LCpcT9w\r\n"
"0tvNfBv4xJxGw0wcYrPDDb8/rjujEDAOMAwGA1UdEwQFMAMBAf8wCgYIKoZIzj0E\r\n"
"AwIDSAAwRQIhAPAonEAkwixlJiyYRQQWpXtkMZax+VlEiS201BG0PpAzAiBh2RsD\r\n"
"NxLKWwf4O7D6JasGBYf9+ZLwl0iaRjTjytO+Kw==\r\n"
"-----END CERTIFICATE-----\r\n";

const uint8_t CERT[] = "-----BEGIN CERTIFICATE-----\r\n"
"MIIBzzCCAXOgAwIBAgIEanK39zAMBggqhkjOPQQDAgUAMDkxCzAJBgNVBAYTAkZ\r\n"
"JMQwwCgYDVQQKDANBUk0xHDAaBgNVBAMME21iZWQtY29ubmVjdG9yLTIwMTgwHh\r\n"
"cNMTcwODI0MDYyNTMxWhcNMTgxMjMxMDYwMDAwWjCBoTFSMFAGA1UEAxNJYjAyY\r\n"
"WM0MGEtYTUwMS00NGQ5LWJlMjktZTZlMGU1MzAzZjg1L2NjY2RiMWI2LTk1MDIt\r\n"
"NDAwMS05YTVhLTY5NjIxMTQ3YWM4NzEMMAoGA1UECxMDQVJNMRIwEAYDVQQKEwl\r\n"
"tYmVkIHVzZXIxDTALBgNVBAcTBE91bHUxDTALBgNVBAgTBE91bHUxCzAJBgNVBA\r\n"
"YTAkZJMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAETdySSoORBP5WKbZ1Glw4g\r\n"
"0t+8tIs+Ox/hE8d3d7Ovm8RGcDvGXdLOzU2fkZwFPYeK+kbsp9/LfWAo3G8TPHd\r\n"
"NjAMBggqhkjOPQQDAgUAA0gAMEUCIGfr2FmeY7HDssU8jxwrsJjYMlsNlpfPhjE\r\n"
"9tv+Nfc9PAiEA+mmA8LMgR618M26fEvjeWX+7nrWU7BimdtXr88BdnNc=\r\n"
"-----END CERTIFICATE-----\r\n";

const uint8_t KEY[] = "-----BEGIN PRIVATE KEY-----\r\n"
"MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQggj8AI0a+ewFJFH/S\r\n"
"5RFzupR+louBrunjSpKf0QNEPoWhRANCAARN3JJKg5EE/lYptnUaXDiDS37y0iz4\r\n"
"7H+ETx3d3s6+bxEZwO8Zd0s7NTZ+RnAU9h4r6Ruyn38t9YCjcbxM8d02\r\n"
"-----END PRIVATE KEY-----\r\n";

#endif //__SECURITY_H__
