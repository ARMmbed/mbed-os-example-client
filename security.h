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
 
#define MBED_DOMAIN "f11f7f32-8378-451e-bc12-cb93c0f49bbb"
#define MBED_ENDPOINT_NAME "007da9eb-c795-4934-9ccc-282cf5048029"
 
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
"MIIB0DCCAXOgAwIBAgIERkTYfjAMBggqhkjOPQQDAgUAMDkxCzAJBgNVBAYTAkZ\r\n"
"JMQwwCgYDVQQKDANBUk0xHDAaBgNVBAMME21iZWQtY29ubmVjdG9yLTIwMTYwHh\r\n"
"cNMTYwODMxMDkxOTI3WhcNMTYxMjMxMDYwMDAwWjCBoTFSMFAGA1UEAxNJZjExZ\r\n"
"jdmMzItODM3OC00NTFlLWJjMTItY2I5M2MwZjQ5YmJiLzAwN2RhOWViLWM3OTUt\r\n"
"NDkzNC05Y2NjLTI4MmNmNTA0ODAyOTEMMAoGA1UECxMDQVJNMRIwEAYDVQQKEwl\r\n"
"tYmVkIHVzZXIxDTALBgNVBAcTBE91bHUxDTALBgNVBAgTBE91bHUxCzAJBgNVBA\r\n"
"YTAkZJMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAETbvePJU/05bMkgumT9TFu\r\n"
"0XZ/aUoiCCqqG0MqoDFacBsLwFK1x8ajY/K9zg8ibeg+HcJLYSEVa0o0bLC9mEo\r\n"
"DzAMBggqhkjOPQQDAgUAA0kAMEYCIQCXUsBX4tNWNu8Ux/KQYzrMWIMESUOZFz3\r\n"
"OT2huurDHEQIhAO8ugCS9UfXCGCvrxi31vfpDmtQIUHJcg0EsPdbecyRH\r\n"
"-----END CERTIFICATE-----\r\n";
 
const uint8_t KEY[] = "-----BEGIN PRIVATE KEY-----\r\n"
"MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgNvAYcMbCNrkTsRB+\r\n"
"iaPxBoeWdDBJ/vwJ8S/pJG8mdmqhRANCAARNu948lT/TlsySC6ZP1MW7Rdn9pSiI\r\n"
"IKqobQyqgMVpwGwvAUrXHxqNj8r3ODyJt6D4dwkthIRVrSjRssL2YSgP\r\n"
"-----END PRIVATE KEY-----\r\n";
 
#endif //__SECURITY_H__