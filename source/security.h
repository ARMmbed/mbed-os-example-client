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

const unsigned char PSK[] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36};
const unsigned char PSK_IDENTITY[] = {0x0F,0x0F};

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
"MIICTDCCAbWgAwIBAgIJAKI3S+LGklSGMA0GCSqGSIb3DQEBCwUAMD8xCzAJBgNV\r\n"
"BAYTAkZJMRMwEQYDVQQIDApTb21lLVN0YXRlMQ0wCwYDVQQHDARPdWx1MQwwCgYD\r\n"
"VQQKDANBUk0wHhcNMTUwNjExMTI0NTU1WhcNMTYwNjEwMTI0NTU1WjA/MQswCQYD\r\n"
"VQQGEwJGSTETMBEGA1UECAwKU29tZS1TdGF0ZTENMAsGA1UEBwwET3VsdTEMMAoG\r\n"
"A1UECgwDQVJNMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDBt8ag12ILm2pn\r\n"
"RHUc2yMBXpdzspDwVV3VDaNTRXypVUOq/nxJc6zr8yG/Pvga2/XVijWQDLABHVhV\r\n"
"PoeulzXVSEHRR2bR2lhqulLzdUzPYJ+yJd4+082akoxdzoom8ms2LFlgiXO7lyBg\r\n"
"1t74xjEQapCkr7Tdl2pD9OTUrcfoEwIDAQABo1AwTjAdBgNVHQ4EFgQUHK0O4iK7\r\n"
"Rv8AsxAqRDlY3TSHWtUwHwYDVR0jBBgwFoAUHK0O4iK7Rv8AsxAqRDlY3TSHWtUw\r\n"
"DAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQsFAAOBgQAPVLihYN65DEeYdf7gDAdP\r\n"
"MNH5RxsQlHdktQkYWcUewe3hvOf8yZUBbmO5OeEQmy5Ca2H1QhUuw+kWHo1l/K9g\r\n"
"DpR1xoDACeycBd/6tAHfaFzTEEDC9Ug839EAHDrbI4ihrPEbP6hyRIaUIM4IojXf\r\n"
"zuqwOcobS3idv8a9Npuohw==\r\n"
"-----END CERTIFICATE-----\r\n";

const uint8_t KEY[] = "-----BEGIN PRIVATE KEY-----\r\n"
"MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAMG3xqDXYgubamdE\r\n"
"dRzbIwFel3OykPBVXdUNo1NFfKlVQ6r+fElzrOvzIb8++Brb9dWKNZAMsAEdWFU+\r\n"
"h66XNdVIQdFHZtHaWGq6UvN1TM9gn7Il3j7TzZqSjF3OiibyazYsWWCJc7uXIGDW\r\n"
"3vjGMRBqkKSvtN2XakP05NStx+gTAgMBAAECgYEAlRimNxsu6jIXJh4paSk8ewxb\r\n"
"0v+n0GJT0uwpPakotofaihxOepYejIfX/IMxoDKVtmImsZUiTelRGGMFORpGomS7\r\n"
"3Pb9awfn1yWiYja9MF7+pgFtXj6LxcB5rcxrP8s9rCs+kEq8yl/khGXmlwlpacUm\r\n"
"Pdg1rEHriykk/5dlyKkCQQD9zpgQKN87bQNOVSgFSNsrwCjEN2MvVUC6ycqVYqQd\r\n"
"oQZrO2lJuf57Jn7kRKJgfqqImwLYaohn5A9gKiqszo+fAkEAw2REr8L9aLoRPe5Q\r\n"
"4+lYCmNmxJjBbJoRviSlxx+p5dxK+8H4J+bnFUWzORFJP0+Bjhii2UHGBoMjuP2s\r\n"
"e8xDDQJADm0IrZp1Hb6TeodSkAJVGsaASq7PP2h8FmHT1uvVYi293Khy0pL0yPEm\r\n"
"MzlHdW6dqMgOwFhFyonm6dNbu5i+BwJBALSPKfIKDMcEev5yBgo3Dga7iLJQPx4u\r\n"
"2gqOLyvtlME3a4tu2fV1qUG9ITuxP7DjteMr3QFLiMVn1lETCpt0TNkCQGIvyjdV\r\n"
"RzYnzLDMulXCsuRBjs0C75KoZrcPPQ416SZFtYASsOl3P0Ih5jgt/1aPmth3Yson\r\n"
"GdnBOWyKzadLmz8=\r\n"
"-----END PRIVATE KEY-----\r\n";

#endif //__SECURITY_H__
