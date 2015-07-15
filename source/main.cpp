/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "mbed-net-sockets/UDPSocket.h"
#include "EthernetInterface.h"
#include "test_env.h"
#include "lwm2m-client/m2minterfacefactory.h"
#include "lwm2m-client/m2mdevice.h"
#include "lwm2m-client/m2minterfaceobserver.h"
#include "lwm2m-client/m2minterface.h"
#include "lwm2m-client/m2mobjectinstance.h"
#include "security.h"

#include "lwipv4_init.h"

// Select connection mode: Psk, Certificate or NoSecurity
M2MSecurity::SecurityModeType CONN_MODE = M2MSecurity::NoSecurity;

// Enter your mbed Device Server's IPv4 address and Port number in
// mentioned format like coap://192.168.0.1
const String &MBED_SERVER_ADDRESS = "coap://<xxx.xxx.xxx.xxx>";
//If you use secure connection port is 5684, for non-secure port is 5683
const int &MBED_SERVER_PORT = 5683;

const String &ENDPOINT_NAME = "lwm2m-endpoint";

const String &MANUFACTURER = "manufacturer";
const String &TYPE = "type";
const String &MODEL_NUMBER = "2015";
const String &SERIAL_NUMBER = "12345";

const uint8_t STATIC_VALUE[] = "Static value";

#if defined(TARGET_K64F)
#define OBS_BUTTON SW2
#define UNREG_BUTTON SW3
#endif


class M2MLWClient: public M2MInterfaceObserver {
public:
    M2MLWClient(){
        _interface = NULL;
        _bootstrapped = false;
        _error = false;
        _registered = false;
        _unregistered = false;
        _register_security = NULL;
        _value = 0;
        _object = NULL;
    }

    ~M2MLWClient() {
        if(_interface) {
            delete _interface;
        }
        if(_register_security){
            delete _register_security;
        }
    }

