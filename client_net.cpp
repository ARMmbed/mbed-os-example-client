/*
 * client_net.cpp
 * 
 * mbed-os-example-client - networking initializations here
 *
 * Copyright (c) 2016 ARM Limited. All rights reserved.
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
#include "mbed.h"
#include "simpleclient.h"

// Check if using mesh networking, define helper
#if MBED_CONF_APP_NETWORK_INTERFACE == MESH_LOWPAN_ND
    #define MESH
#elif MBED_CONF_APP_NETWORK_INTERFACE == MESH_THREAD
    #define MESH
#endif

// Different bearer stacks
#if MBED_CONF_APP_NETWORK_INTERFACE == WIFI
    #if TARGET_UBLOX_EVK_ODIN_W2
        #include "OdinWiFiInterface.h"
        OdinWiFiInterface wifi;
	#else
		#include "ESP8266Interface.h"
		ESP8266Interface wifi(MBED_CONF_APP_WIFI_TX, MBED_CONF_APP_WIFI_RX);
    #endif
#elif MBED_CONF_APP_NETWORK_INTERFACE == ETHERNET
    #include "EthernetInterface.h"
    EthernetInterface eth;
#elif MBED_CONF_APP_NETWORK_INTERFACE == MESH_LOWPAN_ND
    #define MESH
    #include "NanostackInterface.h"
    LoWPANNDInterface mesh;
#elif MBED_CONF_APP_NETWORK_INTERFACE == MESH_THREAD
    #define MESH
    #include "NanostackInterface.h"
    ThreadInterface mesh;
#endif

#if defined(MESH)
    #if MBED_CONF_APP_MESH_RADIO_TYPE == ATMEL
    #include "NanostackRfPhyAtmel.h"
        NanostackRfPhyAtmel rf_phy(ATMEL_SPI_MOSI, ATMEL_SPI_MISO, ATMEL_SPI_SCLK, ATMEL_SPI_CS,
                           ATMEL_SPI_RST, ATMEL_SPI_SLP, ATMEL_SPI_IRQ, ATMEL_I2C_SDA, ATMEL_I2C_SCL);
#elif MBED_CONF_APP_MESH_RADIO_TYPE == MCR20
    #include "NanostackRfPhyMcr20a.h"
        NanostackRfPhyMcr20a rf_phy(MCR20A_SPI_MOSI, MCR20A_SPI_MISO, MCR20A_SPI_SCLK, MCR20A_SPI_CS, MCR20A_SPI_RST, MCR20A_SPI_IRQ);
    #endif //MBED_CONF_APP_RADIO_TYPE
#endif //MESH


#if defined (MESH) || (MBED_CONF_LWIP_IPV6_ENABLED==true)
    // Mesh is always IPV6 - also WiFi and ETH can be IPV6 if IPV6 is enabled
    M2MInterface::NetworkStack NETWORK_STACK = M2MInterface::LwIP_IPv6;
#else
    // Everything else - we assume it's IPv4
    M2MInterface::NetworkStack NETWORK_STACK = M2MInterface::LwIP_IPv4;
#endif

//Select binding mode: UDP or TCP -- note - Mesh networking is IPv6 UDP ONLY
#ifdef MESH
 	M2MInterface::BindingMode SOCKET_MODE = M2MInterface::UDP;
    // Mesh does not have DNS, so must use direct IPV6 address
    const char *MBED_SERVER_ADDRESS="coaps://[2607:f0d0:2601:52::20]:5684";
#else
	// WiFi or Ethernet supports both - TCP by default to avoid
	// NAT problems, but UDP will also work - IF you configure
	// your network right.
    M2MInterface::BindingMode SOCKET_MODE = M2MInterface::TCP;
    // This is address to mbed Device Connector, name based
    // assume all other stacks support DNS properly
    const char *MBED_SERVER_ADDRESS="coap://api.connector.mbed.com:5684";
#endif

/*
 * client_net_init centralize the complexities of handling different
 *                 network stacks
 * IN: network interface ptr 
 * OUT: int        0 connection succeeded
 *                 -1 connection failed
 *
 */
int client_net_init(NetworkInterface **network_interface, RawSerial *output) {
    int connect_success;

    *network_interface = NULL;
    if(output) {
    	output->printf("\r\nConnecting to ");
    }
#if MBED_CONF_APP_NETWORK_INTERFACE == WIFI
    if(output) {
        output->printf("WiFi");
    }
    connect_success = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    *network_interface = &wifi;
#elif MBED_CONF_APP_NETWORK_INTERFACE == ETHERNET
    if(output) {
        output->printf("Ethernet");
    }
    connect_success = eth.connect();
    *network_interface = &eth;
#endif
#ifdef MESH
    if(output) {
        output->printf("Mesh");
    }
    mesh.initialize(&rf_phy);
    connect_success = mesh.connect();
    *network_interface = &mesh;
#endif

#if defined (MESH) || (MBED_CONF_LWIP_IPV6_ENABLED==true)
    if(output) {
       output->printf(" in IPv6 mode\r\n");
    }
#else
    if(output) { 
        output->printf(" in IPv4 mode\r\n");
    }
#endif
   return connect_success;
}
