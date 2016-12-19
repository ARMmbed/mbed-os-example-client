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
#include "mbed.h"
#include "client_net.h"

#define ETHERNET        1
#define WIFI            2
#define MESH_LOWPAN_ND  3
#define MESH_THREAD     4
#define ATMEL           5
#define MCR20           6

#define STRINGIFY(s) #s

// MBED_DOMAIN and MBED_ENDPOINT_NAME come
// from the security.h file copied from connector.mbed.com

struct MbedClientDevice {
    const char* Manufacturer;
    const char* Type;
    const char* ModelNumber;
    const char* SerialNumber;
};

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
	MbedClient(struct MbedClientDevice device);

    // de-constructor for MbedClient object, you can ignore this
    ~MbedClient();

    // debug printf function
    void trace_printer(const char* str);

    /*
    *  Creates M2MInterface using which endpoint can
    *  setup its name, resource type, life time, connection mode,
    *  Currently only LwIPv4 is supported.
    */
    void create_interface(const char *server_address,
                          void *handler=NULL);
    /*
    *  check private variable to see if the registration was sucessful or not
    */
    bool register_successful();

    /*
    *  check private variable to see if un-registration was sucessful or not
    */
    bool unregister_successful();

    /*
    *  Creates register server object with mbed device server address and other parameters
    *  required for client to connect to mbed device server.
    */
    M2MSecurity* create_register_object();

    /*
    * Creates device object which contains mandatory resources linked with
    * device endpoint.
    */
    M2MDevice* create_device_object();
    /*
    * register an object
    */
    void test_register(M2MSecurity *register_object, M2MObjectList object_list);

    /*
    * unregister all objects
    */
    void test_unregister();

    //Callback from mbed client stack when the bootstrap
    // is successful, it returns the mbed Device Server object
    // which will be used for registering the resources to
    // mbed Device server.
    void bootstrap_done(M2MSecurity *server_object);

    //Callback from mbed client stack when the registration
    // is successful, it returns the mbed Device Server object
    // to which the resources are registered and registered objects.
    void object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/);

    //Callback from mbed client stack when the unregistration
    // is successful, it returns the mbed Device Server object
    // to which the resources were unregistered.
    void object_unregistered(M2MSecurity */*server_object*/);

    /*
    * Callback from mbed client stack when registration is updated
    */
    void registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/);

    // Callback from mbed client stack if any error is encountered
    // during any of the LWM2M operations. Error type is passed in
    // the callback.
    void error(M2MInterface::Error error);

    /* Callback from mbed client stack if any value has changed
    *  during PUT operation. Object and its type is passed in
    *  the callback.
    *  BaseType enum from m2mbase.h
    *       Object = 0x0, Resource = 0x1, ObjectInstance = 0x2, ResourceInstance = 0x3
    */
    void value_updated(M2MBase *base, M2MBase::BaseType type);

    /*
    * update the registration period
    */
    void test_update_register();

    /*
    * manually configure the security object private variable
    */
   void set_register_object(M2MSecurity *register_object);

private:
   /*
   *  Private variables used in class
   */
   M2MInterface             *_interface;
   M2MSecurity              *_register_security;
   M2MObject                *_object;
   volatile bool            _bootstrapped;
   volatile bool            _error;
   volatile bool            _registered;
   volatile bool            _unregistered;
   int                      _value;
   struct MbedClientDevice  _device;
   String                   _server_address;
};

#endif // __SIMPLECLIENT_H__
