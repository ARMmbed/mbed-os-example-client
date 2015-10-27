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
#include "sockets/UDPSocket.h"
#include "EthernetInterface.h"
#include "test_env.h"
#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2mdevice.h"
#include "mbed-client/m2minterfaceobserver.h"
#include "mbed-client/m2minterface.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
#include "minar/minar.h"
#include "security.h"

#include "lwipv4_init.h"

using namespace mbed::util;

Serial output(USBTX, USBRX);

//Select binding mode: UDP or TCP
M2MInterface::BindingMode SOCKET_MODE = M2MInterface::UDP;

// This is address to mbed Device Connector
const String &MBED_SERVER_ADDRESS = "coap://ds-test-sl.dev.mbed.com:5684";

const String &MBED_USER_NAME_DOMAIN = MBED_DOMAIN;
const String &ENDPOINT_NAME = MBED_ENDPOINT_NAME;

const String &MANUFACTURER = "manufacturer";
const String &TYPE = "type";
const String &MODEL_NUMBER = "2015";
const String &SERIAL_NUMBER = "12345";

const uint8_t STATIC_VALUE[] = "Static value";

#if defined(TARGET_K64F)
#define OBS_BUTTON SW2
#define UNREG_BUTTON SW3
#endif


class MbedClient: public M2MInterfaceObserver {
public:
    MbedClient(){
        _interface = NULL;
        _bootstrapped = false;
        _error = false;
        _registered = false;
        _unregistered = false;
        _register_security = NULL;
        _value = 0;
        _object = NULL;
    }

    ~MbedClient() {
        if(_interface) {
            delete _interface;
        }
        if(_register_security){
            delete _register_security;
        }
    }

    void trace_printer(const char* str) {
        output.printf("%s\r\n", str);
    }

    void create_interface() {
        // Creates M2MInterface using which endpoint can
        // setup its name, resource type, life time, connection mode,
        // Currently only LwIPv4 is supported.

	// Randomizing listening port for Certificate mode connectivity
	srand(time(NULL));
	uint16_t port = rand() % 65535 + 12345;

        _interface = M2MInterfaceFactory::create_interface(*this,
                                                  ENDPOINT_NAME,
                                                  "test",
                                                  3600,
                                                  port,
                                                  MBED_USER_NAME_DOMAIN,
                                                  SOCKET_MODE,
                                                  M2MInterface::LwIP_IPv4,
                                                  "");
    }

    bool register_successful() {
        return _registered;
    }

    bool unregister_successful() {
        return _unregistered;
    }

