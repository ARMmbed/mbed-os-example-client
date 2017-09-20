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

#ifndef __SIMPLECLIENT_H__
#define __SIMPLECLIENT_H__

#include "mbed-client/m2mcorememory.h"
#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2mdevice.h"
#include "mbed-client/m2minterfaceobserver.h"
#include "mbed-client/m2minterface.h"
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
#include "mbed-client/m2mconfig.h"
#include "mbed-client/m2mblockmessage.h"
#include "security.h"
#ifndef __linux__
#include "mbed.h"
#include "memory_tests.h"
#else
#include <stdio.h>
#endif


#define ETHERNET        1
#define WIFI            2
#define MESH_LOWPAN_ND  3
#define MESH_THREAD     4
#define ATMEL           5
#define MCR20           6
#define SPIRIT1         7

#define STRINGIFY(s) #s

static uint8_t CORE_MEMORY_POOL[10000];

// Check if using mesh networking, define helper
#if MBED_CONF_APP_NETWORK_INTERFACE == MESH_LOWPAN_ND
    #define MESH
#elif MBED_CONF_APP_NETWORK_INTERFACE == MESH_THREAD
    #define MESH
#endif

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
#else
    // WiFi or Ethernet supports both - TCP by default to avoid
    // NAT problems, but UDP will also work - IF you configure
    // your network right.
    M2MInterface::BindingMode SOCKET_MODE = M2MInterface::UDP;
#endif

const unsigned char psk[] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36};
const size_t psk_len = sizeof( psk );
const unsigned char psk_identity[] = {0x0F,0x0F};
const size_t psk_identity_len = sizeof( psk_identity );


// MBED_DOMAIN and MBED_ENDPOINT_NAME come
// from the security.h file copied from connector.mbed.com

struct MbedClientDevice {
    const char* Manufacturer;
    const char* Type;
    const char* ModelNumber;
    const char* SerialNumber;
};

void device_certificate(const uint8_t *& data, uint32_t &length) {
//        data = CERT;
//        length = sizeof(CERT);
           data = psk_identity;
           length = psk_identity_len;
}

void server_certificate(const uint8_t *& data, uint32_t &length) {
//        data = SERVER_CERT;
//        length = sizeof(SERVER_CERT);

        data = psk_identity;
        length = psk_identity_len;
}

void device_key(const uint8_t *& data, uint32_t &length) {
//        data = KEY;
//        length = sizeof(KEY);

        data = psk;
        length = psk_len;
}

/*
* Wrapper for mbed client stack that handles all callbacks, error handling, and
* other shenanigans to make the mbed client stack easier to use.
*
* The end user should only have to care about configuring the parameters at the
* top of this file and making sure they add the security.h file correctly.
* To add resources you can copy the _TODO__ function and add as many instances as
* you want.
*
*/
class MbedClient: public M2MInterfaceObserver {
public:

    // constructor for MbedClient object, initialize private variables
    MbedClient(struct MbedClientDevice device) {
        _interface = NULL;
        _bootstrapped = false;
        _error = false;
        _registered = false;
        _unregistered = false;
        _register_security = NULL;
        _value = 0;
        _object = NULL;
        _device = device;
        //pal_init();
#ifdef MBED_CLIENT_DYNMEM_LIB
        M2MCoreMemory::init(CORE_MEMORY_POOL,10000);
#endif
    }

    // de-constructor for MbedClient object, you can ignore this
    ~MbedClient() {
        M2MDevice::delete_instance();
        if(_register_security){
            delete _register_security;
        }
        if(_interface) {
            delete _interface;
        }
    }

    // debug printf function
    void trace_printer(const char* str) {
        printf("\r\n%s\r\n", str);
    }

