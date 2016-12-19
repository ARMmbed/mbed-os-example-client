/*
 * simpleclient.cpp
 *
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

#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2mdevice.h"
#include "mbed-client/m2minterfaceobserver.h"
#include "mbed-client/m2minterface.h"
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
#include "mbed-client/m2mconfig.h"
#include "mbed-client/m2mblockmessage.h"
#include "mbed-trace/mbed_trace.h"
#include "simpleclient.h"
#include "security.h"
#include "mbed.h"
#include "client_net.h"


#define TRACE_GROUP "sClt"


// constructor for MbedClient object, initialize private variables
MbedClient::MbedClient(struct MbedClientDevice device) :
    _interface(NULL),
    _register_security(NULL),
    _object(NULL),
    _bootstrapped(false),
    _error(false),
    _registered(false),
    _unregistered(false),
    _value(0),
    _device(device)
{
    tr_info("MbedClient::MbedClient");
}

// de-constructor for MbedClient object, you can ignore this
MbedClient::~MbedClient()
{
    if(_interface) {
        delete _interface;
    }
    if(_register_security){
        delete _register_security;
    }
}

// debug printf function
void MbedClient::trace_printer(const char* str)
{
    printf("\r\n%s\r\n", str);
}

/*
 *  Creates M2MInterface using which endpoint can
 *  setup its name, resource type, life time, connection mode,
 *  Currently only LwIPv4 is supported.
*/
void MbedClient::create_interface(const char *server_address,
                                  void *handler)
{
    // Randomizing listening port for Certificate mode connectivity
    _server_address = server_address;
    uint16_t port = 0; // Network interface will randomize with port 0

    // create mDS interface object, this is the base object everything else attaches to
    _interface = M2MInterfaceFactory::create_interface(*this,
                                                      MBED_ENDPOINT_NAME,       // endpoint name string
                                                      "test",                   // endpoint type string
                                                      100,                      // lifetime
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
    }
}

/*
 *  check private variable to see if the registration was sucessful or not
*/
bool MbedClient::register_successful()
{
        return _registered;
}

/*
 *  check private variable to see if un-registration was sucessful or not
*/
bool MbedClient::unregister_successful()
{
    return _unregistered;
}

/*
 *  Creates register server object with mbed device server address and other parameters
 *  required for client to connect to mbed device server.
*/
M2MSecurity* MbedClient::create_register_object()
{
    // create security object using the interface factory.
    // this will generate a security ObjectID and ObjectInstance
    M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);

    // make sure security ObjectID/ObjectInstance was created successfully
    if(security) {
		// Add ResourceID's and values to the security ObjectID/ObjectInstance
		security->set_resource_value(M2MSecurity::M2MServerUri, _server_address);
		security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::Certificate);
		security->set_resource_value(M2MSecurity::ServerPublicKey, SERVER_CERT, sizeof(SERVER_CERT));
		security->set_resource_value(M2MSecurity::PublicKey, CERT, sizeof(CERT));
		security->set_resource_value(M2MSecurity::Secretkey, KEY, sizeof(KEY));
	}
	return security;
}

/*
* Creates device object which contains mandatory resources linked with
* device endpoint.
*/
M2MDevice* MbedClient::create_device_object()
{
	// create device objectID/ObjectInstance
	M2MDevice *device = M2MInterfaceFactory::create_device();
	// make sure device object was created successfully
	if(device) {
		// add resourceID's to device objectID/ObjectInstance
		device->create_resource(M2MDevice::Manufacturer, _device.Manufacturer);
		device->create_resource(M2MDevice::DeviceType, _device.Type);
		device->create_resource(M2MDevice::ModelNumber, _device.ModelNumber);
		device->create_resource(M2MDevice::SerialNumber, _device.SerialNumber);
	}
	return device;
}

/*
* register an object
*/
void MbedClient::test_register(M2MSecurity *register_object, M2MObjectList object_list)
{
	if(_interface) {
		// Register function
		_interface->register_object(register_object, object_list);
	}
}

/*
* unregister all objects
*/
void MbedClient::test_unregister()
{
	if(_interface) {
		// Unregister function
		_interface->unregister_object(NULL); // NULL will unregister all objects
	}
}