    void create_interface() {
        // Creates M2MInterface using which endpoint can
        // setup its name, resource type, life time, connection mode,
        // Currently only LwIPv4 is supported.
        _interface = M2MInterfaceFactory::create_interface(*this,
                                                  ENDPOINT_NAME,
                                                  "test",
                                                  3600,
                                                  MBED_SERVER_PORT,
                                                  "",
                                                  M2MInterface::UDP,
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
            char buffer[6];
            sprintf(buffer,"%d",MBED_SERVER_PORT);
            m2m::String port(buffer);

            m2m::String addr = MBED_SERVER_ADDRESS;
            addr.append(":", 1);
            addr.append(port.c_str(), size_t(port.size()) );
            security->set_resource_value(M2MSecurity::M2MServerUri, addr);
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);
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
                    M2MResource* res = inst->create_dynamic_resource("D","ResourceTest",true);
                    char buffer[20];
                    int size = sprintf(buffer,"%d",_value);
                    res->set_operation(M2MBase::GET_PUT_ALLOWED);
                    res->set_value((const uint8_t*)buffer,
                                   (const uint32_t)size);
                    _value++;

                    inst->create_static_resource("S",
                                                 "ResourceTest",
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
                                   (const uint32_t)size,
                                   true);
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
            printf("\nBootstrapped\n");
        }
    }

    //Callback from mbed client stack when the registration
    // is successful, it returns the mbed Device Server object
    // to which the resources are registered and registered objects.
    void object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
        _registered = true;
        _unregistered = false;
        printf("\nRegistered\n");
    }

    //Callback from mbed client stack when the unregistration
    // is successful, it returns the mbed Device Server object
    // to which the resources were unregistered.
    void object_unregistered(M2MSecurity */*server_object*/){
        _unregistered = true;
        _registered = false;
        printf("\nUnregistered\n");
    }

    void registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/){
    }

    //Callback from mbed client stack if any error is encountered
    // during any of the LWM2M operations. Error type is passed in
    // the callback.
    void error(M2MInterface::Error /*error*/){
        _error = true;
        printf("\nError occured\n");
    }

    //Callback from mbed client stack if any value has changed
    // during PUT operation. Object and its type is passed in
    // the callback.
    void value_updated(M2MBase *base, M2MBase::BaseType type) {
        printf("\nValue updated of Object name %s and Type %d\n",
               base->name().c_str(), type);
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

int main() {

    // This sets up the network interface configuration which will be used
    // by LWM2M Client API to communicate with mbed Device server.
    EthernetInterface eth;
    eth.init(); //Use DHCP
    eth.connect();

    lwipv4_socket_init();

    // Instantiate the class which implements
    // LWM2M Client API
    M2MLWClient lwm2mclient;

    // Set up Hardware interrupt button.
    InterruptIn obs_button(OBS_BUTTON);
    InterruptIn unreg_button(UNREG_BUTTON);

    // On press of SW3 button on K64F board, example application
    // will call unregister API towards mbed Device Server
    unreg_button.fall(&lwm2mclient,&M2MLWClient::test_unregister);

    // On press of SW2 button on K64F board, example application
    // will send observation towards mbed Device Server
    obs_button.fall(&lwm2mclient,&M2MLWClient::update_resource);

    // Create LWM2M Client API interface to manage register and unregister
    lwm2mclient.create_interface();

    // Create LWM2M server object specifying mbed device server
    // information.
    M2MSecurity* register_object = lwm2mclient.create_register_object();

    if( CONN_MODE == M2MSecurity::Certificate ){
        register_object->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::Certificate);
        register_object->set_resource_value(M2MSecurity::ServerPublicKey,SERVER_CERT,sizeof(SERVER_CERT));
        register_object->set_resource_value(M2MSecurity::PublicKey,CERT,sizeof(CERT));
        register_object->set_resource_value(M2MSecurity::Secretkey,KEY,sizeof(KEY));
    }else if( CONN_MODE == M2MSecurity::Psk ){
        register_object->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::Psk);
        register_object->set_resource_value(M2MSecurity::ServerPublicKey,PSK_IDENTITY,sizeof(PSK_IDENTITY));
        register_object->set_resource_value(M2MSecurity::PublicKey,PSK_IDENTITY,sizeof(PSK_IDENTITY));
        register_object->set_resource_value(M2MSecurity::Secretkey,PSK,sizeof(PSK));
    }else{
        register_object->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);
    }

    // Create LWM2M device object specifying device resources
    // as per OMA LWM2M specification.
    M2MDevice* device_object = lwm2mclient.create_device_object();

    // Create Generic object specifying custom resources
    M2MObject* generic_object = lwm2mclient.create_generic_object();

    // Add all the objects that you would like to register
    // into the list and pass the list for register API.
    M2MObjectList object_list;
    object_list.push_back(device_object);
    object_list.push_back(generic_object);

    // Issue register command.
    lwm2mclient.test_register(register_object, object_list);

    // Wait till the register callback is called successfully.
    // Callback comes in object_registered()
    while (!lwm2mclient.register_successful()) { __WFI(); }

    // Wait for the unregister successful callback,
    // Callback comes in object_unregsitered(), this will be
    // waiting for user to press SW2 button on K64F board.
    while (!lwm2mclient.unregister_successful()) { __WFI(); }

    // This will turn on the LED on the board specifying that
    // the application has run successfully.
    notify_completion(lwm2mclient.unregister_successful() &&
                      lwm2mclient.register_successful() );

    // Delete security object created for registration
    if(register_object) {
        delete register_object;
    }

    // Delete device object created for registering device
    // resources.
    if(device_object) {
        M2MDevice::delete_instance();
    }

    // Delete generic object created for registering custom
    // resources.
    if(generic_object) {
        delete generic_object;
    }

    // Disconnect the connect and teardown the network interface
    eth.disconnect();

    return 0;
}