    /*
    *  Creates M2MInterface using which endpoint can
    *  setup its name, resource type, life time, connection mode,
    *  Currently only LwIPv4 is supported.
    */
    bool create_interface(const char *server_address,
                          void *handler=NULL) {
    // Randomizing listening port for Certificate mode connectivity
    memcpy(_server_address, server_address, strlen(server_address));
    uint16_t port = 0; // Network interface will randomize with port 0

    // create mDS interface object, this is the base object everything else attaches to
    _interface = M2MInterfaceFactory::create_interface(*this,
                                                      MBED_ENDPOINT_NAME,       // endpoint name string
                                                      "test",                   // endpoint type string
                                                      1000,                      // lifetime
                                                      port,                     // listen port
                                                      MBED_DOMAIN,              // domain string
                                                      SOCKET_MODE,              // binding mode
                                                      NETWORK_STACK,            // network stack
                                                      "");                      // context address string
    const char *binding_mode = (SOCKET_MODE == M2MInterface::UDP) ? "UDP" : "TCP";
    printf("\r\nSOCKET_MODE : %s\r\n", binding_mode);
    printf("Connecting to %s\r\n", server_address);

    if(_interface) {
        _interface->set_platform_network_handler(handler);
        return true;
    } else {
        return false;
    }

    }

    /*
    *  check private variable to see if the registration was sucessful or not
    */
    bool register_successful() {
        return _registered;
    }

    /*
    *  check private variable to see if un-registration was sucessful or not
    */
    bool unregister_successful() {
        return _unregistered;
    }

    /*
    *  Creates register server object with mbed device server address and other parameters
    *  required for client to connect to mbed device server.
    */
    M2MSecurity* create_register_object() {
        // create security object using the interface factory.
        // this will generate a security ObjectID and ObjectInstance
        M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);

        // make sure security ObjectID/ObjectInstance was created successfully
        if(security) {
            // Add ResourceID's and values to the security ObjectID/ObjectInstance
            security->set_resource_value(M2MSecurity::M2MServerUri, (const uint8_t*)_server_address, strlen(_server_address));
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::Psk);
//            security->set_resource_value(M2MSecurity::ServerPublicKey, SERVER_CERT, sizeof(SERVER_CERT)-1);
//            security->set_resource_value(M2MSecurity::PublicKey, CERT, sizeof(CERT)-1);
//            security->set_resource_value(M2MSecurity::Secretkey, KEY, sizeof(KEY)-1);

            security->set_resource_value(M2MSecurity::ServerPublicKey, psk_identity,psk_identity_len);
            security->set_resource_value(M2MSecurity::PublicKey, psk_identity,psk_identity_len);
            security->set_resource_value(M2MSecurity::Secretkey, psk,psk_len);

//            security->set_device_certificate(device_certificate);
//            security->set_server_certificate(server_certificate);
//            security->set_device_key(device_key);
        }
        return security;
    }

    /*
    * Creates device object which contains mandatory resources linked with
    * device endpoint.
    */
    M2MDevice* create_device_object() {
        // create device objectID/ObjectInstance
        M2MDevice *device = M2MInterfaceFactory::create_device();
        if(!device) {
            return NULL;
        }
        // make sure device object was created successfully
        // add resourceID's to device objectID/ObjectInstance
        device->create_resource(M2MDevice::Manufacturer, _device.Manufacturer);
        device->create_resource(M2MDevice::DeviceType, _device.Type);
        device->create_resource(M2MDevice::ModelNumber, _device.ModelNumber);
        device->create_resource(M2MDevice::SerialNumber, _device.SerialNumber);
        return device;
    }

    /*
    * register an object
    */
    void test_register(M2MSecurity *register_object, M2MObjectList object_list){
        if(_interface) {
            // Register function
            _interface->register_object(register_object, object_list);
        }
    }

    /*
    * unregister all objects
    */
    void test_unregister() {
        if(_interface) {
            // Unregister function
            _interface->unregister_object(NULL); // NULL will unregister all objects
        }
    }

    //Callback from mbed client stack when the bootstrap
    // is successful, it returns the mbed Device Server object
    // which will be used for registering the resources to
    // mbed Device server.
    void bootstrap_done(M2MSecurity *server_object){
        if(server_object) {
            _bootstrapped = true;
            _error = false;
            trace_printer("Bootstrapped");
        }
    }

    //Callback from mbed client stack when the registration
    // is successful, it returns the mbed Device Server object
    // to which the resources are registered and registered objects.
    void object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
        _registered = true;
        _unregistered = false;
        trace_printer("Registered object successfully!");
       // test_unregister();