/* Callback from mbed client stack when the bootstrap
 * is successful, it returns the mbed Device Server object
 * which will be used for registering the resources to
 * mbed Device server.
 */
void MbedClient::bootstrap_done(M2MSecurity *server_object)
{
	if(server_object) {
		_bootstrapped = true;
		_error = false;
		 tr_debug("Bootstrapped");
	}
}

/* Callback from mbed client stack when the registration
 * is successful, it returns the mbed Device Server object
 * to which the resources are registered and registered objects.
 */
void MbedClient::object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/)
{
	_registered = true;
	_unregistered = false;
	this->trace_printer("Registered object successfully!");
}

    //Callback from mbed client stack when the unregistration
    // is successful, it returns the mbed Device Server object
    // to which the resources were unregistered.
void MbedClient::object_unregistered(M2MSecurity */*server_object*/)
{
	this->trace_printer("Unregistered Object Successfully");
	_unregistered = true;
	_registered = false;
}

/*
* Callback from mbed client stack when registration is updated
*/
void MbedClient::registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/)
{
	/* The registration is updated automatically and frequently by the
	*  mbed client stack. This print statement is turned off because it
	*  tends to happen alot.
	*/
	tr_debug("\r\nRegistration Updated\r\n");
}

/* Callback from mbed client stack if any error is encountered
 * during any of the LWM2M operations. Error type is passed in
 * the callback.
 */
void MbedClient::error(M2MInterface::Error error)
{
	_error = true;
	switch(error){
		case M2MInterface::AlreadyExists:
			tr_debug("[ERROR:] M2MInterface::AlreadyExist");
			break;
		case M2MInterface::BootstrapFailed:
			tr_debug("[ERROR:] M2MInterface::BootstrapFailed");
			break;
		case M2MInterface::InvalidParameters:
			tr_debug("[ERROR:] M2MInterface::InvalidParameters");
			break;
		case M2MInterface::NotRegistered:
			tr_debug("[ERROR:] M2MInterface::NotRegistered");
			break;
		case M2MInterface::Timeout:
			tr_debug("[ERROR:] M2MInterface::Timeout");
			break;
		case M2MInterface::NetworkError:
			tr_debug("[ERROR:] M2MInterface::NetworkError");
			break;
		case M2MInterface::ResponseParseFailed:
			tr_debug("[ERROR:] M2MInterface::ResponseParseFailed");
			break;
		case M2MInterface::UnknownError:
			tr_debug("[ERROR:] M2MInterface::UnknownError");
			break;
		case M2MInterface::MemoryFail:
			tr_debug("[ERROR:] M2MInterface::MemoryFail");
			break;
		case M2MInterface::NotAllowed:
			tr_debug("[ERROR:] M2MInterface::NotAllowed");
			break;
		case M2MInterface::SecureConnectionFailed:
			tr_debug("[ERROR:] M2MInterface::SecureConnectionFailed");
			break;
		case M2MInterface::DnsResolvingFailed:
			tr_debug("[ERROR:] M2MInterface::DnsResolvingFailed");
			break;

		default:
			tr_debug("[ERROR:] Unknown error met in simpleclient.cpp");
			break;
	}
 }

/* Callback from mbed client stack if any value has changed
*  during PUT operation. Object and its type is passed in
*  the callback.
*  BaseType enum from m2mbase.h
*       Object = 0x0, Resource = 0x1, ObjectInstance = 0x2, ResourceInstance = 0x3
*/
void MbedClient::value_updated(M2MBase *base, M2MBase::BaseType type)
{
	tr_debug("\r\nPUT Request Received!");
	tr_debug("\r\nName :'%s', \r\nType : '%d' (0 for Object, 1 for Resource), \r\nType : '%s'\r\n",
		   base->name(),
		   type,
		   base->resource_type()
		   );
}

/*
* update the registration period
*/
void MbedClient::test_update_register()
{
	if (_registered) {
		_interface->update_registration(_register_security, 100);
	}
}

/*
* manually configure the security object private variable
*/
void  MbedClient::set_register_object(M2MSecurity *register_object)
{
	if (_register_security == NULL) {
		_register_security = register_object;
	}
}
