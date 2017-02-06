/*
 * client_buttons.h
 *
 * Copyright (c) 2015, 2016 ARM Limited. All rights reserved.
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

#ifndef CLIENT_BUTTONS_H_
#define CLIENT_BUTTONS_H_
#include "mbed.h"

// Define OBSERVATION and UNREGISTER buttons per board as
// boards do not have a "unified" button schema.
// If buttons are not available, app will revert to a timer
// that does the observation (update counter) every 15 seconds.
#ifdef TARGET_K64F
    #define BUTTONS_DEFINED
    #define CLIENT_OBS_BUTTON   SW2
    #define CLIENT_UNREG_BUTTON SW3
#endif

#ifdef TARGET_UBLOX_EVK_ODIN_W2
    #define BUTTONS_DEFINED
    #define CLIENT_OBS_BUTTON   SW0
    #define CLIENT_UNREG_BUTTON SW1
#endif

#endif /* CLIENT_BUTTONS_H_ */