    M2MSecurity* create_register_object() {
        // Creates register server object with mbed device server address and other parameters
        // required for client to connect to mbed device server.
        M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);
        if(security) {
            security->set_resource_value(M2MSecurity::M2MServerUri, MBED_SERVER_ADDRESS);
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::Certificate);
            security->set_resource_value(M2MSecurity::ServerPublicKey,SERVER_CERT,sizeof(SERVER_CERT));
            security->set_resource_value(M2MSecurity::PublicKey,CERT,sizeof(CERT));
            security->set_resource_value(M2MSecurity::Secretkey,KEY,sizeof(KEY));
        }
        return security;
    }

    M2MDevice* create_device_object() {
        // Creates device object which contains mandatory resources linked with
        // device endpoint.
        M2MDevice *device = M2MInterfaceFactory::create_device();
        if(device) {
            device->create_resource(M2MDevice::Manufacturer,MANUFACTURER);
            device->create_resource(M2MDevice::DeviceType,TYPE);
            device->create_resource(M2MDevice::ModelNumber,MODEL_NUMBER);
            device->create_resource(M2MDevice::SerialNumber,SERIAL_NUMBER);
        }
        return device;
    }

    M2MObject* create_generic_object() {
        _object = M2MInterfaceFactory::create_object("Test");
        if(_object) {
            M2MObjectInstance* inst = _object->create_object_instance();
            if(inst) {
                    M2MResource* res = inst->create_dynamic_resource("D",
                                                                     "ResourceTest",
                                                                     M2MResourceInstance::INTEGER,
                                                                     true);
                    char buffer[20];
                    int size = sprintf(buffer,"%d",_value);
                    res->set_operation(M2MBase::GET_PUT_ALLOWED);
                    res->set_value((const uint8_t*)buffer,
                                   (const uint32_t)size);
                    _value++;

                    inst->create_static_resource("S",
                                                 "ResourceTest",
                                                 M2MResourceInstance::STRING,
                                                 STATIC_VALUE,
                                                 sizeof(STATIC_VALUE)-1);
            }
        }
        return _object;
    }

    void update_resource() {
        if(_object) {
            M2MObjectInstance* inst = _object->object_instance();
            if(inst) {
                    M2MResource* res = inst->resource("D");

                    char buffer[20];
                    int size = sprintf(buffer,"%d",_value);
                    res->set_value((const uint8_t*)buffer,
                                   (const uint32_t)size);
                    _value++;
                }
        }
    }

    void test_register(M2MSecurity *register_object, M2MObjectList object_list){
        if(_interface) {
            // Register function
            _interface->register_object(register_object, object_list);
        }
    }

    void test_unregister() {
        if(_interface) {
            // Unregister function
            _interface->unregister_object(NULL);
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
            trace_printer("\nBootstrapped\n");
        }
    }

    //Callback from mbed client stack when the registration
    // is successful, it returns the mbed Device Server object
    // to which the resources are registered and registered objects.
    void object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
        _registered = true;
        _unregistered = false;
        trace_printer("\nRegistered\n");
    }

    //Callback from mbed client stack when the unregistration
    // is successful, it returns the mbed Device Server object
    // to which the resources were unregistered.
    void object_unregistered(M2MSecurity */*server_object*/){
        _unregistered = true;
        _registered = false;
        notify_completion(_unregistered);
        minar::Scheduler::stop();
        trace_printer("\nUnregistered\n");
    }

    void registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/){
    }

    //Callback from mbed client stack if any error is encountered
    // during any of the LWM2M operations. Error type is passed in
    // the callback.
    void error(M2MInterface::Error error){
        _error = true;
        switch(error){
            case M2MInterface::AlreadyExists:
                trace_printer("[ERROR:] M2MInterface::AlreadyExists\n");
                break;
            case M2MInterface::BootstrapFailed:
                trace_printer("[ERROR:] M2MInterface::BootstrapFailed\n");
                break;
            case M2MInterface::InvalidParameters:
                trace_printer("[ERROR:] M2MInterface::InvalidParameters\n");
                break;
            case M2MInterface::NotRegistered:
                trace_printer("[ERROR:] M2MInterface::NotRegistered\n");
                break;
            case M2MInterface::Timeout:
                trace_printer("[ERROR:] M2MInterface::Timeout\n");
                break;
            case M2MInterface::NetworkError:
                trace_printer("[ERROR:] M2MInterface::NetworkError\n");
                break;
            case M2MInterface::ResponseParseFailed:
                trace_printer("[ERROR:] M2MInterface::ResponseParseFailed\n");
                break;
            case M2MInterface::UnknownError:
                trace_printer("[ERROR:] M2MInterface::UnknownError\n");
                break;
            case M2MInterface::MemoryFail:
                trace_printer("[ERROR:] M2MInterface::MemoryFail\n");
                break;
            case M2MInterface::NotAllowed:
                trace_printer("[ERROR:] M2MInterface::NotAllowed\n");
                break;
            default:
                break;
        }
    }

    //Callback from mbed client stack if any value has changed
    // during PUT operation. Object and its type is passed in
    // the callback.
    void value_updated(M2MBase *base, M2MBase::BaseType type) {
        output.printf("\nValue updated of Object name %s and Type %d\n",
               base->name().c_str(), type);
    }

    void test_update_register() {
        if (_registered) {
            _interface->update_registration(_register_security, 3600);
        }
    }

   void set_register_object(M2MSecurity *register_object) {
        if (_register_security == NULL) {
            _register_security = register_object;
        }
    }

private:

    M2MInterface    	*_interface;
    M2MSecurity         *_register_security;
    M2MObject           *_object;
    volatile bool       _bootstrapped;
    volatile bool       _error;
    volatile bool       _registered;
    volatile bool       _unregistered;
    int                 _value;
};

EthernetInterface eth;
// Instantiate the class which implements
// LWM2M Client API
MbedClient mbed_client;

// Set up Hardware interrupt button.
InterruptIn obs_button(OBS_BUTTON);
InterruptIn unreg_button(UNREG_BUTTON);

void app_start(int /*argc*/, char* /*argv*/[]) {

    //Sets the console baud-rate
    output.baud(115200);

    // This sets up the network interface configuration which will be used
    // by LWM2M Client API to communicate with mbed Device server.
    eth.init(); //Use DHCP
    eth.connect();

    lwipv4_socket_init();

    // On press of SW3 button on K64F board, example application
    // will call unregister API towards mbed Device Server
    unreg_button.fall(&mbed_client,&MbedClient::test_unregister);

    // On press of SW2 button on K64F board, example application
    // will send observation towards mbed Device Server
    obs_button.fall(&mbed_client,&MbedClient::update_resource);

    // Create LWM2M Client API interface to manage register and unregister
    mbed_client.create_interface();

    // Create LWM2M server object specifying mbed device server
    // information.
    M2MSecurity* register_object = mbed_client.create_register_object();

    // Create LWM2M device object specifying device resources
    // as per OMA LWM2M specification.
    M2MDevice* device_object = mbed_client.create_device_object();

    // Create Generic object specifying custom resources
    M2MObject* generic_object = mbed_client.create_generic_object();

    // Add all the objects that you would like to register
    // into the list and pass the list for register API.
    M2MObjectList object_list;
    object_list.push_back(device_object);
    object_list.push_back(generic_object);

    mbed_client.set_register_object(register_object);

    // Issue register command.
    FunctionPointer2<void, M2MSecurity*, M2MObjectList> fp(&mbed_client, &MbedClient::test_register);
    minar::Scheduler::postCallback(fp.bind(register_object,object_list));
    minar::Scheduler::postCallback(&mbed_client,&MbedClient::test_update_register).period(minar::milliseconds(25000));
}