#ifndef __linux__
        heap_stats();
#endif
    }

    //Callback from mbed client stack when the unregistration
    // is successful, it returns the mbed Device Server object
    // to which the resources were unregistered.
    void object_unregistered(M2MSecurity */*server_object*/){
        trace_printer("Unregistered Object Successfully");
        _unregistered = true;
        _registered = false;
    }

    /*
    * Callback from mbed client stack when registration is updated
    */
    void registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/){
        /* The registration is updated automatically and frequently by the
        *  mbed client stack. This print statement is turned off because it
        *  tends to happen alot.
        */
        //trace_printer("\r\nRegistration Updated\r\n");
    }

    // Callback from mbed client stack if any error is encountered
    // during any of the LWM2M operations. Error type is passed in
    // the callback.
    void error(M2MInterface::Error error){
        _error = true;
        switch(error){
            case M2MInterface::AlreadyExists:
                trace_printer("[ERROR:] M2MInterface::AlreadyExist");
                break;
            case M2MInterface::BootstrapFailed:
                trace_printer("[ERROR:] M2MInterface::BootstrapFailed");
                break;
            case M2MInterface::InvalidParameters:
                trace_printer("[ERROR:] M2MInterface::InvalidParameters");
                break;
            case M2MInterface::NotRegistered:
                trace_printer("[ERROR:] M2MInterface::NotRegistered");
                break;
            case M2MInterface::Timeout:
                trace_printer("[ERROR:] M2MInterface::Timeout");
                break;
            case M2MInterface::NetworkError:
                trace_printer("[ERROR:] M2MInterface::NetworkError");
                break;
            case M2MInterface::ResponseParseFailed:
                trace_printer("[ERROR:] M2MInterface::ResponseParseFailed");
                break;
            case M2MInterface::UnknownError:
                trace_printer("[ERROR:] M2MInterface::UnknownError");
                break;
            case M2MInterface::MemoryFail:
                trace_printer("[ERROR:] M2MInterface::MemoryFail");
                break;
            case M2MInterface::NotAllowed:
                trace_printer("[ERROR:] M2MInterface::NotAllowed");
                break;
            case M2MInterface::SecureConnectionFailed:
                trace_printer("[ERROR:] M2MInterface::SecureConnectionFailed");
                break;
            case M2MInterface::DnsResolvingFailed:
                trace_printer("[ERROR:] M2MInterface::DnsResolvingFailed");
                break;

            default:
                break;
        }
        printf("Error details : %s\r\n\n",_interface->error_description());
    }

    /* Callback from mbed client stack if any value has changed
    *  during PUT operation. Object and its type is passed in
    *  the callback.
    *  BaseType enum from m2mbase.h
    *       Object = 0x0, Resource = 0x1, ObjectInstance = 0x2, ResourceInstance = 0x3
    */
    void value_updated(M2MBase *base, M2MBase::BaseType type) {
        path_buffer buffer;
         base->uri_path(buffer);
        printf("\r\nPUT Request Received!");
        printf("\r\nName :'%d', \r\nPath : '%s', \r\nType : '%d' (0 for Object, 1 for Resource), \r\nType : '%s'\r\n",
               base->name_id(),
               buffer.c_str(),
               type
               ,base->resource_type());
    }

    /*
    * update the registration period
    */
    void test_update_register() {
        if (_registered) {
            _interface->update_registration(_register_security, 100);
        }
    }

    /*
    * manually configure the security object private variable
    */
   void set_register_object(M2MSecurity *register_object) {
        if (_register_security == NULL) {
            _register_security = register_object;
        }
    }

private:

    /*
    *  Private variables used in class
    */
    M2MInterface    	     *_interface;
    M2MSecurity              *_register_security;
    M2MObject                *_object;
    volatile bool            _bootstrapped;
    volatile bool            _error;
    volatile bool            _registered;
    volatile bool            _unregistered;
    int                      _value;
    struct MbedClientDevice  _device;
    char                    _server_address[128];
};

#endif // __SIMPLECLIENT_H__
