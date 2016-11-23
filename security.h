

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

 

#define MBED_DOMAIN "9d6fe233-9a5f-4eaa-8992-97b1e0fce338"

#define MBED_ENDPOINT_NAME "c154cc0b-8dc0-4ae6-a28b-25c44c86789e"

 

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

"MIIBzzCCAXOgAwIBAgIEPbJdijAMBggqhkjOPQQDAgUAMDkxCzAJBgNVBAYTAkZ\r\n"

"JMQwwCgYDVQQKDANBUk0xHDAaBgNVBAMME21iZWQtY29ubmVjdG9yLTIwMTYwHh\r\n"

"cNMTYwODE5MDg0OTA4WhcNMTYxMjMxMDYwMDAwWjCBoTFSMFAGA1UEAxNJOWQ2Z\r\n"

"mUyMzMtOWE1Zi00ZWFhLTg5OTItOTdiMWUwZmNlMzM4L2MxNTRjYzBiLThkYzAt\r\n"

"NGFlNi1hMjhiLTI1YzQ0Yzg2Nzg5ZTEMMAoGA1UECxMDQVJNMRIwEAYDVQQKEwl\r\n"

"tYmVkIHVzZXIxDTALBgNVBAcTBE91bHUxDTALBgNVBAgTBE91bHUxCzAJBgNVBA\r\n"

"YTAkZJMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEiP8HcjnPyb+nnV2GP7n5T\r\n"

"h7OC9wFZLa78zVdK9/FClHbJJd6zWlaGEYFhirMzcoNYrpwBNOsMmUEmA6YW0+m\r\n"

"1DAMBggqhkjOPQQDAgUAA0gAMEUCIQDHzTkR1MGVm16Jai42bxAYPAds5JG38T0\r\n"

"/OodSMndhhAIgNhLhfotPYL2u0TGLFquQJNNEXNZs604u5n/JsT1BGYo=\r\n"

"-----END CERTIFICATE-----\r\n";

 

const uint8_t KEY[] = "-----BEGIN PRIVATE KEY-----\r\n"

"MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgw2tABV2TaAMckC02\r\n"

"PQ8SAiVbD2o5LHJ9bwfvejviFSShRANCAASI/wdyOc/Jv6edXYY/uflOHs4L3AVk\r\n"

"trvzNV0r38UKUdskl3rNaVoYRgWGKszNyg1iunAE06wyZQSYDphbT6bU\r\n"

"-----END PRIVATE KEY-----\r\n";

 

#endif //__SECURITY_H__
